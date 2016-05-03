/** \file
 *
 *  \brief Defines the fixed-point numbers class
 *
 *  This is used mainly in the RCP, but could be used elsewhere as
 *  needed. Perhaps could be its own library, even.
 *
 */

#pragma once

#include <gmp.h>
#include <gmpxx.h>

#include <cstddef>
#include <type_traits>

// note: the enable-ifs we use to make sure raw integer types are correctly
// signed or not do _not_ check for the signedness of the type; we're more
// liberal and just check that the given integer isn't negative when it can't
// be. If we checked on types, then you'd have to type literal suffixes all the
// time for unsigned fixnums (e.g. Fixed & 100u), and in general I personally
// think unsuffixed literals shouldn't be rigidly "int" numbers.

/** \brief Class for fixed-point math
 *
 *  This class implements fixed-point math needed in various parts of the
 *  program, e.g. the RCP implementation. Uses the GNU gmp library to allow for
 *  a high number of bits, and for ease of implementation (we'd have to do some
 *  of the same stuff gmp does were we not to use it, and that stuff is
 *  difficult and annoying).
 *
 *  Note that we only use the C++ interface for streaming operators, since using
 *  the C structs/functions directly gives us more control (e.g. setting the
 *  initial bitsize of the member mpz item).
 *
 */
template<bool SIGNED, size_t I, size_t F>
class Fixed {
  private:
    mpz_t thenum;

  public:
    // make all Fixed classes friends with each other
    template<bool TS, size_t TI, size_t TF>
    friend class Fixed;

    Fixed() {
        mpz_init2(thenum, I + F);
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed(const T & rawval, const bool & isint = false) {
        mpz_init2(thenum, I + F);

        if (SIGNED) {
            mpz_set_si(thenum, rawval);
        } else {
            mpz_set_ui(thenum, rawval);
        }

        if (isint) {
            mpz_mul_2exp(thenum, thenum, F);
        }
    }

    Fixed(mpz_t rawmprval) {
        mpz_init2(thenum, I + F);
        mpz_set(thenum, rawmprval);
    }

    template<bool TS, size_t TI, size_t TF,
             typename std::enable_if<!((TS == SIGNED) && (I == TI) && (F == TF))>::type* = nullptr>
    Fixed(const Fixed<TS, TI, TF> & that) {
        mpz_init2(thenum, I + F);

        mpz_set(thenum, that.thenum);

        // now we want to make sure to un-negate the number if needed,
        // two's-complement style (i.e. a reinterpret cast).

        if (!SIGNED && TS && mpz_cmp_si(thenum, 0) == -1) {
            mpz_com(thenum, thenum);
            mpz_add_ui(thenum, thenum, 1);
        }

        // now to adjust to the right format
        if (TF < F) {
            mpz_mul_2exp(thenum, thenum, F - TF);
        } else if (TF > F) {
            mpz_fdiv_q_2exp(thenum, thenum, TF - F);
        }

        mpz_t amask;
        mpz_init(amask);
        mpz_set(amask, 1);
        mpz_mul_2exp(amask, amask, I + F);
        mpz_sub_ui(amask, amask, 1);

        mpz_and(thenum, thenum, amask);
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator=(const T & rval) {
        *this = Fixed(rval, false);
        return *this;
    }

    Fixed(const Fixed & that) {
        mpz_init2(thenum, I + F);
        mpz_set(thenum, that.thenum);
    }

    Fixed & operator=(const Fixed & that) {
        if (this != &that) {
            mpz_set(thenum, that.thenum);
        }

        return *this;
    }

    ~Fixed() { mpz_clear(thenum); }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    T getRawVal() const {
        if (SIGNED) {
            return mpz_get_si(thenum);
        } else {
            return mpz_get_ui(thenum);
        }
    }

    Fixed & operator+=(const Fixed & that) {
        mpz_add(thenum, thenum, that.thenum);

        // modulo the result in case of overflow
        if (mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::max()) > 0
            || mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::min()) < 0) {
            mpz_tdiv_r_2exp(thenum, thenum, I + F);
        }

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator+=(const T & oval) {
        return *this += Fixed(oval, true);
    }

    Fixed & operator-=(const Fixed & that) {
        mpz_sub(thenum, thenum, that.thenum);

        // modulo the result in case of overflow
        if (mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::max()) > 0
            || mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::min()) < 0) {
            mpz_tdiv_r_2exp(thenum, thenum, I + F);
        }

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator-=(const T & oval) {
        return *this -= Fixed(oval, true);
    }

    Fixed & operator*=(const Fixed & that) {
        // the use of bigints in this class means the multiply won't lose any
        // precision by nature of being done.
        mpz_mul(thenum, thenum, that.thenum);

        // now to chop off the unnecessary fraction and integer bits.
        mpz_fdiv_q_2exp(thenum, thenum, F);

        // modulo the result in case of overflow
        if (mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::max()) > 0
            || mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::min()) < 0) {
            mpz_tdiv_r_2exp(thenum, thenum, I + F);
        }

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator*=(const T & oval) {
        return *this *= Fixed(oval, true);
    }

    Fixed & operator/=(const Fixed & that) {
        // we have to shift our left operand leftwards beforehand, to counter
        // the natural truncation to happen from the division operation (if we
        // decided to shift the result, then we'd lose all our fractional bits
        // in the division).
        mpz_mul_2exp(thenum, thenum, F);

        // we do truncating division here, analogous to integer / (and analogous
        // to the idea of doing this in infinite precision and then simply
        // ignoring the far-too-precise digits)
        mpz_tdiv_q(thenum, thenum, that.thenum);

        // we won't do any overflow handling here, since that _should_ be
        // impossible; we only expect to happen with extra fraction bits, which
        // are however automatically taken out by how we do division.

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator/=(const T & oval) {
        return *this /= Fixed(oval, true);
    }

    Fixed & operator%=(const Fixed & that) {
        mpz_mul_2exp(thenum, thenum, F);

        mpz_tdiv_r(thenum, thenum, that.thenum);

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator%=(const T & oval) {
        return *this %= Fixed(oval, true);
    }

    Fixed & operator&=(const Fixed & that) {
        mpz_and(thenum, thenum, that.thenum);

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator&=(const T & oval) {
        return *this &= Fixed(oval, false);
    }

    Fixed & operator|=(const Fixed & that) {
        mpz_ior(thenum, thenum, that.thenum);

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator|=(const T & oval) {
        return *this |= Fixed(oval, false);
    }

    Fixed & operator^=(const Fixed & that) {
        mpz_xor(thenum, thenum, that.thenum);

        return *this;
    }

    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator^=(const T & oval) {
        return *this ^= Fixed(oval, false);
    }

    Fixed & operator<<=(const size_t & amt) {
        mpz_mul_2exp(thenum, thenum, amt);

        return *this;
    }

    Fixed & operator >>=(const size_t & amt) {
        mpz_fdiv_q_2exp(thenum, thenum, amt);

        return *this;
    }

    Fixed operator~() const {
        Fixed res(*this);

        mpz_com(res.thenum, res.thenum);

        return *this;
    }

    Fixed operator-() const {
        Fixed res(*this);

        mpz_neg(res.thenum, res.thenum);

        return *this;
    }

    // these done as friend operators just to avoid typing out template
    // parameters for Fixed

    // also, these done with a macro to avoid repetition

#define FRIENDOP(op, flag)                                              \
    friend Fixed operator op (const Fixed & a, const Fixed & b) {       \
        Fixed res(a);                                                   \
        res op##= b;                                                    \
        return res;                                                     \
    }                                                                   \
    template<typename T,                                                \
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr> \
    friend Fixed operator op (const Fixed & a, const T & b) {           \
        Fixed res(a);                                                   \
        res op##= b;                                                    \
        return res;                                                     \
    }                                                                   \
    template<typename T,                                                \
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr> \
    friend Fixed operator op (const T & a, const Fixed & b) {           \
        Fixed res(a, flag);                                             \
        res op##= b;                                                    \
        return res;                                                     \
    }

    FRIENDOP(+, true)
    FRIENDOP(-, true)
    FRIENDOP(*, true)
    FRIENDOP(/, true)
    FRIENDOP(%, true)
    FRIENDOP(&, false)
    FRIENDOP(|, false)
    FRIENDOP(^, false)

#undef FRIENDOP

    friend Fixed operator<<(const Fixed & a, const size_t & b) {
        Fixed res(a);
        res <<= b;
        return res;
    }

    friend Fixed operator>>(const Fixed & a, const size_t & b) {
        Fixed res(a);
        res >>= b;
        return res;
    }

    // for use in boolean contexts
    explicit operator bool() const {
        return mpz_cmp_ui(thenum, 0) != 0;
    }

    // relational ops
    bool operator==(const Fixed & that) const {
        return mpz_cmp(thenum, that.thenum) == 0;
    }

    bool operator<(const Fixed & that) const {
        return mpz_cmp(thenum, that.thenum) == -1;
    }

    bool operator>(const Fixed & that) const {
        return mpz_cmp(thenum, that.thenum) == 1;
    }

    bool operator!=(const Fixed & that) const {
        return !(*this == that);
    }

    bool operator<=(const Fixed & that) const {
        return !(*this > that);
    }

    bool operator>=(const Fixed & that) const {
        return !(*this < that);
    }
};

template<size_t I, size_t F>
using ufix = Fixed<false, I, F>;

template<size_t I, size_t F>
using sfix = Fixed<true, I, F>;

// specialization for Fixed types

namespace std {
    template<bool FS, size_t FI, size_t FF>
    class numeric_limits<Fixed<FS, FI, FF>> {
      public:
        static constexpr bool is_specialized           = true;
        static constexpr bool is_signed                = FS;
        static constexpr bool is_integer               = (FF == 0);
        static constexpr bool is_exact                 = is_integer;
        static constexpr bool has_infinity             = false;
        static constexpr bool has_quiet_NaN            = false;
        static constexpr bool has_signaling_NaN        = false;
        static constexpr float_denorm_style has_denorm = denorm_absent;
        static constexpr bool has_denorm_loss          = false;
        static constexpr float_round_style round_style = round_toward_zero;
        static constexpr bool is_iec559                = false;
        static constexpr bool is_bounded               = true;
        static constexpr bool is_modulo                = true;
        static constexpr int digits                    = FI + FF;
        static constexpr int digits10                  = digits * log10(2);
        static constexpr int max_digits10              = 0;
        static constexpr int radix                     = 2;

        static constexpr Fixed<FS, FI, FF> min() {
            if (FS == false) {
                return Fixed<FS, FI, FF>();
            }

            mpz_t res;
            mpz_init(res);
            mpz_set(res, 1);

            mpz_mul_2exp(res, res, FI + FF);
            mpz_neg(res, res);

            return Fixed<FS, FI, FF>(res);
        }

        static constexpr Fixed<FS, FI, FF> lowest() { return min(); }

        static constexpr Fixed<FS, FI, FF> max() {
            mpz_t res;
            mpz_init(res);
            mpz_set(res, 1);

            mpz_mul_2exp(res, res, FI + FF);
            mpz_sub_ui(res, res, 1);

            return Fixed<FS, FI, FF>(res);
        }

        // note: the standard says this is only meaningful if not an integer,
        // but we'll define this in a meaningful way even if you're using an
        // integer fixnum. Because there's no reason this can't be defined for
        // integers, it's only a bit obvious.
        static constexpr Fixed<FS, FI, FF> epsilon() {
            return Fixed<FS, FI, FF>(1);
        }

        static constexpr Fixed<FS, FI, FF> round_error() {
            if (!is_integer) {
                return Fixed<FS, FI, FF>(1, true);
            } else {
                return Fixed<FS, FI, FF>(0);
            }
        }
    };
}