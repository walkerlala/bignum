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

#include "gmp.h"

namespace bignum {
namespace detail {
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

// A helper class for 'Decimal' to store the actual limbs of gmp integer and its __mpz_struct
//
// For a mpz integer of kDecimalMaxPrecision=96, only 4 limbs is needed at most.
// However, considering the alignment and padding of the 'Decimal' class (__int128_t is 16 bytes
// aligned), the memory footprint of a 'Decimal' is the same even if we use 5 limbs.
template <size_t N>
struct GmpWrapper {
        constexpr static size_t kNumLimbs = N;
        __mpz_struct mpz;
        mp_limb_t limbs[N];

        constexpr GmpWrapper() : mpz{N, 0, &limbs[0]} {
                for (size_t i = 0; i < N; ++i) {
                        limbs[i] = 0ull;
                }
        }

        template <typename... T>
        constexpr GmpWrapper(int sz, T... args)
                : mpz{N, sz, &limbs[0]}, limbs{static_cast<mp_limb_t>(args)...} {
                static_assert(sizeof...(args) == N, "Invalid number of arguments");
        }

        constexpr GmpWrapper(const GmpWrapper &rhs) : mpz{N, rhs.mpz._mp_size, &limbs[0]} {
                for (size_t i = 0; i < N; ++i) {
                        limbs[i] = rhs.limbs[i];
                }
        }
        constexpr GmpWrapper &operator=(const GmpWrapper &rhs) {
                mpz = __mpz_struct{N, rhs.mpz._mp_size, &limbs[0]};
                for (size_t i = 0; i < N; ++i) {
                        limbs[i] = rhs.limbs[i];
                }
                return *this;
        }
        constexpr GmpWrapper(GmpWrapper &&rhs) : mpz{N, rhs.mpz._mp_size, &limbs[0]} {
                for (size_t i = 0; i < N; ++i) {
                        limbs[i] = rhs.limbs[i];
                }
        }
        constexpr GmpWrapper &operator=(GmpWrapper &&rhs) {
                mpz = __mpz_struct{N, rhs.mpz._mp_size, &limbs[0]};
                for (size_t i = 0; i < N; ++i) {
                        limbs[i] = rhs.limbs[i];
                }
                return *this;
        }

        constexpr bool operator==(const GmpWrapper &rhs) const {
                if (mpz._mp_size != rhs.mpz._mp_size) {
                        return false;
                }
                int sz = (mpz._mp_size < 0 ? -mpz._mp_size : mpz._mp_size);
                for (int i = 0; i < sz; i++) {
                        if (mpz._mp_d[i] != rhs.mpz._mp_d[i]) {
                                return false;
                        }
                }
                return true;
        }

        constexpr bool is_zero() const { return mpz._mp_size == 0; }
        constexpr bool is_negative() const { return mpz._mp_size < 0; }

        constexpr void negate() { mpz._mp_size = -mpz._mp_size; }

        constexpr void initialize() {
                mpz._mp_alloc = N;
                mpz._mp_size = 0;
                mpz._mp_d = &limbs[0];
                for (size_t i = 0; i < N; ++i) {
                        limbs[i] = 0ull;
                }
        }

        constexpr bool ptr_check() const { return (mpz._mp_d == &limbs[0]); }
};

using Gmp320 = GmpWrapper<5>;
// Even though 640bit is enough to hold all intermediate result of Gmp320, some internal gmp
// function might try to realloc for more space if the _mp_size==10 (to make sure that memory
// size is enough for subsequent calculation), which would cause invalid memory access.
// Therefore we add 1 more limbs to avoid this issue.
using Gmp640 = GmpWrapper<11>;
static_assert(sizeof(Gmp320) == 56);

// Maximum/minimum value of precision-96 decimal number
// If kMaxPrecision is changed, this value should be updated accordingly.
constexpr auto kMax96DigitsGmpValue =
        Gmp320(5, 0xffffffffffffffff, 0xe1178e80ffffffff, 0x1c46d01ae478b23b, 0x62e7f4a779f5080f,
               0x77d9d58b62cd8a51);
constexpr auto kMin96DigitsGmpValue =
        Gmp320(-5, 0xffffffffffffffff, 0xe1178e80ffffffff, 0x1c46d01ae478b23b, 0x62e7f4a779f5080f,
               0x77d9d58b62cd8a51);
constexpr auto kGmpValueMinus1 = Gmp320(-1, 0x1, 0x0, 0x0, 0x0, 0x0);
}  // namespace detail
}  // namespace bignum
