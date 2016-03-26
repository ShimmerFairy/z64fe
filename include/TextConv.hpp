/** \file TextConv.hpp
 *
 *  \brief Functions for converting text in ROMs
 *
 */

#pragma once

#include "TextAST.hpp"

#include <vector>
#include <cstdint>

std::vector<TextAST> readASCII_OoT(std::vector<uint8_t>::iterator & indata);
std::vector<TextAST> readShiftJIS_OoT(std::vector<uint8_t>::iterator & indata);