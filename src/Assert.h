#pragma once

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <type_traits>

namespace bignum {
// This function is used to trigger a compile-time assertion if the condition is not met in case
// of constexpr context.
inline void __bignum_constexpr_evaluation_failure() {
        // TODO if exception is not allowed, use other methods instead of exception
        throw std::runtime_error("constexpr evaluation failed");
}
}  // namespace bignum

#define __BIGNUM_GET_MACRO_2(_1, _2, NAME, ...) NAME

#define __BIGNUM_ASSERT_1(condition)                                             \
        do {                                                                     \
                if (!(condition)) {                                              \
                        if (std::is_constant_evaluated()) {                      \
                                bignum::__bignum_constexpr_evaluation_failure(); \
                        } else {                                                 \
                                assert(condition);                               \
                                std::abort();                                    \
                        }                                                        \
                }                                                                \
        } while (0)

#define __BIGNUM_ASSERT_2(condition, msg)                                        \
        do {                                                                     \
                if (!(condition)) {                                              \
                        if (std::is_constant_evaluated()) {                      \
                                bignum::__bignum_constexpr_evaluation_failure(); \
                        } else {                                                 \
                                assert(condition);                               \
                                std::abort();                                    \
                        }                                                        \
                }                                                                \
        } while (0)

#define __BIGNUM_ASSERT(...) \
        __BIGNUM_GET_MACRO_2(__VA_ARGS__, __BIGNUM_ASSERT_2, __BIGNUM_ASSERT_1)(__VA_ARGS__)
