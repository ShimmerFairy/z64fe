/** \file
 *
 *  \brief Defines a display list class for graphical RCP stuff, and other
 *         associated things.
 *
 */

#pragma once

#include "RCP/Image.hpp"
#include "RCP/CC.hpp"

#include "endian.hpp"
#include "Exceptions.hpp"
#include "Fixed.hpp"

#include <cstdint>
#include <string>
#include <array>
#include <deque>
#include <map>

#include <iostream>

namespace RCP {
    namespace Command {
        class Any {
          public:
            // XXX if we ever get the ability to demand constructors from
            // subclasses, put that here.

            // XXX also, want to demand static function

            virtual std::string id() = 0;

            virtual ~Any() { }
        };

        class NOOP : public Any {
          public:
            uint32_t tag;

            NOOP(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class VTX : public Any {
          public:
            uint8_t size;
            uint8_t dst_idx;
            uint32_t ram_address;

            VTX(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class MODIFYVTX : public Any {
          public:
            enum class Change {
                RGBA     = 0x10,
                ST       = 0x14,
                XYSCREEN = 0x18,
                ZSCREEN  = 0x1C
            };

            Change where;
            uint16_t dst_idx;
            uint32_t value;

            MODIFYVTX(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class CULLDL : public Any {
          public:
            uint16_t begin_idx;
            uint16_t end_idx;

            CULLDL(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class BRANCH_Z : public Any {
          public:
            uint16_t test_idx;
            uint16_t test_idx_2;
            uint32_t z_value;

            BRANCH_Z(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class TRI1 : public Any {
          public:
            std::array<uint8_t, 3> vtx_idxs;

            TRI1(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class TRI2 : public Any {
          public:
            std::array<uint8_t, 6> vtx_idxs;

            TRI2(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        // QUAD will be treated as TRI2 by us. This loss of info shouldn't be
        // bothersome.
        typedef TRI2 QUAD;

        class DMA_IO : public Any {
          public:
            enum class Mode {
                ToRCP,
                FromRCP,
            };

            Mode direction;
            uint16_t dmem_address;
            uint16_t size;
            uint32_t ram_address;

            DMA_IO(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class TEXTURE : public Any {
          public:
            uint8_t extra_mipmaps;
            uint8_t tile_no;
            bool on;
            ufix<0, 16> scale_S;
            ufix<0, 16> scale_T;

            TEXTURE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class POPMTX : public Any {
          public:
            uint32_t pop_num;

            POPMTX(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class GEOMETRYMODE : public Any {
          public:
            uint32_t clear_this;
            uint32_t set_this;

            GEOMETRYMODE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class MTX : public Any {
          public:
            enum class Loading {
                Load,
                Multiply,
            };

            enum class Stack {
                ModelView,
                Projection,
            };

            bool try_push;
            Loading load_style;
            Stack mtx_stack;
            uint32_t ram_address;

            MTX(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class MOVEWORD : public Any {
          public:
            enum class Index {
                Matrix    = 0x00,
                NumLight  = 0x02,
                Clip      = 0x04,
                Segment   = 0x06,
                Fog       = 0x08,
                LightCol  = 0x0A,
                ForceMtx  = 0x0C,
                PerspNorm = 0x0E,
            };

            Index idx;
            uint16_t offset;
            uint32_t value;

            MOVEWORD(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class MOVEMEM : public Any {
          public:
            enum class Index {
                Viewport = 0x08,
                Light    = 0x0A,
                Matrix   = 0x0E,
            };

            uint8_t size;
            uint8_t offset;
            Index idx;
            uint8_t src_address;

            MOVEMEM(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class LOAD_UCODE : public Any {
          public:
            uint16_t data_size;
            uint32_t text_start;

            LOAD_UCODE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class DL : public Any {
          public:
            enum class Style {
                Call,
                Jump,
            };

            Style ret_type;
            uint32_t goto_address;

            DL(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class ENDDL : public Any {
          public:
            ENDDL(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SPNOOP : public Any {
          public:
            SPNOOP(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPHALF_1 : public Any {
          public:
            uint32_t high_word;

            RDPHALF_1(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        // XXX unpack "value" instead
        class SETOTHERMODE_L : public Any {
          public:
            uint8_t shift;
            uint8_t size;
            uint32_t value;

            SETOTHERMODE_L(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        // XXX unpack "value" instead
        class SETOTHERMODE_H : public Any {
          public:
            uint8_t shift;
            uint8_t size;
            uint32_t value;

            SETOTHERMODE_H(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        // note: assumes 64-bit variant only, at least for now.
        class TEXRECT : public Any {
          public:
            ufix<10, 2> lrx;
            ufix<10, 2> lry;
            uint8_t tile_no;
            ufix<10, 2> ulx;
            ufix<10, 2> uly;

            TEXRECT(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class TEXRECTFLIP : public Any {
          public:
            ufix<10, 2> lrx;
            ufix<10, 2> lry;
            uint8_t tile_no;
            ufix<10, 2> ulx;
            ufix<10, 2> uly;

            TEXRECTFLIP(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPLOADSYNC : public Any {
          public:
            RDPLOADSYNC(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPPIPESYNC : public Any {
          public:
            RDPPIPESYNC(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPTILESYNC : public Any {
          public:
            RDPTILESYNC(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPFULLSYNC : public Any {
          public:
            RDPFULLSYNC(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETKEYGB : public Any {
          public:
            ufix<4, 8> width_G;
            ufix<4, 8> width_B;
            uint8_t center_G;
            uint8_t scale_G;
            uint8_t center_B;
            uint8_t scale_B;

            SETKEYGB(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETKEYR : public Any {
          public:
            ufix<4, 8> width_R;
            uint8_t center_R;
            uint8_t scale_R;

            SETKEYR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETCONVERT : public Any {
          public:
            // actually signed 9-bit numbers
            std::array<sfix<9, 0>, 6> k;

            SETCONVERT(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETSCISSOR : public Any {
          public:
            enum class Mode {
                All  = 0,
                Even = 2,
                Odd  = 3,
            };

            ufix<10, 2> ulx;
            ufix<10, 2> uly;
            Mode scanlines;
            ufix<10, 2> lrx;
            ufix<10, 2> lry;

            SETSCISSOR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETPRIMDEPTH : public Any {
          public:
            int16_t Z;
            int16_t delta_Z;

            SETPRIMDEPTH(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPSETOTHERMODE : public Any {
          public:
            uint32_t high_bits;
            uint32_t low_bits;

            RDPSETOTHERMODE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class LOADTLUT : public Any {
          public:
            uint8_t tile_no;
            uint16_t last_color_idx;

            LOADTLUT(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class RDPHALF_2 : public Any {
          public:
            uint32_t low_word;

            RDPHALF_2(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETTILESIZE : public Any {
          public:
            ufix<10, 2> uls; // u10.2
            ufix<10, 2> ult; // u10.2
            uint8_t tile_no;
            ufix<10, 2> lrs; // u10.2
            ufix<10, 2> lrt; // u10.2

            SETTILESIZE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class LOADBLOCK : public Any {
          public:
            ufix<10, 2> uls; // u10.2
            ufix<10, 2> ult; // u10.2
            uint8_t tile_no;
            uint16_t last_texel_idx;
            ufix<1, 11> dxt; // u1.11

            LOADBLOCK(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class LOADTILE : public Any {
          public:
            ufix<10, 2> uls; // u10.2
            ufix<10, 2> ult; // u10.2
            uint8_t tile_no;
            ufix<10, 2> lrs; // u10.2
            ufix<10, 2> lrt; // u10.2

            LOADTILE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETTILE : public Any {
          public:
            Image::Format tile_fmt;
            uint16_t u64_per_row;
            uint16_t tmem_address;
            uint8_t tile_no;
            uint8_t pal_no;

            bool clamp_T;
            bool mirror_T;
            uint8_t mask_T;
            uint8_t shift_T;

            bool clamp_S;
            bool mirror_S;
            uint8_t mask_S;
            uint8_t shift_S;

            SETTILE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class FILLRECT : public Any {
          public:
            ufix<10, 2> lrx; // RCP truncated(?) to ints
            ufix<10, 2> lry; // RCP truncated(?) to ints
            ufix<10, 2> ulx; // RCP truncated(?) to ints
            ufix<10, 2> uly; // RCP truncated(?) to ints

            FILLRECT(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETFILLCOLOR : public Any {
          public:
            uint32_t rawval;
            // rawval is because the meaning drastically changes depending on
            // the situation, so we can't know until later.

            SETFILLCOLOR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETFOGCOLOR : public Any {
          public:
            uint8_t R;
            uint8_t G;
            uint8_t B;
            uint8_t A;

            SETFOGCOLOR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETBLENDCOLOR : public Any {
          public:
            uint8_t R;
            uint8_t G;
            uint8_t B;
            uint8_t A;

            SETBLENDCOLOR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETPRIMCOLOR : public Any {
          public:
            ufix<0, 8> min_lod;  // u0.8
            ufix<0, 8> lod_frac; // u0.8
            uint8_t R;
            uint8_t G;
            uint8_t B;
            uint8_t A;

            SETPRIMCOLOR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETENVCOLOR : public Any {
          public:
            uint8_t R;
            uint8_t G;
            uint8_t B;
            uint8_t A;

            SETENVCOLOR(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETCOMBINE : public Any {
          public:
            CC::ColorA color_1a, color_2a;
            CC::ColorB color_1b, color_2b;
            CC::ColorC color_1c, color_2c;
            CC::ColorD color_1d, color_2d;
            CC::AlphaA alpha_1a, alpha_2a;
            CC::AlphaB alpha_1b, alpha_2b;
            CC::AlphaC alpha_1c, alpha_2c;
            CC::AlphaD alpha_1d, alpha_2d;

            SETCOMBINE(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETTIMG : public Any {
          public:
            Image::Format tile_fmt;
            uint16_t width;
            uint32_t ram_address;

            SETTIMG(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETZIMG : public Any {
          public:
            uint32_t ram_address;

            SETZIMG(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };

        class SETCIMG : public Any {
          public:
            Image::Format tile_fmt;
            uint16_t width;
            uint32_t ram_address;

            SETCIMG(uint64_t instr);

            static std::string sid();
            std::string id() override;
        };
    }

    /** \brief Aid in casting RCP command items
     *
     *  Simply a helper cast-like function for neatly handling casting from a
     *  generic command object to its specific subclass. It's basically a \c
     *  dynamic_cast with built-in error-handling for our specific case.
     *
     *  \tparam T Type to convert to. Type must be a pointer to a Command::Any
     *            subclass, or another type that delegates to a Command::Any
     *            subclass's static methods and which can accept an assignment
     *            of \c nullptr.
     *
     *  \param[in] ap Pointer to some kind of Command object
     *
     *  \returns A dynamically-casted pointer to the specified type if possible,
     *           or a \c nullptr if not possible.
     *
     *  \warning This will fail at compile time if static member function \c
     *           T::sid() is not callable, or if \c T(nullptr) is not a valid
     *           way to construct \c T, or if the given \c Command::Any object
     *           can't be downcasted via \c dynamic_cast to \c T.
     *
     *  \note This function requires the user to explicitly give the type as a
     *        pointer only to maintain consistency with other casting functions.
     *
     */
    template<typename T>
    T command_cast(Command::Any * ap) {
        T res(nullptr);

        if (ap->id() == T::sid()) {
            res = dynamic_cast<T>(ap);
        }

        return res;
    }

    typedef std::deque<Command::Any *> DisplayList;

    // the main reason this is a separate function is to spare the template
    // function below from being prohibitively long, which is important when it
    // comes to template functions that get written in the header.
    Command::Any * parseOneCmd(uint64_t cmd);

    template<typename Iter>
    std::map<size_t, DisplayList> getDLs(Iter begin, Iter end) {
        std::map<size_t, DisplayList> res;

        // first we try to find the latest offset ending in 0x0 or 0x8.
        size_t starting = std::distance(begin, end) - 1;

        while (((starting & 0xF) != 0) && ((starting & 0xF) != 8)) {
            starting--;
        }

        // now to construct an iterator from that.
        Iter ptr = begin + starting;

        // if we can't read a whole instruction here, skip!
        if (std::distance(ptr, end) < 8) {
            ptr -= 8;
        }

        bool indl = false;
        DisplayList dl;

        /* you may be wondering why we're checking for the end iterator if we're
         * moving backwards. Simply put, we'll cheat the backward movement by
         * manually setting ptr to the end iterator after we're done with
         * reading from the beginning. This lets us neatly interpret a command
         * at the very start of the data, while still going backwards.
         *
         * And if you wonder why we don't just use a reverse iterator so we have
         * a real one-past-the-end, it's because we'd have to then copy all the
         * data into another list as 64-bit uints, and use its reverse
         * iterators. Which wouldn't be too bad, but we don't exactly _need_ to
         * duplicate the given data to get the job done.
         */
        while (ptr < end) {
            uint64_t mbcmd = be_u64(ptr);

            if (mbcmd == 0xDF00000000000000uLL) {
                if (indl) {
                    res[std::distance(begin, ptr) + 8] = dl;
                    dl.clear();
                }

                dl.push_front(new Command::ENDDL(mbcmd));
                indl = true;
            } else if (indl) {
                // only do it here so we don't spend our time parsing possible
                // commands every single iteration, even when we know it can't
                // be done.
                Command::Any * thecmd;

                try {
                    thecmd = parseOneCmd(mbcmd);
                } catch (Exception & e) {
                    std::clog << e.what() << "\n";
                    // if an error occurs, at least for now we'll assume it
                    // means it's actually an invalid command
                    thecmd = nullptr;
                }

                // we could also check if the returned command is an ENDDL, but
                // we (hopefully!) already handled that in the first conditional
                // above, so for now at least be lazy and don't check that.

                if (thecmd != nullptr) {
                    dl.push_front(thecmd);
                } else {
                    res[std::distance(begin, ptr) + 8] = dl;
                    dl.clear();
                    indl = false;
                }
            }

            if (ptr == begin) {
                ptr = end;
            } else {
                ptr -= 8;
            }
        }

        return res;
    }
}

namespace X {
    namespace RCP {
        class BadCommand : public Exception {
          private:
            std::string cname;
            std::string extra_bit;
            uint64_t cmd;

          public:
            BadCommand(std::string cn, uint64_t cd, std::string eb = "");

            std::string what() override;
        };
    }
}