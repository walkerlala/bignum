#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "gmp.h"

//=--------------------------------------------------------------------
// This program are used to test the generated internal representation of
// the mpz_t structure in the GMP library, to see whether the hardcoded mpz_t
// is correct.
//=--------------------------------------------------------------------

constexpr int32_t kDecimalMaxScale = 30;
constexpr int32_t kDecimalPrecision = 96;

struct GmpWrapper {
        constexpr static size_t kNumLimbs = 5;
        __mpz_struct mpz;
        mp_limb_t limbs[kNumLimbs] = {0, 0, 0, 0, 0};

        GmpWrapper() { init(); }

        GmpWrapper(int sz, uint64_t l0, uint64_t l1, uint64_t l2, uint64_t l3, uint64_t l4)
                : mpz{5, sz, &limbs[0]}, limbs{l0, l1, l2, l3, l4} {}

        constexpr void init() {
                mpz._mp_alloc = 5;
                mpz._mp_size = 0;
                mpz._mp_d = &limbs[0];
        }
};
static_assert(sizeof(GmpWrapper) == 56);

std::string decimal_general_to_string(const GmpWrapper &v, int32_t scale) {
        __mpz_struct mpz = v.mpz;
        if (mpz._mp_size == 0) {
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
        constexpr size_t buf_size = 101;
        char buf[buf_size] = {0};

        bool is_negative = (mpz._mp_size < 0);
        int x_size = std::abs(mpz._mp_size);
        int64_t str_size = mpn_get_str((unsigned char *)buf, /*base*/ 10, mpz._mp_d, x_size);
        assert(str_size > 0 && str_size <= 77);
        assert(str_size >= scale);

        /* Convert result to printable chars.  */
        char res_buf[buf_size] = {0};
        const char *const res_buf_end = res_buf + buf_size;
        char *p = res_buf;
        if (is_negative) {
                *p++ = '-';
        }

        auto to_printable = [](int pos) -> char {
                assert(pos >= 0 && pos <= 9);
                int chr = pos + '0';
                return static_cast<char>(chr);
        };

        int64_t num_most_significant_digits = str_size - scale;
        int64_t num_least_significant_digits = scale;
        if (num_most_significant_digits == 0) {
                *p++ = '0';
                assert(p < res_buf_end);
        } else {
                for (int64_t i = 0; i < num_most_significant_digits; i++) {
                        *p++ = to_printable(buf[i]);
                        assert(p < res_buf_end);
                }
        }
        if (num_least_significant_digits > 0) {
                *p++ = '.';
                for (int64_t i = num_most_significant_digits; i < str_size; i++) {
                        *p++ = to_printable(buf[i]);
                        assert(p < res_buf_end);
                }
        }
        return std::string(res_buf, p);
}

int main() {
        GmpWrapper arr[kDecimalMaxScale + 1] = {
                GmpWrapper(1, 0x1, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0xa, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x64, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x3e8, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x2710, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x186a0, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0xf4240, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x989680, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x5f5e100, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x3b9aca00, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x2540be400, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x174876e800, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0xe8d4a51000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x9184e72a000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x5af3107a4000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x38d7ea4c68000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x2386f26fc10000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x16345785d8a0000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0xde0b6b3a7640000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(1, 0x8ac7230489e80000, 0x0, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x6bc75e2d63100000, 0x5, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x35c9adc5dea00000, 0x36, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x19e0c9bab2400000, 0x21e, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x2c7e14af6800000, 0x152d, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x1bcecceda1000000, 0xd3c2, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x161401484a000000, 0x84595, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0xdcc80cd2e4000000, 0x52b7d2, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x9fd0803ce8000000, 0x33b2e3c, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x3e25026110000000, 0x204fce5e, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x6d7217caa0000000, 0x1431e0fae, 0x0, 0x0, 0x0),
                GmpWrapper(2, 0x4674edea40000000, 0xc9f2c9cd0, 0x0, 0x0, 0x0),
        };

        for (size_t i = 0; i <= kDecimalMaxScale; i++) {
                const GmpWrapper *wrapper = &arr[i];
                assert(wrapper);
                assert(wrapper->mpz._mp_size > 0);
                assert(wrapper->mpz._mp_d);

                std::string str = decimal_general_to_string(*wrapper, 0);
                std::cout << "10^" << i << " = " << str << ", len=" << str.size() << std::endl;
        }
}
