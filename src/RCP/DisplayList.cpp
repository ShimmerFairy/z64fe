/** \file
 *
 *  \brief Implements all the RCP command classes.
 *
 */

#include "RCP/DisplayList.hpp"

// used for ensuring we got the right opcode in each constructor.
#include <cassert>

#include <algorithm>
#include <sstream>
#include <iomanip>

namespace RCP {
    namespace Command {
        NOOP::NOOP(uint64_t instr) {
            assert(instr >> 56 == 0x00);

            tag = instr & 0xFFFFFFFF;
        }

        std::string NOOP::sid() { return "G_NOOP"; }
        std::string NOOP::id() { return sid(); }




        VTX::VTX(uint64_t instr) {
            assert(instr >> 56 == 0x01);

            size = instr >> (32 + 12) & 0xFF;
            dst_idx = ((instr >> 32 & 0xFF) >> 1) - size;
            ram_address = instr & 0xFFFFFFFF;

            //// VALIDITY CHECK
            // 1. Make sure size and dst_idx are in range
            if (!(1 <= size && size <= 32)) {
                throw X::RCP::BadCommand(sid(), instr, "Size out of range");
            }

            if (!(dst_idx <= 31)) {
                throw X::RCP::BadCommand(sid(), instr, "Destination index out of range");
            }

            // 2. Check that destination index + size don't exceed 32 vertices.
            if (size + dst_idx > 32) {
                throw X::RCP::BadCommand(sid(), instr, "Attempting to load vertices past endpoint");
            }

            // 3. Check segment address for valid segment
            if ((ram_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid segment for segment address");
            }
        }

        std::string VTX::sid() { return "G_VTX"; }
        std::string VTX::id() { return sid(); }




        MODIFYVTX::MODIFYVTX(uint64_t instr) {
            assert(instr >> 56 == 0x02);

            switch (instr >> (32 + 16) & 0xFF) {
              case 0x10:
                where = Change::RGBA;
                break;

              case 0x14:
                where = Change::ST;
                break;

              case 0x18:
                where = Change::XYSCREEN;
                break;

              case 0x1C:
                where = Change::ZSCREEN;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Invalid modification index");
                break;
            }

            dst_idx = (instr >> 32 & 0xFFFF) / 2;
            value = instr & 0xFFFFFFFF;

            //// VALIDITY CHECK
            if (!(dst_idx <= 31)) {
                throw X::RCP::BadCommand(sid(), instr, "Bad vertex index");
            }
        }

        std::string MODIFYVTX::sid() { return "G_MODIFYVTX"; }
        std::string MODIFYVTX::id() { return sid(); }




        CULLDL::CULLDL(uint64_t instr) {
            assert(instr >> 56 == 0x03);

            begin_idx = (instr >> 32 & 0xFFFF) / 2;
            end_idx = (instr & 0xFFFF) / 2;

            //// VALIDITY CHECK
            // 1. Check indexes
            if (!(begin_idx <= 31)) {
                throw X::RCP::BadCommand(sid(), instr, "Bad start index");
            }

            if (!(end_idx <= 31)) {
                throw X::RCP::BadCommand(sid(), instr, "Bad end index");
            }

            // 2. check that the indices specify a real list
            if (!(begin_idx < end_idx)) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid vertex list given");
            }
        }

        std::string CULLDL::sid() { return "G_CULLDL"; }
        std::string CULLDL::id() { return sid(); }




        BRANCH_Z::BRANCH_Z(uint64_t instr) {
            assert(instr >> 56 == 0x04);

            test_idx = (instr >> (32 + 12) & 0xFFF) / 5;
            test_idx_2 = (instr >> 32 & 0xFFF) / 2;

            if (test_idx != test_idx_2) {
                throw X::RCP::BadCommand(sid(), instr, "Given indices aren't the same");
            }

            z_value = instr & 0xFFFFFFFF;
        }

        std::string BRANCH_Z::sid() { return "G_BRANCH_Z"; }
        std::string BRANCH_Z::id() { return sid(); }




        TRI1::TRI1(uint64_t instr) {
            assert(instr >> 56 == 0x05);

            vtx_idxs[0] = (instr >> (32 + 16) & 0xFF) / 2;
            vtx_idxs[1] = (instr >> (32 +  8) & 0xFF) / 2;
            vtx_idxs[2] = (instr >>  32       & 0xFF) / 2;

            //// VALIDITY CHECK
            for (auto & i : vtx_idxs) {
                if (!(i <= 31)) {
                    throw X::RCP::BadCommand(sid(), instr, "A vertex index was out of range");
                }
            }
        }

        std::string TRI1::sid() { return "G_TRI1"; }
        std::string TRI1::id() { return sid(); }




        TRI2::TRI2(uint64_t instr) {
            // check two of them since we subsume QUAD into this
            assert(instr >> 56 == 0x06 || instr >> 56 == 0x07);

            vtx_idxs[0] = (instr >> (32 + 16) & 0xFF) / 2;
            vtx_idxs[1] = (instr >> (32 +  8) & 0xFF) / 2;
            vtx_idxs[2] = (instr >>  32       & 0xFF) / 2;
            vtx_idxs[3] = (instr >>  16       & 0xFF) / 2;
            vtx_idxs[4] = (instr >>   8       & 0xFF) / 2;
            vtx_idxs[5] = (instr              & 0xFF) / 2;

            for (auto & i : vtx_idxs) {
                if (!(i <= 31)) {
                    throw X::RCP::BadCommand(sid(), instr, "A vertex index was out of range");
                }
            }
        }

        std::string TRI2::sid() { return "G_TRI2"; }
        std::string TRI2::id() { return sid(); }




        DMA_IO::DMA_IO(uint64_t instr) {
            assert(instr >> 56 == 0xD6);

            if (instr >> (32 + 23) & 1) {
                direction = Mode::ToRCP;
            } else {
                direction = Mode::FromRCP;
            }

            dmem_address = (instr >> (32 + 13) & 0x3FF) * 8;
            size = (instr >> 32 & 0xFFF) + 1;
            ram_address = instr & 0xFFFFFFFF;

            //// VALIDITY CHECK
            if ((ram_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid segment for segment address");
            }
        }

        std::string DMA_IO::sid() { return "G_DMA_IO"; }
        std::string DMA_IO::id() { return sid(); }




        TEXTURE::TEXTURE(uint64_t instr) {
            assert(instr >> 56 == 0xD7);

            extra_mipmaps = instr >> (32 + 11) & 0x07;
            tile_no = instr >> (32 + 8) & 0x07;
            on = instr >> 32 & 0xFF;

            scale_S = instr >> 16 & 0xFFFF;
            scale_T = instr & 0xFFFF;

            //// VALIDITY CHECK
            // make sure it's not asking for more mipmaps than possible for this
            // tile descriptor.
            if (tile_no + extra_mipmaps > 0x07) {
                throw X::RCP::BadCommand(sid(), instr, "Too many levels for tile descriptor");
            }
        }

        std::string TEXTURE::sid() { return "G_TEXTURE"; }
        std::string TEXTURE::id() { return sid(); }




        POPMTX::POPMTX(uint64_t instr) {
            assert(instr >> 56 == 0xD8);

            pop_num = (instr & 0xFFFFFFFF) / 64;

            //// VALIDITY CHECK
            // not asking for too many matrices?
            if (pop_num > 18) {
                throw X::RCP::BadCommand(sid(), instr, "Asked to pop too many matrices");
            }
        }

        std::string POPMTX::sid() { return "G_POPMTX"; }
        std::string POPMTX::id() { return sid(); }




        GEOMETRYMODE::GEOMETRYMODE(uint64_t instr) {
            assert(instr >> 56 == 0xD9);

            clear_this = instr >> 32 & 0xFFFFFF;
            clear_this = ~clear_this; // do this after so we know we only invert 32 bits

            set_this = instr & 0xFFFFFFFF;
        }

        std::string GEOMETRYMODE::sid() { return "G_GEOMETRYMODE"; }
        std::string GEOMETRYMODE::id() { return sid(); }




        MTX::MTX(uint64_t instr) {
            assert(instr >> 56 == 0xDA);

            // the push bit is inverted for some reason, so true is false and
            // vice versa.
            try_push = !(instr >> 32 & 1);

            if (instr >> 33 & 1) {
                load_style = Loading::Load;
            } else {
                load_style = Loading::Multiply;
            }

            if (instr >> 34 & 1) {
                mtx_stack = Stack::Projection;
            } else {
                mtx_stack = Stack::ModelView;
            }

            ram_address = instr & 0xFFFFFFFF;

            //// VALIDITY CHECK
            if ((ram_address >> 24) > 0xF) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid segment for segment address");
            }
        }

        std::string MTX::sid() { return "G_MTX"; }
        std::string MTX::id() { return sid(); }




        MOVEWORD::MOVEWORD(uint64_t instr) {
            assert(instr >> 56 == 0xDB);

            switch (instr >> (32 + 16) & 0xFF) {
              case 0x00:
                idx = Index::Matrix;
                break;

              case 0x02:
                idx = Index::NumLight;
                break;

              case 0x04:
                idx = Index::Clip;
                break;

              case 0x06:
                idx = Index::Segment;
                break;

              case 0x08:
                idx = Index::Fog;
                break;

              case 0x0A:
                idx = Index::LightCol;
                break;

              case 0x0C:
                idx = Index::ForceMtx;
                break;

              case 0x0E:
                idx = Index::PerspNorm;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Invalid DMA index");
            }

            offset = instr >> 32 & 0xFFFF;
            value = instr & 0xFFFFFFFF;
        }

        std::string MOVEWORD::sid() { return "G_MOVEWORD"; }
        std::string MOVEWORD::id() { return sid(); }




        MOVEMEM::MOVEMEM(uint64_t instr) {
            assert(instr >> 56 == 0xDC);

            size = ((instr >> (32 + 16) & 0xFF) >> 3) * 8 + 1;
            offset = instr >> (32 + 8) & 0xFF;
            switch (instr >> 32 & 0xFF) {
              case 0x08:
                idx = Index::Viewport;
                break;

              case 0x0A:
                idx = Index::Light;
                break;

              case 0x0E:
                idx = Index::Matrix;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Invalid or unsupported DMA index");
                break;
            }
            src_address = instr & 0xFFFFFFFF;

            if ((src_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid segment for segment address");
            }
        }

        std::string MOVEMEM::sid() { return "G_MOVEMEM"; }
        std::string MOVEMEM::id() { return sid(); }




        LOAD_UCODE::LOAD_UCODE(uint64_t instr) {
            assert(instr >> 56 == 0xDD);

            data_size = instr >> 32 & 0xFFFF;
            text_start = instr & 0xFFFFFFFF;

            if ((text_start >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid segment for segment address");
            }
        }

        std::string LOAD_UCODE::sid() { return "G_LOAD_UCODE"; }
        std::string LOAD_UCODE::id() { return sid(); }




        DL::DL(uint64_t instr) {
            assert(instr >> 56 == 0xDE);

            switch (instr >> (32 + 16) & 0xFF) {
              case 0x00:
                ret_type = Style::Call;
                break;

              case 0x01:
                ret_type = Style::Jump;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Invalid call style for DL command");
                break;
            }

            goto_address = instr & 0xFFFFFFFF;

            if ((goto_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Invalid segment for segment address");
            }
        }

        std::string DL::sid() { return "G_DL"; }
        std::string DL::id() { return sid(); }



        ENDDL::ENDDL(uint64_t instr) { assert(instr >> 56 == 0xDF); }
        std::string ENDDL::sid() { return "G_ENDDL"; }
        std::string ENDDL::id() { return sid(); }




        SPNOOP::SPNOOP(uint64_t instr) { assert(instr >> 56 == 0xE0); }
        std::string SPNOOP::sid() { return "G_SPNOOP"; }
        std::string SPNOOP::id() { return sid(); }




        RDPHALF_1::RDPHALF_1(uint64_t instr) {
            assert(instr >> 56 == 0xE1);

            high_word = instr & 0xFFFFFFFF;
        }

        std::string RDPHALF_1::sid() { return "G_RDPHALF_1"; }
        std::string RDPHALF_1::id()  { return sid(); }




        SETOTHERMODE_L::SETOTHERMODE_L(uint64_t instr) {
            assert(instr >> 56 == 0xE2);

            size = (instr >> 32 & 0xFF) + 1;
            shift = 32 - (instr >> (32 + 8) & 0xFF) - size;
            value = instr & 0xFFFFFFFF;

            // weird validity check to do here, just see if the given value is
            // shifted as expected.

            if ((value & (((1 << size) - 1) << shift)) != value) {
                throw X::RCP::BadCommand(sid(), instr, "Odd value given to set");
            }
        }

        std::string SETOTHERMODE_L::sid() { return "G_SETOTHERMODE_L"; }
        std::string SETOTHERMODE_L::id() { return sid(); }




        SETOTHERMODE_H::SETOTHERMODE_H(uint64_t instr) {
            assert(instr >> 56 == 0xE3);

            size = (instr >> 32 & 0xFF) + 1;
            shift = 32 - (instr >> (32 + 8) & 0xFF) - size;
            value = instr & 0xFFFFFFFF;

            // weird validity check to do here, just see if the given value is
            // shifted as expected.

            if ((value & (((1 << size) - 1) << shift)) != value) {
                throw X::RCP::BadCommand(sid(), instr, "Odd value given to set");
            }
        }

        std::string SETOTHERMODE_H::sid() { return "G_SETOTHERMODE_H"; }
        std::string SETOTHERMODE_H::id() { return sid(); }




        TEXRECT::TEXRECT(uint64_t instr) {
            assert(instr >> 56 == 0xE4);

            lrx = instr >> (32 + 12) & 0xFFF;
            lry = instr >> 32 & 0xFFF;

            tile_no = instr >> 24 & 0x7;

            ulx = instr >> 12 & 0xFFF;
            uly = instr & 0xFFF;
        }

        std::string TEXRECT::sid() { return "G_TEXRECT"; }
        std::string TEXRECT::id() { return sid(); }




        TEXRECTFLIP::TEXRECTFLIP(uint64_t instr) {
            assert(instr >> 56 == 0xE5);

            lrx = instr >> (32 + 12) & 0xFFF;
            lry = instr >> 32 & 0xFFF;

            tile_no = instr >> 24 & 0x7;

            ulx = instr >> 12 & 0xFFF;
            uly = instr & 0xFFF;
        }

        std::string TEXRECTFLIP::sid() { return "G_TEXRECTFLIP"; }
        std::string TEXRECTFLIP::id() { return sid(); }




        RDPLOADSYNC::RDPLOADSYNC(uint64_t instr) { assert(instr >> 56 == 0xE6); }
        std::string RDPLOADSYNC::sid() { return "G_RDPLOADSYNC"; }
        std::string RDPLOADSYNC::id() { return sid(); }




        RDPPIPESYNC::RDPPIPESYNC(uint64_t instr) { assert(instr >> 56 == 0xE7); }
        std::string RDPPIPESYNC::sid() { return "G_RDPPIPESYNC"; }
        std::string RDPPIPESYNC::id() { return sid(); }




        RDPTILESYNC::RDPTILESYNC(uint64_t instr) { assert(instr >> 56 == 0xE8); }
        std::string RDPTILESYNC::sid() { return "G_RDPTILESYNC"; }
        std::string RDPTILESYNC::id() { return sid(); }




        RDPFULLSYNC::RDPFULLSYNC(uint64_t instr) { assert(instr >> 56 == 0xE9); }
        std::string RDPFULLSYNC::sid() { return "G_RDPFULLSYNC"; }
        std::string RDPFULLSYNC::id() { return sid(); }




        SETKEYGB::SETKEYGB(uint64_t instr) {
            assert(instr >> 56 == 0xEA);

            width_G = instr >> (32 + 12) & 0xFFF;
            width_B = instr >> 32 & 0xFFF;

            center_G = instr >> 24 & 0xFF;
            scale_G  = instr >> 16 & 0xFF;

            center_B = instr >> 8 & 0xFF;
            scale_B  = instr & 0xFF;
        }

        std::string SETKEYGB::sid() { return "G_SETKEYGB"; }
        std::string SETKEYGB::id() { return sid(); }




        SETKEYR::SETKEYR(uint64_t instr) {
            assert(instr >> 56 == 0xEB);

            width_R  = instr >> 16 & 0xFFF;
            center_R = instr >> 8 & 0xFF;
            scale_R  = instr & 0xFF;
        }

        std::string SETKEYR::sid() { return "G_SETKEYR"; }
        std::string SETKEYR::id() { return sid(); }




        SETCONVERT::SETCONVERT(uint64_t instr) {
            assert(instr >> 56 == 0xEC);

            std::array<uint16_t, 6> prek;
            prek[0] = instr >> (9 * 5) & 0x1FF;
            prek[1] = instr >> (9 * 4) & 0x1FF;
            prek[2] = instr >> (9 * 3) & 0x1FF;
            prek[3] = instr >> (9 * 2) & 0x1FF;
            prek[4] = instr >> (9 * 1) & 0x1FF;
            prek[5] = instr            & 0x1FF;

            // now to handle doing the sign extension, in a standards-compliant
            // way (not accidentally relying on UB or implementation-defined
            // behavior).
            std::transform(prek.begin(), prek.end(), k.begin(),
                           [](const uint16_t & a) {
                               int16_t res;

                               if (a >> 8) {
                                   // decomplement, then re-negate, in a way
                                   // that won't stumble across UB or i-d
                                   // behavior.
                                   res = 512 - a;
                                   res = -res;
                               } else {
                                   // this one's easy
                                   res = a;
                               }

                               return res;
                           });
        }

        std::string SETCONVERT::sid() { return "G_SETCONVERT"; }
        std::string SETCONVERT::id() { return sid(); }




        SETSCISSOR::SETSCISSOR(uint64_t instr) {
            assert(instr >> 56 == 0xED);

            ulx = instr >> (32 + 12) & 0xFFF;
            uly = instr >> 32 & 0xFFF;

            switch (instr >> 28 & 0x0F) {
              case 0x00:
                scanlines = Mode::All;
                break;

              case 0x02:
                scanlines = Mode::Even;
                break;

              case 0x03:
                scanlines = Mode::Odd;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Invalid scanline mode value");
                break;
            }

            lrx = instr >> 12 & 0xFFF;
            lry = instr & 0xFFF;
        }

        std::string SETSCISSOR::sid() { return "G_SETSCISSOR"; }
        std::string SETSCISSOR::id() { return sid(); }




        SETPRIMDEPTH::SETPRIMDEPTH(uint64_t instr) {
            assert(instr >> 56 == 0xEE);

            // we handle the signed numbers like this to make sure we store
            // things in a standards-compliant way (since we can't guarantee
            // that your system uses two's-complement, even though that means
            // your system has issues, so reinterpret_cast is out. static_cast
            // is out because the standard won't specify how to handle a uint
            // with bigger value than the destination int, so hoping it'll do
            // the correct 2's-comp magic like with signed->unsigned is folly).
            uint16_t uZ = instr >> 16 & 0xFFFF;
            uint16_t udZ = instr & 0xFFFF;

            if (uZ >> 15) {
                Z = 0x10000 - uZ;
                Z = -Z;
            } else {
                Z = uZ;
            }

            if (udZ >> 15) {
                delta_Z = 0x10000 - udZ;
                delta_Z = -delta_Z;
            } else {
                delta_Z = udZ;
            }
        }

        std::string SETPRIMDEPTH::sid() { return "G_SETPRIMDEPTH"; }
        std::string SETPRIMDEPTH::id() { return sid(); }




        RDPSETOTHERMODE::RDPSETOTHERMODE(uint64_t instr) {
            assert(instr >> 56 == 0xEF);

            high_bits = instr >> 32 && 0xFFFFFF;
            low_bits  = instr & 0xFFFFFFFF;
        }

        std::string RDPSETOTHERMODE::sid() { return "G_RDPSETOTHERMODE"; }
        std::string RDPSETOTHERMODE::id() { return sid(); }




        LOADTLUT::LOADTLUT(uint64_t instr) {
            assert(instr >> 56 == 0xF0);

            tile_no = instr >> 24 & 0x7;
            last_color_idx = (instr >> 12 & 0xFFF) >> 2;

            if (last_color_idx > 0xFF) {
                throw X::RCP::BadCommand(sid(), instr, "Too many colors requested");
            }
        }

        std::string LOADTLUT::sid() { return "G_LOADTLUT"; }
        std::string LOADTLUT::id() { return sid(); }




        RDPHALF_2::RDPHALF_2(uint64_t instr) {
            assert(instr >> 56 == 0xF1);

            low_word = instr & 0xFFFFFFFF;
        }

        std::string RDPHALF_2::sid() { return "G_RDPHALF2"; }
        std::string RDPHALF_2::id() { return sid(); }




        SETTILESIZE::SETTILESIZE(uint64_t instr) {
            assert(instr >> 56 == 0xF2);

            uls = instr >> (32 + 12) & 0xFFF;
            ult = instr >> 32 & 0xFFF;
            tile_no = instr >> 24 & 0x7;
            lrs = instr >> 12 & 0xFFF;
            lrt = instr & 0xFFF;
        }

        std::string SETTILESIZE::sid() { return "G_SETTILESIZE"; }
        std::string SETTILESIZE::id() { return sid(); }




        LOADBLOCK::LOADBLOCK(uint64_t instr) {
            assert(instr >> 56 == 0xF3);

            uls = instr >> (32 + 12) & 0xFFF;
            ult = instr >> 32 & 0xFFF;
            tile_no = instr >> 24 & 0x7;
            last_texel_idx = instr >> 12 & 0xFFF;
            dxt = instr & 0xFFF;
        }

        std::string LOADBLOCK::sid() { return "G_LOADBLOCK"; }
        std::string LOADBLOCK::id() { return sid(); }




        LOADTILE::LOADTILE(uint64_t instr) {
            assert(instr >> 56 == 0xF4);

            uls = instr >> (32 + 12) & 0xFFF;
            ult = instr >> 32 & 0xFFF;
            tile_no = instr >> 24 & 0x7;
            lrs = instr >> 12 & 0xFFF;
            lrt = instr & 0xFFF;
        }

        std::string LOADTILE::sid() { return "G_LOADTILE"; }
        std::string LOADTILE::id() { return sid(); }




        SETTILE::SETTILE(uint64_t instr) {
            assert(instr >> 56 == 0xF5);

            Image::Colors icl;
            Image::Size isz;

            switch (instr >> (32 + 21) & 0x7) {
              case 0x00:
                icl = Image::Colors::RGBA;
                break;

              case 0x01:
                icl = Image::Colors::YUV;
                break;

              case 0x02:
                icl = Image::Colors::CI;
                break;

              case 0x03:
                icl = Image::Colors::IA;
                break;

              case 0x04:
                icl = Image::Colors::I;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Impossible color format given");
                break;
            }

            switch (instr >> (32 + 19) & 0x3) {
              case 0x00:
                isz = Image::Size::u4;
                break;

              case 0x01:
                isz = Image::Size::u8;
                break;

              case 0x02:
                isz = Image::Size::u16;
                break;

              case 0x03:
                isz = Image::Size::u32;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Impossible format size given");
            }

            tile_fmt = getFormat(icl, isz);

            u64_per_row = instr >> (32 + 9) & 0x1FF;
            tmem_address = instr >> 32 & 0x1FF;

            tile_no = instr >> 24 & 0x7;

            pal_no = instr >> 20 & 0xF;

            clamp_T  = instr >> 19 & 0x1;
            mirror_T = instr >> 18 & 0x1;
            mask_T   = instr >> 14 & 0xF;
            shift_T  = instr >> 10 & 0xF;

            clamp_S  = instr >>  9 & 0x1;
            mirror_S = instr >>  8 & 0x1;
            mask_S   = instr >>  4 & 0xF;
            shift_S  = instr >>  4 & 0xF;
        }




        std::string SETTILE::sid() { return "G_SETTILE"; }
        std::string SETTILE::id() { return sid(); }




        FILLRECT::FILLRECT(uint64_t instr) {
            assert(instr >> 56 == 0xF6);

            lrx = instr >> (32 + 12) & 0xFFF;
            lry = instr >> 32 & 0xFFF;
            ulx = instr >> 12 & 0xFFF;
            uly = instr & 0xFFF;

            //// VALIDITY CHECK, make sure all these 10.2 numbers are integers
            if ((lrx & 0xFFC) != lrx || (lry & 0xFFC) != lry
             || (ulx & 0xFFC) != ulx || (uly & 0xFFC) != uly) {
                throw X::RCP::BadCommand(sid(), instr, "Coordinates not all integers.");
            }
        }

        std::string FILLRECT::sid() { return "G_FILLRECT"; }
        std::string FILLRECT::id() { return sid(); }




        SETFILLCOLOR::SETFILLCOLOR(uint64_t instr) {
            assert(instr >> 56 == 0xF7);

            rawval = instr & 0xFFFFFFFF;
        }

        std::string SETFILLCOLOR::sid() { return "G_SETFILLCOLOR"; }
        std::string SETFILLCOLOR::id() { return sid(); }




        SETFOGCOLOR::SETFOGCOLOR(uint64_t instr) {
            assert(instr >> 56 == 0xF8);

            R = instr >> 24 & 0xFF;
            G = instr >> 16 & 0xFF;
            B = instr >>  8 & 0xFF;
            A = instr       & 0xFF;
        }

        std::string SETFOGCOLOR::sid() { return "G_SETFOGCOLOR"; }
        std::string SETFOGCOLOR::id() { return sid(); }




        SETBLENDCOLOR::SETBLENDCOLOR(uint64_t instr) {
            assert(instr >> 56 == 0xF9);

            R = instr >> 24 & 0xFF;
            G = instr >> 16 & 0xFF;
            B = instr >>  8 & 0xFF;
            A = instr       & 0xFF;
        }

        std::string SETBLENDCOLOR::sid() { return "G_SETBLENDCOLOR"; }
        std::string SETBLENDCOLOR::id() { return sid(); }




        SETPRIMCOLOR::SETPRIMCOLOR(uint64_t instr) {
            assert(instr >> 56 == 0xFA);

            min_lod = instr >> (32 + 8) & 0xFF;
            lod_frac = instr >> 32 & 0xFF;

            R = instr >> 24 & 0xFF;
            G = instr >> 16 & 0xFF;
            B = instr >>  8 & 0xFF;
            A = instr       & 0xFF;
        }

        std::string SETPRIMCOLOR::sid() { return "G_SETPRIMCOLOR"; }
        std::string SETPRIMCOLOR::id() { return sid(); }




        SETENVCOLOR::SETENVCOLOR(uint64_t instr) {
            assert(instr >> 56 == 0xFB);

            R = instr >> 24 & 0xFF;
            G = instr >> 16 & 0xFF;
            B = instr >>  8 & 0xFF;
            A = instr       & 0xFF;
        }

        std::string SETENVCOLOR::sid() { return "G_SETENVCOLOR"; }
        std::string SETENVCOLOR::id() { return sid(); }




        SETCOMBINE::SETCOMBINE(uint64_t instr) {
            assert(instr >> 56 == 0xFC);

            // this is the only place we static_cast enum values, despite
            // potentially leading to unnamed values, since writing it all out
            // would be prohibitively insane.

            color_1a = static_cast<CC::ColorA>(std::min(instr >> (32 + 20) &  0xF,  0x08uL));
            color_1c = static_cast<CC::ColorC>(std::min(instr >> (32 + 15) & 0x1F,  0x10uL));

            alpha_1a = static_cast<CC::AlphaA>(std::min(instr >> (32 + 12) &  0x7,  0x07uL));
            alpha_1c = static_cast<CC::AlphaC>(std::min(instr >> (32 +  9) &  0x7,  0x07uL));

            color_2a = static_cast<CC::ColorA>(std::min(instr >> (32 +  5) &  0xF,  0x08uL));
            color_2c = static_cast<CC::ColorC>(std::min(instr >> 32        & 0x1F,  0x10uL));

            color_1b = static_cast<CC::ColorB>(std::min(instr >> 28        &  0xF,  0x08uL));
            color_2b = static_cast<CC::ColorB>(std::min(instr >> 24        &  0xF,  0x08uL));

            alpha_2a = static_cast<CC::AlphaA>(std::min(instr >> 21        &  0x7,  0x07uL));
            alpha_2c = static_cast<CC::AlphaC>(std::min(instr >> 18        &  0x7,  0x07uL));

            color_1d = static_cast<CC::ColorD>(std::min(instr >> 15        &  0x7,  0x07uL));

            alpha_1b = static_cast<CC::AlphaB>(std::min(instr >> 12        &  0x7,  0x07uL));
            alpha_1d = static_cast<CC::AlphaD>(std::min(instr >>  9        &  0x7,  0x07uL));

            color_2d = static_cast<CC::ColorD>(std::min(instr >>  6        &  0x7,  0x07uL));

            alpha_2b = static_cast<CC::AlphaB>(std::min(instr >>  3        &  0x7,  0x07uL));
            alpha_2d = static_cast<CC::AlphaD>(std::min(instr              &  0x7,  0x07uL));
        }

        std::string SETCOMBINE::sid() { return "G_SETCOMBINE"; }
        std::string SETCOMBINE::id() { return sid(); }




        SETTIMG::SETTIMG(uint64_t instr) {
            assert(instr >> 56 == 0xFD);

            Image::Colors icl;
            Image::Size isz;

            switch (instr >> (32 + 21) & 0x7) {
              case 0x00:
                icl = Image::Colors::RGBA;
                break;

              case 0x01:
                icl = Image::Colors::YUV;
                break;

              case 0x02:
                icl = Image::Colors::CI;
                break;

              case 0x03:
                icl = Image::Colors::IA;
                break;

              case 0x04:
                icl = Image::Colors::I;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Impossible color format given");
                break;
            }

            switch (instr >> (32 + 19) & 0x3) {
              case 0x00:
                isz = Image::Size::u4;
                break;

              case 0x01:
                isz = Image::Size::u8;
                break;

              case 0x02:
                isz = Image::Size::u16;
                break;

              case 0x03:
                isz = Image::Size::u32;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Impossible format size given");
            }

            tile_fmt    = getFormat(icl, isz);
            width       = (instr >> 32 & 0xFFF) + 1;
            ram_address = instr & 0xFFFFFFFF;

            if ((ram_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Bad segment for segment address");
            }
        }

        std::string SETTIMG::sid() { return "G_SETTIMG"; }
        std::string SETTIMG::id() { return sid(); }




        SETZIMG::SETZIMG(uint64_t instr) {
            assert(instr >> 56 == 0xFE);

            ram_address = instr & 0xFFFFFFFF;

            if ((ram_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Bad segment for segment address");
            }
        }

        std::string SETZIMG::sid() { return "G_SETZIMG"; }
        std::string SETZIMG::id() { return id(); }




        SETCIMG::SETCIMG(uint64_t instr) {
            assert(instr >> 56 == 0xFF);

            Image::Colors icl;
            Image::Size isz;

            switch (instr >> (32 + 21) & 0x7) {
              case 0x00:
                icl = Image::Colors::RGBA;
                break;

              case 0x01:
                icl = Image::Colors::YUV;
                break;

              case 0x02:
                icl = Image::Colors::CI;
                break;

              case 0x03:
                icl = Image::Colors::IA;
                break;

              case 0x04:
                icl = Image::Colors::I;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Impossible color format given");
                break;
            }

            switch (instr >> (32 + 19) & 0x3) {
              case 0x00:
                isz = Image::Size::u4;
                break;

              case 0x01:
                isz = Image::Size::u8;
                break;

              case 0x02:
                isz = Image::Size::u16;
                break;

              case 0x03:
                isz = Image::Size::u32;
                break;

              default:
                throw X::RCP::BadCommand(sid(), instr, "Impossible format size given");
            }

            tile_fmt    = getFormat(icl, isz);
            width       = (instr >> 32 & 0xFFF) + 1;
            ram_address = instr & 0xFFFFFFFF;

            if ((ram_address >> 24) > 0x0F) {
                throw X::RCP::BadCommand(sid(), instr, "Bad segment for segment address");
            }
        }

        std::string SETCIMG::sid() { return "G_SETCIMG"; }
        std::string SETCIMG::id() { return sid(); }
    }

    Command::Any * parseOneCmd(uint64_t cmd) {
        // we use bitmasking to match commands so we have a better
        // chance of weeding out false positives. Mask out the variable
        // parts of a command, see if it still matches.
        if ((cmd & ~0x00000000FFFFFFFFuLL) == 0x0000000000000000uLL) {
            return new Command::NOOP(cmd);

        } else if ((cmd & ~0x000FF0FFFFFFFFFFuLL) == 0x0100000000000000uLL) {
            return new Command::VTX(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0x0200000000000000uLL) {
            return new Command::MODIFYVTX(cmd);

        } else if ((cmd & ~0x0000FFFF0000FFFFuLL) == 0x0300000000000000uLL) {
            return new Command::CULLDL(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0x0400000000000000uLL) {
            return new Command::BRANCH_Z(cmd);

        } else if ((cmd & ~0x00FFFFFF00000000uLL) == 0x0500000000000000uLL) {
            return new Command::TRI1(cmd);

        } else if ((cmd & ~0x00FFFFFF00FFFFFFuLL) == 0x0600000000000000uLL) {
            return new Command::TRI2(cmd);

        } else if ((cmd & ~0x00FFFFFF00FFFFFFuLL) == 0x0700000000000000uLL) {
            return new Command::QUAD(cmd);

        } else if ((cmd & ~0x00FFEFFFFFFFFFFFuLL) == 0xD600000000000000uLL) {
            return new Command::DMA_IO(cmd);

        } else if ((cmd & ~0x00003FFFFFFFFFFFuLL) == 0xD700000000000000uLL) {
            return new Command::TEXTURE(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xD838000200000000uLL) {
            return new Command::POPMTX(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0xD900000000000000uLL) {
            return new Command::GEOMETRYMODE(cmd);

        } else if ((cmd & ~0x000000FFFFFFFFFFuLL) == 0xDA38000000000000uLL) {
            return new Command::MTX(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0xDB00000000000000uLL) {
            return new Command::MOVEWORD(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0xDC00000000000000uLL) {
            return new Command::MOVEMEM(cmd);

        } else if ((cmd & ~0x0000FFFFFFFFFFFFuLL) == 0xDD00000000000000uLL) {
            return new Command::LOAD_UCODE(cmd);

        } else if ((cmd & ~0x00FF0000FFFFFFFFuLL) == 0xDE00000000000000uLL) {
            return new Command::DL(cmd);
        } else if (cmd == 0xDF00000000000000uLL) {
            return new Command::ENDDL(cmd);
        } else if (cmd == 0xE000000000000000uLL) {
            return new Command::SPNOOP(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xE100000000000000uLL) {
            return new Command::RDPHALF_1(cmd);

        } else if ((cmd & ~0x0000FFFFFFFFFFFFuLL) == 0xE200000000000000uLL) {
            return new Command::SETOTHERMODE_L(cmd);

        } else if ((cmd & ~0x0000FFFFFFFFFFFFuLL) == 0xE300000000000000uLL) {
            return new Command::SETOTHERMODE_H(cmd);

        } else if ((cmd & ~0x00FFFFFF07FFFFFFuLL) == 0xE400000000000000uLL) {
            return new Command::TEXRECT(cmd);

        } else if ((cmd & ~0x00FFFFFF07FFFFFFuLL) == 0xE500000000000000uLL) {
            return new Command::TEXRECTFLIP(cmd);

        } else if (cmd == 0xE600000000000000uLL) {
            return new Command::RDPLOADSYNC(cmd);

        } else if (cmd == 0xE700000000000000uLL) {
            return new Command::RDPPIPESYNC(cmd);

        } else if (cmd == 0xE800000000000000uLL) {
            return new Command::RDPTILESYNC(cmd);

        } else if (cmd == 0xE900000000000000uLL) {
            return new Command::RDPFULLSYNC(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0xEA00000000000000uLL) {
            return new Command::SETKEYGB(cmd);

        } else if ((cmd & ~0x000000000FFFFFFFuLL) == 0xEB00000000000000uLL) {
            return new Command::SETKEYR(cmd);

        } else if ((cmd & ~0x003FFFFFFFFFFFFFuLL) == 0xEC00000000000000uLL) {
            return new Command::SETCONVERT(cmd);

        } else if ((cmd & ~0x00FFFFFFF0FFFFFFuLL) == 0xED00000000000000uLL) {
            return new Command::SETSCISSOR(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xEE00000000000000uLL) {
            return new Command::SETPRIMDEPTH(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0xEF00000000000000uLL) {
            return new Command::RDPSETOTHERMODE(cmd);

        } else if ((cmd & ~0x0000000007FFF000uLL) == 0xF000000000000000uLL) {
            return new Command::LOADTLUT(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xF100000000000000uLL) {
            return new Command::RDPHALF_2(cmd);

        } else if ((cmd & ~0x00FFFFFF07FFFFFFuLL) == 0xF200000000000000uLL) {
            return new Command::SETTILESIZE(cmd);

        } else if ((cmd & ~0x00FFFFFF07FFFFFFuLL) == 0xF300000000000000uLL) {
            return new Command::LOADBLOCK(cmd);

        } else if ((cmd & ~0x00FFFFFF07FFFFFFuLL) == 0xF400000000000000uLL) {
            return new Command::LOADTILE(cmd);

        } else if ((cmd & ~0x00FBFFFF07FFFFFFuLL) == 0xF500000000000000uLL) {
            return new Command::SETTILE(cmd);

        } else if ((cmd & ~0x00FFFFFF00FFFFFFuLL) == 0xF600000000000000uLL) {
            return new Command::FILLRECT(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xF700000000000000uLL) {
            return new Command::SETFILLCOLOR(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xF800000000000000uLL) {
            return new Command::SETFOGCOLOR(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xF900000000000000uLL) {
            return new Command::SETBLENDCOLOR(cmd);

        } else if ((cmd & ~0x0000FFFFFFFFFFFFuLL) == 0xFA00000000000000uLL) {
            return new Command::SETPRIMCOLOR(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xFB00000000000000uLL) {
            return new Command::SETENVCOLOR(cmd);

        } else if ((cmd & ~0x00FFFFFFFFFFFFFFuLL) == 0xFC00000000000000uLL) {
            return new Command::SETCOMBINE(cmd);

        } else if ((cmd & ~0x00F80FFFFFFFFFFFuLL) == 0xFD00000000000000uLL) {
            return new Command::SETTIMG(cmd);

        } else if ((cmd & ~0x00000000FFFFFFFFuLL) == 0xFE00000000000000uLL) {
            return new Command::SETZIMG(cmd);

        } else if ((cmd & ~0x00F80FFFFFFFFFFFuLL) == 0xFF00000000000000uLL) {
            return new Command::SETCIMG(cmd);

        } else {
            return nullptr;
        }
    }
}

namespace X {
    namespace RCP {
        BadCommand::BadCommand(std::string cn, uint64_t cd, std::string eb) : cname(cn),
                                                                              extra_bit(eb),
                                                                              cmd(cd) { }

        std::string BadCommand::what() {
            std::stringstream res;

            res << "Can't read potential \"" << cname << "\" command";

            res << " (0x" << std::hex << std::uppercase << std::setfill('0')
                << std::setw(16) << cmd << ")";

            if (extra_bit != "") {
                res << ": " << extra_bit;
            }

            return res.str();
        }
    }
}