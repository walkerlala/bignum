#include <gtest/gtest.h>
#include <iostream>

#include "decimal.h"

#ifdef BIGNUM_DEV_USE_GMP_ONLY
#define BIGNUM_DECIMAL_FIXTURE DecimalTestGmpOnly
#define BIGNUM_TEST_CONSTEXPR
#else
#define BIGNUM_DECIMAL_FIXTURE DecimalTest
#define BIGNUM_TEST_CONSTEXPR constexpr
#endif

namespace bignum {
using namespace detail;

enum class ArithOp {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
};
struct DecimalArithmetic {
        std::string lhs;
        std::string rhs;
        ArithOp op;
        std::string result;
};

enum class CompareOp {
        EQ,
        NE,
        LT,
        LE,
        GT,
        GE,
};
struct DecimalComparison {
        std::string lhs;
        std::string rhs;
        CompareOp op;
        bool result;
        DecimalComparison(std::string lhs, std::string rhs, CompareOp op, bool result)
                : lhs(lhs), rhs(rhs), op(op), result(result) {}
};

class BIGNUM_DECIMAL_FIXTURE : public ::testing::Test {
       protected:
        void DoTestDecimalArithmetic(const std::vector<DecimalArithmetic> &calculations) {
                for (const auto &cals : calculations) {
                        Decimal lhs(cals.lhs);
                        Decimal rhs(cals.rhs);
                        Decimal result;
                        switch (cals.op) {
                                case ArithOp::ADD:
                                        result = lhs + rhs;
                                        break;
                                case ArithOp::SUB:
                                        result = lhs - rhs;
                                        break;
                                case ArithOp::MUL:
                                        result = lhs * rhs;
                                        break;
                                case ArithOp::DIV:
                                        result = lhs / rhs;
                                        break;
                                case ArithOp::MOD:
                                        result = lhs % rhs;
                                        break;
                                default:
                                        __BIGNUM_ASSERT(false);
                                        break;
                        }
                        if (result.to_string() != cals.result) {
                                EXPECT_EQ(result.to_string(), cals.result);
                        }
                }
        }

        void DoTestDecimalComparison(const std::vector<DecimalComparison> &comparisons) {
                for (const auto &comp : comparisons) {
                        Decimal lhs(comp.lhs);
                        Decimal rhs(comp.rhs);
                        bool result;
                        switch (comp.op) {
                                case CompareOp::EQ:
                                        result = lhs == rhs;
                                        break;
                                case CompareOp::NE:
                                        result = lhs != rhs;
                                        break;
                                case CompareOp::LT:
                                        result = lhs < rhs;
                                        break;
                                case CompareOp::LE:
                                        result = lhs <= rhs;
                                        break;
                                case CompareOp::GT:
                                        result = lhs > rhs;
                                        break;
                                case CompareOp::GE:
                                        result = lhs >= rhs;
                                        break;
                                default:
                                        __BIGNUM_ASSERT(false);
                                        break;
                        }
                        if (result != comp.result) {
                                std::cout << "Hello World\n";
                        }
                        if (result != comp.result) {
                                EXPECT_EQ(result, comp.result);
                        }
                }
        }
};

TEST_F(BIGNUM_DECIMAL_FIXTURE, StringConversion) {
        // Simple C string
        EXPECT_EQ(Decimal("0").to_string(), "0");
        EXPECT_EQ(Decimal("0.1").to_string(), "0.1");
        EXPECT_EQ(Decimal("123.1").to_string(), "123.1");
        EXPECT_EQ(Decimal("123.10").to_string(), "123.1");
        EXPECT_EQ(Decimal("-123.10").to_string(), "-123.1");
        EXPECT_EQ(Decimal("123.666").to_string(), "123.666");
        EXPECT_EQ(Decimal("-123.666").to_string(), "-123.666");
        EXPECT_EQ(Decimal("123.000").to_string(), "123");
        EXPECT_EQ(Decimal("-123.000").to_string(), "-123");

        // Batch of c++ string
        std::vector<std::string> dstrings = {
                "0.1",          "0.11223455",    "-0.11223455", "-123.11223455",
                "-44.11223455", "-999.11223455", "12456789",    "101.101",

        };
        for (const auto &str : dstrings) {
                EXPECT_EQ(Decimal(str).to_string(), str);
        }

        // Leading zero truncation
        EXPECT_EQ(Decimal("000.1").to_string(), "0.1");
        EXPECT_EQ(Decimal("00.0000").to_string(), "0");
        EXPECT_EQ(Decimal("00.11223455").to_string(), "0.11223455");
        EXPECT_EQ(Decimal("-00.11223455").to_string(), "-0.11223455");
        EXPECT_EQ(Decimal("-00123.11223455").to_string(), "-123.11223455");
        EXPECT_EQ(Decimal("-0044.11223455").to_string(), "-44.11223455");
        EXPECT_EQ(Decimal("-000999.11223455").to_string(), "-999.11223455");

        // trailing '0' omitted, but not those middle ones
        EXPECT_EQ(Decimal("101.101").to_string(), "101.101");
        EXPECT_EQ(Decimal("-101.101").to_string(), "-101.101");
        EXPECT_EQ(Decimal("101.1010").to_string(), "101.101");
        EXPECT_EQ(Decimal("-101.1010").to_string(), "-101.101");
        EXPECT_EQ(Decimal("200.1000").to_string(), "200.1");
        EXPECT_EQ(Decimal("-200.1000").to_string(), "-200.1");
        EXPECT_EQ(Decimal("0.0000").to_string(), "0");
        EXPECT_EQ(Decimal("-0.0000").to_string(), "0");
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Add) {
        std::vector<DecimalArithmetic> calculations = {
                {"0.12345", "0.54321", ArithOp::ADD, "0.66666"},
                {"123.456", "543.21", ArithOp::ADD, "666.666"},
                {"444.32", "555.123", ArithOp::ADD, "999.443"},
                {"2421341234.133", "123123123.123", ArithOp::ADD, "2544464357.256"},
                {"-0.12345", "-0.54321", ArithOp::ADD, "-0.66666"},
                {"-123.456", "-543.21", ArithOp::ADD, "-666.666"},
                {"-444.32", "-555.123", ArithOp::ADD, "-999.443"},
                {"-2421341234.133", "-123123123.123", ArithOp::ADD, "-2544464357.256"},
                {"-0.12345", "0.54321", ArithOp::ADD, "0.41976"},
                {"-123.456", "543.21", ArithOp::ADD, "419.754"},
                {"-444.32", "555.123", ArithOp::ADD, "110.803"},
                {"-2421341234.133", "123123123.123", ArithOp::ADD, "-2298218111.01"}};
        DoTestDecimalArithmetic(calculations);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Sub) {
        std::vector<DecimalArithmetic> calculations = {
                {"0.12345", "0.54321", ArithOp::SUB, "-0.41976"},
                {"123.456", "543.21", ArithOp::SUB, "-419.754"},
                {"444.32", "555.123", ArithOp::SUB, "-110.803"},
                {"2421341234.133", "123123123.123", ArithOp::SUB, "2298218111.01"},
                {"-0.12345", "-0.54321", ArithOp::SUB, "0.41976"},
                {"-123.456", "-543.21", ArithOp::SUB, "419.754"},
                {"-444.32", "-555.123", ArithOp::SUB, "110.803"},
                {"-2421341234.133", "-123123123.123", ArithOp::SUB, "-2298218111.01"},
                {"-0.12345", "0.54321", ArithOp::SUB, "-0.66666"},
                {"-123.456", "543.21", ArithOp::SUB, "-666.666"},
                {"-444.32", "555.123", ArithOp::SUB, "-999.443"},
                {"-2421341234.133", "123123123.123", ArithOp::SUB, "-2544464357.256"}};
        DoTestDecimalArithmetic(calculations);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Mul) {
        std::vector<DecimalArithmetic> calculations = {
                {"0.12345", "0.54321", ArithOp::MUL, "0.0670592745"},
                {"123.456", "543.21", ArithOp::MUL, "67062.53376"},
                {"444.32", "555.123", ArithOp::MUL, "246652.25136"},
                {"2421341234.133", "123123123.123", ArithOp::MUL, "298123094892954129.157359"},
                {"-0.12345", "-0.54321", ArithOp::MUL, "0.0670592745"},
                {"-123.456", "-543.21", ArithOp::MUL, "67062.53376"},
                {"-444.32", "-555.123", ArithOp::MUL, "246652.25136"},
                {"-2421341234.133", "-123123123.123", ArithOp::MUL, "298123094892954129.157359"},
                {"-0.12345", "0.54321", ArithOp::MUL, "-0.0670592745"},
                {"-123.456", "543.21", ArithOp::MUL, "-67062.53376"},
                {"-444.32", "555.123", ArithOp::MUL, "-246652.25136"},
                {"-2421341234.133", "123123123.123", ArithOp::MUL, "-298123094892954129.157359"}};
        DoTestDecimalArithmetic(calculations);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, StringConstructionOverflow) {
        // String construction of some "large" and "small" values
        {
                BIGNUM_TEST_CONSTEXPR static const char *kDecimalInt128MaxStr =
                        "99999999999999999999999999999999999999";
                BIGNUM_TEST_CONSTEXPR static const char *kDecimalInt128MinStr =
                        "-100000000000000000000000000000000000000";
                Decimal dmax(kDecimalInt128MaxStr);
                EXPECT_EQ(dmax.to_string(), kDecimalInt128MaxStr);

                Decimal dmin(kDecimalInt128MinStr);
                EXPECT_EQ(dmin.to_string(), kDecimalInt128MinStr);
        }

        // 96 digits, should be OK
        {
                const char *PositiveLargeStr =
                        "99999999999999999999999999999999999999999999999999999999999999999999999999"
                        "9999999999999999999999";
                const char *NegativeLargeStr =
                        "-9999999999999999999999999999999999999999999999999999999999999999999999999"
                        "99999999999999999999999";
                Decimal maxv(PositiveLargeStr);
                Decimal minv(NegativeLargeStr);

                EXPECT_EQ(maxv.to_string(), PositiveLargeStr);
                EXPECT_EQ(minv.to_string(), NegativeLargeStr);

                Decimal add_res = maxv + minv;
                EXPECT_EQ(add_res, Decimal("0"));
                EXPECT_EQ(add_res.to_string(), "0");

#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(maxv - minv, testing::KilledBySignal(SIGABRT), "");
                EXPECT_EXIT(maxv * minv, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(maxv - minv, std::runtime_error);
                EXPECT_THROW(maxv * minv, std::runtime_error);
#endif

                Decimal div_res = maxv / minv;
                EXPECT_EQ(div_res, Decimal("-1"));
        }

        // 99 digits, quick and simple, should be rejected.
        {
                const char *PositiveOverflowStr =
                        "10000000000000000000000000000000000000000000000000000000000000000000000000"
                        "0000000000000000000000000";
                const char *NegativeOverflowStr =
                        "-1000000000000000000000000000000000000000000000000000000000000000000000000"
                        "00000000000000000000000000";
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(Decimal{PositiveOverflowStr}, testing::KilledBySignal(SIGABRT), "");
                EXPECT_EXIT(Decimal{NegativeOverflowStr}, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(Decimal{PositiveOverflowStr}, std::runtime_error);
                EXPECT_THROW(Decimal{NegativeOverflowStr}, std::runtime_error);
#endif

                Decimal dd;
                ErrCode err;
                err = dd.assign(PositiveOverflowStr);
                EXPECT_TRUE(!!err);
                err = dd.assign(NegativeOverflowStr);
                EXPECT_TRUE(!!err);
        }

        // 96 digits, with non-zero scale=30 (max scale), should be OK
        {
                const char *PositiveLargeStr =
                        "999999999999999999999999999999999999999999999999999999999999999999."
                        "999999999999999999999999999999";
                const char *NegativeLargeStr =
                        "-999999999999999999999999999999999999999999999999999999999999999999."
                        "999999999999999999999999999999";
                Decimal maxv(PositiveLargeStr);
                Decimal minv(NegativeLargeStr);

                EXPECT_EQ(maxv.to_string(), PositiveLargeStr);
                EXPECT_EQ(minv.to_string(), NegativeLargeStr);

                Decimal add_res = maxv + minv;
                EXPECT_EQ(add_res, Decimal("0"));
                EXPECT_EQ(add_res.to_string(), "0");

#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(maxv - minv, testing::KilledBySignal(SIGABRT), "");
                EXPECT_EXIT(maxv * minv, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(maxv - minv, std::runtime_error);
                EXPECT_THROW(maxv * minv, std::runtime_error);
#endif

                Decimal div_res = maxv / minv;
                EXPECT_EQ(div_res, Decimal("-1"));
        }

        // 96 digits, with scale>kDecimalMaxScale, should be rejected.
        {
                const char *PositiveLargeStr =
                        "99999999999999999999999999999999999999999999999999999999999999999."
                        "9999999999999999999999999999999";
                const char *NegativeLargeStr =
                        "-99999999999999999999999999999999999999999999999999999999999999999."
                        "9999999999999999999999999999999";
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(Decimal{PositiveLargeStr}, testing::KilledBySignal(SIGABRT), "");
                EXPECT_EXIT(Decimal{NegativeLargeStr}, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(Decimal{PositiveLargeStr}, std::runtime_error);
                EXPECT_THROW(Decimal{NegativeLargeStr}, std::runtime_error);
#endif
        }

        // Invalid characters inside decimal string would trigger error.
#ifndef BIGNUM_ENABLE_EXCEPTIONS
        EXPECT_EXIT(Decimal{"1234567890abcdef"}, testing::KilledBySignal(SIGABRT), "");
#else
        EXPECT_THROW(Decimal{"1234567890abcdef"}, std::runtime_error);
#endif

        // Trailing '.' is not acceptable.
#ifndef BIGNUM_ENABLE_EXCEPTIONS
        EXPECT_EXIT(Decimal{"1234567890."}, testing::KilledBySignal(SIGABRT), "");
#else
        EXPECT_THROW(Decimal{"1234567890."}, std::runtime_error);
#endif
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, StringConstructionTrailingZeroTruncation) {
        EXPECT_EQ(Decimal("101.1010").get_scale(), 3);
        EXPECT_EQ(Decimal("101.1010").to_string(), "101.101");
        EXPECT_EQ(Decimal("101.1010"), Decimal("101.101"));

        EXPECT_EQ(Decimal("-101.1010").get_scale(), 3);
        EXPECT_EQ(Decimal("-101.1010").to_string(), "-101.101");
        EXPECT_EQ(Decimal("-101.1010"), Decimal("-101.101"));

        EXPECT_EQ(Decimal("123.0000").get_scale(), 0);
        EXPECT_EQ(Decimal("123.0000").to_string(), "123");
        EXPECT_EQ(Decimal("123").to_string(), "123");
        EXPECT_EQ(Decimal("123.0000"), Decimal("123"));

        EXPECT_EQ(Decimal("-134.0000").get_scale(), 0);
        EXPECT_EQ(Decimal("-134.0000").to_string(), "-134");
        EXPECT_EQ(Decimal("-134").to_string(), "-134");
        EXPECT_EQ(Decimal("-134.0000"), Decimal("-134"));

        EXPECT_EQ(Decimal("0.0000").get_scale(), 0);
        EXPECT_EQ(Decimal("0.0000").to_string(), "0");
        EXPECT_EQ(Decimal("0.0000"), Decimal("0"));

        EXPECT_EQ(Decimal("-0.0000").get_scale(), 0);
        EXPECT_EQ(Decimal("-0.0000").to_string(), "0");
        EXPECT_EQ(Decimal("-0.0000"), Decimal("-0"));
        EXPECT_EQ(Decimal("-0.0000"), Decimal("0"));
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Comparison) {
        // Negative numbers, sorted DESC by its decimal values.
        std::vector<std::string> negatives = {
                "-0.11223455",    "-0.12345",        "-0.12346",      "-0.54321", "-44.11223455",
                "-101.101",       "-101.1020",       "-123.11223455", "-123.456", "-123.666",
                "-444.32",        "-444.33",         "-543.21",       "-555.123", "-999.11223455",
                "-123123123.123", "-2421341234.133",
        };
        {
                std::vector<DecimalComparison> compares;
                for (size_t i = 0; i < negatives.size(); i++) {
                        for (size_t j = 0; j < negatives.size(); j++) {
                                // EQ
                                compares.push_back(DecimalComparison(negatives[i], negatives[j],
                                                                     CompareOp::EQ, i == j));
                                // NE
                                compares.push_back(DecimalComparison(negatives[i], negatives[j],
                                                                     CompareOp::NE, i != j));
                                // LT
                                compares.push_back(DecimalComparison(negatives[i], negatives[j],
                                                                     CompareOp::LT, i > j));
                                // LE
                                compares.push_back(DecimalComparison(negatives[i], negatives[j],
                                                                     CompareOp::LE, i >= j));
                                // GT
                                compares.push_back(DecimalComparison(negatives[i], negatives[j],
                                                                     CompareOp::GT, i < j));
                                // GE
                                compares.push_back(DecimalComparison(negatives[i], negatives[j],
                                                                     CompareOp::GE, i <= j));
                        }
                }
                DoTestDecimalComparison(compares);
        }

        std::vector<std::string> positives = {
                "0",       "0.1",      "0.100001",      "0.11223455",     "0.12345", "0.54321",
                "101.101", "123.1",    "123.456",       "123.666",        "444.32",  "543.21",
                "555.123", "12456789", "123123123.123", "2421341234.133",
        };
        {
                std::vector<DecimalComparison> compares;
                for (size_t i = 0; i < positives.size(); i++) {
                        for (size_t j = 0; j < positives.size(); j++) {
                                // EQ
                                compares.push_back(DecimalComparison(positives[i], positives[j],
                                                                     CompareOp::EQ, i == j));
                                // NE
                                compares.push_back(DecimalComparison(positives[i], positives[j],
                                                                     CompareOp::NE, i != j));
                                // LT
                                compares.push_back(DecimalComparison(positives[i], positives[j],
                                                                     CompareOp::LT, i < j));
                                // LE
                                compares.push_back(DecimalComparison(positives[i], positives[j],
                                                                     CompareOp::LE, i <= j));
                                // GT
                                compares.push_back(DecimalComparison(positives[i], positives[j],
                                                                     CompareOp::GT, i > j));
                                // GE
                                compares.push_back(DecimalComparison(positives[i], positives[j],
                                                                     CompareOp::GE, i >= j));
                        }
                }
                DoTestDecimalComparison(compares);
        }

        // Compare negative with positive numbers
        {
                std::vector<DecimalComparison> compares;
                for (size_t i = 0; i < negatives.size(); i++) {
                        for (size_t j = 0; j < positives.size(); j++) {
                                // EQ
                                compares.push_back(DecimalComparison(negatives[i], positives[j],
                                                                     CompareOp::EQ, false));
                                // NE
                                compares.push_back(DecimalComparison(negatives[i], positives[j],
                                                                     CompareOp::NE, true));
                                // LT
                                compares.push_back(DecimalComparison(negatives[i], positives[j],
                                                                     CompareOp::LT, true));
                                // LE
                                compares.push_back(DecimalComparison(negatives[i], positives[j],
                                                                     CompareOp::LE, true));
                                // GT
                                compares.push_back(DecimalComparison(negatives[i], positives[j],
                                                                     CompareOp::GT, false));
                                // GE
                                compares.push_back(DecimalComparison(negatives[i], positives[j],
                                                                     CompareOp::GE, false));
                        }
                }
        }

        // Compare positive with negative numbers
        {
                std::vector<DecimalComparison> compares;
                for (size_t i = 0; i < positives.size(); i++) {
                        for (size_t j = 0; j < negatives.size(); j++) {
                                // EQ
                                compares.push_back(DecimalComparison(positives[i], negatives[j],
                                                                     CompareOp::EQ, false));
                                // NE
                                compares.push_back(DecimalComparison(positives[i], negatives[j],
                                                                     CompareOp::NE, true));
                                // LT
                                compares.push_back(DecimalComparison(positives[i], negatives[j],
                                                                     CompareOp::LT, false));
                                // LE
                                compares.push_back(DecimalComparison(positives[i], negatives[j],
                                                                     CompareOp::LE, false));
                                // GT
                                compares.push_back(DecimalComparison(positives[i], negatives[j],
                                                                     CompareOp::GT, true));
                                // GE
                                compares.push_back(DecimalComparison(positives[i], negatives[j],
                                                                     CompareOp::GE, true));
                        }
                }
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ComparisonSpecial) {
        // 0 v.s. -0
        EXPECT_EQ((Decimal("0") == Decimal("-0")), true);
        EXPECT_EQ((Decimal("0") < Decimal("-0")), false);
        EXPECT_EQ((Decimal("0") <= Decimal("-0")), true);
        EXPECT_EQ((Decimal("0") > Decimal("-0")), false);
        EXPECT_EQ((Decimal("0") >= Decimal("-0")), true);
        //----------------------------------------------
        EXPECT_EQ((Decimal("0") == Decimal("-0.00")), true);
        EXPECT_EQ((Decimal("0") < Decimal("-0.00")), false);
        EXPECT_EQ((Decimal("0") <= Decimal("-0.00")), true);
        EXPECT_EQ((Decimal("0") > Decimal("-0.00")), false);
        EXPECT_EQ((Decimal("0") >= Decimal("-0.00")), true);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ScaleNarrowDownAfterMultiply) {
        std::vector<DecimalArithmetic> calculations = {
                {"0.4", "0.5", ArithOp::MUL, "0.2"},    {"0.4", "0.6", ArithOp::MUL, "0.24"},
                {"1.4", "1.5", ArithOp::MUL, "2.1"},    {"1.4", "1.6", ArithOp::MUL, "2.24"},
                {"0.1", "0.1", ArithOp::MUL, "0.01"},   {"0.01", "0.1", ArithOp::MUL, "0.001"},

                {"0.40", "0.50", ArithOp::MUL, "0.2"},  {"0.40", "0.60", ArithOp::MUL, "0.24"},
                {"1.40", "1.50", ArithOp::MUL, "2.1"},  {"1.40", "1.60", ArithOp::MUL, "2.24"},
                {"0.10", "0.10", ArithOp::MUL, "0.01"}, {"0.010", "0.10", ArithOp::MUL, "0.001"},

                {"10", "10", ArithOp::MUL, "100"},      {"11", "11", ArithOp::MUL, "121"}};
        DoTestDecimalArithmetic(calculations);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, MulOverflowSignificantDigits) {
#ifndef BIGNUM_ENABLE_EXCEPTIONS
        EXPECT_EXIT(Decimal{detail::kDecimalMaxStr} * Decimal{detail::kDecimalMaxStr},
                    testing::KilledBySignal(SIGABRT), "");
        EXPECT_EXIT(Decimal{detail::kDecimalMinStr} * Decimal{detail::kDecimalMinStr},
                    testing::KilledBySignal(SIGABRT), "");
#else
        EXPECT_THROW(Decimal{detail::kDecimalMaxStr} * Decimal{detail::kDecimalMaxStr},
                     std::runtime_error);
        EXPECT_THROW(Decimal{detail::kDecimalMinStr} * Decimal{detail::kDecimalMinStr},
                     std::runtime_error);
#endif

        // overflow
        {
                const char *str1 = "1000000000000000000000000000000000000000000000000";
                Decimal d1{str1};
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(d1 * d1, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(d1 * d1, std::runtime_error);
#endif
                const char *str2 = "-1000000000000000000000000000000000000000000000000";
                Decimal d2{str2};
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(d2 * d2, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(d2 * d2, std::runtime_error);
#endif
        }

        // not overflow
        {
                const char *str1 = "100000000000000000000000000000000000000000000000";
                Decimal d1{str1};
                EXPECT_EQ((d1 * d1).to_string(),
                          "100000000000000000000000000000000000000000000000000000000000000000000000"
                          "00000000000000000000000");
                const char *str2 = "-100000000000000000000000000000000000000000000000";
                Decimal d2{str2};
                EXPECT_EQ((d2 * d2).to_string(),
                          "100000000000000000000000000000000000000000000000000000000000000000000000"
                          "00000000000000000000000");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, MulOverflowLeastSignificantDigits) {
        {
                Decimal d1("1.123456789123456789123456789555");
                EXPECT_EQ((d1 * d1).to_string(), "1.262155157027587256793019357528");
                EXPECT_EQ((d1 * -d1).to_string(), "-1.262155157027587256793019357528");
                EXPECT_EQ((-d1 * d1).to_string(), "-1.262155157027587256793019357528");
        }

        {
                Decimal d1("1.100000000000001");
                EXPECT_EQ((d1 * d1).to_string(), "1.210000000000002200000000000001");
                EXPECT_EQ((d1 * -d1).to_string(), "-1.210000000000002200000000000001");
                EXPECT_EQ((-d1 * d1).to_string(), "-1.210000000000002200000000000001");
        }
        {
                Decimal d1("1.1000000000000016");
                EXPECT_EQ((d1 * d1).to_string(), "1.210000000000003520000000000003");
                EXPECT_EQ((d1 * -d1).to_string(), "-1.210000000000003520000000000003");
                EXPECT_EQ((-d1 * d1).to_string(), "-1.210000000000003520000000000003");
        }
        {
                Decimal d1("1.1888888888888886");
                EXPECT_EQ((d1 * d1).to_string(), "1.41345679012345610320987654321");
                EXPECT_EQ((d1 * -d1).to_string(), "-1.41345679012345610320987654321");
                EXPECT_EQ((-d1 * d1).to_string(), "-1.41345679012345610320987654321");
        }
        {
                Decimal d1("1.134567900547654");
                EXPECT_EQ((d1 * d1).to_string(), "1.287244320953111297713124903716");
                EXPECT_EQ((d1 * -d1).to_string(), "-1.287244320953111297713124903716");
                EXPECT_EQ((-d1 * d1).to_string(), "-1.287244320953111297713124903716");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Div) {
        std::vector<DecimalArithmetic> calculations = {
                {"1", "3", ArithOp::DIV, "0.3333"},
                {"100000", "3.33", ArithOp::DIV, "30030.03"},
                {"999999", "3.33", ArithOp::DIV, "300300"},
                {"123456", "3.33", ArithOp::DIV, "37073.8739"},

                {"-1", "3", ArithOp::DIV, "-0.3333"},
                {"-100000", "3.33", ArithOp::DIV, "-30030.03"},
                {"-999999", "3.33", ArithOp::DIV, "-300300"},
                {"-123456", "3.33", ArithOp::DIV, "-37073.8739"},

                {"-1", "-3", ArithOp::DIV, "0.3333"},
                {"-100000", "-3.33", ArithOp::DIV, "30030.03"},
                {"-999999", "-3.33", ArithOp::DIV, "300300"},
                {"-123456", "-3.33", ArithOp::DIV, "37073.8739"},

                {"1.00001", "3", ArithOp::DIV, "0.333336667"},
                {"100000.00001", "3.33", ArithOp::DIV, "30030.030033033"},
                {"999999.00001", "3.33", ArithOp::DIV, "300300.000003003"},
                {"123456.00001", "3.33", ArithOp::DIV, "37073.873876877"},

                {"-1.00001", "3", ArithOp::DIV, "-0.333336667"},
                {"-100000.00001", "3.33", ArithOp::DIV, "-30030.030033033"},
                {"-999999.00001", "3.33", ArithOp::DIV, "-300300.000003003"},
                {"-123456.00001", "3.33", ArithOp::DIV, "-37073.873876877"},

                {"1.57565", "3", ArithOp::DIV, "0.525216667"},
                {"100000.57565", "3.33", ArithOp::DIV, "30030.202897898"},
                {"999999.57565", "3.33", ArithOp::DIV, "300300.172867868"},
                {"123456.57565", "3.33", ArithOp::DIV, "37074.046741742"},

                {"-1.57565", "3", ArithOp::DIV, "-0.525216667"},
                {"-100000.57565", "3.33", ArithOp::DIV, "-30030.202897898"},
                {"-999999.57565", "3.33", ArithOp::DIV, "-300300.172867868"},
                {"-123456.57565", "3.33", ArithOp::DIV, "-37074.046741742"},

                {"-1.57565", "-3", ArithOp::DIV, "0.525216667"},
                {"-100000.57565", "-3.33", ArithOp::DIV, "30030.202897898"},
                {"-999999.57565", "-3.33", ArithOp::DIV, "300300.172867868"},
                {"-123456.57565", "-3.33", ArithOp::DIV, "37074.046741742"},

                // DIV -1

                {"1", "-1", ArithOp::DIV, "-1"},
                {"100000", "-1", ArithOp::DIV, "-100000"},
                {"999999", "-1", ArithOp::DIV, "-999999"},
                {"123456", "-1", ArithOp::DIV, "-123456"},

                {"-1", "-1", ArithOp::DIV, "1"},
                {"-100000", "-1", ArithOp::DIV, "100000"},
                {"-999999", "-1", ArithOp::DIV, "999999"},
                {"-123456", "-1", ArithOp::DIV, "123456"},

                {"1.00001", "-1", ArithOp::DIV, "-1.00001"},
                {"100000.00001", "-1", ArithOp::DIV, "-100000.00001"},
                {"999999.00001", "-1", ArithOp::DIV, "-999999.00001"},
                {"123456.00001", "-1", ArithOp::DIV, "-123456.00001"},

                {"-1.00001", "-1", ArithOp::DIV, "1.00001"},
                {"-100000.00001", "-1", ArithOp::DIV, "100000.00001"},
                {"-999999.00001", "-1", ArithOp::DIV, "999999.00001"},
                {"-123456.00001", "-1", ArithOp::DIV, "123456.00001"},

                {"1.57565", "-1", ArithOp::DIV, "-1.57565"},
                {"100000.57565", "-1", ArithOp::DIV, "-100000.57565"},
                {"999999.57565", "-1", ArithOp::DIV, "-999999.57565"},
                {"123456.57565", "-1", ArithOp::DIV, "-123456.57565"},

                {"-1.57565", "-1", ArithOp::DIV, "1.57565"},
                {"-100000.57565", "-1", ArithOp::DIV, "100000.57565"},
                {"-999999.57565", "-1", ArithOp::DIV, "999999.57565"},
                {"-123456.57565", "-1", ArithOp::DIV, "123456.57565"},

                {"1.5756533334441", "3", ArithOp::DIV, "0.5252177778147"},
                {"30030.202898898933", "3.33", ArithOp::DIV, "9018.0789486182981982"},
                {"100000.111111111111111", "3.33", ArithOp::DIV, "30030.0633967300633966967"},
                {"999999.111111111111111", "3.33", ArithOp::DIV, "300300.0333667000333666667"},
                {"123456.111111111111111", "3.33", ArithOp::DIV, "37073.9072405739072405405"},

                {"1.5756533334441", "-3", ArithOp::DIV, "-0.5252177778147"},
                {"30030.202898898933", "-3.33", ArithOp::DIV, "-9018.0789486182981982"},
                {"100000.111111111111111", "-3.33", ArithOp::DIV, "-30030.0633967300633966967"},
                {"999999.111111111111111", "-3.33", ArithOp::DIV, "-300300.0333667000333666667"},
                {"123456.111111111111111", "-3.33", ArithOp::DIV, "-37073.9072405739072405405"},

                {"-1.5756533334441", "-3", ArithOp::DIV, "0.5252177778147"},
                {"-30030.202898898933", "-3.33", ArithOp::DIV, "9018.0789486182981982"},
                {"-100000.111111111111111", "-3.33", ArithOp::DIV, "30030.0633967300633966967"},
                {"-999999.111111111111111", "-3.33", ArithOp::DIV, "300300.0333667000333666667"},
                {"-123456.111111111111111", "-3.33", ArithOp::DIV, "37073.9072405739072405405"},

                // Maxscale exceeded, rounding back to kMaxScale
                {"     1.57565333344415555555599999988", "3.33", ArithOp::DIV,
                 "0.473169169202449115782582582547"},
                {" 30030.20289889893315555555599999988", "3.33", ArithOp::DIV,
                 "9018.078948618298244911578378378342"},
                {"100000.11111111111111155555599999988", "3.33", ArithOp::DIV,
                 "30030.063396730063396863530330330294"},
                {"999999.11111111111111155555599999988", "3.33", ArithOp::DIV,
                 "300300.033366700033366833500300300264"},
                {"123456.11111111111111155555599999988", "3.33", ArithOp::DIV,
                 "37073.907240573907240707374174174138"},

                {"     1.57565333344415555555599999988", "-3.33", ArithOp::DIV,
                 "-0.473169169202449115782582582547"},
                {" 30030.20289889893315555555599999988", "-3.33", ArithOp::DIV,
                 "-9018.078948618298244911578378378342"},
                {"100000.11111111111111155555599999988", "-3.33", ArithOp::DIV,
                 "-30030.063396730063396863530330330294"},
                {"999999.11111111111111155555599999988", "-3.33", ArithOp::DIV,
                 "-300300.033366700033366833500300300264"},
                {"123456.11111111111111155555599999988", "-3.33", ArithOp::DIV,
                 "-37073.907240573907240707374174174138"},

                {"    -1.57565333344415555555599999988", "-3.33", ArithOp::DIV,
                 "0.473169169202449115782582582547"},
                {"-30030.20289889893315555555599999988", "-3.33", ArithOp::DIV,
                 "9018.078948618298244911578378378342"},
                {"-100000.11111111111111155555599999988", "-3.33", ArithOp::DIV,
                 "30030.063396730063396863530330330294"},
                {"-999999.11111111111111155555599999988", "-3.33", ArithOp::DIV,
                 "300300.033366700033366833500300300264"},
                {"-123456.11111111111111155555599999988", "-3.33", ArithOp::DIV,
                 "37073.907240573907240707374174174138"},
        };
        DoTestDecimalArithmetic(calculations);

        // Divison by zero
#ifndef BIGNUM_ENABLE_EXCEPTIONS
        EXPECT_EXIT(Decimal{"1.01"} / Decimal{"0"}, testing::KilledBySignal(SIGABRT), "");
#else
        EXPECT_THROW(Decimal{"1.01"} / Decimal{"0"}, std::runtime_error);
#endif
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Mod) {
        std::vector<DecimalArithmetic> calculations = {
                {"1", "3", ArithOp::MOD, "1"},
                {"100000", "3.33", ArithOp::MOD, "0.1"},
                {"999999", "3.33", ArithOp::MOD, "0"},
                {"123456", "3.33", ArithOp::MOD, "2.91"},
                {"-1", "3", ArithOp::MOD, "-1"},
                {"-100000", "3.33", ArithOp::MOD, "-0.1"},
                {"-999999", "3.33", ArithOp::MOD, "0"},
                {"-123456", "3.33", ArithOp::MOD, "-2.91"},
                {"-1", "-3", ArithOp::MOD, "-1"},
                {"-100000", "-3.33", ArithOp::MOD, "-0.1"},
                {"-999999", "-3.33", ArithOp::MOD, "0"},
                {"-123456", "-3.33", ArithOp::MOD, "-2.91"},
                {"1.00001", "3", ArithOp::MOD, "1.00001"},
                {"100000.00001", "3.33", ArithOp::MOD, "0.10001"},
                {"999999.00001", "3.33", ArithOp::MOD, "0.00001"},
                {"123456.00001", "3.33", ArithOp::MOD, "2.91001"},
                {"-1.00001", "3", ArithOp::MOD, "-1.00001"},
                {"-100000.00001", "3.33", ArithOp::MOD, "-0.10001"},
                {"-999999.00001", "3.33", ArithOp::MOD, "-0.00001"},
                {"-123456.00001", "3.33", ArithOp::MOD, "-2.91001"},
                {"1.57565", "3", ArithOp::MOD, "1.57565"},
                {"100000.57565", "3.33", ArithOp::MOD, "0.67565"},
                {"999999.57565", "3.33", ArithOp::MOD, "0.57565"},
                {"123456.57565", "3.33", ArithOp::MOD, "0.15565"},
                {"-1.57565", "3", ArithOp::MOD, "-1.57565"},
                {"-100000.57565", "3.33", ArithOp::MOD, "-0.67565"},
                {"-999999.57565", "3.33", ArithOp::MOD, "-0.57565"},
                {"-123456.57565", "3.33", ArithOp::MOD, "-0.15565"},
                {"-1.57565", "-3", ArithOp::MOD, "-1.57565"},
                {"-100000.57565", "-3.33", ArithOp::MOD, "-0.67565"},
                {"-999999.57565", "-3.33", ArithOp::MOD, "-0.57565"},
                {"-123456.57565", "-3.33", ArithOp::MOD, "-0.15565"},
                {"1", "-1", ArithOp::MOD, "0"},
                {"100000", "-1", ArithOp::MOD, "0"},
                {"999999", "-1", ArithOp::MOD, "0"},
                {"123456", "-1", ArithOp::MOD, "0"},
                {"-1", "-1", ArithOp::MOD, "0"},
                {"-100000", "-1", ArithOp::MOD, "0"},
                {"-999999", "-1", ArithOp::MOD, "0"},
                {"-123456", "-1", ArithOp::MOD, "0"},
                {"1.00001", "-1", ArithOp::MOD, "0.00001"},
                {"100000.00001", "-1", ArithOp::MOD, "0.00001"},
                {"999999.00001", "-1", ArithOp::MOD, "0.00001"},
                {"123456.00001", "-1", ArithOp::MOD, "0.00001"},
                {"-1.00001", "-1", ArithOp::MOD, "-0.00001"},
                {"-100000.00001", "-1", ArithOp::MOD, "-0.00001"},
                {"-999999.00001", "-1", ArithOp::MOD, "-0.00001"},
                {"-123456.00001", "-1", ArithOp::MOD, "-0.00001"},
                {"1.57565", "-1", ArithOp::MOD, "0.57565"},
                {"100000.57565", "-1", ArithOp::MOD, "0.57565"},
                {"999999.57565", "-1", ArithOp::MOD, "0.57565"},
                {"123456.57565", "-1", ArithOp::MOD, "0.57565"},
                {"-1.57565", "-1", ArithOp::MOD, "-0.57565"},
                {"-100000.57565", "-1", ArithOp::MOD, "-0.57565"},
                {"-999999.57565", "-1", ArithOp::MOD, "-0.57565"},
                {"-123456.57565", "-1", ArithOp::MOD, "-0.57565"},
                {"1.5756533334441", "3", ArithOp::MOD, "1.5756533334441"},
                {"30030.202898898933", "3.33", ArithOp::MOD, "0.262898898933"},
                {"100000.111111111111111", "3.33", ArithOp::MOD, "0.211111111111111"},
                {"999999.111111111111111", "3.33", ArithOp::MOD, "0.111111111111111"},
                {"123456.111111111111111", "3.33", ArithOp::MOD, "3.021111111111111"},
                {"1.5756533334441", "-3", ArithOp::MOD, "1.5756533334441"},
                {"30030.202898898933", "-3.33", ArithOp::MOD, "0.262898898933"},
                {"100000.111111111111111", "-3.33", ArithOp::MOD, "0.211111111111111"},
                {"999999.111111111111111", "-3.33", ArithOp::MOD, "0.111111111111111"},
                {"123456.111111111111111", "-3.33", ArithOp::MOD, "3.021111111111111"},
                {"-1.5756533334441", "-3", ArithOp::MOD, "-1.5756533334441"},
                {"-30030.202898898933", "-3.33", ArithOp::MOD, "-0.262898898933"},
                {"-100000.111111111111111", "-3.33", ArithOp::MOD, "-0.211111111111111"},
                {"-999999.111111111111111", "-3.33", ArithOp::MOD, "-0.111111111111111"},
                {"-123456.111111111111111", "-3.33", ArithOp::MOD, "-3.021111111111111"},
        };
        DoTestDecimalArithmetic(calculations);

        // Modulo by zero
#ifndef BIGNUM_ENABLE_EXCEPTIONS
        EXPECT_EXIT(Decimal{"1.01"} % Decimal{"0"}, testing::KilledBySignal(SIGABRT), "");
#else
        EXPECT_THROW(Decimal{"1.01"} % Decimal{"0"}, std::runtime_error);
#endif
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, DiffSignCompare) {
        std::vector<DecimalComparison> compares = {
                {"123.001", "-432.12", CompareOp::EQ, false},
                {"123.001", "-432.12", CompareOp::NE, true},
                {"123.001", "-432.12", CompareOp::LT, false},
                {"123.001", "-432.12", CompareOp::LE, false},
                {"123.001", "-432.12", CompareOp::GT, true},
                {"123.001", "-432.12", CompareOp::GE, true},
        };
        DoTestDecimalComparison(compares);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, DiffScaleSameSignCompare) {
        std::vector<DecimalComparison> compares = {
                // (30,3) v.s. (19,16)
                //  -> (30+13,3) v.s. (19,16) -> left hand side overflow
                //    -> left side have larger significant part
                //       -> if left is negative, left is smaller
                //       -> else vice versa.
                {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::EQ, false},
                {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::NE, true},
                {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::LT, false},
                {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::LE, false},
                {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::GT, true},
                {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::GE, true},

                {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::EQ, false},
                {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::NE, true},
                {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::LT, true},
                {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::LE, true},
                {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::GT, false},
                {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::GE, false},

                {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::EQ, false},
                {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::NE, true},
                {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::LT, true},
                {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::LE, true},
                {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::GT, false},
                {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::GE, false},

                {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::EQ, false},
                {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::NE, true},
                {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::LT, false},
                {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::LE, false},
                {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::GT, true},
                {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::GE, true},
        };
        DoTestDecimalComparison(compares);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, LargeValueAddOverflow) {
        {
                const char *PositiveLargeStr =
                        "99999999999999999999999999999999999999999999999999999999999999999999999999"
                        "9999999999999999999999";
                Decimal d0(PositiveLargeStr);
                Decimal d1(PositiveLargeStr);
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(d0 + d0, testing::KilledBySignal(SIGABRT), "");
                EXPECT_EXIT(d1 + d1, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(d0 + d0, std::runtime_error);
                EXPECT_THROW(d1 + d1, std::runtime_error);
#endif
        }

        {
                const char *NegativeLargeStr =
                        "-9999999999999999999999999999999999999999999999999999999999999999999999999"
                        "99999999999999999999999";
                Decimal d0(NegativeLargeStr);
                Decimal d1(NegativeLargeStr);
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(d0 + d0, testing::KilledBySignal(SIGABRT), "");
                EXPECT_EXIT(d1 + d1, testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(d0 + d0, std::runtime_error);
                EXPECT_THROW(d1 + d1, std::runtime_error);
#endif
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Int128AddOverflow) {
        __int128_t res128 = 0;
        ErrCode err = kSuccess;

        __int128_t i128max = kInt128Max;
        err = detail::safe_add(res128, i128max, i128max);
        EXPECT_TRUE(!!err);

        __int128_t i128min = kInt128Min;
        err = detail::safe_add(res128, i128min, i128min);
        EXPECT_TRUE(!!err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, safe_mul_int128) {
        __int128_t res128 = 0;
        ErrCode err = kSuccess;

        // +  vs  +, non-overflow
        err = detail::safe_mul(res128, static_cast<__int128_t>(123), static_cast<__int128_t>(456));
        EXPECT_TRUE(!err);
        // +  vs  +, overflow
        err = detail::safe_mul(res128, kInt128Max, kInt128Max);
        EXPECT_TRUE(!!err);
        // +  vs  -, non-overflow
        err = detail::safe_mul(res128, static_cast<__int128_t>(123), static_cast<__int128_t>(-456));
        EXPECT_TRUE(!err);
        // +  vs  -, overflow
        err = detail::safe_mul(res128, kInt128Max, kInt128Min);
        EXPECT_TRUE(!!err);
        // -  vs  +, non-overflow
        err = detail::safe_mul(res128, static_cast<__int128_t>(-123), static_cast<__int128_t>(456));
        EXPECT_TRUE(!err);
        // -  vs  +, overflow
        err = detail::safe_mul(res128, kInt128Min, kInt128Max);
        EXPECT_TRUE(!!err);
        // -  vs  -, non-overflow
        err = detail::safe_mul(res128, static_cast<__int128_t>(-123),
                               static_cast<__int128_t>(-456));
        EXPECT_TRUE(!err);
        // -  vs  -, overflow
        err = detail::safe_mul(res128, kInt128Min, kInt128Min);
        EXPECT_TRUE(!!err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, int128_to_string) {
        EXPECT_EQ(detail::decimal_128_to_string(0, 0), "0");
        EXPECT_EQ(detail::decimal_128_to_string(123, 0), "123");
        EXPECT_EQ(detail::decimal_128_to_string(-123, 0), "-123");
        EXPECT_EQ(detail::decimal_128_to_string(kInt128Max, 0),
                  "170141183460469231731687303715884105727");
        EXPECT_EQ(detail::decimal_128_to_string(kInt128Min, 0),
                  "-170141183460469231731687303715884105728");
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, InitializeDecimalWithEmptyString) {
        Decimal d0("");
        Decimal d1("");
        EXPECT_EQ(d0, d1);
        EXPECT_EQ(d0 + d1, d0);
        EXPECT_EQ(d0 + d1, d1);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, TrailingDot) {
        Decimal d0;
        ErrCode err;

        err = d0.assign("123.0");
        EXPECT_TRUE(!err);

        err = d0.assign("123.");
        EXPECT_TRUE(err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, LargePartOverflow) {
        Decimal d0;
        ErrCode err;

        // Significant digits overflow
        err = d0.assign(
                "1234567890123456789012345679899999999999999999999999012345678901111111111111111111"
                "1111111111111.11");
        EXPECT_TRUE(err);

        // Least significant digits overflow
        err = d0.assign("11.12345678901234567890123456789012345678901");
        EXPECT_TRUE(err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, InvalidCharacters) {
        Decimal d0;
        ErrCode err;

        // Invalid character in significant part.
        err = d0.assign("12345678901abc.11");
        EXPECT_TRUE(err);

        // Invalid character in least significant part.
        err = d0.assign("11.1234567014444abc");
        EXPECT_TRUE(err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, DecimalToStringTrailingLeastSignificantZero) {
        ErrCode err;

        Decimal d0{"124.5"};
        Decimal d1{"123.5"};
        Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "248");

        Decimal d3 = d0 - d1;
        EXPECT_EQ(d3.to_string(), "1");

        err = d0.assign("-124.5");
        EXPECT_TRUE(!err);
        err = d1.assign("123.5");
        EXPECT_TRUE(!err);
        d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "-1");

        d3 = d0 - d1;
        EXPECT_EQ(d3.to_string(), "-248");
}

// Decimal multiply as int128 overflow, but the intermediate result could be held in gmp array.
TEST_F(BIGNUM_DECIMAL_FIXTURE, DecimalMulAsInt128Overflow) {
        {
                Decimal d0{"10000000000.9999999999999999"};
                Decimal d1{"10000000000.9999999999999999"};
                Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "100000000020000000000.9999979999999998");
        }

        {
                Decimal d0{"10000000000.0000000000000004"};
                Decimal d1{"10000000000.0000000000000005"};
                Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "100000000000000000000.000009");
        }
        {
                // Test 5 -> 1 carry.
                Decimal d0{"10000000000.7777777777777777"};
                Decimal d1{"10000000000.7777777777777777"};
                Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "100000000015555555556.160492271604938150617283950617");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, StaticCastToString) {
        // Simple C string
        EXPECT_EQ(static_cast<std::string>(Decimal("0")), "0");
        EXPECT_EQ(static_cast<std::string>(Decimal("0.1")), "0.1");
        EXPECT_EQ(static_cast<std::string>(Decimal("123.1")), "123.1");
        EXPECT_EQ(static_cast<std::string>(Decimal("123.666")), "123.666");
        EXPECT_EQ(static_cast<std::string>(Decimal("-123.666")), "-123.666");

        // Batch of c++ string
        std::vector<std::string> dstrings = {
                "0.1",          "0.11223455",    "-0.11223455", "-123.11223455",
                "-44.11223455", "-999.11223455", "12456789",    "101.101",

        };
        for (const auto &str : dstrings) {
                EXPECT_EQ(static_cast<std::string>(Decimal(str)), str);
        }

        // Leading zero truncation
        EXPECT_EQ(static_cast<std::string>(Decimal("000.1")), "0.1");
        EXPECT_EQ(static_cast<std::string>(Decimal("00.0000")), "0");
        EXPECT_EQ(static_cast<std::string>(Decimal("00.11223455")), "0.11223455");
        EXPECT_EQ(static_cast<std::string>(Decimal("-00.11223455")), "-0.11223455");
        EXPECT_EQ(static_cast<std::string>(Decimal("-00123.11223455")), "-123.11223455");
        EXPECT_EQ(static_cast<std::string>(Decimal("-0044.11223455")), "-44.11223455");
        EXPECT_EQ(static_cast<std::string>(Decimal("-000999.11223455")), "-999.11223455");

        // trailing '0' omitted, but not those middle ones
        EXPECT_EQ(static_cast<std::string>(Decimal("101.101")), "101.101");
        EXPECT_EQ(static_cast<std::string>(Decimal("-101.101")), "-101.101");
        EXPECT_EQ(static_cast<std::string>(Decimal("101.1010")), "101.101");
        EXPECT_EQ(static_cast<std::string>(Decimal("-101.1010")), "-101.101");
        EXPECT_EQ(static_cast<std::string>(Decimal("200.1000")), "200.1");
        EXPECT_EQ(static_cast<std::string>(Decimal("-200.1000")), "-200.1");
        EXPECT_EQ(static_cast<std::string>(Decimal("0.0000")), "0");
        EXPECT_EQ(static_cast<std::string>(Decimal("-0.0000")), "0");
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, DecimalDivAsInt128Overflow) {
        {
                Decimal d0{"9999999999999999999999.22"};
                Decimal d1{"11.9999999999999999"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "833333333333333340277.712778");
        }

        {
                Decimal d0{"-9999999999999999999999.22"};
                Decimal d1{"11.9999999999999999"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "-833333333333333340277.712778");
        }
        {
                Decimal d0{"9999999999999999999999.22"};
                Decimal d1{"-11.9999999999999999"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "-833333333333333340277.712778");
        }
        {
                Decimal d0{"-9999999999999999999999.22"};
                Decimal d1{"-11.9999999999999999"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "833333333333333340277.712778");
        }
        {
                Decimal d0{"9999999999999999999999.2222222222222222"};
                Decimal d1{"11.3333333333333333"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "882352941176470590830.38119953863899263642");
        }
        {
                Decimal d0{"-9999999999999999999999.2222222222222222"};
                Decimal d1{"11.3333333333333333"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "-882352941176470590830.38119953863899263642");
        }
        {
                Decimal d0{"9999999999999999999999.2222222222222222"};
                Decimal d1{"-11.3333333333333333"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "-882352941176470590830.38119953863899263642");
        }
        {
                Decimal d0{"-9999999999999999999999.2222222222222222"};
                Decimal d1{"-11.3333333333333333"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "882352941176470590830.38119953863899263642");
        }

        {
                Decimal d0{"9999999999999999999999.9999999999999999"};
                Decimal d1{"11.1111111111111111"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "900000000000000000900.000000000000000891");
        }
        {
                Decimal d0{"-9999999999999999999999.9999999999999999"};
                Decimal d1{"11.1111111111111111"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "-900000000000000000900.000000000000000891");
        }
        {
                Decimal d0{"9999999999999999999999.9999999999999999"};
                Decimal d1{"-11.1111111111111111"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "-900000000000000000900.000000000000000891");
        }
        {
                Decimal d0{"-9999999999999999999999.9999999999999999"};
                Decimal d1{"-11.1111111111111111"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "900000000000000000900.000000000000000891");
        }

        // Test trailing zero stripped.
        {
                Decimal d0{"1000000000000000000000.8888888888888883"};
                Decimal d1{"10.2222222222222222"};
                Decimal d3 = d0 / d1;
                EXPECT_EQ(d3.to_string(), "97826086956521739343.18714555765595503628");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, StaticCastToDouble) {
        // Simple C string
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("0")), 0.0);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("0.1")), 0.1);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("123.1")), 123.1);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("123.666")), 123.666);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-123.666")), -123.666);

        // Leading zero truncation
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("000.1")), 0.1);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("00.0000")), 0.0);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("00.11223455")), 0.11223455);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-00.11223455")), -0.11223455);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-00123.11223455")), -123.11223455);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-0044.11223455")), -44.11223455);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-000999.11223455")), -999.11223455);

        // trailing '0' omitted, but not those middle ones
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("101.101")), 101.101);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-101.101")), -101.101);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("101.1010")), 101.101);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-101.1010")), -101.101);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("200.1000")), 200.1);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-200.1000")), -200.1);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("0.0000")), 0.0);
        EXPECT_DOUBLE_EQ(static_cast<double>(Decimal("-0.0000")), 0.0);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, SmallNumberConstexprInitializationOK) {
        {
                // Simple C string
                BIGNUM_TEST_CONSTEXPR Decimal d0("0");
                EXPECT_EQ(d0.to_string(), "0");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("0.1");
                EXPECT_EQ(d0.to_string(), "0.1");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("123.1");
                EXPECT_EQ(d0.to_string(), "123.1");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("123.666");
                EXPECT_EQ(d0.to_string(), "123.666");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.666");
                EXPECT_EQ(d0.to_string(), "-123.666");
        }

        // Leading zero truncation
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("000.1");
                EXPECT_EQ(d0.to_string(), "0.1");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("00.0000");
                EXPECT_EQ(d0.to_string(), "0");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("00.11223455");
                EXPECT_EQ(d0.to_string(), "0.11223455");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-00.11223455");
                EXPECT_EQ(d0.to_string(), "-0.11223455");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-00123.11223455");
                EXPECT_EQ(d0.to_string(), "-123.11223455");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0044.11223455");
                EXPECT_EQ(d0.to_string(), "-44.11223455");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-000999.11223455");
                EXPECT_EQ(d0.to_string(), "-999.11223455");
        }

        // trailing '0' omitted, but not those middle ones
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("101.101");
                EXPECT_EQ(d0.to_string(), "101.101");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-101.101");
                EXPECT_EQ(d0.to_string(), "-101.101");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("101.1010");
                EXPECT_EQ(d0.to_string(), "101.101");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-101.1010");
                EXPECT_EQ(d0.to_string(), "-101.101");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("200.1000");
                EXPECT_EQ(d0.to_string(), "200.1");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-200.1000");
                EXPECT_EQ(d0.to_string(), "-200.1");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("0.0000");
                EXPECT_EQ(d0.to_string(), "0");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.0000");
                EXPECT_EQ(d0.to_string(), "0");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprAdd) {
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "0.66666");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "666.666");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "999.443");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "2544464357.256");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "-0.66666");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "-666.666");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "-999.443");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "-2544464357.256");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "0.41976");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "419.754");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "110.803");
        }
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
                EXPECT_EQ(d2.to_string(), "-2298218111.01");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprSub) {
        // {"0.12345", "0.54321", ArithOp::SUB, "-0.41976"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-0.41976");
        }
        // {"123.456", "543.21", ArithOp::SUB, "-419.754"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-419.754");
        }
        // {"444.32", "555.123", ArithOp::SUB, "-110.803"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-110.803");
        }
        // {"2421341234.133", "123123123.123", ArithOp::SUB, "2298218111.01"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "2298218111.01");
        }
        // {"-0.12345", "-0.54321", ArithOp::SUB, "0.41976"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "0.41976");
        }
        // {"-123.456", "-543.21", ArithOp::SUB, "419.754"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "419.754");
        }
        // {"-444.32", "-555.123", ArithOp::SUB, "110.803"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "110.803");
        }
        // {"-2421341234.133", "-123123123.123", ArithOp::SUB, "-2298218111.01"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-2298218111.01");
        }
        // {"-0.12345", "0.54321", ArithOp::SUB, "-0.66666"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-0.66666");
        }
        // {"-123.456", "543.21", ArithOp::SUB, "-666.666"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-666.666");
        }
        // {"-444.32", "555.123", ArithOp::SUB, "-999.443"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-999.443");
        }
        // {"-2421341234.133", "123123123.123", ArithOp::SUB, "-2544464357.256"}
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 - d1;
                EXPECT_EQ(d2.to_string(), "-2544464357.256");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprMul) {
        // {"0.12345", "0.54321", ArithOp::MUL, "0.0670592745"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "0.0670592745");
        }
        // {"123.456", "543.21", ArithOp::MUL, "67062.53376"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "67062.53376");
        }
        // {"444.32", "555.123", ArithOp::MUL, "246652.25136"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "246652.25136");
        }
        // {"2421341234.133", "123123123.123", ArithOp::MUL, "298123094892954129.157359"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "298123094892954129.157359");
        }
        // {"-0.12345", "-0.54321", ArithOp::MUL, "0.0670592745"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "0.0670592745");
        }
        // {"-123.456", "-543.21", ArithOp::MUL, "67062.53376"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "67062.53376");
        }
        // {"-444.32", "-555.123", ArithOp::MUL, "246652.25136"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "246652.25136");
        }
        // {"-2421341234.133", "-123123123.123", ArithOp::MUL, "298123094892954129.157359"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "298123094892954129.157359");
        }
        // {"-0.12345", "0.54321", ArithOp::MUL, "-0.0670592745"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-0.12345");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0.54321");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "-0.0670592745");
        }
        // {"-123.456", "543.21", ArithOp::MUL, "-67062.53376"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("543.21");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "-67062.53376");
        }
        // {"-444.32", "555.123", ArithOp::MUL, "-246652.25136"},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-444.32");
                BIGNUM_TEST_CONSTEXPR Decimal d1("555.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "-246652.25136");
        }
        // {"-2421341234.133", "123123123.123", ArithOp::MUL, "-298123094892954129.157359"};
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-2421341234.133");
                BIGNUM_TEST_CONSTEXPR Decimal d1("123123123.123");
                BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 * d1;
                EXPECT_EQ(d2.to_string(), "-298123094892954129.157359");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprCompare) {
        // {"123.001", "-432.12", CompareOp::EQ, false}, {"123.001", "-432.12", CompareOp::NE,
        // true},
        // {"123.001", "-432.12", CompareOp::LT, false}, {"123.001", "-432.12", CompareOp::LE,
        // false},
        // {"123.001", "-432.12", CompareOp::GT, true},  {"123.001", "-432.12", CompareOp::GE,
        // true},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("123.001");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-432.12");
                BIGNUM_TEST_CONSTEXPR bool b0 = d0 == d1;
                BIGNUM_TEST_CONSTEXPR bool b1 = d0 != d1;
                BIGNUM_TEST_CONSTEXPR bool b2 = d0 < d1;
                BIGNUM_TEST_CONSTEXPR bool b3 = d0 <= d1;
                BIGNUM_TEST_CONSTEXPR bool b4 = d0 > d1;
                BIGNUM_TEST_CONSTEXPR bool b5 = d0 >= d1;
                EXPECT_EQ(b0, false);
                EXPECT_EQ(b1, true);
                EXPECT_EQ(b2, false);
                EXPECT_EQ(b3, false);
                EXPECT_EQ(b4, true);
                EXPECT_EQ(b5, true);
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprCompare_2) {
        // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::EQ, false},
        // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::NE, true},
        // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::LT, false},
        // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::LE, false},
        // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::GT, true},
        // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::GE, true},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("999999999999999999999999999.001");
                BIGNUM_TEST_CONSTEXPR Decimal d1("432.1234567891234567");
                BIGNUM_TEST_CONSTEXPR bool b0 = d0 == d1;
                BIGNUM_TEST_CONSTEXPR bool b1 = d0 != d1;
                BIGNUM_TEST_CONSTEXPR bool b2 = d0 < d1;
                BIGNUM_TEST_CONSTEXPR bool b3 = d0 <= d1;
                BIGNUM_TEST_CONSTEXPR bool b4 = d0 > d1;
                BIGNUM_TEST_CONSTEXPR bool b5 = d0 >= d1;
                EXPECT_EQ(b0, false);
                EXPECT_EQ(b1, true);
                EXPECT_EQ(b2, false);
                EXPECT_EQ(b3, false);
                EXPECT_EQ(b4, true);
                EXPECT_EQ(b5, true);
        }

        // {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::EQ, false},
        // {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::NE, true},
        // {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::LT, true},
        // {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::LE, true},
        // {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::GT, false},
        // {"432.1234567891234567", "999999999999999999999999999.001", CompareOp::GE, false},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("432.1234567891234567");
                BIGNUM_TEST_CONSTEXPR Decimal d1("999999999999999999999999999.001");
                BIGNUM_TEST_CONSTEXPR bool b0 = d0 == d1;
                BIGNUM_TEST_CONSTEXPR bool b1 = d0 != d1;
                BIGNUM_TEST_CONSTEXPR bool b2 = d0 < d1;
                BIGNUM_TEST_CONSTEXPR bool b3 = d0 <= d1;
                BIGNUM_TEST_CONSTEXPR bool b4 = d0 > d1;
                BIGNUM_TEST_CONSTEXPR bool b5 = d0 >= d1;
                EXPECT_EQ(b0, false);
                EXPECT_EQ(b1, true);
                EXPECT_EQ(b2, true);
                EXPECT_EQ(b3, true);
                EXPECT_EQ(b4, false);
                EXPECT_EQ(b5, false);
        }

        // {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::EQ, false},
        // {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::NE, true},
        // {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::LT, true},
        // {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::LE, true},
        // {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::GT, false},
        // {"-999999999999999999999999999.001", "-432.1234567891234567", CompareOp::GE, false},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-999999999999999999999999999.001");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-432.1234567891234567");
                BIGNUM_TEST_CONSTEXPR bool b0 = d0 == d1;
                BIGNUM_TEST_CONSTEXPR bool b1 = d0 != d1;
                BIGNUM_TEST_CONSTEXPR bool b2 = d0 < d1;
                BIGNUM_TEST_CONSTEXPR bool b3 = d0 <= d1;
                BIGNUM_TEST_CONSTEXPR bool b4 = d0 > d1;
                BIGNUM_TEST_CONSTEXPR bool b5 = d0 >= d1;
                EXPECT_EQ(b0, false);
                EXPECT_EQ(b1, true);
                EXPECT_EQ(b2, true);
                EXPECT_EQ(b3, true);
                EXPECT_EQ(b4, false);
                EXPECT_EQ(b5, false);
        }

        // {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::EQ, false},
        // {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::NE, true},
        // {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::LT, false},
        // {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::LE, false},
        // {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::GT, true},
        // {"-432.1234567891234567", "-999999999999999999999999999.001", CompareOp::GE, true},
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("-432.1234567891234567");
                BIGNUM_TEST_CONSTEXPR Decimal d1("-999999999999999999999999999.001");
                BIGNUM_TEST_CONSTEXPR bool b0 = d0 == d1;
                BIGNUM_TEST_CONSTEXPR bool b1 = d0 != d1;
                BIGNUM_TEST_CONSTEXPR bool b2 = d0 < d1;
                BIGNUM_TEST_CONSTEXPR bool b3 = d0 <= d1;
                BIGNUM_TEST_CONSTEXPR bool b4 = d0 > d1;
                BIGNUM_TEST_CONSTEXPR bool b5 = d0 >= d1;
                EXPECT_EQ(b0, false);
                EXPECT_EQ(b1, true);
                EXPECT_EQ(b2, false);
                EXPECT_EQ(b3, false);
                EXPECT_EQ(b4, true);
                EXPECT_EQ(b5, true);
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, string_initialize_with_leading_space) {
        BIGNUM_TEST_CONSTEXPR Decimal d0("  123.456");
        BIGNUM_TEST_CONSTEXPR Decimal d1("123.456");
        EXPECT_EQ(d0, d1);
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
        BIGNUM_TEST_CONSTEXPR Decimal d3("246.912");
        EXPECT_EQ(d2, d3);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, string_initialize_with_trailing_space) {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123.456  ");
        BIGNUM_TEST_CONSTEXPR Decimal d1("123.456");
        EXPECT_EQ(d0, d1);
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 + d1;
        BIGNUM_TEST_CONSTEXPR Decimal d3("246.912");
        EXPECT_EQ(d2, d3);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, string_initialize_with_leading_space_no_constexpr) {
        Decimal d0("  123.456");
        Decimal d1("123.456");
        EXPECT_EQ(d0, d1);
        Decimal d2 = d0 + d1;
        Decimal d3("246.912");
        EXPECT_EQ(d2, d3);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, string_initialize_with_trailing_space_no_constexpr) {
        Decimal d0("123.456  ");
        Decimal d1("123.456");
        EXPECT_EQ(d0, d1);
        Decimal d2 = d0 + d1;
        Decimal d3("246.912");
        EXPECT_EQ(d2, d3);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, string_initialize_with_leading_traling_zero) {
        {
                BIGNUM_TEST_CONSTEXPR Decimal d0("000123.456");
                BIGNUM_TEST_CONSTEXPR Decimal d1("0000.456");
                BIGNUM_TEST_CONSTEXPR Decimal d2("-000123.000");
                BIGNUM_TEST_CONSTEXPR Decimal d3("-0.000");
                BIGNUM_TEST_CONSTEXPR Decimal d4("-0.123");

                EXPECT_EQ(d0.to_string(), "123.456");
                EXPECT_EQ(d1.to_string(), "0.456");
                EXPECT_EQ(d2.to_string(), "-123");
                EXPECT_EQ(d3.to_string(), "0");
                EXPECT_EQ(d4.to_string(), "-0.123");
        }

        {
                Decimal d0("000123999999999999999999999999999999999999.4569");
                Decimal d1("000099999999999.456999999999999999999999999999");
                Decimal d2("-00012388888888.000000000000000000000000000000");
                Decimal d3("-000000000000000.000000000000000000000000000000");
                Decimal d4("-00000000000000000.123444444444444444444444444444");

                EXPECT_EQ(d0.to_string(), "123999999999999999999999999999999999999.4569");
                EXPECT_EQ(d1.to_string(), "99999999999.456999999999999999999999999999");
                EXPECT_EQ(d2.to_string(), "-12388888888");
                EXPECT_EQ(d3.to_string(), "0");
                EXPECT_EQ(d4.to_string(), "-0.123444444444444444444444444444");
        }
}

// Non-accept table string:
//      ".123"
//      "-.123"
//      ".123..."
//      "-.123..."
TEST_F(BIGNUM_DECIMAL_FIXTURE, non_accept_table_string) {
        Decimal d0;
        EXPECT_TRUE(!!d0.assign(".123"));
        EXPECT_TRUE(!!d0.assign("-.123"));
        EXPECT_TRUE(!!d0.assign(".123999999999999999999999999999"));
        EXPECT_TRUE(!!d0.assign("-.123999999999999999999999999999"));
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, ostream_operator) {
        Decimal d1("123.345");

        std::ostringstream oss;
        oss << d1;

        EXPECT_EQ(oss.str(), "123.345");
        return;
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, cast_to_int64_int128) {
        // from int64 to int64 -> never overflow
        {
                BIGNUM_TEST_CONSTEXPR Decimal d1("123.345");
                int64_t i = static_cast<int64_t>(d1);
                EXPECT_EQ(i, 123);
        }

        // from __int128_t to int64_t, large value would overflow
        {
                BIGNUM_TEST_CONSTEXPR Decimal d1("12345678987654321001.11");
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(static_cast<int64_t>(d1), testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(static_cast<int64_t>(d1), std::runtime_error);
#endif
        }

        // from gmp to int64_t, large value would overflow
        {
                Decimal d1("12345678987654300000000000000002100999999991.11");
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(static_cast<int64_t>(d1), testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(static_cast<int64_t>(d1), std::runtime_error);
#endif
        }

        // from 64 to 128, never overflow
        {
                BIGNUM_TEST_CONSTEXPR Decimal d1("123.345");
                __int128_t i = static_cast<__int128_t>(d1);
                EXPECT_EQ(i, 123);
        }

        // from 128 to 128, never overflow
        {
                BIGNUM_TEST_CONSTEXPR Decimal d1("12345678987654321001.11");
                __int128_t i = static_cast<__int128_t>(d1);
                EXPECT_EQ(i, static_cast<__int128_t>(123456789876543ll) * 100000 + 21001);
        }

        // from gmp to 128, large value would overflow
        {
                Decimal d1("12345678987654300000000000000002100999999991.11");
#ifndef BIGNUM_ENABLE_EXCEPTIONS
                EXPECT_EXIT(static_cast<int64_t>(d1), testing::KilledBySignal(SIGABRT), "");
#else
                EXPECT_THROW(static_cast<int64_t>(d1), std::runtime_error);
#endif
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, dval) {
        double dval = 3.1415926;
        Decimal d1(dval);
        EXPECT_EQ(d1.to_string(), "3.14159260000000007");

        // This is prohibited to prevent misuse.
        // [[maybe_unused]] Decimal d2(3.1415926);

        // but += float/double literval is allowed to make it easier to use.
        Decimal d3 = d1 + 3.1415926;
        EXPECT_EQ(d3.to_string(), "6.28318520000000014");
}

#if 0
TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprMod) {
    //=------------------------------------------
    // Transform these tests into BIGNUM_TEST_CONSTEXPR tests
    //=------------------------------------------
    // {"1", "3", ArithOp::MOD, "1"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1");
    }
    // {"100000", "3.33", ArithOp::MOD, "0.1"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.1");
    }
    // {"999999", "3.33", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"123456", "3.33", ArithOp::MOD, "2.91"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "2.91");
    }
    // {"-1", "3", ArithOp::MOD, "-1"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1");
    }
    // {"-100000", "3.33", ArithOp::MOD, "-0.1"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.1");
    }
    // {"-999999", "3.33", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-123456", "3.33", ArithOp::MOD, "-2.91"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-2.91");
    }
    // {"-1", "-3", ArithOp::MOD, "-1"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1");
    }
    // {"-100000", "-3.33", ArithOp::MOD, "-0.1"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.1");
    }
    // {"-999999", "-3.33", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-123456", "-3.33", ArithOp::MOD, "-2.91"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-2.91");
    }
    // {"1.00001", "3", ArithOp::MOD, "1.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.00001");
    }
    // {"100000.00001", "3.33", ArithOp::MOD, "0.10001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.10001");
    }
    // {"999999.00001", "3.33", ArithOp::MOD, "0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"123456.00001", "3.33", ArithOp::MOD, "2.91001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "2.91001");
    }
    // {"-1.00001", "3", ArithOp::MOD, "-1.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.00001");
    }
    // {"-100000.00001", "3.33", ArithOp::MOD, "-0.10001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.10001");
    }
    // {"-999999.00001", "3.33", ArithOp::MOD, "-0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-123456.00001", "3.33", ArithOp::MOD, "-2.91001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-2.91001");
    }
    // {"1.57565", "3", ArithOp::MOD, "1.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.57565");
    }
    // {"100000.57565", "3.33", ArithOp::MOD, "0.67565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.67565");
    }
    // {"999999.57565", "3.33", ArithOp::MOD, "0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"123456.57565", "3.33", ArithOp::MOD, "0.15565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.15565");
    }
    // {"-1.57565", "3", ArithOp::MOD, "-1.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.57565");
    }
    // {"-100000.57565", "3.33", ArithOp::MOD, "-0.67565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.67565");
    }
    // {"-999999.57565", "3.33", ArithOp::MOD, "-0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-123456.57565", "3.33", ArithOp::MOD, "-0.15565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.15565");
    }
    // {"-1.57565", "-3", ArithOp::MOD, "-1.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.57565");
    }
    // {"-100000.57565", "-3.33", ArithOp::MOD, "-0.67565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.67565");
    }
    // {"-999999.57565", "-3.33", ArithOp::MOD, "-0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-123456.57565", "-3.33", ArithOp::MOD, "-0.15565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.15565");
    }
    // {"1", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"100000", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"999999", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"123456", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-1", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-100000", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-999999", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-123456", "-1", ArithOp::MOD, "0"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"1.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"100000.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"999999.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"123456.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"-1.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-100000.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-999999.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-123456.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.00001");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"1.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"100000.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"999999.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"123456.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"-1.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-100000.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-999999.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-123456.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.57565");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"1.5756533334441", "3", ArithOp::MOD, "1.5756533334441"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1.5756533334441");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.5756533334441");
    }
    // {"30030.202898898933", "3.33", ArithOp::MOD, "0.262898898933"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("30030.202898898933");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.262898898933");
    }
    // {"100000.111111111111111", "3.33", ArithOp::MOD, "0.211111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.211111111111111");
    }
    // {"999999.111111111111111", "3.33", ArithOp::MOD, "0.111111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.111111111111111");
    }
    // {"123456.111111111111111", "3.33", ArithOp::MOD, "3.021111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "3.021111111111111");
    }
    // {"1.5756533334441", "-3", ArithOp::MOD, "1.5756533334441"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("1.5756533334441");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.5756533334441");
    }
    // {"30030.202898898933", "-3.33", ArithOp::MOD, "0.262898898933"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("30030.202898898933");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.262898898933");
    }
    // {"100000.111111111111111", "-3.33", ArithOp::MOD, "0.211111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.211111111111111");
    }
    // {"999999.111111111111111", "-3.33", ArithOp::MOD, "0.111111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.111111111111111");
    }
    // {"123456.111111111111111", "-3.33", ArithOp::MOD, "3.021111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "3.021111111111111");
    }
    // {"-1.5756533334441", "-3", ArithOp::MOD, "-1.5756533334441"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.5756533334441");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.5756533334441");
    }
    // {"-30030.202898898933", "-3.33", ArithOp::MOD, "-0.262898898933"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-30030.202898898933");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.262898898933");
    }
    // {"-100000.111111111111111", "-3.33", ArithOp::MOD, "-0.211111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.211111111111111");
    }
    // {"-999999.111111111111111", "-3.33", ArithOp::MOD, "-0.111111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.111111111111111");
    }
    // {"-123456.111111111111111", "-3.33", ArithOp::MOD, "-3.021111111111111"},
    {
        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.111111111111111");
        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-3.021111111111111");
    }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, Int256AddOverflow) {
    using namespace boost::multiprecision;
    int256_t res256 = 0;
    ErrCode err = kSuccess;

    int256_t i256max = kInt256Max;
    err = safe_add_int256(res256, i256max, i256max);
    EXPECT_TRUE(!!err);

    int256_t i256min = kInt256Min;
    err = safe_add_int256(res256, i256min, i256min);
    EXPECT_TRUE(!!err);

    // Non-overflow cases
    // int256_t i256val = 123;
    err = safe_add_int256(res256, i256val, i256val);
    EXPECT_TRUE(!err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, safe_mul_int256) {
    using namespace boost::multiprecision;
    int256_t res256 = 0;
    ErrCode err = kSuccess;

    // +  vs  +, non-overflow
    err = safe_mul_int256(res256, 123, 456);
    EXPECT_TRUE(!err);
    // +  vs  +, overflow
    err = safe_mul_int256(res256, kInt256Max, kInt256Max);
    EXPECT_TRUE(!!err);
    // +  vs  -, non-overflow
    err = safe_mul_int256(res256, 123, -456);
    EXPECT_TRUE(!err);
    // +  vs  -, overflow
    err = safe_mul_int256(res256, kInt256Max, kInt256Min);
    EXPECT_TRUE(!!err);
    // -  vs  +, non-overflow
    err = safe_mul_int256(res256, -123, 456);
    EXPECT_TRUE(!err);
    // -  vs  +, overflow
    err = safe_mul_int256(res256, kInt256Min, kInt256Max);
    EXPECT_TRUE(!!err);
    // -  vs  -, non-overflow
    err = safe_mul_int256(res256, -123, -456);
    EXPECT_TRUE(!err);
    // -  vs  -, overflow
    err = safe_mul_int256(res256, kInt256Min, kInt256Min);
    EXPECT_TRUE(!!err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, safe_div_int128) {
    __int128_t res128 = 0;
    ErrCode err = kSuccess;

    // Divison by zero
    err = safe_div_int128(res128, 123, 0);
    EXPECT_TRUE(!!err);

    // int128min / -1
    err = safe_div_int128(res128, kInt128Min, -1);
    EXPECT_TRUE(!!err);

    // normal, non-overflow division
    err = safe_div_int128(res128, 123, 456);
    EXPECT_TRUE(!err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, safe_div_int256) {
    using namespace boost::multiprecision;
    int256_t res256 = 0;
    ErrCode err = kSuccess;

    // Divison by zero
    err = safe_div_int256(res256, 123, 0);
    EXPECT_TRUE(!!err);

    // int256min / -1
    err = safe_div_int256(res256, kInt256Min, -1);
    EXPECT_TRUE(!!err);

    // normal, non-overflow division
    err = safe_div_int256(res256, 123, 456);
    EXPECT_TRUE(!err);
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, int256_to_string) {
    using namespace boost::multiprecision;
    EXPECT_EQ(convert_int256_to_string(0), "0");
    EXPECT_EQ(convert_int256_to_string(123), "123");
    EXPECT_EQ(convert_int256_to_string(-123), "-123");
    EXPECT_EQ(convert_int256_to_string(kInt256Max),
              "57896044618658097711785492504343953926634992332820282019728792003956564819967");
    EXPECT_EQ(convert_int256_to_string(kInt256Min),
              "-57896044618658097711785492504343953926634992332820282019728792003956564819968");
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, invalid_str_constexpr_init_failure) {
        // Invalid BIGNUM_TEST_CONSTEXPR initialization. Should fail to compile.
        {
            BIGNUM_TEST_CONSTEXPR Decimal d0("1af.123");
            EXPECT_EQ(d0.to_string(), "0");
        }
}

TEST_F(BIGNUM_DECIMAL_FIXTURE, large_num_constexpr_init_failure) {
        // Invalid BIGNUM_TEST_CONSTEXPR initialization. Should fail to compile.
        {
            BIGNUM_TEST_CONSTEXPR Decimal d0("1af.123");
            EXPECT_EQ(d0.to_string(), "0");
        }
}

// TODO have to make mpz_* function BIGNUM_TEST_CONSTEXPR for these to be BIGNUM_TEST_CONSTEXPR
// TODO or provide a method using __int128_t or int64 for small number
//TEST_F(BIGNUM_DECIMAL_FIXTURE, ConstExprDiv) {
//    // {"1", "3", ArithOp::DIV, "0.3333"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.3333");
//    }
//    // {"100000", "3.33", ArithOp::DIV, "30030.03"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.03");
//    }
//    // {"999999", "3.33", ArithOp::DIV, "300300"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300");
//    }
//    // {"123456", "3.33", ArithOp::DIV, "37073.8739"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37073.8739");
//    }
//    // {"-1", "3", ArithOp::DIV, "-0.3333"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-0.3333");
//    }
//    // {"-100000", "3.33", ArithOp::DIV, "-30030.03"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-30030.03");
//    }
//    // {"-999999", "3.33", ArithOp::DIV, "-300300"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-300300");
//    }
//    // {"-123456", "3.33", ArithOp::DIV, "-37073.8739"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-37073.8739");
//    }
//    // {"-1", "-3", ArithOp::DIV, "0.3333"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.3333");
//    }
//    // {"-100000", "-3.33", ArithOp::DIV, "30030.03"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.03");
//    }
//    // {"-999999", "-3.33", ArithOp::DIV, "300300"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300");
//    }
//    // {"-123456", "-3.33", ArithOp::DIV, "37073.8739"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37073.8739");
//    }
//    // {"1.00001", "3", ArithOp::DIV, "0.333336667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.333336667");
//    }
//    // {"100000.00001", "3.33", ArithOp::DIV, "30030.030033033"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.030033033");
//    }
//    // {"999999.00001", "3.33", ArithOp::DIV, "300300.000003003"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300.000003003");
//    }
//    // {"123456.00001", "3.33", ArithOp::DIV, "37073.873876877"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37073.873876877");
//    }
//    // {"-1.00001", "3", ArithOp::DIV, "-0.333336667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-0.333336667");
//    }
//    // {"-100000.00001", "3.33", ArithOp::DIV, "-30030.030033033"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-30030.030033033");
//    }
//    // {"-999999.00001", "3.33", ArithOp::DIV, "-300300.000003003"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-300300.000003003");
//    }
//    // {"-123456.00001", "3.33", ArithOp::DIV, "-37073.873876877"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-37073.873876877");
//    }
//    // {"1.57565", "3", ArithOp::DIV, "0.525216667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.525216667");
//    }
//    // {"100000.57565", "3.33", ArithOp::DIV, "30030.202897898"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.202897898");
//    }
//    // {"999999.57565", "3.33", ArithOp::DIV, "300300.172867868"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300.172867868");
//    }
//    // {"123456.57565", "3.33", ArithOp::DIV, "37074.046741742"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37074.046741742");
//    }
//    // {"-1.57565", "3", ArithOp::DIV, "-0.525216667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-0.525216667");
//    }
//    // {"-100000.57565", "3.33", ArithOp::DIV, "-30030.202897898"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-30030.202897898");
//    }
//    // {"-999999.57565", "3.33", ArithOp::DIV, "-300300.172867868"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-300300.172867868");
//    }
//    // {"-123456.57565", "3.33", ArithOp::DIV, "-37074.046741742"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-37074.046741742");
//    }
//    // {"-1.57565", "-3", ArithOp::DIV, "0.525216667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.525216667");
//    }
//    // {"-100000.57565", "-3.33", ArithOp::DIV, "30030.202897898"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.202897898");
//    }
//    // {"-999999.57565", "-3.33", ArithOp::DIV, "300300.172867868"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300.172867868");
//    }
//    // {"-123456.57565", "-3.33", ArithOp::DIV, "37074.046741742"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37074.046741742");
//    }
//    // {"1", "-1", ArithOp::DIV, "-1"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-1");
//    }
//    // {"100000", "-1", ArithOp::DIV, "-100000"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-100000");
//    }
//    // {"999999", "-1", ArithOp::DIV, "-999999"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-999999");
//    }
//    // {"123456", "-1", ArithOp::DIV, "-123456"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-123456");
//    }
//    // {"-1", "-1", ArithOp::DIV, "1"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "1");
//    }
//    // {"-100000", "-1", ArithOp::DIV, "100000"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "100000");
//    }
//    // {"-999999", "-1", ArithOp::DIV, "999999"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "999999");
//    }
//    // {"-123456", "-1", ArithOp::DIV, "123456"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "123456");
//    }
//    // {"1.00001", "-1", ArithOp::DIV, "-1.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-1.00001");
//    }
//    // {"100000.00001", "-1", ArithOp::DIV, "-100000.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-100000.00001");
//    }
//    // {"999999.00001", "-1", ArithOp::DIV, "-999999.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-999999.00001");
//    }
//    // {"123456.00001", "-1", ArithOp::DIV, "-123456.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-123456.00001");
//    }
//    // {"-1.00001", "-1", ArithOp::DIV, "1.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "1.00001");
//    }
//    // {"-100000.00001", "-1", ArithOp::DIV, "100000.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "100000.00001");
//    }
//    // {"-999999.00001", "-1", ArithOp::DIV, "999999.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "999999.00001");
//    }
//    // {"-123456.00001", "-1", ArithOp::DIV, "123456.00001"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.00001");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "123456.00001");
//    }
//    // {"1.57565", "-1", ArithOp::DIV, "-1.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-1.57565");
//    }
//    // {"100000.57565", "-1", ArithOp::DIV, "-100000.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-100000.57565");
//    }
//    // {"999999.57565", "-1", ArithOp::DIV, "-999999.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-999999.57565");
//    }
//    // {"123456.57565", "-1", ArithOp::DIV, "-123456.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-123456.57565");
//    }
//    // {"-1.57565", "-1", ArithOp::DIV, "1.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "1.57565");
//    }
//    // {"-100000.57565", "-1", ArithOp::DIV, "100000.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "100000.57565");
//    }
//    // {"-999999.57565", "-1", ArithOp::DIV, "999999.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "999999.57565");
//    }
//    // {"-123456.57565", "-1", ArithOp::DIV, "123456.57565"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.57565");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-1");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "123456.57565");
//    }
//    // {"1.5756533334441", "3", ArithOp::DIV, "0.5252177778147"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1.5756533334441");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.5252177778147");
//    }
//    // {"30030.202898898933", "3.33", ArithOp::DIV, "9018.0789486182981982"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("30030.202898898933");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "9018.0789486182981982");
//    }
//    // {"100000.111111111111111", "3.33", ArithOp::DIV, "30030.0633967300633967"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.0633967300633967");
//    }
//    // {"999999.111111111111111", "3.33", ArithOp::DIV, "300300.0333667000333667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300.0333667000333667");
//    }
//    // {"123456.111111111111111", "3.33", ArithOp::DIV, "37073.9072405739072405"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37073.9072405739072405");
//    }
//    // {"1.5756533334441", "-3", ArithOp::DIV, "-0.5252177778147"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("1.5756533334441");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-0.5252177778147");
//    }
//    // {"30030.202898898933", "-3.33", ArithOp::DIV, "-9018.0789486182981982"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("30030.202898898933");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-9018.0789486182981982");
//    }
//    // {"100000.111111111111111", "-3.33", ArithOp::DIV, "-30030.0633967300633967"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("100000.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-30030.0633967300633967");
//    }
//    // {"999999.111111111111111", "-3.33", ArithOp::DIV, "-300300.0333667000333667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("999999.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-300300.0333667000333667");
//    }
//    // {"123456.111111111111111", "-3.33", ArithOp::DIV, "-37073.9072405739072405"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("123456.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "-37073.9072405739072405");
//    }
//    // {"-1.5756533334441", "-3", ArithOp::DIV, "0.5252177778147"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-1.5756533334441");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "0.5252177778147");
//    }
//    // {"-30030.202898898933", "-3.33", ArithOp::DIV, "9018.0789486182981982"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-30030.202898898933");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "9018.0789486182981982");
//    }
//    // {"-100000.111111111111111", "-3.33", ArithOp::DIV, "30030.0633967300633967"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-100000.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "30030.0633967300633967");
//    }
//    // {"-999999.111111111111111", "-3.33", ArithOp::DIV, "300300.0333667000333667"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-999999.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "300300.0333667000333667");
//    }
//    // {"-123456.111111111111111", "-3.33", ArithOp::DIV, "37073.9072405739072405"},
//    {
//        BIGNUM_TEST_CONSTEXPR Decimal d0("-123456.111111111111111");
//        BIGNUM_TEST_CONSTEXPR Decimal d1("-3.33");
//        BIGNUM_TEST_CONSTEXPR Decimal d2 = d0 / d1;
//        EXPECT_EQ(d2.to_string(), "37073.9072405739072405");
//    }
//}

#endif
}  // namespace bignum
