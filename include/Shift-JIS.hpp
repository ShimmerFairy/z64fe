/** \file Shift-JIS.hpp
 *
 *  \brief Conversion table for Shift-JIS to unicode codepoints
 *
 *  Taken from the list at
 *  http://www.unicode.org/Public/MAPPINGS/OBSOLETE/EASTASIA/JIS/SHIFTJIS.TXT
 *
 *  Note that this converts to 32-bit codepoints, NOT to utf8 or otherwise.
 *
 */

#pragma once

#include <cstdint>
#include <map>

#include "sjtable.inc"