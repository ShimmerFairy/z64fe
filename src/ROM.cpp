/** \file ROM.cpp
 *
 *  \brief Defines ROM class stuff
 *
 */

#include "ROM.hpp"
#include "endian.hpp"
#include "yaz0.hpp"

bool ROMRecord::isCompressed() const { return pend != 0 && !isMissing(); }
bool ROMRecord::isMissing() const { return pstart == 0xFFFF'FFFF && pend == 0xFFFF'FFFF; }

size_t ROMRecord::psize() const {
    if (pend == 0) {
        return vsize();
    } else {
        return pend - pstart;
    }
}

size_t ROMRecord::vsize() const {
    return vend - vstart;
}

ROMFile::ROMFile(const std::vector<uint8_t> & fd, const ROMRecord & fa) : fileData(fd),
                                                                          foundAt(fa) { }

ROMRecord ROMFile::record() const { return foundAt; }
size_t ROMFile::size() const { return fileData.size(); }

uint8_t ROMFile::at(size_t idx) const { return fileData.at(idx); }

ROMFile ROMFile::decompress() const {
    if (foundAt.isCompressed() && !decompressed) {
        ROMFile res(yaz0_decompress(fileData), foundAt);
        res.decompressed = true;
        return res;
    } else {
        return *this;
    }
}

ROM::ROM(const std::vector<uint8_t> & rd) : rawData(rd) { }

void ROM::byteSwap() {
    for (size_t i = 0; i < rawData.size(); i += 2) {
        std::swap(rawData[i], rawData[i+1]);
    }
}

size_t ROM::bootstrapTOC(size_t firstEntry) {
    // first, look for the TOC file's entry --- it's a lot nicer to work with a
    // definite file, instead of waiting until we run into something that
    // doesn't look like a file record.

    auto fpoint = rawData.begin() + firstEntry;

    ROMRecord tocrec;

    while (fpoint < rawData.end()) {
        tocrec.vstart = be_u32(fpoint); fpoint += 4;
        tocrec.vend   = be_u32(fpoint); fpoint += 4;
        tocrec.pstart = be_u32(fpoint); fpoint += 4;
        tocrec.pend   = be_u32(fpoint); fpoint += 4;

        if (tocrec.pstart == firstEntry) {
            break;
        }
    }

    if (tocrec.pstart != firstEntry) {
        throw "OH NO NO ENTRY;";
    }

    if (tocrec.isCompressed()) {
        throw "COMPRESSED TOC NYI";
    }

    if (tocrec.isMissing()) {
        throw "TOC MISSING?";
    }

    std::vector<uint8_t> tocList;

    fpoint = rawData.begin() + firstEntry;

    std::copy(fpoint, fpoint + tocrec.psize(), std::back_inserter(tocList));

    // now we go through the toc file and collect those entries

    for (auto i = tocList.begin(); i < tocList.end();) {
        ROMRecord rr;
        rr.vstart = be_u32(i); i += 4;
        rr.vend   = be_u32(i); i += 4;
        rr.pstart = be_u32(i); i += 4;
        rr.pend   = be_u32(i); i += 4;

        fileList.push_back(rr);
    }

    // hey, we went through the toc, so now return the number of entries.
    return fileList.size();
}

size_t ROM::numfiles() const { return fileList.size(); }

ROMFile ROM::fileAt(size_t idx) {
    if (fcache.count(idx) == 0) {
        std::vector<uint8_t> nd;

        std::copy(rawData.begin() + fileList.at(idx).pstart,
                  rawData.begin() + fileList.at(idx).pstart + fileList.at(idx).psize(),
                  std::back_inserter(nd));

        fcache[idx] = ROMFile(nd, fileList.at(idx));
    }

    return fcache[idx];
}

ROMRecord ROM::fileidx(size_t idx) const { return fileList.at(idx); }

size_t ROM::size() const { return rawData.size(); }

std::string ROM::get_rname() const {
    // lazy way to read until null byte
    return std::string(reinterpret_cast<const char*>(rawData.data()) + 0x20);
}

std::string ROM::get_rcode() const {
    return std::string(rawData.begin() + 0x3B, rawData.begin() + 0x3F);
}