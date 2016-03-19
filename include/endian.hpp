/** \file endian.hpp
 *
 *  \brief Functions for the wonderful world of endianness!
 *
 */

#pragma once

#include <cstdint>

template<typename ForwardIt>
uint32_t be_u16(const ForwardIt & starting) {
    uint16_t res = 0;
    ForwardIt pnt = starting;

    res |= static_cast<uint16_t>(*pnt++) <<  8;
    res |= static_cast<uint16_t>(*pnt++);

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