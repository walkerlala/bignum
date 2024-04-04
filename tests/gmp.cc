#include <gtest/gtest.h>
#include <iostream>

#include "Decimal.h"

namespace bignum {
using namespace detail;

class GmpTest : public ::testing::Test {};

TEST_F(GmpTest, gmp_constant) {
        auto gmp_max = kMaxGmpValue;
        ASSERT_EQ(mpz_to_string(&gmp_max.mpz, /*scale*/ 0),
                  "99999999999999999999999999999999999999999999999999999999999999999");

        auto gmp_min = kMinGmpValue;
        ASSERT_EQ(mpz_to_string(&gmp_min.mpz, /*scale*/ 0),
                  "-99999999999999999999999999999999999999999999999999999999999999999");

        auto gmp_minus1 = kGmpValueMinus1;
        ASSERT_EQ(mpz_to_string(&gmp_minus1.mpz, /*scale*/ 0), "-1");

        auto gmp_v5 = kGmpValue5;
        ASSERT_EQ(mpz_to_string(&gmp_v5.mpz, /*scale*/ 0), "5");

        auto gmp_v10 = kGmpValue10;
        ASSERT_EQ(mpz_to_string(&gmp_v10.mpz, /*scale*/ 0), "10");

        const char *gmp_pow_values[] = {
                /* 0  */ "1",
                /* 1  */ "10",
                /* 2  */ "100",
                /* 3  */ "1000",
                /* 4  */ "10000",
                /* 5  */ "100000",
                /* 6  */ "1000000",
                /* 7  */ "10000000",
                /* 8  */ "100000000",
                /* 9  */ "1000000000",
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
        for (int i = 0; i < 41; i++) {
                auto gmp_v = get_gmp320_power10(i);
                ASSERT_EQ(mpz_to_string(&gmp_v.mpz, /*scale*/ 0), gmp_pow_values[i]);
        }
}

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
