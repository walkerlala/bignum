#include <gtest/gtest.h>
#include <iostream>

#include "Decimal.h"

// TODO test constant
// constexpr auto kMaxGmpValue =
//         GmpWrapper(4, 0xffffffffffffffff, 0x4e3945ef7a253609, 0x1c7fc3908a8bef46, 0xf31627, 0x0);
// constexpr auto kMinGmpValue =
//         GmpWrapper(-4, 0xffffffffffffffff, 0x4e3945ef7a253609, 0x1c7fc3908a8bef46, 0xf31627,
//         0x0);
// constexpr auto kMinus1GmpValue = GmpWrapper(-1, 0x1, 0x0, 0x0, 0x0, 0x0);
//
// TODO test gmp_power10

namespace bignum {
using namespace detail;

class GmpTest : public ::testing::Test {};

TEST_F(GmpTest, init_gmp_with_int64_raw) {
        {
                int64_t i64 = 100;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test;
                gmp_test.mpz._mp_d[0] = std::abs(i64);
                gmp_test.mpz._mp_size = 1;

                ASSERT_EQ(gmp_good, gmp_test);
        }

        {
                int64_t i64 = -100;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test;
                gmp_test.mpz._mp_d[0] = std::abs(i64);
                gmp_test.mpz._mp_size = -1;

                ASSERT_EQ(gmp_good, gmp_test);
        }

        {
                int64_t i64 = INT64_MIN;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test;
                gmp_test.mpz._mp_d[0] = static_cast<uint64_t>(INT64_MAX) + 1;
                gmp_test.mpz._mp_size = -1;

                ASSERT_EQ(gmp_good, gmp_test);
        }
}

TEST_F(GmpTest, init_gmp_with_int64_bignum) {
        {
                int64_t i64 = 100;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test = convert_int64_to_gmp(i64);

                ASSERT_EQ(gmp_good, gmp_test);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "100");
        }

        {
                int64_t i64 = -100;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test = convert_int64_to_gmp(i64);

                ASSERT_EQ(gmp_good, gmp_test);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "-100");
        }

        {
                int64_t i64 = INT64_MIN;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test = convert_int64_to_gmp(i64);

                ASSERT_EQ(gmp_good, gmp_test);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "-9223372036854775808");
        }

        {
                int64_t i64 = INT64_MAX;

                Gmp320 gmp_good;
                mpz_set_si(&gmp_good.mpz, i64);

                Gmp320 gmp_test = convert_int64_to_gmp(i64);

                ASSERT_EQ(gmp_good, gmp_test);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "9223372036854775807");
        }
}

TEST_F(GmpTest, init_gmp_with_int128) {
        {
                __int128_t i128 = 100;
                Gmp320 gmp_test = convert_int128_to_gmp(i128);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "100");
        }

        {
                __int128_t i128 = -100;
                Gmp320 gmp_test = convert_int128_to_gmp(i128);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "-100");
        }

        {
                __int128_t i128 = INT64_MIN;
                Gmp320 gmp_test = convert_int128_to_gmp(i128);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "-9223372036854775808");
        }

        {
                __int128_t i128 = INT64_MAX;
                Gmp320 gmp_test = convert_int128_to_gmp(i128);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0), "9223372036854775807");
        }

        {
                __int128_t i128 = kInt128Min;
                Gmp320 gmp_test = convert_int128_to_gmp(i128);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0),
                          "-170141183460469231731687303715884105728");
        }

        {
                __int128_t i128 = kInt128Max;
                Gmp320 gmp_test = convert_int128_to_gmp(i128);
                ASSERT_EQ(mpz_to_string(&gmp_test.mpz, /*scale*/ 0),
                          "170141183460469231731687303715884105727");
        }
}
}  // namespace bignum
