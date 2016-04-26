/** \file
 *
 *  \brief Implement whatever is needed for RCP images.
 *
 */

#include "RCP/Image.hpp"

namespace RCP {
    namespace Image {
        Format getFormat(Colors c, Size s) {
            switch (c) {
              case Colors::RGBA:
                switch (s) {
                  case Size::u16:
                    return Format::RGBA_16;
                    break;

                  case Size::u32:
                    return Format::RGBA_32;
                    break;
                }
                break;

              case Colors::YUV:
                if (s == Size::u16) {
                    return Format::YUV_16;
                }
                break;

              case Colors::CI:
                switch (s) {
                  case Size::u4:
                    return Format::CI_4;
                    break;

                  case Size::u8:
                    return Format::CI_8;
                    break;

                  case Size::u16:
                    return Format::CI_16;
                    break;
                }
                break;

              case Colors::IA:
                switch (s) {
                  case Size::u4:
                    return Format::IA_4;
                    break;

                  case Size::u8:
                    return Format::IA_8;
                    break;

                  case Size::u16:
                    return Format::IA_16;
                    break;
                }
                break;

              case Colors::I:
                switch (s) {
                  case Size::u4:
                    return Format::I_4;
                    break;

                  case Size::u8:
                    return Format::I_8;
                    break;

                  case Size::u16:
                    return Format::I_16;
                    break;
                }
                break;
            }

            throw X::RCP::Image::NoSuchFormat();
        }

        Colors getColors(Format f) {
            switch (f) {
              case Format::RGBA_16:
              case Format::RGBA_32:
                return Colors::RGBA;
                break;

              case Format::YUV_16:
                return Colors::YUV;
                break;

              case Format::CI_4:
              case Format::CI_8:
              case Format::CI_16:
                return Colors::CI;
                break;

              case Format::IA_4:
              case Format::IA_8:
              case Format::IA_16:
                return Colors::IA;
                break;

              case Format::I_4:
              case Format::I_8:
              case Format::I_16:
                return Colors::I;
                break;
            }

            throw X::RCP::Image::BadFormat();
        }

        Size getSize(Format f) {
            switch (f) {
              case Format::CI_4:
              case Format::IA_4:
              case Format::I_4:
                return Size::u4;
                break;

              case Format::CI_8:
              case Format::IA_8:
              case Format::I_8:
                return Size::u8;
                break;

              case Format::RGBA_16:
              case Format::YUV_16:
              case Format::IA_16:
              case Format::CI_16:
              case Format::I_16:
                return Size::u16;
                break;

              case Format::RGBA_32:
                return Size::u32;
                break;
            }

            throw X::RCP::Image::BadFormat();
        }
    }
}

namespace X {
    namespace RCP {
        namespace Image {
            std::string NoSuchFormat::what() {
                return "The given image format is not allowed in the RCP!";
            }

            std::string BadFormat::what() {
                return "Bad format enum given!";
            }
        }
    }
}