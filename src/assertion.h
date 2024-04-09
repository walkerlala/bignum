#pragma once

#include <cassert>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <type_traits>

// The bignum library leverage some gcc/clang builtin functions
// and use long/long long/int64_t interchangably.
static_assert(sizeof(long) == 8);
static_assert(sizeof(long long) == 8);

namespace bignum {
// This function is declared as non-constexpr so that when the condition is not met,
// the `if (std::is_constant_evaluated())` branch will be taken and the program will not compile.
inline void __bignum_constexpr_evaluation_failure() {}

template <typename T>
inline void __bignum_runtime_assertion_failure([[maybe_unused]] T &&msg) {
#ifdef BIGNUM_ENABLE_EXCEPTIONS
        throw std::runtime_error(std::forward<T>(msg));
#else
        std::abort();
#endif
}
}  // namespace bignum

#define __BIGNUM_GET_MACRO_2(_1, _2, NAME, ...) NAME

#define __BIGNUM_ASSERT_1(condition)                                             \
        do {                                                                     \
                if (!(condition)) {                                              \
                        if (std::is_constant_evaluated()) {                      \
                                bignum::__bignum_constexpr_evaluation_failure(); \
                        } else {                                                 \
                                bignum::__bignum_runtime_assertion_failure(      \
                                        "Assertion failed: " #condition);        \
                        }                                                        \
                }                                                                \
        } while (0)

#define __BIGNUM_ASSERT_2(condition, msg)                                        \
        do {                                                                     \
                if (!(condition)) {                                              \
                        if (std::is_constant_evaluated()) {                      \
                                bignum::__bignum_constexpr_evaluation_failure(); \
                        } else {                                                 \
                                bignum::__bignum_runtime_assertion_failure(msg); \
                        }                                                        \
                }                                                                \
        } while (0)

#define __BIGNUM_ASSERT(...) \
        __BIGNUM_GET_MACRO_2(__VA_ARGS__, __BIGNUM_ASSERT_2, __BIGNUM_ASSERT_1)(__VA_ARGS__)
