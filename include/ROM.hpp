/** \file ROM.hpp
 *
 *  \brief Declares a class for holding a ROM file.
 *
 */

#pragma once

#include <cstdint>
#include <vector>
#include <cstddef>

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

class ROM {
  private:
    std::vector<uint8_t> rawData;
    std::vector<ROMRecord> fileList;

  public:
    ROM() = default;
    ROM(const std::vector<uint8_t> & rd);

    void byteSwap();

    size_t bootstrapTOC(size_t firstEntry);

    size_t numfiles();
    ROMRecord fileidx(size_t idx);
};