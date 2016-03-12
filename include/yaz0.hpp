/** \file yaz0.hpp
 *
 *  \brief Function(s?) to handle Yaz0 compression
 *
 */

#pragma once

#include <cstdint>
#include <vector>

std::vector<uint8_t> yaz0_decompress(std::vector<uint8_t> ciphertxt);