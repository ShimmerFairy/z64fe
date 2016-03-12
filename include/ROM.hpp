/** \file ROM.hpp
 *
 *  \brief Declares a class for holding a ROM file.
 *
 */

#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <cstddef>
#include <string>

struct ROMRecord {
    uint32_t vstart;
    uint32_t vend;
    uint32_t pstart;
    uint32_t pend;

    bool isCompressed() const;
    bool isMissing() const;
    size_t psize() const;
    size_t vsize() const;
};

class ROMFile {
  private:
    std::vector<uint8_t> fileData;
    ROMRecord foundAt;
    bool decompressed = false; // used to avoid re-decomp attempts

  public:
    ROMFile() = default;
    ROMFile(const std::vector<uint8_t> & fd, const ROMRecord & fa);

    ROMRecord record() const;

    size_t size() const;

    uint8_t at(size_t idx) const;

    ROMFile decompress() const;
};

class ROM {
  private:
    std::vector<uint8_t> rawData;
    std::vector<ROMRecord> fileList;

    std::map<size_t, ROMFile> fcache;

  public:
    ROM() = default;
    ROM(const std::vector<uint8_t> & rd);

    void byteSwap();

    size_t bootstrapTOC(size_t firstEntry);

    size_t numfiles() const;
    ROMFile fileAt(size_t idx);

    ROMRecord fileidx(size_t idx) const;

    size_t size() const;

    std::string get_rname() const;
    std::string get_rcode() const;
};