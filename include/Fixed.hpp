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
 *  \tparam SIGNED indicates if this is a signed or unsigned fixed-point number.
 *
 *  \tparam I The number of bits for the integral part. May be zero, in which
 *            case the fixnum can't reach 1.0
 *
 *  \tparam F The number of bits for the fractional part. May be zero, in which
 *            case the fixum is an integral type.
 *
 *  \warning At least currently, overflow on signed integers is not guaranteed
 *           (just like with standard C++). Behavior may be unexpected for your
 *           purposes.
 *
 */
template<bool SIGNED, size_t I, size_t F>
class Fixed {
  private:
    mpz_t thenum; ///< The bigint we use to store the number

  public:
    // make all Fixed classes friends with each other
    template<bool TS, size_t TI, size_t TF> friend class Fixed;

    /** \brief Zero-initializing constructor
     *
     *  This constructor, naturally enough, initializes the fixed-point number
     *  to zero.
     *
     */
    Fixed() {
        mpz_init2(thenum, I + F);
    }

    /** \brief Construct fixed-point number from integer type.
     *
     *  This constructor takes an integral type value and turns it into a
     *  fixed-point number. The optional flag parameter indicates if the given
     *  value is meant to be interpreted as an integer, or as raw data to
     *  initialize the value with.
     *
     *  If interpreted as an integer, the value is appropriately shifted so the
     *  fixed-point number is mathematically equal to the given value.
     *
     *  If interpreted as raw data, the integer is presumed to encode the number
     *  in written order; that is, the F fraction bits are the F least
     *  significant bits, and the next I bits are the integral bits. Any
     *  remaining bits in the number are ignored.
     *
     *  \tparam T the type of the value to construct from. Must be of integral
     *            type (as detected by the \c std::is_integral class).
     *
     *  \param[in] rawval The value to construct the fixnum from. May be
     *                    interpreted as an integral or as raw data.
     *
     *  \param[in] isint Flag indicating how to interpret the data. If true,
     *                   data is an integer, if false data is raw. Optional, and
     *                   false by default.
     *
     *  \warning Results are undefined if an unsigned fixnum is given a negative
     *           number here.
     *
     */
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

    /** \brief Construct from existing gmp integer.
     *
     *  This constructor takes a premade mpz_t integer and copies it. Only meant
     *  for the numeric_limits specialization, is not guaranteed to last.
     *
     *  \param[in] rawmprval The mpz_t integer to copy.
     *
     */
    Fixed(mpz_t rawmprval) {
        mpz_init2(thenum, I + F);
        mpz_set(thenum, rawmprval);
    }

    /** \brief Constructor using a value of a different fixed-point type
     *
     *  This constructor takes a fixed-point number whose type is different to
     *  that of the destination type, and takes on its value as precisely as it
     *  can. If the destination type has too few integer and/or fraction bits,
     *  precision will be lost. (Too-precise fraction bits will disappear, as
     *  well as integer bits which are too great in magnitude).
     *
     *  \tparam TS Whether the source type is signed or not.
     *
     *  \tparam TI Integer bit count of source type.
     *
     *  \tparam TF Fraction bit count of source type.
     *
     *  Note that these template parameters will be deduced from the Fixed type
     *  given as argument.
     *
     *  \param[in] that Value to copy to this number.
     *
     */
    template<bool TS, size_t TI, size_t TF,
             typename std::enable_if<!((TS == SIGNED) && (I == TI) && (F == TF))>::type* = nullptr>
    Fixed(const Fixed<TS, TI, TF> & that) {
        mpz_init2(thenum, I + F);

        mpz_set(thenum, that.thenum);

        // now we want to make sure to un-negate the number if needed,
        // two's-complement style (i.e. a reinterpret cast).

        if (!SIGNED && TS && mpz_cmp_si(thenum, 0) < 0) {
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
        mpz_set_ui(amask, 1);
        mpz_mul_2exp(amask, amask, I + F);
        mpz_sub_ui(amask, amask, 1);

        mpz_and(thenum, thenum, amask);
    }

    /** \brief Raw value assignment operator
     *
     *  Assigns the value given to the Fixed number. The given value is always
     *  interpreted as raw data (i.e. the value constructor with the default
     *  false option).
     *
     *  \tparam T the type of the value on the right-hand side. Must be integral
     *            (as detected by \c std::is_integral).
     *
     *  \param[in] rval The raw value to assign to this object.
     *
     *  \returns A reference to the assigned-to object.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator=(const T & rval) {
        *this = Fixed(rval, false);
        return *this;
    }

    /** \brief Copy constructor
     */
    Fixed(const Fixed & that) {
        mpz_init2(thenum, I + F);
        mpz_set(thenum, that.thenum);
    }

    /** \brief Copy assignment operator
     */
    Fixed & operator=(const Fixed & that) {
        if (this != &that) {
            mpz_set(thenum, that.thenum);
        }

        return *this;
    }

    /** \brief Destructor
     */
    ~Fixed() { mpz_clear(thenum); }

    /** \brief Return number as raw data
     *
     *  This returns the number stored internally as raw data. If the number
     *  stored within can fit into an <tt>unsigned long</tt> or <tt>signed
     *  long</tt> as well as the chosen return type, then the return value can
     *  be passed to the Fixed(const T &) constructor to construct the same
     *  value again.
     *
     *  If this is an unsigned Fixed class, then the returned number will be
     *  truncated to the size of an <tt>unsigned long</tt>, as that's the return
     *  type of gmp's retrieval function for unsigned numbers. For signed
     *  numbers, this is <tt>signed long</tt>. Further truncation may happen in
     *  the conversion to type \c T.
     *
     *  \tparam T The type of value to return, must be integral.
     *
     *  \returns The value of the fixed-point number as raw data, truncated in
     *           magnitude to the smaller of <tt>(un)signed long</tt> or \c T.
     *
     *  \warning Further truncation from <tt>signed long</tt> to \c T cannot be
     *           guaranteed, as per the standard. Conversion of a negative value
     *           to an unsigned type \c T also cannot be guaranteed.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    T getRawVal() const {
        if (SIGNED) {
            return mpz_get_si(thenum);
        } else {
            return mpz_get_ui(thenum);
        }
    }

    /** \brief Addition-assignment operator against another Fixed of same type.
     *
     *  Adds the given fixed-point number to the Fixed object on the left,
     *  performing modulo arithmetic as needed to fit in the specified range.
     *
     *  \param[in] that Another Fixed number of the same type as the destination
     *                  number.
     *
     *  \returns A reference to the modified Fixed number.
     *
     *  \warning Overflow behavior is undefined for signed Fixed
     *           classes. Current behavior will not necessarily be kept in the
     *           future.
     *
     */
    Fixed & operator+=(const Fixed & that) {
        mpz_add(thenum, thenum, that.thenum);

        // modulo the result in case of overflow
        if (mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::max()) > 0
            || mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::min()) < 0) {
            mpz_tdiv_r_2exp(thenum, thenum, I + F);
        }

        return *this;
    }

    /** \brief Addition-assignment operator with integral type
     *
     *  Converts the given value on the right to an integer of the same Fixed
     *  type of the value on the left, and adds them together.
     *
     *  \tparam T type of the value given on the right. Must be integral, as
     *            stated by \c std::is_integral.
     *
     *  \param[in] oval The value to interpret as an integer.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As an arithmetic operation, the value on the right is always
     *        interpreted as an integer, \em not as raw data.
     *
     *  \warning Behavior is undefined if overflow occurs on signed Fixed
     *           classes.
     *
     *  \warning Behavior is undefined if the Fixed object on the left is of
     *           unsigned type and the value on the right is a negative number.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator+=(const T & oval) {
        return *this += Fixed(oval, true);
    }

    /** \brief Subtraction-assignment operator with same-typed Fixed value.
     *
     *  Does subtraction with the two values on either side, and stores the
     *  result in the Fixed on the left. Modulo arithmetic is done in the case
     *  of overflow.
     *
     *  \param[in] that Value to subtract from the left-hand side value.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \warning Behavior is undefined for overflow on signed Fixed classes.
     *
     */
    Fixed & operator-=(const Fixed & that) {
        mpz_sub(thenum, thenum, that.thenum);

        // modulo the result in case of overflow
        if (mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::max()) > 0
            || mpz_cmp_ui(thenum, std::numeric_limits<Fixed>::min()) < 0) {
            mpz_tdiv_r_2exp(thenum, thenum, I + F);
        }

        return *this;
    }

    /** \brief Subtraction-assignment operator with integral type
     *
     *  Converts the given value on the right to an integer of the same Fixed
     *  type of the value on the left, and subtracts them.
     *
     *  \tparam T type of the value given on the right. Must be integral, as
     *            stated by \c std::is_integral.
     *
     *  \param[in] oval The value to interpret as an integer.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As an arithmetic operation, the value on the right is always
     *        interpreted as an integer, \em not as raw data.
     *
     *  \warning Behavior is undefined if overflow occurs on signed Fixed
     *           classes.
     *
     *  \warning Behavior is undefined if the Fixed object on the left is of
     *           unsigned type and the value on the right is a negative number.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator-=(const T & oval) {
        return *this -= Fixed(oval, true);
    }

    /** \brief Multiplication-assignment operator with same-typed Fixed number.
     *
     *  Multiplies the two given values, and stores the result in the left-hand
     *  value. Modulo arithmetic is performed in the case of overflow.
     *
     *  \param[in] that The value to multiply the left-hand side against.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \warning Behavior is undefined in the case of overflow on signed Fixed
     *           classes.
     *
     */
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

    /** \brief Multiplication-assignment operator with integral type
     *
     *  Converts the given value on the right to an integer of the same Fixed
     *  type of the value on the left, and multiplies them together.
     *
     *  \tparam T type of the value given on the right. Must be integral, as
     *            stated by \c std::is_integral.
     *
     *  \param[in] oval The value to interpret as an integer.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As an arithmetic operation, the value on the right is always
     *        interpreted as an integer, \em not as raw data.
     *
     *  \warning Behavior is undefined if overflow occurs on signed Fixed
     *           classes.
     *
     *  \warning Behavior is undefined if the Fixed object on the left is of
     *           unsigned type and the value on the right is a negative number.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator*=(const T & oval) {
        return *this *= Fixed(oval, true);
    }

    /** \brief Division-assignment operator against value of same Fixed type.
     *
     *  Divides the left-hand side by the right-hand side and stores it in the
     *  left-hand side. Truncation division is performed, i.e. the same kind
     *  done between two integers in standard C++. Note that modulo arithmetic
     *  is \em not currently performed, since values should never exceed the
     *  allowed magnitude of the class after a division operation.
     *
     *  \param[in] that Value to divide the left by.
     *
     *  \returns A reference to the modified Fixed object.
     *
     */
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

    /** \brief Division-assignment operator against an integral value.
     *
     *  Converts the right-hand side to a Fixed object and divides the left-hand
     *  side by it.
     *
     *  \tparam T The type of the value on the right, must be integral.
     *
     *  \param[in] oval The value to divide the left by.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As an arithmetic operation, the item on the right is always
     *        interpreted as an integer, \em not as raw data.
     *
     *  \warning Behavior is undefined if the left-hand side is an unsigned
     *           Fixed type, and the right-hand side is a negative number.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator/=(const T & oval) {
        return *this /= Fixed(oval, true);
    }

    /** \brief Modulo-assignment operator against value of same Fixed type.
     *
     *  Divides the left-hand side by the right-hand side and stores the
     *  remainder in the left-hand side. Truncation division is performed to
     *  arrive at the remainder, i.e. the same kind done between two integers in
     *  standard C++. Note that no corrections are made on the result, since we
     *  don't expect values from this operation to overflow.
     *
     *  \param[in] that Value to divide the left by.
     *
     *  \returns A reference to the modified Fixed object.
     *
     */
    Fixed & operator%=(const Fixed & that) {
        mpz_mul_2exp(thenum, thenum, F);

        mpz_tdiv_r(thenum, thenum, that.thenum);

        return *this;
    }

    /** \brief Modulo-assignment operator against an integral value.
     *
     *  Converts the right-hand side to a Fixed object and divides the left-hand
     *  side by it to get the remainder.
     *
     *  \tparam T The type of the value on the right, must be integral.
     *
     *  \param[in] oval The value to divide the left by for the integral.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As an arithmetic operation, the item on the right is always
     *        interpreted as an integer, \em not as raw data.
     *
     *  \warning Behavior is undefined if the left-hand side is an unsigned
     *           Fixed type, and the right-hand side is a negative number.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator%=(const T & oval) {
        return *this %= Fixed(oval, true);
    }

    /** \brief AND-assignment operator with another Fixed object.
     *
     *  Does a bitwise AND between the two Fixed values and stores the result in
     *  the left-hand side.
     *
     *  \param[in] that The value to AND the left side against.
     *
     *  \returns A reference to the modified Fixed object.
     *
     */
    Fixed & operator&=(const Fixed & that) {
        mpz_and(thenum, thenum, that.thenum);

        return *this;
    }

    /** \brief AND-assignment operator with an integral value.
     *
     *  Does a bitwise AND between the Fixed object and the integral value
     *  interpreted as raw data, and stores the result in the Fixed object.
     *
     *  \tparam T type of the right-hand value. Must be integral.
     *
     *  \param[in] oval The value to AND against the fixed value.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As a bitwise operation, the right-hand value is always interpreted
     *        as a raw fixed-point value, \em not as an integer.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator&=(const T & oval) {
        return *this &= Fixed(oval, false);
    }

    /** \brief OR-assignment operator with another Fixed object.
     *
     *  Does a bitwise OR between the two Fixed values and stores the result in
     *  the left-hand side.
     *
     *  \param[in] that The value to OR the left side against.
     *
     *  \returns A reference to the modified Fixed object.
     *
     */
    Fixed & operator|=(const Fixed & that) {
        mpz_ior(thenum, thenum, that.thenum);

        return *this;
    }

    /** \brief OR-assignment operator with an integral value.
     *
     *  Does a bitwise OR between the Fixed object and the integral value
     *  interpreted as raw data, and stores the result in the Fixed object.
     *
     *  \tparam T type of the right-hand value. Must be integral.
     *
     *  \param[in] oval The value to OR against the fixed value.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As a bitwise operation, the right-hand value is always interpreted
     *        as a raw fixed-point value, \em not as an integer.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator|=(const T & oval) {
        return *this |= Fixed(oval, false);
    }

    /** \brief XOR-assignment operator with another Fixed object.
     *
     *  Does a bitwise XOR between the two Fixed values and stores the result in
     *  the left-hand side.
     *
     *  \param[in] that The value to XOR the left side against.
     *
     *  \returns A reference to the modified Fixed object.
     *
     */
    Fixed & operator^=(const Fixed & that) {
        mpz_xor(thenum, thenum, that.thenum);

        return *this;
    }

    /** \brief XOR-assignment operator with an integral value.
     *
     *  Does a bitwise XOR between the Fixed object and the integral value
     *  interpreted as raw data, and stores the result in the Fixed object.
     *
     *  \tparam T type of the right-hand value. Must be integral.
     *
     *  \param[in] oval The value to XOR against the fixed value.
     *
     *  \returns A reference to the modified Fixed object.
     *
     *  \note As a bitwise operation, the right-hand value is always interpreted
     *        as a raw fixed-point value, \em not as an integer.
     *
     */
    template<typename T,
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    Fixed & operator^=(const T & oval) {
        return *this ^= Fixed(oval, false);
    }

    /** \brief Left shift assignment operator
     *
     *   This operator does a left shift of the Fixed object by the number of
     *   bits specified on the right-hand side, and stores that in the Fixed
     *   object.
     *
     *   \param[in] amt The number of bits to shift leftwards by.
     *
     *   \returns A reference to the modified Fixed object.
     *
     */
    Fixed & operator<<=(const size_t & amt) {
        mpz_mul_2exp(thenum, thenum, amt);

        return *this;
    }

    /** \brief Right shift assignment operator
     *
     *   This operator does an \b arithmetic (i.e. sign-extended) right shift of
     *   the Fixed object by the number of bits specified on the right-hand
     *   side, and stores that in the Fixed object.
     *
     *   \param[in] amt The number of bits to shift leftwards by.
     *
     *   \returns A reference to the modified Fixed object.
     *
     */
    Fixed & operator >>=(const size_t & amt) {
        mpz_fdiv_q_2exp(thenum, thenum, amt);

        return *this;
    }

    /** \brief Inverts the bits of the number
     *
     *  This prefix operator returns a \em copy of the number with each bit
     *  inverted.
     *
     *  \returns A new Fixed object representing the bitwise complement of the
     *           number.
     *
     */
    Fixed operator~() const {
        Fixed res(*this);

        mpz_com(res.thenum, res.thenum);

        return *this;
    }

    /** \brief Negates the number
     *
     *  This prefix operator returns a \em copy of the number's additive
     *  inverse.
     *
     *  \warning This operator <b>does nothing</b> on unsigned Fixed classes. It
     *           simply returns a copy of itself.
     *
     *  \returns A copy of the Fixed object, negated if possible.
     *
     */
    Fixed operator-() const {
        Fixed res(*this);

        if (SIGNED) {
            mpz_neg(res.thenum, res.thenum);
        }

        return *this;
    }

    // these done as friend operators just to avoid typing out template
    // parameters for Fixed

    // also, these done with a macro to avoid repetition

#define FRIENDOP(op, flag)                                              \
    /** \brief Operator op between two same Fixed objects.              \
     *                                                                  \
     *  See the documentation on its assignment form for details.       \
     *                                                                  \
     */                                                                 \
    friend Fixed operator op (const Fixed & a, const Fixed & b) {       \
        Fixed res(a);                                                   \
        res op##= b;                                                    \
        return res;                                                     \
    }                                                                   \
    /** \brief Operator op between a Fixed object and integral          \
     *                                                                  \
     *  See the documentation on its assignment form for details.       \
     *                                                                  \
     */                                                                 \
    template<typename T,                                                \
             typename std::enable_if<std::is_integral<T>::value>::type* = nullptr> \
    friend Fixed operator op (const Fixed & a, const T & b) {           \
        Fixed res(a);                                                   \
        res op##= b;                                                    \
        return res;                                                     \
    }                                                                   \
    /** \brief Operator op between an integral and Fixed object         \
     *                                                                  \
     *  See the documentation on its assignment form for details.       \
     *                                                                  \
     */                                                                 \
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

    /** \brief Left shift Fixed number.
     *
     *  This operator returns a \em copy of the Fixed number on the left when
     *  it's left-shifted by the number of bits on the right.
     *
     *  \param[in,out] a The Fixed object to shift.
     *
     *  \param[in,out] b The number of bits to shift by.
     *
     *  \returns A new Fixed object representing \c a left-shifted.
     *
     *  \note For a mutating left-shift, use \c <<= instead.
     *
     */
    friend Fixed operator<<(const Fixed & a, const size_t & b) {
        Fixed res(a);
        res <<= b;
        return res;
    }

    /** \brief Right shift Fixed number.
     *
     *  This operator returns a \em copy of the Fixed number on the left when
     *  it's right-shifted by the number of bits on the right. This is an \b
     *  arithmetic right-shift, i.e. the sign-extended kind.
     *
     *  \param[in,out] a The Fixed object to shift.
     *
     *  \param[in,out] b The number of bits to shift by.
     *
     *  \returns A new Fixed object representing \c a right-shifted.
     *
     *  \note For a mutating right-shift, use \c >>= instead.
     *
     */
    friend Fixed operator>>(const Fixed & a, const size_t & b) {
        Fixed res(a);
        res >>= b;
        return res;
    }

    /** \brief Boolean cast operator
     *
     *  This is an \c explicit cast operator to bool, to allow similar usage to
     *  other numeric types.
     *
     *  \returns \c false if the number is zero, \c true otherwise.
     *
     */
    explicit operator bool() const {
        return mpz_cmp_ui(thenum, 0) != 0;
    }

    /** \brief Tests if two Fixed objects are equal.
     */
    bool operator==(const Fixed & that) const {
        return mpz_cmp(thenum, that.thenum) == 0;
    }

    /** \brief Tests if the left-hand Fixed object is less than the right.
     */
    bool operator<(const Fixed & that) const {
        return mpz_cmp(thenum, that.thenum) < 0;
    }

    /** \brief Tests if the left-hand Fixed object is greater than the right.
     */
    bool operator>(const Fixed & that) const {
        return mpz_cmp(thenum, that.thenum) > 0;
    }

    /** \brief Tests if two Fixed objects are not equal.
     */
    bool operator!=(const Fixed & that) const {
        return !(*this == that);
    }

    /** \brief Tests if the left-hand Fixed object is less than or equal to the
     *         right.
     */
    bool operator<=(const Fixed & that) const {
        return !(*this > that);
    }

    /** \brief Tests if the left-hand Fixed object is greater than or equal to
     *         the right.
     */
    bool operator>=(const Fixed & that) const {
        return !(*this < that);
    }

    /** \brief Output streaming operator
     *
     *  Writes to an instance of std::ostream the number contained in the Fixed
     *  object. The output matches what the value means, as opposed to its raw
     *  form.
     *
     *  \param[in,out] os The stream to write to.
     *
     *  \param[in] that The Fixed number to write to stream.
     *
     *  \returns The \c std::ostream reference that was passed to this operator.
     *
     */
    friend std::ostream & operator<<(std::ostream & os, const Fixed & that) {
        mpf_t asfloat;
        mpf_init2(asfloat, I + F);
        mpf_set_z(asfloat, that.thenum);
        mpf_div_2exp(asfloat, asfloat, F);

        os << asfloat;

        mpf_clear(asfloat);

        return os;
    }

    /** \brief Input streaming operator
     *
     *  Reads a radix-point-based representation of the fixed-point number into
     *  the given Fixed object. If the input number is too precise, the extra
     *  fractional bits will be cut off. Similarly, an input number that's out
     *  of range will be subjected to modulo arithmetic.
     *
     *  \param[in,out] is A reference to the input stream
     *
     *  \param[out] that The Fixed to write to.
     *
     *  \returns The \c std::istream reference that was passed to this operator.
     *
     */
    friend std::istream & operator>>(std::istream & is, Fixed & that) {
        mpf_t asfloat;
        mpf_init2(asfloat, I + F);

        is >> asfloat;

        mpf_mul_2exp(asfloat, asfloat, F);

        mpz_set_f(that.thenum, asfloat);

        mpf_clear(asfloat);

        if (mpz_cmp_ui(that.thenum, std::numeric_limits<Fixed>::max()) > 0
            || mpz_cmp_ui(that.thenum, std::numeric_limits<Fixed>::min()) < 0) {
            mpz_tdiv_r_2exp(that.thenum, that.thenum, I + F);
        }

        return is;
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
            mpz_set_ui(res, 1);

            mpz_mul_2exp(res, res, FI + FF);
            mpz_neg(res, res);

            Fixed<FS, FI, FF> theres(res);
            mpz_clear(res);

            return theres;
        }

        static constexpr Fixed<FS, FI, FF> lowest() { return min(); }

        static constexpr Fixed<FS, FI, FF> max() {
            mpz_t res;
            mpz_init(res);
            mpz_set_ui(res, 1);

            mpz_mul_2exp(res, res, FI + FF);
            mpz_sub_ui(res, res, 1);

            Fixed<FS, FI, FF> theres(res);
            mpz_clear(res);

            return theres;
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