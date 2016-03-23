/** \file utility.hpp
 *
 *  \brief Various utility functions for our purposes.
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

std::string sizeToIEC(size_t inbytes);

std::string code_to_utf8(uint32_t codep);