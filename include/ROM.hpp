/** \file ROM.hpp
 *
 *  \brief Declares a class for holding a ROM file.
 *
 */

#pragma once

#include "Config.hpp"
#include "ConfigTree.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <cstddef>
#include <string>

namespace ROM {

    struct Record {
        uint32_t vstart;
        uint32_t vend;
        uint32_t pstart;
        uint32_t pend;
        std::string fname;

        bool isCompressed() const;
        bool isMissing() const;
        size_t psize() const;
        size_t vsize() const;
    };

    class File {
      private:
        std::vector<uint8_t> fileData;
        Record foundAt;
        bool decompressed = false; // used to avoid re-decomp attempts

      public:
        File() = default;
        File(const std::vector<uint8_t> & fd, const Record & fa);

        Record record() const;

        size_t size() const;

        uint8_t at(size_t idx) const;

        File decompress() const;

        std::vector<uint8_t> getData() const;

        std::vector<uint8_t>::iterator begin();
        std::vector<uint8_t>::iterator end();
    };

    class ROM {
      private:
        std::vector<uint8_t> rawData;
        std::vector<Record> fileList;

        std::map<size_t, File> fcache;

        std::map<Config::Language, std::map<uint16_t, uint32_t>> text_ids;

        Config::Version rver;
        ConfigTree ctree;

        void bootstrapCompTime(size_t strat);

      public:
        ROM() = default;
        ROM(const std::vector<uint8_t> & rd);
        void byteSwap();
        size_t bootstrapTOC(size_t firstEntry);

        size_t numfiles() const;
        File fileAt(size_t idx);

        File fileAtVAddr(size_t addr);

        File fileAtName(std::string name);

        Record recordAt(size_t idx) const;

        size_t size() const;

        std::string get_rname() const;
        std::string get_rcode() const;

        std::vector<uint8_t> getData() const;

        Config::Version getVersion() const;

        bool hasConfigKey(std::initializer_list<std::string> parts) const;

        std::string configKey(std::initializer_list<std::string> parts) const;

        void analyzeMsgTbl();
        size_t sizeMsgTbl() const;
        std::string langStrMsgTbl() const;
        std::map<Config::Language, std::map<uint16_t, uint32_t>> msgTbl() const;
    };
}