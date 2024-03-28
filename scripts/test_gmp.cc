#include <gmp.h>
#include <iostream>

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

int main() {
    // func_cdiv();
    // func_fdiv();
    // func_tdiv();

    [[maybe_unused]] GmpWrapper a;
    return 0;
}
