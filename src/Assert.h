#pragma once

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <type_traits>

/**
 * Common macros used by this project
 */

#define GET_MACRO_2(_1, _2, NAME, ...) NAME
#define GET_MACRO_3(_1, _2, _3, NAME, ...) NAME
#define GET_MACRO_4(_1, _2, _3, _4, NAME, ...) NAME

inline void constexpr_evaluation_failure_marker() {
    throw std::runtime_error("constexpr evaluation failed");
}

#define ASSERT_1(condition)                            \
    do {                                               \
        if (!(condition)) {                            \
            if (std::is_constant_evaluated()) {        \
                constexpr_evaluation_failure_marker(); \
            } else {                                   \
                std::abort();                          \
            }                                          \
        }                                              \
    } while (0)

#define ASSERT_2(condition, msg)                       \
    do {                                               \
        if (!(condition)) {                            \
            if (std::is_constant_evaluated()) {        \
                constexpr_evaluation_failure_marker(); \
            } else {                                   \
                std::abort();                          \
            }                                          \
        }                                              \
    } while (0)

#define ASSERT(...) GET_MACRO_2(__VA_ARGS__, ASSERT_2, ASSERT_1)(__VA_ARGS__)
