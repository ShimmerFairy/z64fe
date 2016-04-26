/** \file
 *
 *  \brief Stuff for images (textures) in use in the RCP.
 *
 */

#pragma once

#include "Exceptions.hpp"

namespace RCP {
    namespace Image {
        enum class Colors {
            RGBA = 0,
            YUV  = 1,
            CI   = 2,
            IA   = 3,
            I    = 4,
        };

        enum class Size {
            u4  = 0,
            u8  = 1,
            u16 = 2,
            u32 = 3,
        };

        enum class Format {
            RGBA_16,
            RGBA_32,
            YUV_16,
            CI_4,
            CI_8,
            CI_16, // for loading purposes
            IA_4,
            IA_8,
            IA_16,
            I_4,
            I_8,
            I_16, // for loading purposes
        };

        Format getFormat(Colors c, Size s);

        Colors getColors(Format f);
        Size getSize(Format f);
    }
}

namespace X {
    namespace RCP {
        namespace Image {
            class NoSuchFormat : public Exception {
              public:
                std::string what() override;
            };

            class BadFormat : public Exception {
              public:
                std::string what() override;
            };
        }
    }
}