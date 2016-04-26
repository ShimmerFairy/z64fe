/** \file
 *
 *  \brief Declare Color Combiner things.
 *
 */

#pragma once

namespace RCP {
    namespace CC {
        enum class ColorA {
            COMBINED    = 0x00,
            TEXEL0      = 0x01,
            TEXEL1      = 0x02,
            PRIMITIVE   = 0x03,
            SHADE       = 0x04,
            ENVIRONMENT = 0x05,
            ONE         = 0x06,
            NOISE       = 0x07,
            ZERO        = 0x08,
        };

        enum class ColorB {
            COMBINED    = 0x00,
            TEXEL0      = 0x01,
            TEXEL1      = 0x02,
            PRIMITIVE   = 0x03,
            SHADE       = 0x04,
            ENVIRONMENT = 0x05,
            CENTER      = 0x06,
            K4          = 0x07,
            ZERO        = 0x08,
        };

        enum class ColorC {
            COMBINED        = 0x00,
            TEXEL0          = 0x01,
            TEXEL1          = 0x02,
            PRIMITIVE       = 0x03,
            SHADE           = 0x04,
            ENVIRONMENT     = 0x05,
            SCALE           = 0x06,
            COMBINED_ALPHA  = 0x07,
            TEXEL0_ALPHA    = 0x08,
            TEXEL1_ALPHA    = 0x09,
            PRIMITIVE_ALPHA = 0x0A,
            SHADE_ALPHA     = 0x0B,
            ENV_ALPHA       = 0x0C,
            LOD_FRACTION    = 0x0D,
            PRIM_LOD_FRAC   = 0x0E,
            K5              = 0x0F,
            ZERO            = 0x10,
        };

        enum class ColorD {
            COMBINED    = 0x00,
            TEXEL0      = 0x01,
            TEXEL1      = 0x02,
            PRIMITIVE   = 0x03,
            SHADE       = 0x04,
            ENVIRONMENT = 0x05,
            ONE         = 0x06,
            ZERO        = 0x07,
        };

        enum class AlphaA {
            COMBINED    = 0x00,
            TEXEL0      = 0x01,
            TEXEL1      = 0x02,
            PRIMITIVE   = 0x03,
            SHADE       = 0x04,
            ENVIRONMENT = 0x05,
            ONE         = 0x06,
            ZERO        = 0x07,
        };

        typedef AlphaA AlphaB;
        typedef AlphaA AlphaD;

        enum class AlphaC {
            LOD_FRACTION  = 0x00,
            TEXEL0        = 0x01,
            TEXEL1        = 0x02,
            PRIMITIVE     = 0x03,
            SHADE         = 0x04,
            ENVIRONMENT   = 0x05,
            PRIM_LOD_FRAC = 0x06,
            ZERO          = 0x07,
        };
    }
}