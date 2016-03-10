/** \file ROM.cpp
 *
 *  \brief Defines ROM class stuff
 *
 */

#include "ROM.hpp"
#include "endian.hpp"

bool ROMRecord::isCompressed() { return pend != 0 && !isMissing(); }
bool ROMRecord::isMissing() { return pstart == 0xFFFF'FFFF && pend == 0xFFFF'FFFF; }

size_t ROMRecord::psize() {
    if (pend == 0) {
        return vend - vstart;
    } else {
        return pend - pstart;
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

size_t ROM::numfiles() { return fileList.size(); }
ROMRecord ROM::fileidx(size_t idx) { return fileList.at(idx); }