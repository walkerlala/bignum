#include "Decimal.h"

#include <cstdint>
#include <string>

namespace bignum {
namespace detail {
template <UnsignedIntegralType T>
static std::string decimal_unsigned_integral_to_string(T v, int32_t scale, bool is_negative) {
        // int128_t -> 39 + 1 + 1 + 1 = 42 (1 for '-', 1 for '.', 1 for '\0')
        // int64_t -> 19 + 1 + 1 + 1 = 22
        //
        // Considering that scale might be equal to the number digits, e.g., 0.12345,
        // where scale is 5, we need to add 1 more digit for the leading 0.
        // So 42 -> 43, 22 -> 23
        constexpr size_t result_buf_size = (sizeof(T) == 16) ? 43 : 22;
        char result_buffer[result_buf_size] = {0};
        char *p = &(result_buffer[0]);
        char *pstart = p;
        const char *pend = p + result_buf_size;

        // 123.10 -> 123.1
        bool has_met_non_zero = false;
        for (int32_t i = 0; i < scale; i++) {
                int64_t remainder = static_cast<int64_t>(v % 10);
                v /= 10;
                if (!remainder && !has_met_non_zero) {
                        continue;
                }
                if (!has_met_non_zero) {
                        has_met_non_zero = true;
                }
                int64_t digit = remainder + '0';
                __BIGNUM_ASSERT(digit >= '0' && digit <= '9');
                *p++ = static_cast<char>(digit);
                __BIGNUM_ASSERT(p < pend);
        }

        // Guarantee at least one '0' after the '.' decimal point.
        // e.g., 123.00 -> 123. -> 123.0
        if (!has_met_non_zero && scale > 0) {
                *p++ = '0';
        }

        // Need decimal points if there is any least significant part
        if (p > pstart) {
                *p++ = '.';
                __BIGNUM_ASSERT(p < pend);
        }

        if (!v) {
                *p++ = '0';
                __BIGNUM_ASSERT(p < pend);
        } else {
                for (; v;) {
                        int64_t remainder = static_cast<int64_t>(v % 10);
                        v /= 10;
                        int64_t digit = remainder + '0';
                        __BIGNUM_ASSERT(digit >= '0' && digit <= '9');
                        *p++ = static_cast<char>(digit);
                        __BIGNUM_ASSERT(p < pend);
                }
        }
        if (is_negative && p > pstart) {
                *p++ = '-';
                __BIGNUM_ASSERT(p < pend);
        }
        std::reverse(pstart, p);
        return std::string(pstart, p);
}

std::string decimal64_to_string(int64_t v, int32_t scale) {
        bool is_negative = (v < 0);
        uint64_t uv = 0;
        if (v == INT64_MIN) {
                uv = static_cast<uint64_t>(INT64_MAX) + 1;
        } else {
                uv = is_negative ? -v : v;
        }
        return decimal_unsigned_integral_to_string(uv, scale, is_negative);
}

std::string decimal128_to_string(__int128_t v, int32_t scale) {
        bool is_negative = (v < 0);
        __uint128_t uv = 0;
        if (v == kInt128Min) {
                uv = static_cast<__uint128_t>(kInt128Max) + 1;
        } else {
                uv = is_negative ? -v : v;
        }
        return decimal_unsigned_integral_to_string(uv, scale, is_negative);
}

std::string mpz_to_string(const __mpz_struct *mpz, int32_t scale) {
        if (mpz->_mp_size == 0) {
                return "0";
        }

        // With kNumLimbs=5 limbs, there is 320 bits (signed).
        // Therefore there is at most 97 digits for this mpz object.
        // +1 for possible '-' sign,
        // +1 for possible '.' sign,
        // +1 for '\0'
        // So this is 100.
        //
        // Considering that scale might be equal to the number digits, e.g., 0.12345,
        // where scale is 5, we need to add 1 more digit for the leading 0.
        // So this is 101.
        constexpr int64_t buf_size = 101;
        char buf[buf_size] = {0};

        bool is_negative = (mpz->_mp_size < 0);
        int x_size = std::abs(mpz->_mp_size);
        // str_size greater than scale: 1.123 -> str_size=4, scale=3
        // str_size equal to scale: 0.123 -> str_size=3, scale=3
        // str_size less than scale: 0.001 -> str_size=1, scale=3
        int64_t str_size = mpn_get_str((unsigned char *)buf, /*base*/ 10, mpz->_mp_d, x_size);
        __BIGNUM_ASSERT(str_size > 0 && str_size < buf_size);

        // Convert result to printable chars
        char res_buf[buf_size] = {0};
        const char *const res_buf_end = res_buf + buf_size;
        char *p = res_buf;
        if (is_negative) {
                *p++ = '-';
        }

        auto to_printable = [](int pos) -> char {
                __BIGNUM_ASSERT(pos >= 0 && pos <= 9);
                int chr = pos + '0';
                return static_cast<char>(chr);
        };

        int64_t num_most_significant_digits = constexpr_max(0, str_size - scale);
        if (num_most_significant_digits == 0) {
                *p++ = '0';
                __BIGNUM_ASSERT(p < res_buf_end);
        } else {
                for (int64_t i = 0; i < num_most_significant_digits; i++) {
                        *p++ = to_printable(buf[i]);
                        __BIGNUM_ASSERT(p < res_buf_end);
                }
        }

        if (scale > 0) {
                *p++ = '.';
                char *pdot = p - 1;

                int64_t least_significant_leading_zeros = constexpr_max(0, scale - str_size);
                for (int64_t i = 0; i < least_significant_leading_zeros; i++) {
                        *p++ = '0';
                        __BIGNUM_ASSERT(p < res_buf_end);
                }

                for (int64_t i = num_most_significant_digits; i < str_size; i++) {
                        *p++ = to_printable(buf[i]);
                        __BIGNUM_ASSERT(p < res_buf_end);
                }

                // Trim trailing '0'
                for (; p > pdot + 1; p--) {
                        if (*(p - 1) != '0') {
                                break;
                        }
                }

                // "1." -> "1"
                if (p == pdot + 1) {
                        p = pdot;
                }
        }

        return std::string(res_buf, p);
}

std::string decimal_general_to_string(const Gmp320 &v, int32_t scale) {
        return mpz_to_string(&(v.mpz), scale);
}

std::string decimal_general_to_string(const Gmp640 &v, int32_t scale) {
        return mpz_to_string(&(v.mpz), scale);
}
}  // namespace detail

}  // namespace bignum
