/*
 * This file is part of bignum.
 *
 * bignum is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * bignum is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with bignum.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2024-present  bignum developers
 */
#pragma once

#include <cassert>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <type_traits>

#include "errcode.h"

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
        assert(false);
        std::abort();
}

template <typename T>
inline void __bignum_expr_error([[maybe_unused]] ErrCode err, [[maybe_unused]] T &&msg) {
#ifdef BIGNUM_ENABLE_EXCEPTIONS
        throw DecimalError(err, std::forward<T>(msg));
#else
        assert(false && msg);
        std::abort();
#endif
}
}  // namespace bignum

#define __BIGNUM_GET_MACRO_2(_1, _2, NAME, ...) NAME

// bignum assertion, used for internal sanity checks.
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

// bignum error, used for reporting error.
// trigger exception or abort, depending on the macro BIGNUM_ENABLE_EXCEPTIONS.
#define __BIGNUM_CHECK_ERROR_1(err)                                                       \
        do {                                                                              \
                if (!(err)) {                                                             \
                        if (std::is_constant_evaluated()) {                               \
                                bignum::__bignum_constexpr_evaluation_failure();          \
                        } else {                                                          \
                                bignum::__bignum_expr_error(err, "Decimal error: " #err); \
                        }                                                                 \
                }                                                                         \
        } while (0)

#define __BIGNUM_CHECK_ERROR_2(err, msg)                                         \
        do {                                                                     \
                if (!(err)) {                                                    \
                        if (std::is_constant_evaluated()) {                      \
                                bignum::__bignum_constexpr_evaluation_failure(); \
                        } else {                                                 \
                                bignum::__bignum_expr_error(err, msg);           \
                        }                                                        \
                }                                                                \
        } while (0)

#define __BIGNUM_CHECK_ERROR(...)                                                         \
        __BIGNUM_GET_MACRO_2(__VA_ARGS__, __BIGNUM_CHECK_ERROR_2, __BIGNUM_CHECK_ERROR_1) \
        (__VA_ARGS__)
