#pragma once

#include "assertion.h"
#include "errcode.h"
#include "gmp_wrapper.h"

#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>
#include <type_traits>

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
// Maybe add int256 later TODO add boost compile option for int256_t
template <typename T>
concept LargeIntegralType = IntegralType<T> && sizeof(T) == 16;

template <typename T>
concept FloatingPointType =
        std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, long double>;

// TODO implement op for ostream

namespace detail {
constexpr int32_t kDecimalMaxScale = 30;
constexpr int32_t kDecimalMaxPrecision = 96;
constexpr int32_t kDecimalDivIncrScale = 4;

constexpr __int128_t kInt128Max = (static_cast<__int128_t>(INT64_MAX) << 64) | UINT64_MAX;
constexpr __int128_t kInt128Min = static_cast<__int128_t>(INT64_MIN) << 64;

constexpr const char *kDecimalMaxStr =
        "999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
        "999999";
constexpr const char *kDecimalMinStr =
        "-99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999";

template <IntegralType T>
constexpr inline T type_max() {
        if constexpr (std::is_same_v<T, __int128_t>) {
                return detail::kInt128Max;
        } else {
                return std::numeric_limits<T>::max();
        }
}

template <IntegralType T>
constexpr inline T type_min() {
        if constexpr (std::is_same_v<T, __int128_t>) {
                return detail::kInt128Min;
        } else {
                return std::numeric_limits<T>::min();
        }
}

// std::abs() is not constexpr before c++23.
template <IntegralType T>
constexpr auto constexpr_abs(T n) -> std::make_unsigned_t<T> {
        return n < 0 ? -n : n;
}

// std::min(l, r) does not accept cases like "int vs int64_t" nor "long long int vs int64_t",
// which is noisy. So we roll our own.
template <IntegralType T, IntegralType U>
constexpr auto constexpr_min(T a, U b) -> std::conditional_t<(sizeof(T) >= sizeof(U)), T, U> {
        static_assert(std::is_signed_v<T> == std::is_signed_v<U>);
        return a < b ? a : b;
}

template <IntegralType T, IntegralType U>
constexpr auto constexpr_max(T a, U b) -> std::conditional_t<(sizeof(T) >= sizeof(U)), T, U> {
        static_assert(std::is_signed_v<T> == std::is_signed_v<U>);
        return a > b ? a : b;
}

constexpr int64_t get_int64_power10(int32_t scale) {
        /* clang-format off */
        constexpr int64_t kPower10[] = {
            /* 0  */ 1LL,
            /* 1  */ 10LL,
            /* 2  */ 100LL,
            /* 3  */ 1000LL,
            /* 4  */ 10000LL,
            /* 5  */ 100000LL,
            /* 6  */ 1000000LL,
            /* 7  */ 10000000LL,
            /* 8  */ 100000000LL,
            /* 9  */ 1000000000LL,
            /* 10 */ 10000000000LL,
            /* 11 */ 100000000000LL,
            /* 12 */ 1000000000000LL,
            /* 13 */ 10000000000000LL,
            /* 14 */ 100000000000000LL,
            /* 15 */ 1000000000000000LL,
            /* 16 */ 10000000000000000LL,
            /* 17 */ 100000000000000000LL,
            /* 18 */ 1000000000000000000LL,
            /* 19 would be too large to fit into a int64_t */
        };
        /* clang-format on */
        constexpr int64_t num_power10 = sizeof(kPower10) / sizeof(kPower10[0]);
        if (scale < 0 || scale >= num_power10) {
                return -1;
        }
        return kPower10[scale];
}

constexpr __int128_t get_int128_power10(int32_t scale) {
        /* clang-format off */
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
        /* clang-format on */
        constexpr int64_t num_power10 = sizeof(kPower10) / sizeof(kPower10[0]);
        if (scale < 0 || scale >= num_power10) {
                return -1;
        }
        return kPower10[scale];
}

template <IntegralType T>
constexpr T get_integral_power10(int32_t scale) {
        if constexpr (std::is_same_v<T, __int128_t>) {
                return get_int128_power10(scale);
        } else if constexpr (std::is_same_v<T, int64_t>) {
                return get_int64_power10(scale);
        } else {
                static_assert(std::is_same_v<T, void>, "Unsupported type");
        }
}

constexpr inline Gmp320 get_gmp320_power10(int32_t scale) {
        /* clang-format off */
        const Gmp320 kPower10[] = {
            /* 0 */  Gmp320(1, 0x1, 0x0, 0x0, 0x0, 0x0),
            /* 1 */  Gmp320(1, 0xa, 0x0, 0x0, 0x0, 0x0),
            /* 2 */  Gmp320(1, 0x64, 0x0, 0x0, 0x0, 0x0),
            /* 3 */  Gmp320(1, 0x3e8, 0x0, 0x0, 0x0, 0x0),
            /* 4 */  Gmp320(1, 0x2710, 0x0, 0x0, 0x0, 0x0),
            /* 5 */  Gmp320(1, 0x186a0, 0x0, 0x0, 0x0, 0x0),
            /* 6 */  Gmp320(1, 0xf4240, 0x0, 0x0, 0x0, 0x0),
            /* 7 */  Gmp320(1, 0x989680, 0x0, 0x0, 0x0, 0x0),
            /* 8 */  Gmp320(1, 0x5f5e100, 0x0, 0x0, 0x0, 0x0),
            /* 9 */  Gmp320(1, 0x3b9aca00, 0x0, 0x0, 0x0, 0x0),
            /* 10 */ Gmp320(1, 0x2540be400, 0x0, 0x0, 0x0, 0x0),
            /* 11 */ Gmp320(1, 0x174876e800, 0x0, 0x0, 0x0, 0x0),
            /* 12 */ Gmp320(1, 0xe8d4a51000, 0x0, 0x0, 0x0, 0x0),
            /* 13 */ Gmp320(1, 0x9184e72a000, 0x0, 0x0, 0x0, 0x0),
            /* 14 */ Gmp320(1, 0x5af3107a4000, 0x0, 0x0, 0x0, 0x0),
            /* 15 */ Gmp320(1, 0x38d7ea4c68000, 0x0, 0x0, 0x0, 0x0),
            /* 16 */ Gmp320(1, 0x2386f26fc10000, 0x0, 0x0, 0x0, 0x0),
            /* 17 */ Gmp320(1, 0x16345785d8a0000, 0x0, 0x0, 0x0, 0x0),
            /* 18 */ Gmp320(1, 0xde0b6b3a7640000, 0x0, 0x0, 0x0, 0x0),
            /* 19 */ Gmp320(1, 0x8ac7230489e80000, 0x0, 0x0, 0x0, 0x0),
            /* 20 */ Gmp320(2, 0x6bc75e2d63100000, 0x5, 0x0, 0x0, 0x0),
            /* 21 */ Gmp320(2, 0x35c9adc5dea00000, 0x36, 0x0, 0x0, 0x0),
            /* 22 */ Gmp320(2, 0x19e0c9bab2400000, 0x21e, 0x0, 0x0, 0x0),
            /* 23 */ Gmp320(2, 0x2c7e14af6800000, 0x152d, 0x0, 0x0, 0x0),
            /* 24 */ Gmp320(2, 0x1bcecceda1000000, 0xd3c2, 0x0, 0x0, 0x0),
            /* 25 */ Gmp320(2, 0x161401484a000000, 0x84595, 0x0, 0x0, 0x0),
            /* 26 */ Gmp320(2, 0xdcc80cd2e4000000, 0x52b7d2, 0x0, 0x0, 0x0),
            /* 27 */ Gmp320(2, 0x9fd0803ce8000000, 0x33b2e3c, 0x0, 0x0, 0x0),
            /* 28 */ Gmp320(2, 0x3e25026110000000, 0x204fce5e, 0x0, 0x0, 0x0),
            /* 29 */ Gmp320(2, 0x6d7217caa0000000, 0x1431e0fae, 0x0, 0x0, 0x0),
            /* 30 */ Gmp320(2, 0x4674edea40000000, 0xc9f2c9cd0, 0x0, 0x0, 0x0),
            /* 31 */ Gmp320(2, 0xc0914b2680000000, 0x7e37be2022, 0x0, 0x0, 0x0),
            /* 32 */ Gmp320(2, 0x85acef8100000000, 0x4ee2d6d415b, 0x0, 0x0, 0x0),
            /* 33 */ Gmp320(2, 0x38c15b0a00000000, 0x314dc6448d93, 0x0, 0x0, 0x0),
            /* 34 */ Gmp320(2, 0x378d8e6400000000, 0x1ed09bead87c0, 0x0, 0x0, 0x0),
            /* 35 */ Gmp320(2, 0x2b878fe800000000, 0x13426172c74d82, 0x0, 0x0, 0x0),
            /* 36 */ Gmp320(2, 0xb34b9f1000000000, 0xc097ce7bc90715, 0x0, 0x0, 0x0),
            /* 37 */ Gmp320(2, 0xf436a000000000, 0x785ee10d5da46d9, 0x0, 0x0, 0x0),
            /* 38 */ Gmp320(2, 0x98a224000000000, 0x4b3b4ca85a86c47a, 0x0, 0x0, 0x0),
            /* 39 */ Gmp320(3, 0x5f65568000000000, 0xf050fe938943acc4, 0x2, 0x0, 0x0),
            /* 40 */ Gmp320(3, 0xb9f5610000000000, 0x6329f1c35ca4bfab, 0x1d, 0x0, 0x0),
        };
        /* clang-format on */
        constexpr int64_t num_power10 = sizeof(kPower10) / sizeof(kPower10[0]);
        if (scale < 0 || scale >= num_power10) {
                return kGmpValueMinus1;
        }
        return kPower10[scale];
}

constexpr inline Gmp320 conv_64_to_gmp320(int64_t i64) {
        Gmp320 gmp;
        if (i64 == 0) {
                gmp.mpz._mp_size = 0;
        } else if (i64 > 0) {
                gmp.mpz._mp_d[0] = static_cast<uint64_t>(i64);
                gmp.mpz._mp_size = 1;
        } else if (i64 < 0) {
                if (i64 == INT64_MIN) {
                        gmp.mpz._mp_d[0] = static_cast<uint64_t>(INT64_MAX) + 1;
                } else {
                        gmp.mpz._mp_d[0] = constexpr_abs(i64);
                }
                gmp.mpz._mp_size = -1;
        } else {
                assert(false);
        }
        return gmp;
}

constexpr inline Gmp320 convert_uint128_to_gmp(__uint128_t u128) {
        Gmp320 gmp;
        auto *mpz = &gmp.mpz;
        if (u128 == 0) {
                mpz->_mp_size = 0;
        } else if (u128 > 0 && u128 <= static_cast<__uint128_t>(UINT64_MAX)) {
                mpz->_mp_d[0] = reinterpret_cast<uint64_t *>(&u128)[0];
                mpz->_mp_size = 1;
        } else if (u128 > static_cast<__uint128_t>(UINT64_MAX)) {
                mpz->_mp_d[0] = reinterpret_cast<uint64_t *>(&u128)[0];
                mpz->_mp_d[1] = reinterpret_cast<uint64_t *>(&u128)[1];
                mpz->_mp_size = 2;
        } else {
                __BIGNUM_ASSERT(false);
        }
        return gmp;
}

constexpr inline Gmp320 conv_128_to_gmp320(__int128_t i128) {
        if (i128 >= 0) {
                return convert_uint128_to_gmp(static_cast<__uint128_t>(i128));
        }

        Gmp320 gmp;
        if (i128 != kInt128Min) {
                __uint128_t positive_i128 = constexpr_abs(i128);
                gmp = convert_uint128_to_gmp(positive_i128);
        } else {
                __uint128_t positive_i128 = static_cast<__uint128_t>(kInt128Max) + 1;
                gmp = convert_uint128_to_gmp(positive_i128);
        }
        gmp.mpz._mp_size = -gmp.mpz._mp_size;
        return gmp;
}

template <typename T, typename U>
inline ErrCode check_gmp_out_of_range(const T &test_value, const U &min_value, const U &max_value) {
        int res = mpz_cmp(&test_value.mpz, &max_value.mpz);
        if (res > 0) {
                return kErr;
        }

        res = mpz_cmp(&test_value.mpz, &min_value.mpz);
        if (res < 0) {
                return kErr;
        }

        return kOk;
}

template <typename T, typename U>
constexpr inline void copy_gmp_to_gmp(T &dst, const U &src) {
        int src_len = constexpr_abs(src.mpz._mp_size);
        __BIGNUM_ASSERT(src_len <= dst.mpz._mp_alloc);
        for (int i = 0; i < src_len; ++i) {
                dst.mpz._mp_d[i] = src.mpz._mp_d[i];
        }
        for (int i = src_len; i < dst.mpz._mp_alloc; ++i) {
                dst.mpz._mp_d[i] = 0;
        }
        dst.mpz._mp_size = src.mpz._mp_size;
}

constexpr inline Gmp640 conv_64_to_gmp640(int64_t i64) {
        Gmp320 res320 = conv_64_to_gmp320(i64);

        Gmp640 res640;
        copy_gmp_to_gmp(res640, res320);
        return res640;
}

constexpr inline Gmp640 conv_128_to_gmp640(__int128_t i128) {
        Gmp320 res320 = conv_128_to_gmp320(i128);

        Gmp640 res640;
        copy_gmp_to_gmp(res640, res320);
        return res640;
}

template <IntegralType T>
constexpr int cmp_integral(T a, T b) {
        if (a < b) {
                return -1;
        } else if (a > b) {
                return 1;
        } else {
                return 0;
        }
}

// Used for decimal comparision where internal integer representation 'a' and 'b' has the same
// scale, but 'a' or 'b' might has "trailing delta value".
//
// A "trailing delta value" take effect when a==b. For example, the original value of "a" is 1.234
// and the original value of "b" is 1.23455555. To compare these 2 decimal values, the original
// "b" is cast to "1234" with scale=3 and have a "delta value" of 55555.
// So now a==b, but the "delta value" of "b" is not zero, so "b" win.
template <IntegralType T>
constexpr int cmp_integral_with_delta(T a, T b, int lr_delta) {
        // lr_delta == 0: a has delta value
        // lr_delta == 1: b has delta value
        assert(lr_delta == 0 || lr_delta == 1);
        if (a == b) {
                if (lr_delta == 0) {  // a has delta value
                        if (a < 0) {
                                return -1;
                        } else {
                                return 1;
                        }
                } else {  // b has delta value
                        if (a < 0) {
                                return 1;
                        } else {
                                return -1;
                        }
                }
        } else if (a < b) {
                return -1;
        } else {
                return 1;
        }
}

template <typename T, typename U>
inline int cmp_gmp(const T &a, const U &b) {
        int res = mpz_cmp(&a.mpz, &b.mpz);
        return res;
}

template <IntegralType T>
constexpr inline ErrCode safe_add(T &res, T lhs, T rhs) {
        // Overflow detection.
        // For GCC, a simple "((lhs + rhs) - lhs) != rhs" is enough, but clang apply optimizations
        // to this expression and it will always return false.
        // So we use the following case-by-case expr to detect overflow.
        if constexpr (sizeof(T) <= 8) {
                long long int i64res = 0;
                if (__builtin_saddll_overflow(lhs, rhs, &i64res)) {
                        return kErr;
                }
                if (!std::is_same_v<int64_t, T> &&
                    (i64res > type_max<T>() || i64res < type_min<T>())) {
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

template <IntegralType T>
constexpr inline ErrCode safe_mul(T &res, T lhs, T rhs) {
        // Overflow detection.
        // For GCC, a simple "(lhs && n128 / lhs != rhs)" is enough, but clang apply optimizations
        // to this expression and it will always return false.
        // So we use the following expression to detect overflow.
        if constexpr (sizeof(T) <= 8) {
                long i64res = 0;
                if (__builtin_smull_overflow(lhs, rhs, &i64res)) {
                        return kErr;
                }
                if (!std::is_same_v<int64_t, T> &&
                    (i64res > type_max<T>() || i64res < type_min<T>())) {
                        return kErr;
                }
                res = i64res;
        } else {
                if (lhs > 0) {
                        if (rhs > 0) {
                                if (lhs > (type_max<T>() / rhs)) {
                                        return kErr;
                                }
                        } else {
                                if (rhs < (type_min<T>() / lhs)) {
                                        return kErr;
                                }
                        }
                } else {
                        if (rhs > 0) {
                                if (lhs < (type_min<T>() / rhs)) {
                                        return kErr;
                                }
                        } else {
                                if ((lhs != 0) && (rhs < (type_max<T>() / lhs))) {
                                        return kErr;
                                }
                        }
                }
                res = lhs * rhs;
        }
        return kOk;
}

template <IntegralType T>
constexpr inline ErrCode decimal_add_integral(T &res, int32_t &res_scale, T lhs, int32_t lscale,
                                              T rhs, int32_t rscale) {
        if (lscale > rscale) {
                T p10 = get_integral_power10<T>(lscale - rscale);
                if (p10 < 0) {
                        return kDecimalAddSubOverflow;
                }
                if (safe_add(res, lhs, rhs * p10)) {
                        return kDecimalAddSubOverflow;
                }
                res_scale = lscale;
        } else {
                T p10 = get_integral_power10<T>(rscale - lscale);
                if (p10 < 0) {
                        return kDecimalAddSubOverflow;
                }
                if (safe_add(res, lhs * p10, rhs)) {
                        return kDecimalAddSubOverflow;
                }
                res_scale = rscale;
        }
        return kOk;
}

template <IntegralType T>
constexpr inline ErrCode decimal_mul_integral(T &res, int32_t &res_scale, T lhs, int32_t lscale,
                                              T rhs, int32_t rscale) {
        ErrCode err = safe_mul(res, lhs, rhs);
        // For int128 or int256 onwards, if overflow, try to trim trailing zeros and multiply again,
        // e.g., 1.000 * 1.000 = 1.000000  =>  1 * 1 = 1
        // This is not done for int64, as division is expensive and overflowing to int128 and then
        // multiplying again is usually more worthwhile.
        if (err != kOk && sizeof(T) > 8) {
                bool ltrim = false;
                while (lscale > 0) {
                        if (lhs % 10 == 0) {
                                lhs /= 10;
                                lscale--;
                                ltrim = true;
                        } else {
                                break;
                        }
                }

                bool rtrim = false;
                while (rscale > 0) {
                        if (rhs % 10 == 0) {
                                rhs /= 10;
                                rscale--;
                                rtrim = true;
                        } else {
                                break;
                        }
                }

                if (ltrim || rtrim) {
                        err = safe_mul(res, lhs, rhs);
                }
        }
        if (err) {
                return kDecimalMulOverflow;
        }

        if (lscale + rscale <= kDecimalMaxScale) {
                res_scale = lscale + rscale;
                return kOk;
        }

        int32_t delta_scale = lscale + rscale - kDecimalMaxScale;
        assert(delta_scale > 0);

        if (delta_scale - 1 > 0) {
                T div_first_part = detail::get_integral_power10<T>(delta_scale - 1);
                res /= div_first_part;
        }

        T mod_result = detail::constexpr_abs(res) % 10;
        res /= 10;

        // round-half-up: round away from zero
        if (mod_result >= 5) {
                if (res > 0) {
                        res += 1;
                } else {
                        res -= 1;
                }
        }
        res_scale = kDecimalMaxScale;
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

                // This is internal function and we have ensured no overflow at the call site.
                // So only check for overflow in debug mode.
#ifndef NDEBUG
                // Debug build: check overflow
                if (safe_mul(v128, v128, static_cast<__int128_t>(10)) ||
                    safe_add(v128, v128, static_cast<__int128_t>(v))) {
                        __BIGNUM_ASSERT(false,
                                        "Overflow detected when converting string to __int128_t");
                }
#else
                v128 = v128 * 10 + v;
#endif
        }
        res = v128;
        return kOk;
}

std::string decimal_64_to_string(int64_t v, int32_t scale);
std::string decimal_128_to_string(__int128_t v, int32_t scale);
std::string my_mpz_to_string(const __mpz_struct *mpz, int32_t scale);
std::string decimal_gmp_to_string(const Gmp320 &v, int32_t scale);
std::string decimal_gmp_to_string(const Gmp640 &v, int32_t scale);

}  // namespace detail

//=-----------------------------------------------------------------------------
// Big decimal implementation similar to MySQL's DECIMAL data type in execution layer.
//
//   - Maximum precision (i.e., maximum total number of digits) is 96 and maximum scale
//     (i.e., maximum number of digits after the decimal point) is 30.
//
//   - Unlike a "DECIMAL" column in database, where you have to specify the precision and
//     scale, this class does not require specifing the precision and scale.
//     The precision and scale are determined by the input value and
//     intermediate calculation result, automatically.
//
//   - Decimal is always signed. It can represent negative numbers.
//
//   - negative scale is NOT supported;
//
//   - calculation overflow would trigger assertion error by default, unless the
//     error-handling interfaces are used;
//
//   - initialization using constructor that
//       * overflows the maximum precision or
//       * results in error (e.g., invalid argument)
//     would trigger assertion.
//     To initialize safely (with error code returned), use the default constructor
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
//     Note that the maxinum precision is 96, so if the input value has more
//     than 96 digits after rounding to scale=7 or scale=16, error or assertion
//     would also be triggered.
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
        constexpr static int32_t kMaxScale = detail::kDecimalMaxScale;
        constexpr static int32_t kMaxPrecision = detail::kDecimalMaxPrecision;

        // Every division increase current scale by 4 until max scale is reached.
        // Note that even when max scale is reached, the scale is still increased
        // before division. Intermediate result would be increased to scale 16+4=20,
        // and then after the calculation, it is decreased to scale 16.
        constexpr static int32_t kDivIncreaseScale = detail::kDecimalDivIncrScale;

       public:
        constexpr DecimalImpl() : m_i64(0), m_padding0{0}, m_dtype(DType::kInt64), m_scale(0) {}

        // Construction using integral value, without scale (scale=0).
        //
        // Constructor for preset scale, e.g., (i=123, scale=2) => 123.00, is not
        // provided on purpose. This is not the same as the database decimal type,
        // which has a fixed scale data type. The scale of this class is dynamic and
        // stored within each object.
        template <SmallIntegralType U>
        constexpr DecimalImpl(U i) : m_i64(i), m_padding0{0}, m_dtype(DType::kInt64), m_scale(0) {}

        template <LargeIntegralType U>
        constexpr DecimalImpl(U i)
                : m_i128(i), m_padding0{0}, m_dtype(DType::kInt128), m_scale(0) {}

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
                __BIGNUM_ASSERT(!err, "Decimal initialization with floating point value overflows");
        }

        // The whole point of this T/U template stuff is for 'static_assert' to generate
        // compile-time error if needed.
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
        constexpr DecimalImpl(std::string_view sv) : m_padding0{0} {
                __BIGNUM_ASSERT(!assign(sv), "Invalid decimal string");
        }
        constexpr DecimalImpl(const char *s) : m_padding0{0} {
                __BIGNUM_ASSERT(!assign(std::string_view(s)), "Invalid decimal string");
        }

        constexpr DecimalImpl(const DecimalImpl &rhs) : m_padding0{0} { copy(rhs); }
        constexpr DecimalImpl(DecimalImpl &&rhs) : m_padding0{0} { copy(rhs); }
        constexpr DecimalImpl &operator=(const DecimalImpl &rhs) {
                copy(rhs);
                return *this;
        }
        constexpr DecimalImpl &operator=(DecimalImpl &&rhs) {
                copy(rhs);
                return *this;
        }

        ~DecimalImpl() = default;

        //=--------------------------------------------------------
        // Assignment/conversion operators.
        // Return error code instead of triggering assertion.
        //=--------------------------------------------------------
        template <IntegralType U>
        constexpr ErrCode assign(U i);

        template <FloatingPointType U>
        /*constexpr*/ ErrCode assign(U &f);

        constexpr ErrCode assign(std::string_view sv);

        //=--------------------------------------------------------
        // Cast operators.
        //=--------------------------------------------------------
        std::string to_string() const;
        explicit operator std::string() const { return to_string(); }
        explicit constexpr operator double() const;
        constexpr operator bool() const;

        // TODO
        // to int64_t and __int128_t

        //=----------------------------------------------------------
        // getters && setters
        //=----------------------------------------------------------
        constexpr int32_t get_scale() const { return m_scale; }
        constexpr bool is_negative() const;

        //=--------------------------------------------------------
        // Arithmetic operators.
        // Trigger assertion on overflow or error.
        //
        // Decimal arithmetic operators, e.g., '+', '-', '*', '/', cannot return error code on
        // overflow, so if overflow they trigger assertion. The is reasonable because Decimal is to
        // used for representing "exact" values such as money, where, precision=96 is large enough
        // for most cases and overflow is not expected.
        //
        // For explicit error handling, use the "add(..)", "sub(..)", "mul(..)", "div(..)"
        // interfaces.
        //=--------------------------------------------------------

        //=-=--------------------------------------------------------
        // operator +=
        //=-=--------------------------------------------------------
        constexpr ErrCode add(const DecimalImpl &rhs);

        constexpr DecimalImpl &operator+=(const DecimalImpl &rhs) {
                ErrCode err = add(rhs);
                __BIGNUM_ASSERT(!err, "Decimal addition overflow");  // FIXME constexpr string with
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
                ErrCode err = sub(rhs);
                __BIGNUM_ASSERT(!err, "Decimal subtraction overflow");  // FIXME constexpr string
                                                                        // with value inside
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
        // the maximum scale (using round-half-up rule) if necessary.
        //=----------------------------------------------------------
        constexpr ErrCode mul(const DecimalImpl &rhs);

        constexpr DecimalImpl &operator*=(const DecimalImpl &rhs) {
                ErrCode err = mul(rhs);
                __BIGNUM_ASSERT(!err, "Decimal multiplication overflow");  // FIXME constexpr string
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
        // In our implementation, there is no 'overflow' for division. There would only be
        // 'division by zero' error. This is unlike primitive integer division,
        // where overflow can happen in case of (INT64_MIN / -1) for int64_t type.
        //
        // In case of division result that cannot be represented exactly, every division would
        // increase the scale by 4 until max scale is reached.
        // For example, 1.28/3.3 => 0.38787878787878793.. => 0.387879, which is 2+4=6 digits
        // after decimal point.
        //
        // Intermediate result might be increased to scale kMaxScale+4+1=35 before rounding,
        // where intermediate result would be calculated using 35 least significant digits.
        // After the division, it is rounded back to maximum scale 30.
        //=----------------------------------------------------------
        constexpr ErrCode div(const DecimalImpl &rhs);

        constexpr DecimalImpl &operator/=(const DecimalImpl &rhs) {
                ErrCode err = div(rhs);
                __BIGNUM_ASSERT(!err,
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
                ErrCode err = mod(rhs);
                __BIGNUM_ASSERT(!err, "DecimalImpl modulo err");
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

        constexpr void sanity_check() const;

       private:
        template <typename U>
        constexpr void store_gmp_value(const U &gv) {
                init_internal_gmp();
                detail::copy_gmp_to_gmp(m_gmp, gv);
                m_dtype = DType::kGmp;
        }

        constexpr void copy(const DecimalImpl &rhs) {
                m_dtype = rhs.m_dtype;
                m_scale = rhs.m_scale;
                if (m_dtype == DType::kInt64) {
                        m_i64 = rhs.m_i64;
                } else if (m_dtype == DType::kInt128) {
                        m_i128 = rhs.m_i128;
                } else {
                        assert(m_dtype == DType::kGmp);
                        store_gmp_value(rhs.m_gmp);
                }
        }

        constexpr ErrCode assign_str_128(const char *start, const char *end);
        constexpr ErrCode assign_str_gmp(const char *start, const char *end);

        constexpr void init_internal_gmp();
        constexpr void negate();

        constexpr ErrCode add_i64_i64(int64_t l64, int32_t lscale, int64_t r64, int32_t rscale);
        constexpr ErrCode add_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                        int32_t rscale);
        // TODO this interface is not good, because it incurs copy
        constexpr ErrCode add_gmp_gmp(detail::Gmp320 l, int32_t lscale, detail::Gmp320 r,
                                      int32_t rscale);

        constexpr ErrCode mul_i64_i64(int64_t l64, int32_t lscale, int64_t r64, int32_t rscale);
        constexpr ErrCode mul_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                        int32_t rscale);

        // TODO this interface is not good, because it incurs copy
        // TODO The previous issues is probably due to the  init_xxx() at the very beginning of the
        //      function.
        constexpr ErrCode mul_gmp_gmp(detail::Gmp320 l, int32_t lscale, detail::Gmp320 r,
                                      int32_t rscale);

        constexpr ErrCode div_i64_i64(int64_t l64, int32_t lscale, int64_t r64, int32_t rscale);
        constexpr ErrCode div_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                        int32_t rscale);
        // TODO this interface is not good, because it incurs copy
        // TODO The previous issues is probably due to the  init_xxx() at the very beginning of the
        //      function.
        constexpr ErrCode div_gmp_gmp(detail::Gmp320 l, int32_t lscale, detail::Gmp320 r,
                                      int32_t rscale);

        constexpr ErrCode mod_i64_i64(int64_t l64, int32_t lscale, int64_t r64, int32_t rscale);
        constexpr ErrCode mod_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                        int32_t rscale);
        // TODO this interface is not good, because it incurs copy
        // TODO The previous issues is probably due to the  init_xxx() at the very beginning of the
        //      function.
        constexpr ErrCode mod_gmp_gmp(detail::Gmp320 l, int32_t lscale, detail::Gmp320 r,
                                      int32_t rscale);

        constexpr int cmp(const DecimalImpl &rhs) const;
        constexpr int cmp_i64_i64(int64_t l64, int32_t lscale, int64_t r64, int32_t rscale) const;
        constexpr int cmp_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                    int32_t rscale) const;
        constexpr int cmp_gmp_gmp(const detail::Gmp320 &l320, int32_t lscale,
                                  const detail::Gmp320 &r320, int32_t rscale) const;

       private:
        enum class DType : uint8_t {
                kInt64 = 0,
                kInt128 = 1,
                kGmp = 2,
        };

        // If a decimal is small enough, we would try to store it in int64_t so that
        // we can use int64_t arithmetic to speed up the calculation. The same for
        // int128_t.
        //
        // However, it is not gaurenteed that the decimal would be stored in its
        // smallest type, meaning that, a decimal that is able to fit in int64_t
        // might be stored in int128_t or gmp struct. We try best to store the decimal in its
        // smallest type, but it is not gaurenteed all the time.
        union {
                struct {
                        union {
                                int64_t m_i64;
                                __int128_t m_i128;
                        };
                        char m_padding0[40];
                        DType m_dtype;
                        int32_t m_scale;
                };

                detail::Gmp320 m_gmp;
        };
};
using Decimal = DecimalImpl<>;
static_assert(sizeof(Decimal) == 64);

template <typename T>
constexpr void DecimalImpl<T>::init_internal_gmp() {
        m_gmp.initialize();
}

template <typename T>
constexpr void DecimalImpl<T>::negate() {
        if (m_dtype == DType::kInt64) {
                if (m_i64 == INT64_MIN) {
                        m_dtype = DType::kInt128;
                        m_i128 = -static_cast<__int128_t>(m_i64);
                } else {
                        m_i64 = -m_i64;
                }
        } else if (m_dtype == DType::kInt128) {
                if (m_i128 == detail::kInt128Min) {
                        store_gmp_value(detail::conv_128_to_gmp320(m_i128));
                } else {
                        m_i128 = -m_i128;
                }
        } else {
                assert(m_dtype == DType::kGmp);
                m_gmp.negate();
        }
}

template <typename T>
template <IntegralType U>
constexpr inline ErrCode DecimalImpl<T>::assign(U i) {
        m_scale = 0;
        if (sizeof(U) <= 8) {
                m_dtype = DType::kInt64;
                m_i64 = i;
        } else {
                m_dtype = DType::kInt128;
                m_i128 = i;
        }

#if ((!defined(NDEBUG)) && defined(BIGNUM_DEV_USE_GMP_ONLY))
        if (m_dtype == DType::kInt64) {
                store_gmp_value(detail::conv_64_to_gmp320(m_i64));
        } else if (m_dtype == DType::kInt128) {
                store_gmp_value(detail::conv_128_to_gmp320(m_i128));
        }
#endif

        return kOk;
}

template <typename T>
template <FloatingPointType U>
/*constexpr*/ inline ErrCode DecimalImpl<T>::assign(U &v) {
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

        // skip trailing space
        while (ptr < end && end[-1] == ' ') {
                end--;
        }

        // skip leading zeros, except for zeros that is the only digit or the ones before decimal
        // point.
        while (ptr + 1 < end && *ptr == '0' && ptr[1] != '.') {
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
        //
        // For int64_t/int128_t, convert it into int128_t and then see whether it fits
        // into a int64_t at the final stage. There is no `assign_str64()` because
        // it is no necessary: the procedure of converting a string into a integer is
        // much more expensive than casting a int128_t into a int64_t.
        ErrCode err = kOk;
        if ((ptr[0] == '-' && slen <= 39) || slen <= 38) {
                err = assign_str_128(ptr, end);
        } else {
                err = assign_str_gmp(ptr, end);
        }
        if (err) {
                return err;
        }

        sanity_check();

#if ((!defined(NDEBUG)) && defined(BIGNUM_DEV_USE_GMP_ONLY))
        if (m_dtype == DType::kInt64) {
                store_gmp_value(detail::conv_64_to_gmp320(m_i64));
        } else if (m_dtype == DType::kInt128) {
                store_gmp_value(detail::conv_128_to_gmp320(m_i128));
        }
#endif

        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::assign_str_128(const char *start, const char *end) {
        // Caller guarantees that
        //   - string is not empty;
        //   - leading/trailing spaces are removed;
        //  - leading zeros are removed, except for zeros that is the only digit or the ones before
        //    decimal point
        //  - string is short enough to fit into int128_t. So we don't need to check overflow here.
        //    (have to check for invalid characters, though)
        // Note that trailing zeros are not removed.
        [[maybe_unused]] const size_t slen = end - start;
        const char *ptr = start;
        assert(slen > 0 && ((ptr[0] == '-' && slen <= 39) || slen <= 38));

        bool is_negative = false;
        if (*ptr == '-') {
                is_negative = true;
                ptr++;
        }
        const char *digit_start = ptr;

        const char *pdot = std::find(ptr, end, '.');
        if (pdot >= end) {
                pdot = nullptr;
        }
        if (pdot) {
                if (pdot == digit_start) {
                        // ".123" and "-.123" are not acceptable.
                        return kInvalidArgument;
                } else if (pdot + 1 >= end) {
                        // The '.' character could not be at the very end,
                        // i.e., '1234.' is not acceptable.
                        return kInvalidArgument;
                }
        }

        // Already limit number of digits in caller, so the significant digits could not
        // overflow. But the significant part might still contains invalid chacters.
        __int128_t significant_v128 = 0;
        ErrCode err = detail::convert_str_to_int128(significant_v128, ptr, (pdot ? pdot : end));
        if (err) {
                return err;
        }

        // Trailing '0' truncation:
        // 123.1000 -> 123.1
        // 123.000 -> 123
        if (pdot) {
                // trim continuous trailing '0'
                const char *pz = end - 1;
                while (pz > pdot && *pz == '0') {
                        --pz;
                }

                if (pz == pdot) {
                        end = pdot;
                        pdot = nullptr;
                } else {
                        end = pz + 1;
                }
        }

        int32_t scale = 0;
        if (pdot) {
                scale = end - (pdot + 1);
                // Num of least significant digits cannot overflow kMaxScale
                if (scale > kMaxScale) {
                        return kDecimalScaleOverflow;
                }

                __int128_t least_significant_v128 = 0;
                // Least significant digits would not overflow __int128_t, but might contains
                // invalid characters.
                err = detail::convert_str_to_int128(least_significant_v128, pdot + 1, end);
                if (err) {
                        return err;
                }

                __int128_t scale_multiplier = detail::get_int128_power10(scale);

                significant_v128 = significant_v128 * scale_multiplier + least_significant_v128;
        }
        m_scale = scale;

        __int128_t res128 = (is_negative ? -significant_v128 : significant_v128);
        if (res128 <= INT64_MAX && res128 >= INT64_MIN) {
                m_dtype = DType::kInt64;
                m_i64 = static_cast<int64_t>(res128);
        } else {
                m_dtype = DType::kInt128;
                m_i128 = res128;
        }
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::assign_str_gmp(const char *start, const char *end) {
        // Caller guarantees that
        //  - string is not empty;
        //  - leading/trailing spaces are removed;
        //  - leading zeros are removed, except for zeros that is the only digit or the ones before
        //    decimal point
        // Note that trailing zeros are not removed.
        const size_t slen = end - start;
        assert(slen > 0);
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
        const char *digit_start = ptr;

        // Copy all the digits out into a buffer for subsequent mpn_set_str().
        // Leading zeros are skipped.
        // At most kMaxPrecision digits. +1 for the '\0'.
        char buf[kMaxPrecision + 1] = {0};
        char *pbuf = buf;
        const char *pdot = nullptr;
        for (; ptr < end; ++ptr) {
                int pv = *ptr;
                if (pv == '.') {
                        pdot = ptr;
                        if (pdot == digit_start) {
                                // ".0123" and "-.0123" are not acceptable.
                                return kInvalidArgument;
                        } else if (pdot + 1 >= end) {
                                // The '.' character could not be at the very end,
                                // i.e., '1234.' is not acceptable.
                                return kInvalidArgument;
                        } else if (end - (pdot + 1) > kMaxScale) {
                                return kInvalidArgument;
                        }
                        continue;
                } else if (pv < '0' || pv > '9') {
                        return kInvalidArgument;
                }
                *pbuf++ = detail::gmp_digit_value_tab[pv];
                num_digits++;

                // The string is too long (more than kMaxPrecision digits) to fit into our
                // representation.
                if (num_digits > kMaxPrecision) {
                        return kInvalidArgument;
                }
        }

        // Trailing '0' truncation:
        // 123.1000 -> 123.1
        // 123.000 -> 123
        if (pdot) {
                unsigned char zero_val = detail::gmp_digit_value_tab[static_cast<int>('0')];
                const char *pz = end - 1;
                while (pz > pdot && *pz == zero_val) {
                        --pz;
                }

                if (pz == pdot) {
                        end = pdot;
                        pdot = nullptr;
                } else {
                        end = pz + 1;
                }
        }

        scale = pdot ? end - (pdot + 1) : 0;

        init_internal_gmp();
        int64_t xsize = mpn_set_str(m_gmp.mpz._mp_d, (unsigned char *)buf, num_digits, /*base*/ 10);
        m_gmp.mpz._mp_size = (is_negative ? -xsize : xsize);

        m_dtype = DType::kGmp;
        m_scale = scale;
        return kOk;
}

template <typename T>
inline std::string DecimalImpl<T>::to_string() const {
        if (m_dtype == DType::kInt64) {
                return detail::decimal_64_to_string(m_i64, m_scale);
        } else if (m_dtype == DType::kInt128) {
                return detail::decimal_128_to_string(m_i128, m_scale);
        } else {
                assert(m_dtype == DType::kGmp);
                return detail::decimal_gmp_to_string(m_gmp, m_scale);
        }
}

template <typename T>
constexpr inline DecimalImpl<T>::operator double() const {
        int scale = m_scale;
        __BIGNUM_ASSERT(scale >= 0);
        if (m_dtype == DType::kInt64) {
                return static_cast<double>(m_i64) / detail::get_int128_power10(scale);
        } else if (m_dtype == DType::kInt128) {
                return static_cast<double>(m_i128) / detail::get_int128_power10(scale);
        } else {
                assert(m_dtype == DType::kGmp);
                double res = mpz_get_d(&m_gmp.mpz);
                while (scale > 0) {
                        int s = detail::constexpr_min(scale, 18);
                        int64_t scale_div = detail::get_int64_power10(s);
                        res /= static_cast<double>(scale_div);
                        scale -= s;
                }
                return res;
        }
}

template <typename T>
constexpr inline bool DecimalImpl<T>::is_negative() const {
        if (m_dtype == DType::kInt64) {
                return m_i64 < 0;
        } else if (m_dtype == DType::kInt128) {
                return m_i128 < 0;
        } else {
                assert(m_dtype == DType::kGmp);
                return m_gmp.is_negative();
        }
}

template <typename T>
constexpr inline DecimalImpl<T>::operator bool() const {
        if (m_dtype == DType::kInt64) {
                return m_i64 != 0;
        } else if (m_dtype == DType::kInt128) {
                return m_i128 != 0;
        } else {
                assert(m_dtype == DType::kGmp);
                bool is_zero = (m_gmp.mpz._mp_size == 0);
                return !is_zero;
        }
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::add_i64_i64(int64_t l64, int32_t lscale, int64_t r64,
                                                     int32_t rscale) {
        int64_t res64 = 0;
        int32_t res_scale = 0;
        ErrCode err = detail::decimal_add_integral(res64, res_scale, l64, lscale, r64, rscale);
        if (err) {
                return err;
        }
        m_dtype = DType::kInt64;
        m_i64 = res64;
        m_scale = res_scale;
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::add_i128_i128(__int128_t l128, int32_t lscale,
                                                       __int128_t r128, int32_t rscale) {
        __int128_t res128 = 0;
        int32_t res_scale = 0;
        ErrCode err = detail::decimal_add_integral(res128, res_scale, l128, lscale, r128, rscale);
        if (err) {
                return err;
        }
        m_dtype = DType::kInt128;
        m_i128 = res128;
        m_scale = res_scale;
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::add_gmp_gmp(detail::Gmp320 l, int32_t lscale,
                                                     detail::Gmp320 r, int32_t rscale) {
        detail::Gmp640 res640;
        if (lscale > rscale) {
                const detail::Gmp320 pow = detail::get_gmp320_power10(lscale - rscale);

                detail::Gmp640 intermediate;
                mpz_mul(&intermediate.mpz, &r.mpz, &pow.mpz);
                mpz_add(&res640.mpz, &intermediate.mpz, &l.mpz);

        } else if (lscale < rscale) {
                const detail::Gmp320 pow = detail::get_gmp320_power10(rscale - lscale);

                detail::Gmp640 intermediate;
                mpz_mul(&intermediate.mpz, &l.mpz, &pow.mpz);
                mpz_add(&res640.mpz, &intermediate.mpz, &r.mpz);
        } else {
                mpz_add(&res640.mpz, &l.mpz, &r.mpz);
        }

        // Check whether the result exceed maximum value of precision kMaxPrecision
        if (detail::check_gmp_out_of_range(res640, detail::kMinGmpValue, detail::kMaxGmpValue)) {
                return kDecimalAddSubOverflow;
        }

        store_gmp_value(res640);
        m_scale = detail::constexpr_max(lscale, rscale);
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::add(const DecimalImpl<T> &rhs) {
        sanity_check();
        rhs.sanity_check();

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

                        err = add_i128_i128(static_cast<__int128_t>(m_i64), m_scale,
                                            static_cast<__int128_t>(rhs.m_i64), rhs.m_scale);
                        __BIGNUM_ASSERT(err == kOk);
                        return kOk;

                } else if (rhs.m_dtype == DType::kInt128) {
                        err = add_i128_i128(static_cast<__int128_t>(m_i64), m_scale, rhs.m_i128,
                                            rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = add_gmp_gmp(detail::conv_64_to_gmp320(m_i64), m_scale,
                                          detail::conv_128_to_gmp320(rhs.m_i128), rhs.m_scale);
                        __BIGNUM_ASSERT(!err);
                        return kOk;

                } else if (rhs.m_dtype == DType::kGmp) {
                        return add_gmp_gmp(detail::conv_64_to_gmp320(m_i64), m_scale, rhs.m_gmp,
                                           rhs.m_scale);

                } else {
                        __BIGNUM_ASSERT(false);
                        return kErr;
                }
        } else if (m_dtype == DType::kInt128) {
                if (rhs.m_dtype == DType::kInt64) {
                        err = add_i128_i128(m_i128, m_scale, static_cast<__int128_t>(rhs.m_i64),
                                            rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = add_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale,
                                          detail::conv_64_to_gmp320(rhs.m_i64), rhs.m_scale);
                        __BIGNUM_ASSERT(!err);
                        return kOk;

                } else if (rhs.m_dtype == DType::kInt128) {
                        err = add_i128_i128(m_i128, m_scale, rhs.m_i128, rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = add_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale,
                                          detail::conv_128_to_gmp320(rhs.m_i128), rhs.m_scale);
                        __BIGNUM_ASSERT(!err);
                        return kOk;

                } else if (rhs.m_dtype == DType::kGmp) {
                        return add_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale, rhs.m_gmp,
                                           rhs.m_scale);

                } else {
                        __BIGNUM_ASSERT(false);
                        return kErr;
                }
        } else if (m_dtype == DType::kGmp) {
                if (rhs.m_dtype == DType::kInt64) {
                        return add_gmp_gmp(m_gmp, m_scale, detail::conv_64_to_gmp320(rhs.m_i64),
                                           rhs.m_scale);

                } else if (rhs.m_dtype == DType::kInt128) {
                        return add_gmp_gmp(m_gmp, m_scale, detail::conv_128_to_gmp320(rhs.m_i128),
                                           rhs.m_scale);

                } else if (rhs.m_dtype == DType::kGmp) {
                        return add_gmp_gmp(m_gmp, m_scale, rhs.m_gmp, rhs.m_scale);

                } else {
                        __BIGNUM_ASSERT(false);
                        return kErr;
                }
        } else {
                __BIGNUM_ASSERT(false);
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
constexpr inline ErrCode DecimalImpl<T>::mul_i64_i64(int64_t l64, int32_t lscale, int64_t r64,
                                                     int32_t rscale) {
        int64_t res64 = 0;
        int32_t res_scale = 0;
        ErrCode err = detail::decimal_mul_integral(res64, res_scale, l64, lscale, r64, rscale);
        if (err) {
                return err;
        }
        m_dtype = DType::kInt64;
        m_i64 = res64;
        m_scale = res_scale;
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::mul_i128_i128(__int128_t l128, int32_t lscale,
                                                       __int128_t r128, int32_t rscale) {
        __int128_t res128 = 0;
        int32_t res_scale = 0;
        ErrCode err = detail::decimal_mul_integral(res128, res_scale, l128, lscale, r128, rscale);
        if (err) {
                return err;
        }
        m_dtype = DType::kInt128;
        m_i128 = res128;
        m_scale = res_scale;
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::mul_gmp_gmp(detail::Gmp320 l, int32_t lscale,
                                                     detail::Gmp320 r, int32_t rscale) {
        __BIGNUM_ASSERT(lscale >= 0 && lscale <= kMaxScale);
        __BIGNUM_ASSERT(rscale >= 0 && rscale <= kMaxScale);

        // kNumLimbs=5 limbs -> 320 bit
        // 2 * 320bit -> 640 bit (80 bytes) -> 10 * int64_t
        detail::Gmp640 res640;
        mpz_mul(&res640.mpz, &l.mpz, &r.mpz);

        if (lscale + rscale > detail::kDecimalMaxScale) {
                bool is_negative = res640.mpz._mp_size < 0;
                res640.mpz._mp_size = detail::constexpr_abs(res640.mpz._mp_size);

                int32_t delta_scale = lscale + rscale - detail::kDecimalMaxScale;
                // Need to do the rounding, so first div by (10 ^ (delta_scale - 1))
                // As both lscale and rscale is <=kDecimalMaxScale, delta_scale is
                // <=kDecimalMaxScale. So, the result of the division is <=10 ^ 30, which can be
                // stored in an int128_t.

                if (delta_scale - 1 > 0) {
                        __int128_t div_first_part = detail::get_int128_power10(delta_scale - 1);
                        detail::Gmp320 divisor = detail::conv_128_to_gmp320(div_first_part);
                        // => res640 /= divisor
                        detail::Gmp640 tmp640;
                        mpz_tdiv_q(&tmp640.mpz, &res640.mpz, &divisor.mpz);
                        res640 = tmp640;
                }

                detail::Gmp640 remainder;
                mpz_tdiv_r_ui(&remainder.mpz, &res640.mpz, 10);

                detail::Gmp640 tmp640;
                mpz_tdiv_q_ui(&tmp640.mpz, &res640.mpz, 10);
                res640 = tmp640;

                int cmp5 = mpz_cmp_ui(&remainder.mpz, 5);
                if (cmp5 >= 0) {
                        mpz_add_ui(&res640.mpz, &res640.mpz, 1);
                }
                if (is_negative) {
                        res640.negate();
                }

                m_scale = detail::kDecimalMaxScale;
        } else {
                m_scale = lscale + rscale;
        }

        if (detail::check_gmp_out_of_range(res640, detail::kMinGmpValue, detail::kMaxGmpValue)) {
                return kDecimalMulOverflow;
        }

        store_gmp_value(res640);
        return kOk;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::mul(const DecimalImpl<T> &rhs) {
        sanity_check();
        rhs.sanity_check();

        ErrCode err = kErr;
        if (m_dtype == DType::kInt64) {
                if (rhs.m_dtype == DType::kInt64) {
                        err = mul_i64_i64(m_i64, m_scale, rhs.m_i64, rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = mul_i128_i128(static_cast<__int128_t>(m_i64), m_scale,
                                            static_cast<__int128_t>(rhs.m_i64), rhs.m_scale);
                        __BIGNUM_ASSERT(err == kOk);
                        return kOk;

                } else if (rhs.m_dtype == DType::kInt128) {
                        err = mul_i128_i128(static_cast<__int128_t>(m_i64), m_scale, rhs.m_i128,
                                            rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = mul_gmp_gmp(detail::conv_64_to_gmp320(m_i64), m_scale,
                                          detail::conv_128_to_gmp320(rhs.m_i128), rhs.m_scale);
                        __BIGNUM_ASSERT(!err);
                        return kOk;

                } else if (rhs.m_dtype == DType::kGmp) {
                        return mul_gmp_gmp(detail::conv_64_to_gmp320(m_i64), m_scale, rhs.m_gmp,
                                           rhs.m_scale);

                } else {
                        __BIGNUM_ASSERT(false);
                        return kErr;
                }
        } else if (m_dtype == DType::kInt128) {
                if (rhs.m_dtype == DType::kInt64) {
                        err = mul_i128_i128(m_i128, m_scale, static_cast<__int128_t>(rhs.m_i64),
                                            rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = mul_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale,
                                          detail::conv_64_to_gmp320(rhs.m_i64), rhs.m_scale);
                        __BIGNUM_ASSERT(!err);
                        return kOk;

                } else if (rhs.m_dtype == DType::kInt128) {
                        err = mul_i128_i128(m_i128, m_scale, rhs.m_i128, rhs.m_scale);
                        if (!err) {
                                return kOk;
                        }

                        err = mul_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale,
                                          detail::conv_128_to_gmp320(rhs.m_i128), rhs.m_scale);
                        __BIGNUM_ASSERT(!err);
                        return err;

                } else if (rhs.m_dtype == DType::kGmp) {
                        return mul_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale, rhs.m_gmp,
                                           rhs.m_scale);

                } else {
                        __BIGNUM_ASSERT(false);
                        return kErr;
                }
        } else if (m_dtype == DType::kGmp) {
                if (rhs.m_dtype == DType::kInt64) {
                        return mul_gmp_gmp(m_gmp, m_scale, detail::conv_64_to_gmp320(rhs.m_i64),
                                           rhs.m_scale);

                } else if (rhs.m_dtype == DType::kInt128) {
                        return mul_gmp_gmp(m_gmp, m_scale, detail::conv_128_to_gmp320(rhs.m_i128),
                                           rhs.m_scale);

                } else if (rhs.m_dtype == DType::kGmp) {
                        return mul_gmp_gmp(m_gmp, m_scale, rhs.m_gmp, rhs.m_scale);

                } else {
                        __BIGNUM_ASSERT(false);
                        return kErr;
                }
        } else {
                __BIGNUM_ASSERT(false);
                return kErr;
        }
        return kErr;
}

template <typename T>
constexpr inline ErrCode DecimalImpl<T>::div(const DecimalImpl<T> &rhs) {
        sanity_check();
        rhs.sanity_check();

        // Division only have 1 case of result in our implementation: div by zero.
        // However, the intermediate calculation might overflow.
        //
        // Since div is slow enough even using primitive type, we could just use GMP
        // for all cases and shrink the result after calculation.
        detail::Gmp320 l320;
        int32_t lscale = m_scale;
        if (m_dtype == DType::kInt64) {
                l320 = detail::conv_64_to_gmp320(m_i64);
        } else if (m_dtype == DType::kInt128) {
                l320 = detail::conv_128_to_gmp320(m_i128);
        } else if (m_dtype == DType::kGmp) {
                l320 = m_gmp;
        } else {
                __BIGNUM_ASSERT(false);
                return kErr;
        }

        detail::Gmp320 r320;
        int32_t rscale = rhs.m_scale;
        if (rhs.m_dtype == DType::kInt64) {
                r320 = detail::conv_64_to_gmp320(rhs.m_i64);
        } else if (rhs.m_dtype == DType::kInt128) {
                r320 = detail::conv_128_to_gmp320(rhs.m_i128);
        } else if (rhs.m_dtype == DType::kGmp) {
                r320 = rhs.m_gmp;
        } else {
                __BIGNUM_ASSERT(false);
                return kErr;
        }

        if (r320.is_zero()) {
                return kDivByZero;
        } else if (l320.is_zero()) {
                m_scale = 0;
                m_dtype = DType::kInt64;
                m_i64 = 0;
                return kOk;
        }

        bool l_negative = l320.is_negative();
        l320.mpz._mp_size = detail::constexpr_abs(l320.mpz._mp_size);

        bool r_negative = r320.is_negative();
        r320.mpz._mp_size = detail::constexpr_abs(r320.mpz._mp_size);

        bool result_negative = (l_negative != r_negative);

        // calculation algorithm:
        //    newl = (l320 * 10 ^ (rscale + kDecimalDivIncrScale + 1))
        //    res640 = newl // rhs
        //    remainder = res640 % 10
        //    res640 /= 10
        //    if (remainder >= 5) {
        //      if (res640 < 0) {
        //         res640 -= 1
        //      } else {
        //         res640 += 1
        //      }
        //    }
        //    res_scale = lscale + kDecimalDivIncrScale
        const detail::Gmp320 mul_rhs =
                detail::get_gmp320_power10(rscale + detail::kDecimalDivIncrScale + 1);
        detail::Gmp640 newl;
        mpz_mul(&newl.mpz, &l320.mpz, &mul_rhs.mpz);

        detail::Gmp640 res640;
        mpz_tdiv_q(&res640.mpz, &newl.mpz, &r320.mpz);

        if (lscale + detail::kDecimalDivIncrScale > detail::kDecimalMaxScale) {
                int trim_scale = lscale + detail::kDecimalDivIncrScale - detail::kDecimalMaxScale;
                detail::Gmp640 tmp;
                detail::Gmp320 trim_scale_pow320 = detail::get_gmp320_power10(trim_scale);
                mpz_tdiv_q(&tmp.mpz, &res640.mpz, &trim_scale_pow320.mpz);
                res640 = tmp;
        }

        detail::Gmp320 remainder;
        mpz_tdiv_r_ui(&remainder.mpz, &res640.mpz, 10);

        detail::Gmp640 tmp;
        mpz_tdiv_q_ui(&tmp.mpz, &res640.mpz, 10);
        res640 = tmp;

        if (mpz_cmp_ui(&remainder.mpz, 5) >= 0) {
                mpz_add_ui(&res640.mpz, &res640.mpz, 1);
        }

        if (result_negative) {
                res640.negate();
        }

        if (detail::check_gmp_out_of_range(res640, detail::kMinGmpValue, detail::kMaxGmpValue)) {
                return kDecimalMulOverflow;
        }

        store_gmp_value(res640);
        m_scale = detail::constexpr_min(detail::kDecimalMaxScale,
                                        lscale + detail::kDecimalDivIncrScale);
        sanity_check();
        return kOk;
}

// modulus rule for negative number:
//   Suppose M is negative number, N is positive or negative, then we have:
//       M % N = M % abs(N) = - (-M % abs(N))
template <typename T>
constexpr inline ErrCode DecimalImpl<T>::mod(const DecimalImpl<T> &rhs) {
        sanity_check();
        rhs.sanity_check();

        detail::Gmp640 l640;
        int32_t lscale = m_scale;
        if (m_dtype == DType::kInt64) {
                l640 = detail::conv_64_to_gmp640(m_i64);
        } else if (m_dtype == DType::kInt128) {
                l640 = detail::conv_128_to_gmp640(m_i128);
        } else if (m_dtype == DType::kGmp) {
                detail::copy_gmp_to_gmp(l640, m_gmp);
        } else {
                __BIGNUM_ASSERT(false);
                return kErr;
        }

        detail::Gmp640 r640;
        int32_t rscale = rhs.m_scale;
        if (rhs.m_dtype == DType::kInt64) {
                r640 = detail::conv_64_to_gmp640(rhs.m_i64);
        } else if (rhs.m_dtype == DType::kInt128) {
                r640 = detail::conv_128_to_gmp640(rhs.m_i128);
        } else if (rhs.m_dtype == DType::kGmp) {
                detail::copy_gmp_to_gmp(r640, rhs.m_gmp);
        } else {
                __BIGNUM_ASSERT(false);
                return kErr;
        }

        if (r640.is_zero()) {
                return kDivByZero;
        } else if (l640.is_zero()) {
                m_scale = 0;
                m_dtype = DType::kInt64;
                m_i64 = 0;
                return kOk;
        }

        bool l_negative = l640.is_negative();
        l640.mpz._mp_size = detail::constexpr_abs(l640.mpz._mp_size);

        // Always mod by posititve number, i.e.,
        // If rhs is negative, we need to calculate -l640 % abs(r640)
        r640.mpz._mp_size = detail::constexpr_abs(r640.mpz._mp_size);

        // First align the scale of two numbers
        if (lscale < rscale) {
                const detail::Gmp320 mul_lhs = detail::get_gmp320_power10(rscale - lscale);
                mpz_mul(&l640.mpz, &l640.mpz, &mul_lhs.mpz);
                lscale = rscale;
        } else if (lscale > rscale) {
                const detail::Gmp320 mul_rhs = detail::get_gmp320_power10(lscale - rscale);
                mpz_mul(&r640.mpz, &r640.mpz, &mul_rhs.mpz);
                rscale = lscale;
        }

        // Get the exact divide result (no fractional part)
        detail::Gmp640 quotient;
        mpz_tdiv_q(&quotient.mpz, &l640.mpz, &r640.mpz);

        // remainder is l640 - quotient * r640
        detail::Gmp640 qr;
        mpz_mul(&qr.mpz, &quotient.mpz, &r640.mpz);

        detail::Gmp640 remainder;
        mpz_sub(&remainder.mpz, &l640.mpz, &qr.mpz);

        if (l_negative) {
                remainder.negate();
        }

        store_gmp_value(remainder);
        m_scale = lscale;

#ifndef NDEBUG
        __BIGNUM_ASSERT(!detail::check_gmp_out_of_range(remainder, detail::kMinGmpValue,
                                                        detail::kMaxGmpValue));
#endif

        return kOk;
}

template <typename T>
constexpr inline int DecimalImpl<T>::cmp_i64_i64(int64_t l64, int32_t lscale, int64_t r64,
                                                 int32_t rscale) const {
        if (l64 < 0 && r64 >= 0) {
                return -1;
        } else if (l64 >= 0 && r64 < 0) {
                return 1;
        }

        if (lscale == rscale) {
                return detail::cmp_integral(l64, r64);
        } else if (rscale > lscale) {
                int32_t scale_diff = rscale - lscale;
                int64_t newl = 0;
                if (!detail::safe_mul(newl, l64, detail::get_int64_power10(scale_diff))) {
                        return detail::cmp_integral(newl, r64);
                }

                // cast to __int128_t and do it again.
                __int128_t newl128;
                if (!detail::safe_mul(newl128, static_cast<__int128_t>(l64),
                                      detail::get_int128_power10(scale_diff))) {
                        return detail::cmp_integral(newl128, static_cast<__int128_t>(r64));
                }

                // otherwise, simply divide the one with larger scale by 10^diff, and compare again.
                int64_t newr = r64 / detail::get_int64_power10(scale_diff);
                return detail::cmp_integral_with_delta(l64, newr, /*lr_delta*/ 1);
        } else {
                assert(rscale < lscale);
                int32_t scale_diff = lscale - rscale;
                int64_t newr = 0;
                if (!detail::safe_mul(newr, r64, detail::get_int64_power10(scale_diff))) {
                        return detail::cmp_integral(l64, newr);
                }

                // cast to __int128_t and do it again.
                __int128_t newr128;
                if (!detail::safe_mul(newr128, static_cast<__int128_t>(r64),
                                      detail::get_int128_power10(scale_diff))) {
                        return detail::cmp_integral(static_cast<__int128_t>(l64), newr128);
                }

                // otherwise, simply divide the one with larger scale by 10^diff, and compare again.
                int64_t newl = l64 / detail::get_int64_power10(scale_diff);
                return detail::cmp_integral_with_delta(newl, r64, /*lr_delta*/ 0);
        }

        __BIGNUM_ASSERT(false);
        return 0;
}

template <typename T>
constexpr inline int DecimalImpl<T>::cmp_i128_i128(__int128_t l128, int32_t lscale, __int128_t r128,
                                                   int32_t rscale) const {
        if (l128 < 0 && r128 >= 0) {
                return -1;
        } else if (l128 >= 0 && r128 < 0) {
                return 1;
        }

        if (lscale == rscale) {
                return detail::cmp_integral(l128, r128);
        } else if (rscale > lscale) {
                // First try to adjust l128 to the same scale as r128 by multiplying 10^diff.
                // If overflow, adjust r128 to the same scale as l128 by dividing 10^diff.
                __int128_t newl = 0;
                if (!detail::safe_mul(newl, l128, detail::get_int128_power10(rscale - lscale))) {
                        return detail::cmp_integral(newl, r128);
                }

                __int128_t newr = r128 / detail::get_int128_power10(rscale - lscale);
                return detail::cmp_integral_with_delta(l128, newr, /*lr_delta*/ 1);
        } else {
                assert(rscale < lscale);
                __int128_t newr = 0;
                if (!detail::safe_mul(newr, r128, detail::get_int128_power10(lscale - rscale))) {
                        return detail::cmp_integral(l128, newr);
                }

                __int128_t newl = l128 / detail::get_int128_power10(lscale - rscale);
                return detail::cmp_integral_with_delta(newl, r128, /*lr_delta*/ 0);
        }
}

template <typename T>
constexpr inline int DecimalImpl<T>::cmp_gmp_gmp(const detail::Gmp320 &l320, int32_t lscale,
                                                 const detail::Gmp320 &r320, int32_t rscale) const {
        if (l320.is_negative() && !r320.is_negative()) {
                return -1;
        } else if (!l320.is_negative() && r320.is_negative()) {
                return 1;
        }

        if (lscale == rscale) {
                return detail::cmp_gmp(l320, r320);
        } else if (rscale > lscale) {
                detail::Gmp640 newl;
                detail::Gmp320 delta_scale_pow320 = detail::get_gmp320_power10(rscale - lscale);
                mpz_mul(&newl.mpz, &l320.mpz, &(delta_scale_pow320.mpz));

                return detail::cmp_gmp(newl, r320);
        } else {
                assert(rscale < lscale);
                detail::Gmp640 newr;
                detail::Gmp320 delta_scale_pow320 = detail::get_gmp320_power10(lscale - rscale);
                mpz_mul(&newr.mpz, &r320.mpz, &(delta_scale_pow320.mpz));

                return detail::cmp_gmp(l320, newr);
        }
}

template <typename T>
constexpr inline int DecimalImpl<T>::cmp(const DecimalImpl &rhs) const {
        sanity_check();
        rhs.sanity_check();

        if (is_negative() && !rhs.is_negative()) {
                return -1;
        } else if (!is_negative() && rhs.is_negative()) {
                return 1;
        }

        int res = 0;
        if (m_dtype == DType::kInt64) {
                if (rhs.m_dtype == DType::kInt64) {
                        res = cmp_i64_i64(m_i64, m_scale, rhs.m_i64, rhs.m_scale);
                } else if (rhs.m_dtype == DType::kInt128) {
                        res = cmp_i128_i128(static_cast<__int128_t>(m_i64), m_scale, rhs.m_i128,
                                            rhs.m_scale);
                } else if (rhs.m_dtype == DType::kGmp) {
                        res = cmp_gmp_gmp(detail::conv_64_to_gmp320(m_i64), m_scale, rhs.m_gmp,
                                          rhs.m_scale);
                } else {
                        __BIGNUM_ASSERT(false);
                }
        } else if (m_dtype == DType::kInt128) {
                if (rhs.m_dtype == DType::kInt64) {
                        res = cmp_i128_i128(m_i128, m_scale, static_cast<__int128_t>(rhs.m_i64),
                                            rhs.m_scale);
                } else if (rhs.m_dtype == DType::kInt128) {
                        res = cmp_i128_i128(m_i128, m_scale, rhs.m_i128, rhs.m_scale);
                } else if (rhs.m_dtype == DType::kGmp) {
                        res = cmp_gmp_gmp(detail::conv_128_to_gmp320(m_i128), m_scale, rhs.m_gmp,
                                          rhs.m_scale);
                } else {
                        __BIGNUM_ASSERT(false);
                }
        } else if (m_dtype == DType::kGmp) {
                if (rhs.m_dtype == DType::kInt64) {
                        res = cmp_gmp_gmp(m_gmp, m_scale, detail::conv_64_to_gmp320(rhs.m_i64),
                                          rhs.m_scale);
                } else if (rhs.m_dtype == DType::kInt128) {
                        res = cmp_gmp_gmp(m_gmp, m_scale, detail::conv_128_to_gmp320(rhs.m_i128),
                                          rhs.m_scale);
                } else if (rhs.m_dtype == DType::kGmp) {
                        res = cmp_gmp_gmp(m_gmp, m_scale, rhs.m_gmp, rhs.m_scale);
                } else {
                        __BIGNUM_ASSERT(false);
                }
        } else {
                __BIGNUM_ASSERT(false);
        }
        return res;
}

template <typename T>
constexpr inline bool DecimalImpl<T>::operator==(const DecimalImpl<T> &rhs) const {
        int res = cmp(rhs);
        return res == 0;
}

template <typename T>
constexpr inline bool DecimalImpl<T>::operator<(const DecimalImpl<T> &rhs) const {
        int res = cmp(rhs);
        return res < 0;
}

template <typename T>
constexpr inline bool DecimalImpl<T>::operator<=(const DecimalImpl<T> &rhs) const {
        int res = cmp(rhs);
        return res <= 0;
}

template <typename T>
constexpr inline void DecimalImpl<T>::sanity_check() const {
#ifndef NDEBUG
        __BIGNUM_ASSERT(m_scale >= 0 && m_scale <= kMaxScale);
        __BIGNUM_ASSERT(m_dtype != DType::kGmp || m_gmp.ptr_check());
#endif
}
}  // namespace bignum
