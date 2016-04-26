/** \file endian.hpp
 *
 *  \brief Functions for the wonderful world of endianness!
 *
 */

#pragma once

#include <cstdint>

template<typename ForwardIt>
uint16_t be_u16(const ForwardIt & starting) {
    uint16_t res = 0;
    ForwardIt pnt = starting;

    res |= static_cast<uint16_t>(*pnt++) <<  8;
    res |= static_cast<uint16_t>(*pnt++);

    return res;
}

template<typename ForwardIt>
uint32_t be_u24(const ForwardIt & starting) {
    uint32_t res = 0;
    ForwardIt pnt = starting;

    res |= static_cast<uint32_t>(*pnt++) << 16;
    res |= static_cast<uint32_t>(*pnt++) <<  8;
    res |= static_cast<uint32_t>(*pnt++);

    return res;
}

template<typename ForwardIt>
uint32_t be_u32(const ForwardIt & starting) {
    uint32_t res = 0;
    ForwardIt pnt = starting;

    res |= static_cast<uint32_t>(*pnt++) << 24;
    res |= static_cast<uint32_t>(*pnt++) << 16;
    res |= static_cast<uint32_t>(*pnt++) <<  8;
    res |= static_cast<uint32_t>(*pnt++);

    return res;
}

template <typename ForwardIt>
uint64_t be_u64(const ForwardIt & starting) {
    uint64_t res = 0;
    ForwardIt pnt = starting;

    res |= static_cast<uint64_t>(*pnt++) << 56;
    res |= static_cast<uint64_t>(*pnt++) << 48;
    res |= static_cast<uint64_t>(*pnt++) << 40;
    res |= static_cast<uint64_t>(*pnt++) << 32;
    res |= static_cast<uint64_t>(*pnt++) << 24;
    res |= static_cast<uint64_t>(*pnt++) << 16;
    res |= static_cast<uint64_t>(*pnt++) <<  8;
    res |= static_cast<uint64_t>(*pnt++);

    return res;
}