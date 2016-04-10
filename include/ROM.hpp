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

/** \brief Namespace for ROM classes
 *
 *  This namespace contains all the classes needed for dealing with ROM files
 *  and the game files within them.
 *
 */
namespace ROM {
    /** \brief Class representing a TOC entry
     *
     *  This class is for representing one entry in the game's "table of
     *  contents" for its files. It also offers convenience member functions for
     *  handling the quirks in extracting certain kinds of info.
     *
     *  For the C++ programmer, it's useful to note that both pairs of addresses
     *  (\c vstart and \c vend, and \c pstart and \c pend) are equivalent to \c
     *  std::begin and \c std::end respectively, aside from any quirks in the
     *  physical address listings. In other words, if I felt like coming up with
     *  the iterators, we could transform pstart and a potentially corrected
     *  pend into iterators into the ROM.
     *
     */
    struct Record {
        uint32_t vstart;   ///< Virtual file location
        uint32_t vend;     ///< Virtual file one-past-the-end
        uint32_t pstart;   ///< Actual, physical location in ROM
        uint32_t pend;     ///< Physical in-ROM one-past-the-end
        std::string fname; ///< Human-readable name, if currently known

        /** \brief States if record's file is compressed.
         *
         *  This test tells you if the file this record describes is
         *  compressed. This is determined by seeing if the entry gives a
         *  non-zero physical size, and isn't missing (see <tt>isMissing()</tt>).
         *
         *  Uncompressed files set the physical one-past-the-end address to
         *  zero, indicating that the size, and thus "true" one-past-the-end
         *  physical address, can be inferred from the virtual addresses.
         *
         *  \returns Boolean indicating the result of this test.
         *
         */
        bool isCompressed() const;

        /** \brief Indicates if the file is "missing" (i.e. not present in the
         *         ROM)
         *
         *  This test tells you if the file listed here is actually missing from
         *  the ROM. These files, naturally, are useless aside from historical
         *  value.
         *
         *  This is indicated in the table of contents by having the physical
         *  start and end addresses set to \c 0xFFFFFFFF (or -1 if interpreted
         *  as signed 32-bit number).
         *
         *  Worth noting that only Majora's Mask ROMs seem to have missing
         *  entries, it's entirely possible OoT's engine wouldn't properly
         *  handle them if that's the case.
         *
         *  \returns Boolean indicating the result of this test.
         *
         */
        bool isMissing() const;

        /** \brief Returns the physical, in-ROM size of the file.
         *
         *  This function returns the physical size of the file this record
         *  describes. It automatically handles the case of uncompressed files,
         *  so deciding between this and \c vsize() in your own code is not
         *  necessary.
         *
         *  \returns A \c size_t integer representing the size in bytes.
         *
         */
        size_t psize() const;

        /** \brief Returns the virtual size of the file.
         *
         *  This function indicates the virtual size of the file this record
         *  describes. If the file is compressed, this is the post-decompression
         *  size. Note that "missing" files may have a non-zero virtual size.
         *
         *  \returns A \c size_t integer representing the size in bytes.
         *
         */
        size_t vsize() const;
    };

    /** \brief Class holding a file of the game
     *
     *  Objects of this class hold the contents of a game's file, as described
     *  by a Record. Each File comes with the Record that described it,
     *  functioning here as metadata.
     *
     */
    class File {
      private:
        std::vector<uint8_t> fileData; ///< raw bytes of the file
        Record foundAt;                ///< file's Record, aka its metadata
        bool decompressed = false;     ///< flag to avoid re-decompression attempts

      public:
        File() = default;

        /** \brief Constructs a file from its Record and data
         *
         *  This constructor takes a vector of bytes for the file data, and the
         *  Record that located the file.
         *
         *  \param[in] fd The raw data, as a vector of bytes
         *
         *  \param[in] fa The Record that helped locate the data.
         *
         */
        File(const std::vector<uint8_t> & fd, const Record & fa);

        /** \brief Returns the Record for the file.
         *
         *  This function returns a \em copy of the Record describing the
         *  file. This method doesn't let you modify the file's actual Record.
         *
         *  \returns Copy of file's Record.
         *
         */
        Record record() const;

        /** \brief Returns the size of the file.
         *
         *  This function indicates the size of the file in bytes, effectively
         *  the size of the raw data vector. Note that its correspondence to \c
         *  Record::psize() or \c Record::vsize() depends on whether the file
         *  was compressed originally, and if so if it's a decompressed version
         *  of the file.
         *
         *  This function therefore is not a perfect replacement of either \c
         *  Record::psize() or \c Record::vsize(); if you want those numbers,
         *  you must get the file's record (see \c \ref record).
         *
         *  \returns A \c size_t giving the size in bytes of the file.
         *
         */
        size_t size() const;

        /** \brief Returns the byte at the given index.
         *
         *  This function returns the byte at the specified index in the raw
         *  data. It uses the vector's \c at() function internally, so
         *  out-of-bounds accesses are checked for and will cause an exception.
         *
         *  \param[in] idx Index into the raw data
         *
         *  \returns Byte at the given position.
         *
         */
        uint8_t at(size_t idx) const;

        /** \brief Decompresses this file, and returns the new File.
         *
         *  This member function decompresses the File it's called on, and
         *  returns a \em new \c File as a result. If the file is already
         *  decompressed, or was never compressed to begin with, returns a \em
         *  copy of itself.
         *
         *  \warning Due to the likely chance that you may think otherwise, this
         *           function <b>does not</b> modify itself; the decompressed
         *           file returned is a \em new \c File object. This function is
         *           declared with the attribute \c [[nodiscard]] to serve as a
         *           safeguard against this.
         *
         *  \returns a new \c File object of the file decompressed, or if not
         *           necessary to decompress a copy of the file.
         *
         */

        // XXX temporary Band-Aid® Brand bandaid for GCC (5.3)'s weird current
        // decision to use a different name while C++17 is in draft status (yes,
        // [[nodiscard]] is slated for C++17, we're using it anyway since it
        // doesn't hurt if not supported yet, and is kinda important).
#ifdef __GNUG__
        [[gnu::warn_unused_result]] File decompress() const;
#else
        [[nodiscard]] File decompress() const;
#endif

        std::vector<uint8_t> getData() const;

        std::vector<uint8_t>::iterator begin();
        std::vector<uint8_t>::iterator end();
    };

    class ROM {
      private:
        std::vector<uint8_t> rawData;

        std::vector<Record> fileList;

        mutable std::map<size_t, File> fcache;

        Config::Version rver;
        ConfigTree ctree;

        bool wasBS = false;

        void maybeCache(const Record & r) const;

        void unByteSwap();
        void bootstrapTOC(size_t firstEntry);
        void bootstrapCompTime(size_t strat);

      public:
        ROM(std::vector<uint8_t> rfile);

        bool wasByteswapped() const;

        size_t numFiles() const;

        File fileAtNum(size_t idx) const;
        File fileAtVAddress(size_t addr) const;
        File fileAtName(std::string name) const;

        Record recordAtNum(size_t idx) const;
        Record recordAtVAddress(size_t addr) const;
        Record recordAtName(std::string name) const;

        size_t size() const;

        std::string get_rname() const;
        std::string get_rcode() const;

        std::vector<uint8_t> getData() const;

        Config::Version getVersion() const;

        bool hasConfigKey(std::initializer_list<std::string> parts) const;
        std::string configKey(std::initializer_list<std::string> parts) const;
    };
}