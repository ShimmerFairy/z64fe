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

        /** \brief Returns a copy of the file's data as-is.
         *
         *  This function returns a copy of the vector of bytes constituting the
         *  file's raw data. If the file is currently compressed, you will get
         *  back a compressed vector.
         *
         *  \returns a \c std::vector<uint8_t> containing the file's data.
         *
         */
        std::vector<uint8_t> getData() const;

        /** \brief Returns an iterator to the beginning of the data.
         *
         *  This is just "forwarding" the internal vector's begin iterator.
         *
         *  \returns Vector iterator
         *
         */
        std::vector<uint8_t>::iterator begin();

        /** \brief Returns an iterator to one-past-the-end of the data.
         *
         *  THis just "forwards" the internal vector's end iterator.
         *
         *  \returns Vector iterator
         *
         */
        std::vector<uint8_t>::iterator end();
    };

    /** \brief Class for a ROM file
     *
     *  This class represents one ROM file, i.e. one Z64 game. The ROM collects
     *  all the necessary information to identify itself and its files in
     *  construction, and loads individual files on demand. (It also memoizes
     *  these file loads so future accesses aren't so time-consuming.)
     *
     *  Since this class currently loads the entirety of ROM into memory, it's
     *  currently recommended that you don't copy ROM objects, but rather pass
     *  around pointers or references. (Memory mapping files is being considered
     *  for the future.)
     *
     */
    class ROM {
      private:
        std::vector<uint8_t> rawData;          ///< Raw file data
        std::vector<Record> fileList;          ///< List of extracted TOC entries
        mutable std::map<size_t, File> fcache; ///< cache of files that have been previously requested
        mutable std::map<size_t, File> decfcache; ///< cache of decompressed files

        Config::Version rver; ///< Programmatically determined ROM version
        ConfigTree ctree;     ///< Config data for data hard/impossible to get out of ROM currently

        bool wasBS = false;   ///< Indicates if the ROM was originally byteswapped

        /** \brief Private function for handling file caching
         *
         *  This function takes the record of a file to retrieve, and if it
         *  isn't cached yet does so. It then returns the file in question.
         *
         *  \param[in] r The file's record.
         *
         *  \param[in] autodecomp Indicates if the file should be automatically
         *                        decompressed.
         *
         *  \returns A File matching the given record.
         *
         */
        File cachedAccess(const Record & r, bool autodecomp) const;

        /** \brief Private function for un-byteswapping ROMs
         *
         *  This function de-byteswaps the raw data if need be. It does _not_
         *  set the wasBS variable.
         *
         */
        void unByteSwap();

        /** \brief Handles extracting info from the Table of Contents
         *
         *  The bootstrapping refers to finding the TOC from a known reference
         *  point, finding its own entry in itself, and using that entry to
         *  extract the TOC file and read it for entries.
         *
         *  We do it in this manner to make sure we read the entire TOC file, as
         *  opposed to just reading until an arbitrary endpoint is reached
         *  (which in hacked ROMs may not even be true).
         *
         *  \param[in] firstEntry the offset into the ROM of the TOC's first
         *                        record.
         *
         */
        void bootstrapTOC(size_t firstEntry);

        /** \brief Handles reading the timestamp in the file for figuring out
         *         the version.
         *
         *  We use the compilation timestamp available in the ROM as the most
         *  reliable and specific way to figure out the version of the
         *  ROM. Other potential methods would've made it harder or impossible
         *  to determine the precise version of the ROM.
         *
         *  \note Checking the CRC might be another way, it's worth looking
         *        into. (The downside may be that the CRC doesn't have any
         *        meaning to the game, and could \em theoretically occur in
         *        another, non-Zelda ROM.
         *
         *  \param[in] strat Where to start looking for the timestamp.
         *
         */
        void bootstrapCompTime(size_t strat);

      public:
        /** \brief Constructor for ROM object
         *
         *  Takes the raw data of a file and sets itself up to handle the ROM,
         *  assuming it's a proper Zelda ROM. Exceptions can be thrown if the
         *  file is in some way invalid.
         *
         *  \param[in] rfile A vector of bytes from the file in question.
         *
         */
        ROM(std::vector<uint8_t> rfile);

        /** \brief Indicates if the file was originally byteswapped
         *
         *  This testing function simply tells you if the file originally handed
         *  to this ROM object was byteswapped (mainly useful for a "save this
         *  ROM un-byteswapped" feature).
         *
         *  \returns Boolean indicating if it was, in fact, byteswapped.
         *
         */
        bool wasByteswapped() const;

        /** \brief Indicates the number of files in the ROM
         *
         *  This function simply returns the number of files that were in the
         *  TOC.
         *
         *  \returns Number of files as \c size_t type.
         *
         */
        size_t numFiles() const;

        /** \brief Returns the nth file in the TOC.
         *
         *  This is file retrieval by index into the list of extracted
         *  records. (E.g. "the first file listed in the TOC file".) Used in
         *  cases where we operate on TOC indices, mainly the file list shown in
         *  the dock widget.
         *
         *  Bounds-checking is performed, and will raise an exception if that
         *  check fails.
         *
         *  \param[in] idx The index into the TOC list.
         *
         *  \param[in] autodecomp Indicates if the file should be automatically
         *                        decompressed. Default is \c true.
         *
         *  \returns The requested file
         *
         *  \exception std::out_of_range The given index goes beyond the number
         *                               of files available.
         *
         *  This function caches results for faster subsequent accesses.
         *
         */
        File fileAtNum(size_t idx, bool autodecomp = true) const;

        /** \brief Returns the file at the specified virtual address.
         *
         *  This is file retrieval by a known beginning virtual address for a
         *  file. Used in cases where, for instance, one file's data references
         *  another via virtual address.
         *
         *  This function is \em not forgiving. The given address \b must be one
         *  that was listed in the TOC as a starting virtual address. This
         *  function will not load from an arbitrary address nor select the
         *  "closest file", or anything else of this nature. An error will be
         *  thrown if the address wasn't listed.
         *
         *  \param[in] addr The given virtual address to load.
         *
         *  \param[in] autodecomp Indicates if the file should be automatically
         *                        decompressed. Default is \c true.
         *
         *  \returns The requested file, if the address is valid.
         *
         *  \exception X::BadIndex The given virtual address doesn't point to
         *                         the start of a file.
         *
         *  This function caches results for faster subsequent accesses.
         *
         */
        File fileAtVAddress(size_t addr, bool autodecomp = true) const;

        /** \brief Returns the file with the given name.
         *
         *  This is file retrieval by a known human-readable name for the
         *  file. This is used cases where a specific file is needed regardless
         *  of its location between versions (e.g. message box tables).
         *
         *  An error will result if the name does not exist, or if a
         *  configuration file with possible file names couldn't be found at
         *  construction time.
         *
         *  \param[in] name The name of the requested file.
         *
         *  \param[in] autodecomp Indicates if the file should be automatically
         *                        decompressed. Default is \c true.
         *
         *  \returns The requested file.
         *
         *  \exception X::NoConfig No configuration file was found for the ROM.
         *
         *  \exception X::BadIndex The given name couldn't be found.
         *
         *  This function caches results for faster subsequent accesses.
         *
         */
        File fileAtName(std::string name, bool autodecomp = true) const;

        /** \brief Returns the Record for the nth TOC entry.
         *
         *  This returns just the record as extracted from TOC, by index into
         *  that TOC.
         *
         *  \param[in] idx Index into the TOC list.
         *
         *  \returns The requested record.
         *
         *  \exception std::out_of_range Index goes outside possible number of
         *                               files.
         *
         */
        Record recordAtNum(size_t idx) const;

        /** \brief Returns the Record for the given starting address
         *
         *  This returns just the record as extracted from the TOC, based on it
         *  having the right starting address. Inexact matches fail, an exact
         *  and precise valid address is needed.
         *
         *  \param[in] addr The starting virtual address.
         *
         *  \returns The requested record.
         *
         *  \exception X::BadIndex The given virtual address doesn't point to
         *                         the start of a file.
         *
         */
        Record recordAtVAddress(size_t addr) const;

        /** \brief Returns the Record for the file with the given name
         *
         *  This returns the record whose file has the known name matching the
         *  given one. This doesn't work if a config file couldn't be found, or
         *  if the given name doesn't exactly match any known ones for the ROM.
         *
         *  \param[in] name The name requested.
         *
         *  \returns The requested record.
         *
         *  \exception X::NoConfig No configuration file was found for the ROM.
         *
         *  \exception X::BadIndex The given name couldn't be found.
         *
         */
        Record recordAtName(std::string name) const;

        /** \brief The size of the ROM file.
         *
         *  This returns the size of the ROM's data as it was passed in, in
         *  bytes.
         *
         *  \returns Size of ROM in bytes.
         *
         */
        size_t size() const;

        /** \brief Returns the internal ROM name
         *
         *  This returns from the standard N64 ROM header the internal name of
         *  the ROM.
         *
         *  \returns The internal ROM name.
         *
         */
        std::string get_rname() const;

        /** \brief Returns the internal ROM code
         *
         *  This returns the 4-character code for the game from the standard N64
         *  ROM header.
         *
         *  \warning This code does \em not uniquely identify Zelda ROMs;
         *           multiple versions can have the same code. Use \c
         *           getVersion() to identify the ROM by version.
         *
         *  \returns The 4-character internal ROM code.
         *
         */
        std::string get_rcode() const;

        /** \brief Returns the raw ROM data as-is.
         *
         *  This gives a copy of the ROM's internal data, useful in cases where
         *  we must save the ROM back to file (e.g. saving an unbyteswapped
         *  version of the ROM).
         *
         *  \returns Byte vector of the ROM data.
         *
         */
        std::vector<uint8_t> getData() const;

        /** \brief Returns the ROM's version
         *
         *  This function returns an enum value representing the ROM's
         *  version. This is determined during the constructor, by examining the
         *  compilation timestamp (see bootstrapCompTime() ).
         *
         *  This is the recommended way to figure out the ROM's version for
         *  special behavior.
         *
         *  \returns A \c Config::Version enum value for the version.
         *
         */
        Config::Version getVersion() const;

        /** \brief Indicates if a configuration key exists in the ROM's config
         *         file.
         *
         *  This is a simple boolean test to see if the given key exists in the
         *  config tree. An error will be raised if there is no config tree in
         *  the first place. Otherwise, the presence of a key will return true,
         *  absence will return false.
         *
         *  This is useful in cases where only the presence of the key matters
         *  and its value is inconsequential, or when behavior has to depend on
         *  whether a key with an important value even exists.
         *
         *  \param[in] parts The "path" to the config key, with the key being
         *                   tested the last element.
         *
         *  \returns \c true if the key exists, or \c false if the key (or any
         *           of the other parts leading up to the key) doesn't exist.
         *
         *  \exception X::BadIndex
         *             There is no configuration file for the ROM, therefore the
         *             result is guaranteed false. You can treat this exception
         *             as "false" if presence of the key doesn't need to depend
         *             on a config file being around in the first place.
         *
         */
        bool hasConfigKey(std::initializer_list<std::string> parts) const;

        /** \brief Returns the value associated with the given config key.
         *
         *  This function will return the string value for the given key,
         *  assuming the given key exists (otherwise returns empty string). Not
         *  having a config file at all will raise an error.
         *
         *  \param[in] parts The "path" to the key whose value is desired.
         *
         *  \returns The string value for the key if it exists, or the null
         *           string otherwise.
         *
         *  \exception X::BadIndex There is no configuration file to read keys
         *                         from in the first place.
         *
         */
        std::string configKey(std::initializer_list<std::string> parts) const;

        /** \brief Returns the two CRC values from the header.
         *
         *  Returns the two 32-bit values in the header that help verify at
         *  least a portion of the ROM.
         *
         *  \returns A \c std::pair holding the two values, in order.
         *
         */
        std::pair<uint32_t, uint32_t> getCRC() const;
    };
}