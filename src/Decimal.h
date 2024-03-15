#pragma once

#include "Assert.h"
#include "ErrCode.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>
#include <type_traits>

#include "gmp.h"

namespace bignum {
template <typename T>
concept IntegralType = ((std::is_integral_v<T> && std::is_signed_v<T>) ||
                        std::is_same_v<T, __int128_t>);

template <typename T>
concept UnsignedIntegralType = ((std::is_integral_v<T> || std::is_same_v<T, __int128_t>) ||
                                !std::is_signed_v<T>);

// integral type that is at most 64bits
template <typename T>
concept SmallIntegralType = IntegralType<T> && sizeof(T) <= 8;

// large integral: int128_t
template <typename T>
concept LargeIntegralType = IntegralType<T> && sizeof(T) == 16;

template <typename T>
concept FloatingPointType =
    std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;

constexpr __int128_t kInt128Max = (static_cast<__int128_t>(INT64_MAX) << 64) | UINT64_MAX;
constexpr __int128_t kInt128Min = static_cast<__int128_t>(INT64_MIN) << 64;

namespace detail {
struct GmpWrapper {
    constexpr static size_t kNumLimbs = 5;
    __mpz_struct mpz;
    mp_limb_t limbs[kNumLimbs] = {0, 0, 0, 0, 0};

    constexpr void init() {
        mpz._mp_alloc = 5;
        mpz._mp_size = 0;
        mpz._mp_d = &limbs[0];
    }
};
static_assert(sizeof(GmpWrapper) == 56);

template <IntegralType T>
constexpr inline T type_max() {
    if constexpr (std::is_same_v<T, __int128_t>) {
        return kInt128Max;
    } else {
        return std::numeric_limits<T>::max();
    }
}

template <IntegralType T>
constexpr inline T type_min() {
    if constexpr (std::is_same_v<T, __int128_t>) {
        return kInt128Min;
    } else {
        return std::numeric_limits<T>::min();
    }
}

template <IntegralType T>
constexpr inline ErrCode safe_add(T &res, T lhs, T rhs) {
    // Overflow detection.
    // For GCC, a simple "((lhs + rhs) - lhs) != rhs" is enough, but clang apply optimizations
    // to this expression and it will always return false.
    // So we use the following case-by-case expr to detect overflow.
    if constexpr (sizeof(T) <= 8) {
        int64_t i64res = 0;
        if (__builtin_saddll_overflow(lhs, rhs, &i64res)) {
            return kErr;
        }
        if (!std::is_same_v<int64_t, T> && (i64res > type_max<T>() || i64res < type_min<T>())) {
            return kErr;
        }
        res = i64res;
    } else {
        if (((rhs > 0) && (lhs > (type_max<T>() - rhs))) ||
            ((rhs < 0) && (lhs < (type_min<T>() - rhs)))) {
            return kErr;
        } else {
            res = lhs + rhs;
        }
    }
    return kOk;
}

// Convert a string into __int128_t and assume no overflow would occur.
// Leading '0' characters would be ignored, i.e., "000123" is the same as "123".
// Return error if non-digit characters are found in the string.
constexpr inline ErrCode convert_str_to_int128(__int128_t &res, const char *ptr, const char *end) {
    __int128_t v128 = 0;
    bool met_non_zero_significant = false;
    for (; ptr < end; ++ptr) {
        int pv = *ptr;
        if (pv < '0' || pv > '9') {
            return kInvalidArgument;  // Invalid character
        }
        int v = pv - '0';
        // Ignore leading zeros
        if (!v && !met_non_zero_significant) {
            continue;
        }
        if (!met_non_zero_significant) {
            met_non_zero_significant = true;
        }
        v128 = v128 * 10 + v;
    }
    res = v128;
    return kOk;
}

std::string decimal64_to_string(int64_t v, int32_t scale);
std::string decimal128_to_string(__int128_t v, int32_t scale);
std::string decimal_general_to_string(const GmpWrapper &v, int32_t scale);

constexpr __int128_t get_int128_power10(int32_t scale) {
    constexpr __int128_t kPower10[] = {
        /* 0  */ static_cast<__int128_t>(1LL),
        /* 1  */ static_cast<__int128_t>(10LL),
        /* 2  */ static_cast<__int128_t>(100LL),
        /* 3  */ static_cast<__int128_t>(1000LL),
        /* 4  */ static_cast<__int128_t>(10000LL),
        /* 5  */ static_cast<__int128_t>(100000LL),
        /* 6  */ static_cast<__int128_t>(1000000LL),
        /* 7  */ static_cast<__int128_t>(10000000LL),
        /* 8  */ static_cast<__int128_t>(100000000LL),
        /* 9  */ static_cast<__int128_t>(1000000000LL),
        /* 10 */ static_cast<__int128_t>(10000000000LL),
        /* 11 */ static_cast<__int128_t>(100000000000LL),
        /* 12 */ static_cast<__int128_t>(1000000000000LL),
        /* 13 */ static_cast<__int128_t>(10000000000000LL),
        /* 14 */ static_cast<__int128_t>(100000000000000LL),
        /* 15 */ static_cast<__int128_t>(1000000000000000LL),
        /* 16 */ static_cast<__int128_t>(10000000000000000LL),
        /* 17 */ static_cast<__int128_t>(100000000000000000LL),
        /* 18 */ static_cast<__int128_t>(1000000000000000000LL),
        /* 19 */ static_cast<__int128_t>(1000000000000000000LL) * 10LL,
        /* 20 */ static_cast<__int128_t>(1000000000000000000LL) * 100LL,
        /* 21 */ static_cast<__int128_t>(1000000000000000000LL) * 1000LL,
        /* 22 */ static_cast<__int128_t>(1000000000000000000LL) * 10000LL,
        /* 23 */ static_cast<__int128_t>(1000000000000000000LL) * 100000LL,
        /* 24 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000LL,
        /* 25 */ static_cast<__int128_t>(1000000000000000000LL) * 10000000LL,
        /* 26 */ static_cast<__int128_t>(1000000000000000000LL) * 100000000LL,
        /* 27 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000000LL,
        /* 28 */ static_cast<__int128_t>(1000000000000000000LL) * 10000000000LL,
        /* 29 */ static_cast<__int128_t>(1000000000000000000LL) * 100000000000LL,
        /* 30 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000000000LL,
        /* 31 */ static_cast<__int128_t>(1000000000000000000LL) * 10000000000000LL,
        /* 32 */ static_cast<__int128_t>(1000000000000000000LL) * 100000000000000LL,
        /* 33 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000000000000LL,
        /* 34 */ static_cast<__int128_t>(1000000000000000000LL) * 10000000000000000LL,
        /* 35 */ static_cast<__int128_t>(1000000000000000000LL) * 100000000000000000LL,
        /* 36 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000000000000000LL,
        /* 37 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000000000000000LL * 10LL,
        /* 38 */ static_cast<__int128_t>(1000000000000000000LL) * 1000000000000000000LL * 100LL,
    };
    constexpr int64_t num_power10 = sizeof(kPower10) / sizeof(kPower10[0]);
    ASSERT(scale >= 0 && scale < num_power10);
    return kPower10[scale];
}

/* clang-format off */
/* Table to be indexed by character, to get its numerical value.  Assumes ASCII
   character set.

   First part of table supports common usages, where 'A' and 'a' have the same
   value; this supports bases 2..36

   At offset 208, values for bases 37..62 start.  Here, 'A' has the value 10
   (in decimal) and 'a' has the value 36.  */
#define X 0xff
const unsigned char gmp_digit_value_tab[] =
{
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, X, X, X, X, X, X,
  X,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
  25,26,27,28,29,30,31,32,33,34,35,X, X, X, X, X,
  X,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
  25,26,27,28,29,30,31,32,33,34,35,X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, X, X, X, X, X, X,
  X,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
  25,26,27,28,29,30,31,32,33,34,35,X, X, X, X, X,
  X,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,
  51,52,53,54,55,56,57,58,59,60,61,X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,
  X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X
};
#undef X
/* clang-format on */

}  // namespace detail

//=-----------------------------------------------------------------------------
// Big decimal implementation similar to MySQL's DECIMAL data type in execution
// layer.
//
//   - Maximum precision (i.e., maximum total number of digits) is 65 and maximum scale
//     (i.e., maximum number of digits after the decimal point) is 30.
//
//   - Unlike a "DECIMAL" column in database, where you have to specify the precision and
//     scale, this class does not require you to specify the precision and
//     scale. The precision and scale are determined by the input value and
//     intermediate calculation result, automatically.
//
//   - Decimal is always signed. It can represent negative numbers.
//
//   - negative scale is not supported;
//
//   - calculation overflow would trigger assertion error by default, unless specified
//     error-handling interfaces are used;
//
//   - initialization using constructor that
//       * overflows the maximum precision or
//       * results in error (e.g., invalid argument)
//     would trigger assertion.
//     To initialize safely (with error code returned), use default constructor
//     and then use the "assign(..)" interfaces to initialize the value;
//
//   - Initialize with float32/float64 is allowed, but there would be rounding
//     if the value is not "precise" and take more than 7 or 16 decimal digits,
//     where the result would be rounded to scale=7 or scale=16
//     (for float32 or float64 respectively).
//     Rounding is done using the round-half-up rule, i.e., if the next digit is
//     5 or greater, the current digit is rounded away from zero, otherwise it is
//     rounded towards zero. So 1.5 would be rounded to 2,
//     and -1.5 would be rounded to -2.
//
//     Note that the maxinum precision is 65, so if the input value has more
//     than 65 digits after rounding to scale=7 or scale=16, error or assertion
//     would be triggered.
//
//     For initializing with const value, it is recommended to use const string
//     instead of const float/double to avoid precision loss, i.e., `Decimal
//     d("1.23")` is better than `Decimal d(1.23)`. The Decimal constructor is
//     constexpr and for numbers that are able to fit into a int128_t,
//     there is no runtime overhead. To prevent misuse, constructor
//     with literal float/double is marked as deleted and would result in
//     compile error.
//
// Error code:
//  - Error code is returned by the interfaces that may fail, e.g., overflow, such
//    as `add`, `sub`, `mul`, `div`, etc.
//  - return type is ErrCode, which is convertable to int, with value 0 for non-error,
//    and others for error. If user cares about specified error code, the `ErrCode` type
//    could be used directly.
//=-----------------------------------------------------------------------------
template <typename T = void>
class DecimalImpl final {
   public:
    constexpr static int32_t kMaxScale = 30;
    constexpr static int32_t kMaxPrecision = 65;

    // Every division increase current scale by 4 until max scale is reached.
    // Note that even when max scale is reached, the scale is still increased
    // before division. Intermediate result would be increased to scale 16+4=20,
    // and then after the calculation, it is decreased to scale 16.
    constexpr static int32_t kDecimalDivIncreaseScale = 4;

   public:
    constexpr DecimalImpl() : m_i64(0), m_dtype(DType::kInt64), m_scale(0) {}

    // Construction using integral value, without scale (scale=0).
    //
    // Constructor for preset scale, e.g., (i=123, scale=2) => 123.00, is not
    // provided on purpose. This is not the same as the database decimal type,
    // which has a fixed scale data type. The scale of this class is dynamic and
    // stored within each object.
    template <SmallIntegralType U>
    constexpr DecimalImpl(U i) : m_i64(i), m_dtype(DType::kInt64), m_scale(0) {}

    template <LargeIntegralType U>
    constexpr DecimalImpl(U i) : m_i128(i), m_dtype(DType::kInt128), m_scale(0) {}

    // Construction using floating point value.
    //
    // Construction using floating point value would result in rounding if the
    // value is not "precise" and have more than 7 or 16 least significant digits
    // for float or double. See comments above for more details. Therefore, we
    // intentionally prohibit constructor for literal float/double to avoid
    // misuse.
    //
    // This interface would trigger assertion on overflow or error.
    // User who want explicit error handling should use the assign(..) interfaces.
    template <FloatingPointType U>
    constexpr DecimalImpl(U &v) {
        // accept reference only, so literal float would not override here
        ErrCode err = assign(v);
        ASSERT(!err, "Decimal initialization with floating point value overflows");
    }

    template <FloatingPointType U>
    constexpr DecimalImpl(U) {
        static_assert(std::is_same_v<U, T>,
                      "Using literal floating point value to initialize Decimal is "
                      "error-prone, "
                      "use string literal instead, i.e., use Decimal(\"1.23\") instead of "
                      "Decimal(1.23)");
    }

    // Construction using string value.
    //
    // If the input string is constexpr, the constructor is constexpr.
    // The scale is deduced from the string automatically, but would shrink to as
    // small as possible. For example, "123.10" would be stored as (i=1231,
    // scale=1).
    //
    // This interface would trigger assertion on overflow or error.
    // User who want explicit error handling should use the assign(..) interfaces.
    constexpr DecimalImpl(std::string_view sv) { ASSERT(!assign(sv), "Invalid decimal string"); }

    DecimalImpl(const DecimalImpl &) = default;
    DecimalImpl(DecimalImpl &&) = default;
    DecimalImpl &operator=(const DecimalImpl &) = default;
    DecimalImpl &operator=(DecimalImpl &&) = default;

    ~DecimalImpl() = default;

    //=--------------------------------------------------------
    // Assignment/conversion operators.
    // Return error code instead of triggering assertion.
    //=--------------------------------------------------------
    template <IntegralType U>
    constexpr ErrCode assign(U i) {
        m_scale = 0;
        if (sizeof(U) <= 8) {
            m_dtype = DType::kInt64;
            m_i64 = i;
        } else {
            m_dtype = DType::kInt128;
            m_i128 = i;
        }
        return kOk;
    }

    template <FloatingPointType U>
    constexpr ErrCode assign(U &f);

    constexpr ErrCode assign(std::string_view sv);

    //=--------------------------------------------------------
    // Cast operators.
    //=--------------------------------------------------------
    std::string to_string() const;
    explicit operator std::string() const { return to_string(); }
    explicit constexpr operator double() const;
    constexpr operator bool() const;

    //=--------------------------------------------------------
    // Arithmetic operators.
    // Trigger assertion on overflow or error.
    //
    // Decimal arithmetic operators does not return error on overflow, but trigger
    // assertion instead. The is reasonable because Decimal is to used for
    // representing "exact" values such as money, where, precision=65 is large
    // enough for most cases and overflow is not expected.
    //
    // For explicit error handling, use the "add(..)", "sub(..)", "mul(..)",
    // "div(..)" interfaces.
    //=--------------------------------------------------------

    //=-=--------------------------------------------------------
    // operator +=
    //=-=--------------------------------------------------------
    constexpr ErrCode add(const DecimalImpl &rhs);

    constexpr DecimalImpl &operator+=(const DecimalImpl &rhs) {
        ErrCode err = add(rhs);
        ASSERT(!err, "Decimal addition overflow");  // FIXME constexpr string with
                                                    // value inside
        return *this;
    }

    // Even though `Decimal(1.23)` is not allowed, `decimal_value += 1.23` is
    // allowed to make it easier to use.
    constexpr DecimalImpl &operator+=(double f) {
        double d = static_cast<double>(f);
        *this += DecimalImpl{d};
        return *this;
    }

    //=-=--------------------------------------------------------
    // operator +
    //=-=--------------------------------------------------------
    constexpr DecimalImpl operator+(const DecimalImpl &rhs) const {
        DecimalImpl ret = *this;
        ret += rhs;
        return ret;
    }
    constexpr DecimalImpl operator+(double f) const {
        double d = static_cast<double>(f);
        return *this + DecimalImpl{d};
    }

    //=-=--------------------------------------------------------
    // operator -=
    //=-=--------------------------------------------------------
    constexpr ErrCode sub(const DecimalImpl &rhs);

    constexpr DecimalImpl &operator-=(const DecimalImpl &rhs) {
        ErrCode err = sub_and_set(rhs);
        ASSERT(!err, "Decimal subtraction overflow");  // FIXME constexpr string with
                                                       // value inside
        return *this;
    }
    constexpr DecimalImpl &operator-=(double f) {
        double d = static_cast<double>(f);
        *this -= DecimalImpl{d};
        return *this;
    }

    //=-=--------------------------------------------------------
    // operator -
    //=-=--------------------------------------------------------
    constexpr DecimalImpl operator-(const DecimalImpl &rhs) const {
        DecimalImpl ret = *this;
        ret -= rhs;
        return ret;
    }
    constexpr DecimalImpl operator-(double f) const {
        double d = static_cast<double>(f);
        return *this - DecimalImpl{d};
    }

    constexpr DecimalImpl operator-() const {
        DecimalImpl ret = *this;
        ret.negate();
        return ret;
    }

    //=----------------------------------------------------------
    // operator *=
    //
    // Decimal multiplication overflowing significant digits would trigger
    // assertion. However, decimal multiplication overflowing least significant
    // digits would not trigger assertion. Instead, the result would be rounded to
    // the maximum scale if necessary.
    //
    // After each multiplication, '0' after '.' at the end of decimal would be
    // removed to make the resulting scale as small as possible. This would
    // effectively avoid scaling the least significant digits to max scale after
    // continous multiplcation.
    //=----------------------------------------------------------
    constexpr ErrCode mul(const DecimalImpl &rhs);

    constexpr DecimalImpl &operator*=(const DecimalImpl &rhs) {
        ErrCode err = mul_and_set(rhs);
        ASSERT(!err, "Decimal multiplication overflow");  // FIXME constexpr string
                                                          // with value inside
        return *this;
    }
    constexpr DecimalImpl &operator*=(double f) {
        double d = static_cast<double>(f);
        *this *= DecimalImpl{d};
        return *this;
    }

    //=-=--------------------------------------------------------
    // operator *
    //=-=--------------------------------------------------------
    constexpr DecimalImpl operator*(const DecimalImpl &rhs) const {
        DecimalImpl d = *this;
        d *= rhs;
        return d;
    }
    constexpr DecimalImpl operator*(double f) const {
        double d = static_cast<double>(f);
        return *this * DecimalImpl{d};
    }

    //=----------------------------------------------------------
    // operator /=
    //
    // Decimal division overflowing significant digits would trigger assertion.
    // However, decimal division overflowing least significant digits would NOT
    // trigger assertion. Instead, every division would increase the scale by 4
    // until max scale is reached. For example, 1.28/3.3 => 0.38787878787878793..
    // => 0.387879, which is 2+4=6 digits after decimal point.
    //
    // Intermediate result might be increased to scale kMaxScale+4=34 before
    // calculation, where intermediate result would be calculated using 34 least
    // significant digits. After the division, it is rounded back to scale 30.
    //
    // Divison by zero would trigger assertion error.
    //=----------------------------------------------------------
    constexpr ErrCode div(const DecimalImpl &rhs);

    constexpr DecimalImpl &operator/=(const DecimalImpl &rhs) {
        ErrCode err = div_and_set(rhs);
        ASSERT(!err,
               "Decimal division by zero or overflow");  // FIXME constexpr string
                                                         // with value inside
        return *this;
    }
    constexpr DecimalImpl &operator/=(double f) {
        double d = static_cast<double>(f);
        *this /= DecimalImpl{d};
        return *this;
    }

    //=-=--------------------------------------------------------
    // operator /
    //=-=--------------------------------------------------------
    constexpr DecimalImpl operator/(const DecimalImpl &rhs) const {
        DecimalImpl d = *this;
        d /= rhs;
        return d;
    }
    constexpr DecimalImpl operator/(double f) const {
        double d = static_cast<double>(f);
        return *this / DecimalImpl{d};
    }

    //=----------------------------------------------------------
    // operator %=
    //
    // Modulo operator. If the rhs is not and integer, or if rhs is zero, trigger
    // assertion.
    //=----------------------------------------------------------
    constexpr ErrCode mod(const DecimalImpl &rhs);

    constexpr DecimalImpl &operator%=(const DecimalImpl &rhs) {
        ErrCode err = mod_and_set(rhs);
        ASSERT(!err,
               "DecimalImpl modulo by zero or non-integer");  // FIXME constexpr string
                                                              // with value inside
        return *this;
    }
    constexpr DecimalImpl &operator%=(double f) {
        double d = static_cast<double>(f);
        *this %= DecimalImpl{d};
        return *this;
    }

    //=-=--------------------------------------------------------
    // operator %
    //=-=--------------------------------------------------------
    constexpr DecimalImpl operator%(const DecimalImpl &rhs) const {
        DecimalImpl d = *this;
        d %= rhs;
        return d;
    }
    constexpr DecimalImpl operator%(double f) const {
        double d = static_cast<double>(f);
        return *this % DecimalImpl{d};
    }

    //=--------------------------------------------------------
    // Comparison operators.
    //=--------------------------------------------------------
    constexpr bool operator==(const DecimalImpl &rhs) const;
    constexpr bool operator<(const DecimalImpl &rhs) const;
    constexpr bool operator<=(const DecimalImpl &rhs) const;
    constexpr bool operator!=(const DecimalImpl &rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const DecimalImpl &rhs) const { return !(*this <= rhs); }
    constexpr bool operator>=(const DecimalImpl &rhs) const { return !(*this < rhs); }

    constexpr bool operator==(double f) const {
        double d = static_cast<double>(f);
        return *this == DecimalImpl{d};
    }
    constexpr bool operator<(double f) const {
        double d = static_cast<double>(f);
        return *this < DecimalImpl{d};
    }
    constexpr bool operator<=(double f) const {
        double d = static_cast<double>(f);
        return *this <= DecimalImpl{d};
    }
    constexpr bool operator!=(double f) const { return !(*this == f); }
    constexpr bool operator>(double f) const { return !(*this <= f); }
    constexpr bool operator>=(double f) const { return !(*this < f); }

   private:
    constexpr ErrCode assign_str128(const char *start, const char *end);
    constexpr ErrCode assign_str_general(const char *start, const char *end);

    constexpr void init_my_mpz();

    constexpr void negate();

    constexpr __int128_t get_128_from_64() const;
    constexpr detail::GmpWrapper get_gmp_from_64() const;
    constexpr detail::GmpWrapper get_gmp_from_128() const;

    constexpr ErrCode add_i64_i64(int64_t l64, int32_t lscale, int64_t r64, int32_t rscale);
    constexpr ErrCode add_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                    int32_t rscale);
    constexpr ErrCode add_gmp_gmp(const detail::GmpWrapper &l, int32_t lscale,
                                  const detail::GmpWrapper &r, int32_t rscale);

   private:
    enum class DType : uint8_t {
        kInt64 = 0,
        kInt128 = 1,
        kGeneral = 2,
    };

    // If a decimal is small enough, we would try to store it in int64_t so that
    // we can use int64_t arithmetic to speed up the calculation. The same for
    // int128_t and general.
    //
    // However, it is not gaurenteed that the decimal would be stored in its
    // smallest type, meaning that, a decimal that is able to fit in int64_t
    // might be stored in int128_t or general. We try best to store the decimal in its
    // smallest type, but it is not gaurenteed.
    union {
        struct {
            union {
                int64_t m_i64;
                __int128_t m_i128;
            };
            char _padding0[48];
        };

        detail::GmpWrapper m_gmp;

        struct {
            char _padding2[56];
            DType m_dtype;
            int32_t m_scale;
        };
    };
};
using Decimal = DecimalImpl<>;
static_assert(sizeof(Decimal) == 64);

template <typename T>
template <FloatingPointType U>
constexpr inline ErrCode DecimalImpl<T>::assign(U &v) {
    std::ostringstream oss;
    if constexpr (std::is_same_v<U, float>) {
        oss << std::fixed << std::setprecision(7) << v;
    } else {
        oss << std::fixed << std::setprecision(17) << v;
    }
    return assign(oss.str());
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::assign(std::string_view sv) {
    const char *ptr = &(sv[0]);
    const char *end = ptr + sv.size();

    // Skip leading space
    while (ptr < end && *ptr == ' ') {
        ptr++;
    }

    const size_t slen = end - ptr;
    if (!slen) {
        assign(0ll);
        return kOk;
    }

    // A int64_t can represent at most 18 digits fully. 19 digits might not fit
    // into int64_t. A int128_t can represent at most 38 digits fully. 39 digits
    // might not fit into int128_t.
    ErrCode err = kOk;
    if ((ptr[0] == '-' && slen <= 39) || slen <= 38) {
        // Convert it into int128_t and then see whether it fits into a int64_t.
        // There is no assign_str64() because it is no necessary: the process of
        // converting a string into a integral is much more expensive than casting
        // a int128_t into a int64_t.
        err = assign_str128(ptr, end);
    } else {
        err = assign_str_general(ptr, end);
    }

    return err;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::assign_str128(const char *start, const char *end) {
    // Caller guarantees that
    //   - string is not empty;
    //   - trailing spaces are removed;
    //   - string is not too long to fit into int128_t.
    const size_t slen = end - start;
    const char *ptr = start;
    assert(slen && ((ptr[0] == '-' && slen <= 39) || slen <= 38));

    bool is_negative = false;
    if (*ptr == '-') {
        is_negative = true;
        ptr++;
    }

    const char *pdot = std::find(ptr, end, '.');
    ASSERT(pdot <= end);
    bool has_dot = (pdot < end);

    // The '.' character could not be at the very end, i.e., '1234.' is not
    // acceptable.
    if (has_dot && (pdot + 1 >= end)) {
        return kInvalidArgument;
    }

    // Already limit number of digits above, so the significant digits could not
    // overflow. But the significant part might still contains invalid chacters.
    __int128_t significant_v128 = 0;
    ErrCode err = detail::convert_str_to_int128(significant_v128, ptr, pdot);
    if (err) {
        return err;
    }

    // Trailing '0' truncation:
    // 123.1000 -> 123.1
    // 123.000 -> 123
    if (has_dot) {
        while (end > pdot + 1 && *(end - 1) == '0') {
            end--;
        }
        has_dot = (pdot < end);
    }

    int32_t scale = 0;
    if (has_dot) {
        scale = end - (pdot + 1);
        // Already limit number of digits above, so __int128_t can always represent
        // all digits. However, the scale might still overflow if the # of least
        // significant digits is more than 30.
        if (scale > kMaxScale) {
            return kDecimalScaleOverflow;
        }

        __int128_t least_significant_v128 = 0;
        // Already limit number of digits above, so the least significant digits
        // could not overflow. But the least significant part might still contains,
        // like, invalid chacters.
        err = detail::convert_str_to_int128(least_significant_v128, pdot + 1, end);
        if (err) {
            return err;
        }

        __int128_t scale_multiplier = detail::get_int128_power10(scale);

        significant_v128 = significant_v128 * scale_multiplier + least_significant_v128;
    }

    m_i128 = (is_negative ? -significant_v128 : significant_v128);
    m_scale = scale;

    if (scale < 18 && m_i128 <= INT64_MAX && m_i128 >= INT64_MIN) {
        m_dtype = DType::kInt64;
        m_i64 = static_cast<int64_t>(m_i128);
    } else {
        m_dtype = DType::kInt128;
    }
    return kOk;
}

template <typename T>
constexpr void DecimalImpl<T>::init_my_mpz() {
    m_gmp.init();
}

template <typename T>
constexpr void DecimalImpl<T>::negate() {
    if (m_dtype == DType::kInt64) {
        m_i64 = -m_i64;
    } else if (m_dtype == DType::kInt128) {
        m_i128 = -m_i128;
    } else {
        ASSERT(m_dtype == DType::kGeneral);
        m_gmp.mpz._mp_size = -m_gmp.mpz._mp_size;
    }
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::assign_str_general(const char *start, const char *end) {
    // Caller guarantees that
    //  - string is not empty;
    //  - trailing spaces are removed;
    const size_t slen = end - start;
    assert(slen);
    const char *ptr = start;

    if (slen > kMaxPrecision + 1 + 1) {
        return kInvalidArgument;
    }

    bool is_negative = false;
    int64_t scale = 0;
    int64_t num_digits = 0;

    if (*ptr == '-') {
        is_negative = true;
        ptr++;
    }

    // Copy all the digits out into a buffer, to be initialized by mpn_set_str.
    // Leading zeros are skipped.
    // At most kMaxPrecision digits. +1 for the '\0'.
    char buf[kMaxPrecision + 1] = {0};
    char *pbuf = buf;
    const char *pdot = nullptr;
    bool met_non_zero_significant = false;
    for (; ptr < end; ++ptr) {
        int pv = *ptr;
        if (pv == '0' && !met_non_zero_significant) {
            continue;
        }
        if (!met_non_zero_significant) {
            met_non_zero_significant = true;
        }
        if (pv == '.') {
            pdot = ptr;
            continue;
        }
        if (pv < '0' || pv > '9') {
            return kInvalidArgument;
        }
        *pbuf++ = detail::gmp_digit_value_tab[pv];
        num_digits++;
        assert(num_digits <= kMaxPrecision);
    }
    scale = pdot ? end - (pdot + 1) : 0;

    init_my_mpz();
    int64_t xsize = mpn_set_str(m_gmp.mpz._mp_d, (unsigned char *)buf, num_digits, /*base*/ 10);
    m_gmp.mpz._mp_size = (is_negative ? -xsize : xsize);

    m_dtype = DType::kGeneral;
    m_scale = scale;

    return kOk;
}

template <typename T>
inline std::string DecimalImpl<T>::to_string() const {
    if (m_dtype == DType::kInt64) {
        return detail::decimal64_to_string(m_i64, m_scale);
    } else if (m_dtype == DType::kInt128) {
        return detail::decimal128_to_string(m_i128, m_scale);
    } else {
        return detail::decimal_general_to_string(m_gmp, m_scale);
    }
}

template <typename T>
constexpr inline DecimalImpl<T>::operator double() const {
    if (m_dtype == DType::kInt64) {
        return static_cast<double>(m_i64) / detail::get_int128_power10(m_scale);
    } else if (m_dtype == DType::kInt128) {
        return static_cast<double>(m_i128) / detail::get_int128_power10(m_scale);
    } else {
        double res = mpz_get_d(&m_gmp.mpz);

        int scale = m_scale;
        ASSERT(scale >= 0);
        while (scale > 0) {
            int s = std::min(scale, 38);
            __int128_t scale_div = detail::get_int128_power10(s);
            res /= scale_div;
            scale -= s;
        }
        return res;
    }
}

template <typename T>
constexpr inline DecimalImpl<T>::operator bool() const {
    if (m_dtype == DType::kInt64) {
        return m_i64 != 0;
    } else if (m_dtype == DType::kInt128) {
        return m_i128 != 0;
    } else {
        bool is_zero = (m_gmp.mpz._mp_size == 0);
        return !is_zero;
    }
}

template <typename T>
constexpr inline __int128_t DecimalImpl<T>::get_128_from_64() const {
    assert(m_dtype == DType::kInt64);
    return static_cast<__int128_t>(m_i64);
}

template <typename T>
constexpr inline detail::GmpWrapper DecimalImpl<T>::get_gmp_from_64() const {
    assert(m_dtype == DType::kInt64);

    detail::GmpWrapper tmp_gmp;
    tmp_gmp.init();

    mpz_set_si(&tmp_gmp.mpz, m_i64);

    return tmp_gmp;
}

template <typename T>
constexpr inline detail::GmpWrapper DecimalImpl<T>::get_gmp_from_128() const {
    assert(m_dtype == DType::kInt128);

    detail::GmpWrapper tmp_gmp;
    tmp_gmp.init();

    mpz_import(&tmp_gmp.mpz, 2, 1, sizeof(m_i128), 0, 0, &m_i128);

    return tmp_gmp;
}

template <typename T>
constexpr ErrCode DecimalImpl<T>::add_i64_i64(int64_t l64, int32_t lscale, int64_t r64,
                                              int32_t rscale) {
    // TODO
    // int64_t res64 = 0;
    // int64_t res_scale = 0;
    // ErrCode err = detail::decimal_add_integral(res64, res_scale, l64, lscale, r64, rscale);
    // if (err) {
    //     return err;
    // }
    // m_dtype = DType::kInt64;
    // m_i64 = res64;
    // m_scale = res_scale;
    return kOk;
}

template <typename T>
constexpr ErrCode DecimalImpl<T>::add_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                                int32_t rscale) {
    // TODO
    // __int128_t res128 = 0;
    // int32_t res_scale = 0;
    // ErrCode err = detail::decimal_add_integral(res128, res_scale, l128, lscale, r128, rscale);
    // if (err) {
    //     return err;
    // }
    // m_dtype = DType::kInt128;
    // m_i128 = res128;
    // m_scale = res_scale;
    return kOk;
}

template <typename T>
constexpr ErrCode DecimalImpl<T>::add_gmp_gmp(const detail::GmpWrapper &l, int32_t lscale,
                                              const detail::GmpWrapper &r, int32_t rscale) {
    // TODO
    return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::add(const DecimalImpl<T> &rhs) {
    // Calculating in int64 mode is the fastest, but it can overflow, in which
    // case we need to switch to int128 mode. If int128 mode also overflows, we
    // need to switch to gmp mode. If gmp mode overflows the pre-defined maximum,
    // we return overflow error.
    ErrCode err = kErr;
    if (m_dtype == DType::kInt64) {
        if (rhs.m_dtype == DType::kInt64) {
            err = add_i64_i64(m_i64, m_scale, rhs.m_i64, rhs.m_scale);
            if (!err) {
                return kOk;
            }

            err = add_i128_i128(get_128_from_64(), m_scale, rhs.get_128_from_64(), rhs.m_scale);
            ASSERT(err == kOk);
            return kOk;

        } else if (rhs.m_dtype == DType::kInt128) {
            err = add_i128_i128(get_128_from_64(), m_scale, rhs.m_i128, rhs.m_scale);
            if (!err) {
                return kOk;
            }

            err = add_gmp_gmp(get_gmp_from_64(), m_scale, rhs.get_gmp_from_128(), rhs.m_scale);
            ASSERT(!err);
            return kOk;

        } else if (rhs.m_dtype == DType::kGeneral) {
            err = add_gmp_gmp(get_gmp_from_64(), m_scale, rhs.m_gmp.mpz, rhs.m_scale);
            return err;

        } else {
            ASSERT(false);
            return kErr;
        }
    } else if (m_dtype == DType::kInt128) {
        if (rhs.m_dtype == DType::kInt64) {
            err = add_i128_i128(m_i128, m_scale, rhs.get_128_from_64(), rhs.m_scale);
            if (!err) {
                return kOk;
            }

            err = add_gmp_gmp(get_gmp_from_128(), m_scale, rhs.get_gmp_from_64(), rhs.m_scale);
            ASSERT(!err);
            return kOk;

        } else if (rhs.m_dtype == DType::kInt128) {
            err = add_i128_i128(m_i128, m_scale, rhs.m_i128, rhs.m_scale);
            if (!err) {
                return kOk;
            }

            err = add_gmp_gmp(get_gmp_from_128(), m_scale, rhs.get_gmp_from_128(), rhs.m_scale);
            ASSERT(!err);
            return kOk;

        } else if (rhs.m_dtype == DType::kGeneral) {
            err = add_gmp_gmp(get_gmp_from_128(), m_scale, rhs.m_gmp.mpz, rhs.m_scale);
            return err;

        } else {
            ASSERT(false);
            return kErr;
        }
    } else if (m_dtype == DType::kGeneral) {
        if (rhs.m_dtype == DType::kInt64) {
            err = add_gmp_gmp(m_gmp.mpz, m_scale, rhs.get_gmp_from_64(), rhs.m_scale);
            return err;

        } else if (rhs.m_dtype == DType::kInt128) {
            err = add_gmp_gmp(m_gmp.mpz, m_scale, rhs.get_gmp_from_128(), rhs.m_scale);
            return err;

        } else if (rhs.m_dtype == DType::kGeneral) {
            err = add_gmp_gmp(m_gmp.mpz, m_scale, rhs.m_gmp.mpz, rhs.m_scale);
            return err;

        } else {
            ASSERT(false);
            return kErr;
        }
    } else {
        ASSERT(false);
        return kErr;
    }
    return kErr;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::sub(const DecimalImpl<T> &rhs) {
    auto v = rhs;
    v.negate();
    return add(v);
}
template <typename T>
constexpr inline ErrCode DecimalImpl<T>::mul(const DecimalImpl<T> &rhs) {
    return kOk;
}
template <typename T>
constexpr inline ErrCode DecimalImpl<T>::div(const DecimalImpl<T> &rhs) {
    return kOk;
}
template <typename T>
constexpr inline ErrCode DecimalImpl<T>::mod(const DecimalImpl<T> &rhs) {
    return kOk;
}
}  // namespace bignum
