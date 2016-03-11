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

    bool isCompressed();
    bool isMissing();
    size_t psize();
    size_t vsize();
};

class ROMFile {
  private:
    std::vector<uint8_t> fileData;
    ROMRecord foundAt;

  public:
    ROMFile() = default;
    ROMFile(const std::vector<uint8_t> & fd, const ROMRecord & fa);

    ROMRecord record();

    size_t size();
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

    size_t numfiles();
    ROMFile fileAt(size_t idx);

    ROMRecord fileidx(size_t idx);

    size_t size();

    std::string get_rname();
    std::string get_rcode();
};