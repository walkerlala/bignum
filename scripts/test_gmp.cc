#include <gmp.h>
#include <iostream>
#include <string>
#include <cassert>
#include <cstring>

void func_cdiv() {
    mpz_t dividend, divisor, quotient;
    mpz_init_set_ui(dividend, 123);
    mpz_init_set_ui(divisor, 234);
    mpz_init(quotient);

    mpz_cdiv_q(quotient, dividend, divisor);
    std::cout << "Ceiling division (mpz_cdiv_q) result: ";
    mpz_out_str(stdout, 10, quotient);
    std::cout << std::endl;
    mpz_clears(dividend, divisor, quotient, NULL);
}

void func_fdiv() {
    mpz_t dividend, divisor, quotient;
    mpz_init_set_ui(dividend, 123);
    mpz_init_set_ui(divisor, 234);
    mpz_init(quotient);

    mpz_fdiv_q(quotient, dividend, divisor);
    std::cout << "Flooring division (mpz_fdiv_q) result: ";
    mpz_out_str(stdout, 10, quotient);
    std::cout << std::endl;
    mpz_clears(dividend, divisor, quotient, NULL);
}

void func_tdiv() {
    mpz_t dividend, divisor, quotient;
    mpz_init_set_ui(dividend, 123);
    mpz_init_set_ui(divisor, 234);
    mpz_init(quotient);

    mpz_tdiv_q(quotient, dividend, divisor);
    std::cout << "towards zero division (mpz_tdiv_q) result: ";
    mpz_out_str(stdout, 10, quotient);
    std::cout << std::endl;
    mpz_clears(dividend, divisor, quotient, NULL);
}

template <size_t kNumLimbs>
struct GmpWrapperImpl {
    __mpz_struct mpz;
    mp_limb_t limbs[kNumLimbs];

    constexpr GmpWrapperImpl() : mpz{kNumLimbs, 0, &limbs[0]} {
        for (size_t i = 0; i < kNumLimbs; ++i) {
            limbs[i] = 0ull;
        }
    }

    template <typename... T>
    constexpr GmpWrapperImpl(int sz, T... args)
        : mpz{kNumLimbs, sz, &limbs[0]}, limbs{static_cast<mp_limb_t>(args)...} {
        static_assert(sizeof...(args) == kNumLimbs, "Invalid number of arguments");
    }

    constexpr void negate() { mpz._mp_size = -mpz._mp_size; }

    constexpr void initialize() {
        mpz._mp_alloc = kNumLimbs;
        mpz._mp_size = 0;
        mpz._mp_d = &limbs[0];
    }
};
using GmpWrapper = GmpWrapperImpl<5>;


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

    /* Convert result to printable chars.  */
    char res_buf[buf_size] = {0};
    //const char *const res_buf_end = res_buf + buf_size;
    char *p = res_buf;
    if (is_negative) {
        *p++ = '-';
    }

    auto to_printable = [](int pos) -> char {
        int chr = pos + '0';
        return static_cast<char>(chr);
    };

    int64_t num_most_significant_digits = str_size - scale;
    int64_t num_least_significant_digits = scale;
    if (num_most_significant_digits == 0) {
        *p++ = '0';
    } else {
        for (int64_t i = 0; i < num_most_significant_digits; i++) {
            *p++ = to_printable(buf[i]);
        }
    }
    if (num_least_significant_digits > 0) {
        *p++ = '.';

        for (int64_t i = num_most_significant_digits; i < str_size; i++) {
            *p++ = to_printable(buf[i]);
        }

        // Trim trailing '0'
        char *pdot = p - (str_size - num_most_significant_digits) - 1;
        assert(*pdot == '.');
        for (; p > pdot + 1; p--) {
            if (*(p-1) != '0') {
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


int main() {
    // func_cdiv();
    // func_fdiv();
    // func_tdiv();

    [[maybe_unused]] GmpWrapper a;
    __int128_t x = static_cast<__int128_t>(1000000000099999ll) * static_cast<__int128_t>(100000000000) + static_cast<__int128_t>(99999999999);
    a.initialize();
    a.mpz._mp_size = 2;
    std::memcpy(a.mpz._mp_d, &x, sizeof(x));

    std::cout << decimal_general_to_string(a, 0) << std::endl;
    return 0;
}
