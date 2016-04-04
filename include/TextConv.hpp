/** \file TextConv.hpp
 *
 *  \brief Functions for converting text in ROMs
 *
 */

#pragma once

#include "TextAST.hpp"

#include <vector>
#include <cstdint>

std::vector<TextAST::Box> readASCII_OoT(std::vector<uint8_t>::iterator & indata);
std::vector<TextAST::Box> readShiftJIS_OoT(std::vector<uint8_t>::iterator & indata);

std::vector<TextAST::Box> readASCII_MM(std::vector<uint8_t>::iterator & indata);
std::vector<TextAST::Box> readShiftJIS_MM(std::vector<uint8_t>::iterator & indata);