#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

#include "gmp.h"

//=--------------------------------------------------------------------
// This program are used to generate the internal representation of the
// mpz_t structure in the GMP library, which are then hardcoded into
// the Decimal implementation to avoid runtime intialization overhead
//=--------------------------------------------------------------------

constexpr int32_t kDecimalMaxNumPow10 = 40;

/* clang-format off */
/* This array is copied directly from gmp 6.3.0 source code.

   Table to be indexed by character, to get its numerical value.  Assumes ASCII
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

struct GmpWrapper {
        constexpr static size_t kNumLimbs = 5;
        __mpz_struct mpz;
        mp_limb_t limbs[kNumLimbs] = {0, 0, 0, 0, 0};

        GmpWrapper() { init(); }
        constexpr void init() {
                mpz._mp_alloc = 5;
                mpz._mp_size = 0;
                mpz._mp_d = &limbs[0];
        }
};
static_assert(sizeof(GmpWrapper) == 56);

class GmpPow10StrArray {
       public:
        static GmpPow10StrArray &instance() {
                static GmpPow10StrArray instance;
                return instance;
        }

        std::string_view get(size_t scale) const {
                assert(scale <= kDecimalMaxNumPow10);
                size_t dest_len = scale + 1;
                const char *dest_ptr = m_interpreted[scale].data();
                return std::string_view(dest_ptr, dest_len);
        }

       private:
        GmpPow10StrArray() {
                for (size_t i = 0; i <= kDecimalMaxNumPow10; i++) {
                        const char *src_str = get_original_pow10_str(i);
                        assert(src_str);

                        std::array<char, kDecimalMaxNumPow10 + 1> &dest_arr = m_interpreted[i];
                        size_t dest_len = i + 1;

                        for (size_t n = 0; n < dest_len; n++) {
                                dest_arr[n] = gmp_digit_value_tab[static_cast<int>(src_str[n])];
                        }
                }
        }

        constexpr const char *get_original_pow10_str(int32_t scale) {
                /* clang-format off */
        constexpr const char *kPow10StrArray[] = {
            /* 0 */  "1",
            /* 1 */  "10",
            /* 2 */  "100",
            /* 3 */  "1000",
            /* 4 */  "10000",
            /* 5 */  "100000",
            /* 6 */  "1000000",
            /* 7 */  "10000000",
            /* 8 */  "100000000",
            /* 9 */  "1000000000",
            /* 10 */ "10000000000",
            /* 11 */ "100000000000",
            /* 12 */ "1000000000000",
            /* 13 */ "10000000000000",
            /* 14 */ "100000000000000",
            /* 15 */ "1000000000000000",
            /* 16 */ "10000000000000000",
            /* 17 */ "100000000000000000",
            /* 18 */ "1000000000000000000",
            /* 19 */ "10000000000000000000",
            /* 20 */ "100000000000000000000",
            /* 21 */ "1000000000000000000000",
            /* 22 */ "10000000000000000000000",
            /* 23 */ "100000000000000000000000",
            /* 24 */ "1000000000000000000000000",
            /* 25 */ "10000000000000000000000000",
            /* 26 */ "100000000000000000000000000",
            /* 27 */ "1000000000000000000000000000",
            /* 28 */ "10000000000000000000000000000",
            /* 29 */ "100000000000000000000000000000",
            /* 30 */ "1000000000000000000000000000000",
            /* 31 */ "10000000000000000000000000000000",
            /* 32 */ "100000000000000000000000000000000",
            /* 33 */ "1000000000000000000000000000000000",
            /* 34 */ "10000000000000000000000000000000000",
            /* 35 */ "100000000000000000000000000000000000",
            /* 36 */ "1000000000000000000000000000000000000",
            /* 37 */ "10000000000000000000000000000000000000",
            /* 38 */ "100000000000000000000000000000000000000",
            /* 39 */ "1000000000000000000000000000000000000000",
            /* 40 */ "10000000000000000000000000000000000000000",
        };
                /* clang-format on */
                constexpr int64_t num_power10 = sizeof(kPow10StrArray) / sizeof(kPow10StrArray[0]);
                if (scale < 0 || scale >= num_power10) {
                        return nullptr;
                }
                return kPow10StrArray[scale];
        }

       private:
        std::array<std::array<char, kDecimalMaxNumPow10 + 1>, kDecimalMaxNumPow10 + 1>
                m_interpreted;
};

class GmpPow10Array {
       public:
        static GmpPow10Array &instance() {
                static GmpPow10Array instance;
                return instance;
        }

        const GmpWrapper *get(size_t scale) const {
                assert(scale <= kDecimalMaxNumPow10);
                return &m_array[scale];
        }

       private:
        GmpPow10Array() {
                for (size_t i = 0; i <= kDecimalMaxNumPow10; i++) {
                        std::string_view sv = GmpPow10StrArray::instance().get(i);
                        GmpWrapper &wrapper = m_array[i];
                        int64_t xsize = mpn_set_str(wrapper.mpz._mp_d, (unsigned char *)sv.data(),
                                                    sv.size(), /*base*/ 10);
                        assert(xsize >= 0);
                        wrapper.mpz._mp_size = xsize;
                }
        }
        std::array<GmpWrapper, kDecimalMaxNumPow10 + 1> m_array;
};

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
        for (size_t i = 0; i <= kDecimalMaxNumPow10; i++) {
                const GmpWrapper *wrapper = GmpPow10Array::instance().get(i);
                assert(wrapper);
                assert(wrapper->mpz._mp_size > 0);
                assert(wrapper->mpz._mp_d);

                std::string str = decimal_general_to_string(*wrapper, 0);
                std::cout << "10^" << i << " = " << str << ", len=" << str.size() << std::endl;
        }
        for (size_t i = 0; i <= kDecimalMaxNumPow10; i++) {
                const GmpWrapper *wrapper = GmpPow10Array::instance().get(i);
                std::cout << i << ", GmpWrapper(" << wrapper->mpz._mp_size << ", 0x" << std::hex
                          << wrapper->limbs[0] << ", 0x" << wrapper->limbs[1] << ", 0x"
                          << wrapper->limbs[2] << ", 0x" << wrapper->limbs[3] << ", 0x"
                          << wrapper->limbs[4] << ")," << std::dec << std::endl;
        }

        // gmp min value and max value, precision 96
        const char *original_max_val_str =
                "9999999999999999999999999999999999999999999999999999999999999999999999999999999999"
                "99999999999999";
        char max_val_str[100] = {0};
        for (int i = 0; i < 96; i++) {
                max_val_str[i] = gmp_digit_value_tab[static_cast<int>(original_max_val_str[i])];
        }

        GmpWrapper max_val;
        int64_t xsize =
                mpn_set_str(max_val.mpz._mp_d, (unsigned char *)max_val_str, 96, /*base*/ 10);
        assert(xsize >= 0);
        max_val.mpz._mp_size = xsize;

        GmpWrapper min_val = max_val;
        mpz_neg(&min_val.mpz, &min_val.mpz);

        std::cout << "Max_GmpWrapper(" << max_val.mpz._mp_size << ", 0x" << std::hex
                  << max_val.limbs[0] << ", 0x" << max_val.limbs[1] << ", 0x" << max_val.limbs[2]
                  << ", 0x" << max_val.limbs[3] << ", 0x" << max_val.limbs[4] << ")," << std::dec
                  << std::endl;

        std::cout << "Min_GmpWrapper(" << min_val.mpz._mp_size << ", 0x" << std::hex
                  << min_val.limbs[0] << ", 0x" << min_val.limbs[1] << ", 0x" << min_val.limbs[2]
                  << ", 0x" << min_val.limbs[3] << ", 0x" << min_val.limbs[4] << ")," << std::dec
                  << std::endl;

        // gmp -1
        const char *original_minus_one_str = "1";
        char minus_one_str[2] = {0};
        for (int i = 0; i < 1; i++) {
                minus_one_str[i] = gmp_digit_value_tab[static_cast<int>(original_minus_one_str[i])];
        }

        GmpWrapper minus_one;
        xsize = mpn_set_str(minus_one.mpz._mp_d, (unsigned char *)minus_one_str, 1, /*base*/ 10);
        assert(xsize >= 0);
        minus_one.mpz._mp_size = -xsize;

        std::cout << "Minus_1_GmpWrapper(" << minus_one.mpz._mp_size << ", 0x" << std::hex
                  << minus_one.limbs[0] << ", 0x" << minus_one.limbs[1] << ", 0x"
                  << minus_one.limbs[2] << ", 0x" << minus_one.limbs[3] << ", 0x"
                  << minus_one.limbs[4] << ")," << std::dec << std::endl;

        // gmp 10
        GmpWrapper ten;
        mpz_set_ui(&ten.mpz, 10);
        std::cout << "10_GmpWrapper(" << ten.mpz._mp_size << ", 0x" << std::hex << ten.limbs[0]
                  << ", 0x" << ten.limbs[1] << ", 0x" << ten.limbs[2] << ", 0x" << ten.limbs[3]
                  << ", 0x" << ten.limbs[4] << ")," << std::dec << std::endl;

        // gmp 5
        GmpWrapper five;
        mpz_set_ui(&five.mpz, 5);
        std::cout << "5_GmpWrapper(" << five.mpz._mp_size << ", 0x" << std::hex << five.limbs[0]
                  << ", 0x" << five.limbs[1] << ", 0x" << five.limbs[2] << ", 0x" << five.limbs[3]
                  << ", 0x" << five.limbs[4] << ")," << std::dec << std::endl;

        // gmp 4
        mpz_sub_ui(&five.mpz, &five.mpz, 1);
        std::cout << "4_GmpWrapper(" << five.mpz._mp_size << ", 0x" << std::hex << five.limbs[0]
                  << ", 0x" << five.limbs[1] << ", 0x" << five.limbs[2] << ", 0x" << five.limbs[3]
                  << ", 0x" << five.limbs[4] << ")," << std::dec << std::endl;

        // test div by 10
        GmpWrapper v1000;
        mpz_set_ui(&v1000.mpz, 1000);
        mpz_tdiv_q(&v1000.mpz, &v1000.mpz, &ten.mpz);
        std::cout << "1000/10 = " << decimal_general_to_string(v1000, 0) << std::endl;
        // std::cout << "100_GmpWrapper(" << v1000.mpz._mp_size
        //           << ", 0x" << std::hex
        //           << v1000.limbs[0] << ", 0x"
        //           << v1000.limbs[1] << ", 0x"
        //           << v1000.limbs[2] << ", 0x"
        //           << v1000.limbs[3] << ", 0x"
        //           << v1000.limbs[4] << "),"
        //           << std::dec << std::endl;
}
