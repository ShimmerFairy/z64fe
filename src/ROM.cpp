/** \file ROM.cpp
 *
 *  \brief Defines ROM class stuff
 *
 */

#include "ROM.hpp"
#include "endian.hpp"
#include "yaz0.hpp"
#include "Exceptions.hpp"

#include <QString>

#include <algorithm>
#include <iostream>

namespace ROM {
    bool Record::isCompressed() const { return pend != 0 && !isMissing(); }
    bool Record::isMissing() const { return pstart == 0xFFFF'FFFF && pend == 0xFFFF'FFFF; }

    size_t Record::psize() const {
        if (pend == 0) {
            return vsize();
        } else {
            return pend - pstart;
        }
    }

    size_t Record::vsize() const {
        return vend - vstart;
    }

    File::File(const std::vector<uint8_t> & fd, const Record & fa) : fileData(fd),
                                                                     foundAt(fa) { }

    Record File::record() const { return foundAt; }
    size_t File::size() const { return fileData.size(); }

    uint8_t File::at(size_t idx) const { return fileData.at(idx); }

    File File::decompress() const {
        if (foundAt.isCompressed() && !decompressed) {
            File res(yaz0_decompress(fileData), foundAt);
            res.decompressed = true;
            return res;
        } else {
            return *this;
        }
    }

    std::vector<uint8_t> File::getData() const { return fileData; }

    std::vector<uint8_t>::iterator File::begin() { return fileData.begin(); }
    std::vector<uint8_t>::iterator File::end() { return fileData.end(); }

    ROM::ROM(const std::vector<uint8_t> & rd) : rawData(rd) { }

    void ROM::byteSwap() {
        for (size_t i = 0; i < rawData.size(); i += 2) {
            std::swap(rawData[i], rawData[i+1]);
        }
    }

    size_t ROM::bootstrapTOC(size_t firstEntry) {
        // before anything, lets save the written compile timestamp to identify
        // versions (we do this instead of other methods so we don't have to worry
        // about uncompressed and compressed versions of ROMs).
        bootstrapCompTime(firstEntry - 0x30);

        // first, look for the TOC file's entry --- it's a lot nicer to work with a
        // definite file, instead of waiting until we run into something that
        // doesn't look like a file record.

        auto fpoint = rawData.begin() + firstEntry;

        Record tocrec;

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
            throw X::BadROM("Didn't find the Table of Contents in its own list, which may indicate a problem.");
        }

        if (tocrec.isCompressed()) {
            throw X::BadROM("The table of contents is claiming to be compressed, despite us finding its own entry in itself, suggesting it's NOT compressed. Due to this contradiction, we won't handle this ROM.");
        }

        if (tocrec.isMissing()) {
            throw X::BadROM("The table of contents appears to be 'missing' in its own list, despite being present enough for us to find it. This contradiction probably isn't the only one, and in any case must be fixed before we'll touch it.");
        }

        std::vector<uint8_t> tocList;

        fpoint = rawData.begin() + firstEntry;

        std::copy(fpoint, fpoint + tocrec.psize(), std::back_inserter(tocList));

        // now we go through the toc file and collect those entries

        bool canName = !ctree.isEmpty();

        for (auto i = tocList.begin(); i < tocList.end();) {
            Record rr;
            rr.vstart = be_u32(i); i += 4;
            rr.vend   = be_u32(i); i += 4;
            rr.pstart = be_u32(i); i += 4;
            rr.pend   = be_u32(i); i += 4;

            if (canName) {
                try {
                    rr.fname = ctree.findKey({"fileList"}, QString("0x%1").arg(
                                                 QString("%1").arg(rr.vstart, 8, 16, QChar('0')).toUpper()).toStdString());
                } catch (...) {
                    rr.fname = "";
                }
            } else {
                rr.fname = "";
            }

            fileList.push_back(rr);
        }

        // hey, we went through the toc, so now return the number of entries.
        return fileList.size();
    }

    size_t ROM::numfiles() const { return fileList.size(); }

    File ROM::fileAt(size_t idx) {
        if (fcache.count(idx) == 0) {
            std::vector<uint8_t> nd;

            std::copy(rawData.begin() + fileList.at(idx).pstart,
                      rawData.begin() + fileList.at(idx).pstart + fileList.at(idx).psize(),
                      std::back_inserter(nd));

            fcache[idx] = File(nd, fileList.at(idx));
        }

        return fcache[idx];
    }

    File ROM::fileAtVAddr(size_t addr) {
        auto rightFile = std::find_if(fileList.begin(), fileList.end(),
                                      [&](const Record & a) {
                                          return a.vstart == addr;
                                      });

        if (rightFile != fileList.end()) {
            return fileAt(std::distance(fileList.begin(), rightFile));
        }

        throw X::BadIndex("virtual address, not matching any files' starting positions");
    }

    File ROM::fileAtName(std::string name) {
        if (ctree.isEmpty()) {
            throw X::NoConfig("Finding files by name");
        }

        try {
            return fileAtVAddr(std::stoul(ctree.getValue({"fileList", name}), nullptr, 0));
        } catch (...) {
            return fileAtVAddr(std::stoul(ctree.getValue({"fileList", "fakeNames", name}), nullptr, 0));
        }
    }

    Record ROM::recordAt(size_t idx) const { return fileList.at(idx); }

    size_t ROM::size() const { return rawData.size(); }

    std::string ROM::get_rname() const {
        // lazy way to read until null byte
        return std::string(reinterpret_cast<const char *>(rawData.data()) + 0x20);
    }

    std::string ROM::get_rcode() const {
        return std::string(rawData.begin() + 0x3B, rawData.begin() + 0x3F);
    }

    std::vector<uint8_t> ROM::getData() const { return rawData; }

    void ROM::bootstrapCompTime(size_t strat) {
        // to find the compile timestamp, we first store the known area into the
        // string, since it'll be a little easier to comb through a string in this
        // case.

        std::string compileString = std::string(reinterpret_cast<char *>(rawData.data()) + strat, 0x30);

        // now to find the first null, after the username@hostname part
        size_t after_atpart = compileString.find_first_of('\0');

        if (after_atpart == std::string::npos) {
            throw X::BadROM("Somehow didn't find null in string!");
        }

        // now to find the first non-null after that, which'll be the timestamp
        size_t after_nulls = compileString.find_first_not_of('\0', after_atpart);

        if (after_nulls == std::string::npos) {
            throw X::BadROM("Somehow didn't find timestamp! We need this to determine the cartridge version, and your ROM appears to not have it, which may indicate a badly(?) hacked ROM.");
        }

        // and finally, set the compile string to just the timestamp (we do it this
        // way to avoid any trailing nulls in the known area)
        compileString = std::string(reinterpret_cast<char *>(rawData.data()) + strat + after_nulls);

        // now that that's done, we go ahead and figure out the correct version enum to set
        if (compileString == "98-10-21 04:56:31") {
            rver = Config::Version::OOT_NTSC_1_0;
        } else if (compileString == "98-10-26 10:58:45") {
            rver = Config::Version::OOT_NTSC_1_1;
        } else if (compileString == "98-11-12 18:17:03") {
            rver = Config::Version::OOT_NTSC_1_2;
        } else if (compileString == "98-11-10 14:32:22") {
            rver = Config::Version::OOT_PAL_1_0;
        } else if (compileString == "98-11-18 17:36:49") {
            rver = Config::Version::OOT_PAL_1_1;
        } else if (compileString == "03-02-21 00:16:31") {
            rver = Config::Version::OOT_MQ_DEBUG;
        } else if (compileString == "00-03-31 02:22:11") {
            rver = Config::Version::MM_JP_1_0;
        } else if (compileString == "00-04-04 09:34:16") {
            rver = Config::Version::MM_JP_1_1;
        } else if (compileString == "00-07-31 17:04:16") {
            rver = Config::Version::MM_US;
        } else if (compileString == "00-09-25 11:16:53") {
            rver = Config::Version::MM_EU_1_0;
        } else if (compileString == "00-09-29 09:29:41") {
            rver = Config::Version::MM_EU_1_1;
        } else if (compileString == "00-09-29 09:29:05") {
            rver = Config::Version::MM_DEBUG;
        } else {
            rver = Config::Version::UNKNOWN;
        }

        ctree = getConfigTree(rver);
    }

    Config::Version ROM::getVersion() const {
        return rver;
    }

    bool ROM::hasConfigKey(std::initializer_list<std::string> parts) const {
        if (ctree.isEmpty()) {
            return false;
        }

        try {
            ctree.getValue(parts);
            return true;
        } catch (Exception & e) {
            std::cerr << e.what() << "\n";
            return false;
        }
    }

    std::string ROM::configKey(std::initializer_list<std::string> parts) const {
        if (!hasConfigKey(parts)) {
            return "";
        }

        return ctree.getValue(parts);
    }
}