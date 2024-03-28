#include <gtest/gtest.h>
#include <iostream>

#include "Decimal.h"

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

class DecimalTest : public ::testing::Test {
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
            EXPECT_EQ(result, comp.result);
        }
    }
};

TEST_F(DecimalTest, StringConversion) {
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

TEST_F(DecimalTest, Add) {
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

TEST_F(DecimalTest, Sub) {
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

TEST_F(DecimalTest, Mul) {
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
        {"-2421341234.133", "123123123.123", ArithOp::MUL, "-298123094892954129.157359"}
    };
    DoTestDecimalArithmetic(calculations);
}

#if 0
TEST_F(DecimalTest, VeryLargeInt128Overflow) {
    __int128_t very_large_value = kInt128Max;
    __int128_t very_small_value = kInt128Min;
    // Overflow assertion on very large value
    EXPECT_EXIT(Decimal{very_large_value}, testing::KilledBySignal(SIGABRT), "");
    EXPECT_EXIT(Decimal{very_small_value}, testing::KilledBySignal(SIGABRT), "");

    // Test assign(...) interface.
    Decimal dd;
    ErrCode err;
    err = dd.assign(very_large_value);
    EXPECT_TRUE(!!err);
    err = dd.assign(very_small_value);
    EXPECT_TRUE(!!err);

    // Make sure that kDecimalInt128Min and kDecimalInt128Max are correct.
    Decimal dmax(Decimal::kDecimalInt128Max);
    EXPECT_EQ(dmax.to_string(), Decimal::kDecimalInt128MaxStr);
    Decimal dmin(Decimal::kDecimalInt128Min);
    EXPECT_EQ(dmin.to_string(), Decimal::kDecimalInt128MinStr);

    // No even +1 or -1 can be assigned to Decimal.
    EXPECT_EXIT(Decimal{Decimal::kDecimalInt128Max + 1}, testing::KilledBySignal(SIGABRT), "");
    EXPECT_EXIT(Decimal{Decimal::kDecimalInt128Min - 1}, testing::KilledBySignal(SIGABRT), "");
    err = dd.assign(Decimal::kDecimalInt128Max + 1);
    EXPECT_TRUE(!!err);
    err = dd.assign(Decimal::kDecimalInt128Min - 1);
    EXPECT_TRUE(!!err);
}
#endif

TEST_F(DecimalTest, StringConstructionOverflow) {
    // String construction of some "large" and "small" values
    {
        constexpr static const char *kDecimalInt128MaxStr = "99999999999999999999999999999999999999";
        constexpr static const char *kDecimalInt128MinStr = "-100000000000000000000000000000000000000";
        Decimal dmax(kDecimalInt128MaxStr);
        EXPECT_EQ(dmax.to_string(), kDecimalInt128MaxStr);

        Decimal dmin(kDecimalInt128MinStr);
        EXPECT_EQ(dmin.to_string(), kDecimalInt128MinStr);
    }

    // 65 digits, should be OK
    {
        const char *PositiveLargeStr = "99999999999999999999999999999999999999999999999999999999999999999";
        const char *NegativeLargeStr = "-99999999999999999999999999999999999999999999999999999999999999999";
        Decimal maxv(PositiveLargeStr);
        Decimal minv(NegativeLargeStr);

        EXPECT_EQ(maxv.to_string(), PositiveLargeStr);
        EXPECT_EQ(minv.to_string(), NegativeLargeStr);

        Decimal add_res = maxv + minv;
        EXPECT_EQ(add_res, Decimal("0"));
        EXPECT_EQ(add_res.to_string(), "0");

        EXPECT_EXIT(maxv - minv, testing::KilledBySignal(SIGABRT), "");
        EXPECT_EXIT(maxv * minv, testing::KilledBySignal(SIGABRT), "");

        Decimal div_res = maxv / minv;
        EXPECT_EQ(div_res, Decimal("-1"));
    }

    // 69 digits, quick and simple, should be rejected.
    {
        const char *PositiveOverflowStr = "100000000000000000000000000000000000000000000000000000000000000000000";
        const char *NegativeOverflowStr = "-100000000000000000000000000000000000000000000000000000000000000000000";
        EXPECT_EXIT(Decimal{PositiveOverflowStr}, testing::KilledBySignal(SIGABRT), "");
        EXPECT_EXIT(Decimal{NegativeOverflowStr}, testing::KilledBySignal(SIGABRT), "");

        Decimal dd;
        ErrCode err;
        err = dd.assign(PositiveOverflowStr);
        EXPECT_TRUE(!!err);
        err = dd.assign(NegativeOverflowStr);
        EXPECT_TRUE(!!err);
    }

    // 65 digits, with non-zero scale=30 (max scale), should be OK
    {
        const char *PositiveLargeStr = "99999999999999999999999999999999999.999999999999999999999999999999";
        const char *NegativeLargeStr = "-99999999999999999999999999999999999.999999999999999999999999999999";
        Decimal maxv(PositiveLargeStr);
        Decimal minv(NegativeLargeStr);

        EXPECT_EQ(maxv.to_string(), PositiveLargeStr);
        EXPECT_EQ(minv.to_string(), NegativeLargeStr);

        Decimal add_res = maxv + minv;
        EXPECT_EQ(add_res, Decimal("0"));
        EXPECT_EQ(add_res.to_string(), "0");

        EXPECT_EXIT(maxv - minv, testing::KilledBySignal(SIGABRT), "");
        EXPECT_EXIT(maxv * minv, testing::KilledBySignal(SIGABRT), "");

        Decimal div_res = maxv / minv;
        EXPECT_EQ(div_res, Decimal("-1"));
    }

    // 65 digits, with scale>kDecimalMaxScale, should be rejected.
    {
        const char *PositiveLargeStr = "9999999999999999999999999999999999.9999999999999999999999999999999";
        const char *NegativeLargeStr = "-9999999999999999999999999999999999.9999999999999999999999999999999";
        EXPECT_EXIT(Decimal{PositiveLargeStr}, testing::KilledBySignal(SIGABRT), "");
        EXPECT_EXIT(Decimal{NegativeLargeStr}, testing::KilledBySignal(SIGABRT), "");
    }

    // Invalid characters inside decimal string would trigger error.
    EXPECT_EXIT(Decimal{"1234567890abcdef"}, testing::KilledBySignal(SIGABRT), "");

    // Trailing '.' is not acceptable.
    EXPECT_EXIT(Decimal{"1234567890."}, testing::KilledBySignal(SIGABRT), "");
}

#if 0
TEST_F(DecimalTest, StringConstructionTrailingZeroTruncation) {
    EXPECT_EQ(Decimal("101.1010").get_scale(), 3);
    EXPECT_EQ(Decimal("-101.1010").get_scale(), 3);
    EXPECT_EQ(Decimal("123.0000").get_scale(), 0);
    EXPECT_EQ(Decimal("-134.0000").get_scale(), 0);
    EXPECT_EQ(Decimal("0.0000").get_scale(), 0);
    EXPECT_EQ(Decimal("-0.0000").get_scale(), 0);
}

TEST_F(DecimalTest, Comparison) {
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
                compares.push_back(
                    DecimalComparison(negatives[i], negatives[j], CompareOp::EQ, i == j));
                // NE
                compares.push_back(
                    DecimalComparison(negatives[i], negatives[j], CompareOp::NE, i != j));
                // LT
                compares.push_back(
                    DecimalComparison(negatives[i], negatives[j], CompareOp::LT, i > j));
                // LE
                compares.push_back(
                    DecimalComparison(negatives[i], negatives[j], CompareOp::LE, i >= j));
                // GT
                compares.push_back(
                    DecimalComparison(negatives[i], negatives[j], CompareOp::GT, i < j));
                // GE
                compares.push_back(
                    DecimalComparison(negatives[i], negatives[j], CompareOp::GE, i <= j));
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
                compares.push_back(
                    DecimalComparison(positives[i], positives[j], CompareOp::EQ, i == j));
                // NE
                compares.push_back(
                    DecimalComparison(positives[i], positives[j], CompareOp::NE, i != j));
                // LT
                compares.push_back(
                    DecimalComparison(positives[i], positives[j], CompareOp::LT, i < j));
                // LE
                compares.push_back(
                    DecimalComparison(positives[i], positives[j], CompareOp::LE, i <= j));
                // GT
                compares.push_back(
                    DecimalComparison(positives[i], positives[j], CompareOp::GT, i > j));
                // GE
                compares.push_back(
                    DecimalComparison(positives[i], positives[j], CompareOp::GE, i >= j));
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
                compares.push_back(
                    DecimalComparison(negatives[i], positives[j], CompareOp::EQ, false));
                // NE
                compares.push_back(
                    DecimalComparison(negatives[i], positives[j], CompareOp::NE, true));
                // LT
                compares.push_back(
                    DecimalComparison(negatives[i], positives[j], CompareOp::LT, true));
                // LE
                compares.push_back(
                    DecimalComparison(negatives[i], positives[j], CompareOp::LE, true));
                // GT
                compares.push_back(
                    DecimalComparison(negatives[i], positives[j], CompareOp::GT, false));
                // GE
                compares.push_back(
                    DecimalComparison(negatives[i], positives[j], CompareOp::GE, false));
            }
        }
    }

    // Compare positive with negative numbers
    {
        std::vector<DecimalComparison> compares;
        for (size_t i = 0; i < positives.size(); i++) {
            for (size_t j = 0; j < negatives.size(); j++) {
                // EQ
                compares.push_back(
                    DecimalComparison(positives[i], negatives[j], CompareOp::EQ, false));
                // NE
                compares.push_back(
                    DecimalComparison(positives[i], negatives[j], CompareOp::NE, true));
                // LT
                compares.push_back(
                    DecimalComparison(positives[i], negatives[j], CompareOp::LT, false));
                // LE
                compares.push_back(
                    DecimalComparison(positives[i], negatives[j], CompareOp::LE, false));
                // GT
                compares.push_back(
                    DecimalComparison(positives[i], negatives[j], CompareOp::GT, true));
                // GE
                compares.push_back(
                    DecimalComparison(positives[i], negatives[j], CompareOp::GE, true));
            }
        }
    }
}

TEST_F(DecimalTest, ComparisonSpecial) {
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

TEST_F(DecimalTest, ScaleNarrowDownAfterMultiply) {
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

TEST_F(DecimalTest, LargeNumberAnnotation) {
    EXPECT_EQ(Decimal{"123456"}.to_string_pretty(true), "123,456");
    EXPECT_EQ(Decimal{"456"}.to_string_pretty(true), "456");
    EXPECT_EQ(Decimal{"0"}.to_string_pretty(true), "0");
    EXPECT_EQ(Decimal{"0009"}.to_string_pretty(true), "9");

    EXPECT_EQ(Decimal{"123456.123456"}.to_string_pretty(true), "123,456.123456");
    EXPECT_EQ(Decimal{"456.123456"}.to_string_pretty(true), "456.123456");
    EXPECT_EQ(Decimal{"0.123456"}.to_string_pretty(true), "0.123456");
    EXPECT_EQ(Decimal{"0009.123456"}.to_string_pretty(true), "9.123456");
}

TEST_F(DecimalTest, MulOverflowSignificantDigits) {
    EXPECT_EXIT(Decimal{Decimal::kDecimalInt128MaxStr} * Decimal{Decimal::kDecimalInt128MaxStr},
                testing::KilledBySignal(SIGABRT), "");
    EXPECT_EXIT(Decimal{Decimal::kDecimalInt128MinStr} * Decimal{Decimal::kDecimalInt128MinStr},
                testing::KilledBySignal(SIGABRT), "");

    // overflow
    {
        const char *str1 = "10000000000000000000";
        Decimal d1{str1};
        EXPECT_EXIT(d1 * d1, testing::KilledBySignal(SIGABRT), "");
        const char *str2 = "-10000000000000000000";
        Decimal d2{str2};
        EXPECT_EXIT(d2 * d2, testing::KilledBySignal(SIGABRT), "");
    }

    // not overflow
    {
        const char *str1 = "1000000000000000000";
        Decimal d1{str1};
        EXPECT_EQ((d1 * d1).to_string(), "1000000000000000000000000000000000000");
        const char *str2 = "-1000000000000000000";
        Decimal d2{str2};
        EXPECT_EQ((d2 * d2).to_string(), "1000000000000000000000000000000000000");
    }
}

TEST_F(DecimalTest, MulOverflowLeastSignificantDigits) {
    {
        Decimal d1("1.100000001");
        EXPECT_EQ((d1 * d1).to_string(), "1.2100000022");
    }

    {
        Decimal d1("1.10000000001");
        EXPECT_EQ((d1 * d1).to_string(), "1.210000000022");
    }
    {
        Decimal d1("1.10000000016");
        EXPECT_EQ((d1 * d1).to_string(), "1.210000000352");
    }
    {
        Decimal d1("1.18888888886");
        EXPECT_EQ((d1 * d1).to_string(), "1.4134567900547654");
    }
    {
        Decimal d1("1.134567900547654");
        EXPECT_EQ((d1 * d1).to_string(), "1.2872443209531113");
    }
}

TEST_F(DecimalTest, Div) {
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

        // Maxscale exceeded, rounding back to kMaxScale
        {"1.5756533334441", "3", ArithOp::DIV, "0.5252177778147"},
        {"30030.202898898933", "3.33", ArithOp::DIV, "9018.0789486182981982"},
        {"100000.111111111111111", "3.33", ArithOp::DIV, "30030.0633967300633967"},
        {"999999.111111111111111", "3.33", ArithOp::DIV, "300300.0333667000333667"},
        {"123456.111111111111111", "3.33", ArithOp::DIV, "37073.9072405739072405"},

        {"1.5756533334441", "-3", ArithOp::DIV, "-0.5252177778147"},
        {"30030.202898898933", "-3.33", ArithOp::DIV, "-9018.0789486182981982"},
        {"100000.111111111111111", "-3.33", ArithOp::DIV, "-30030.0633967300633967"},
        {"999999.111111111111111", "-3.33", ArithOp::DIV, "-300300.0333667000333667"},
        {"123456.111111111111111", "-3.33", ArithOp::DIV, "-37073.9072405739072405"},

        {"-1.5756533334441", "-3", ArithOp::DIV, "0.5252177778147"},
        {"-30030.202898898933", "-3.33", ArithOp::DIV, "9018.0789486182981982"},
        {"-100000.111111111111111", "-3.33", ArithOp::DIV, "30030.0633967300633967"},
        {"-999999.111111111111111", "-3.33", ArithOp::DIV, "300300.0333667000333667"},
        {"-123456.111111111111111", "-3.33", ArithOp::DIV, "37073.9072405739072405"},
    };
    DoTestDecimalArithmetic(calculations);

    // Divison by zero
    EXPECT_EXIT(Decimal{"1.01"} / Decimal{"0"}, testing::KilledBySignal(SIGABRT), "");
}

TEST_F(DecimalTest, Mod) {
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
    EXPECT_EXIT(Decimal{"1.01"} % Decimal{"0"}, testing::KilledBySignal(SIGABRT), "");
}

TEST_F(DecimalTest, DiffSignCompare) {
    std::vector<DecimalComparison> compares = {
        {"123.001", "-432.12", CompareOp::EQ, false}, {"123.001", "-432.12", CompareOp::NE, true},
        {"123.001", "-432.12", CompareOp::LT, false}, {"123.001", "-432.12", CompareOp::LE, false},
        {"123.001", "-432.12", CompareOp::GT, true},  {"123.001", "-432.12", CompareOp::GE, true},
    };
    DoTestDecimalComparison(compares);
}

TEST_F(DecimalTest, DiffScaleSameSignCompare) {
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

TEST_F(DecimalTest, LargeValueAddOverflow) {
    Decimal d0(Decimal::kDecimalInt128Max);
    Decimal d1(Decimal::kDecimalInt128Min);
    EXPECT_EXIT(d0 + d0, testing::KilledBySignal(SIGABRT), "");
    EXPECT_EXIT(d1 + d1, testing::KilledBySignal(SIGABRT), "");
}

TEST_F(DecimalTest, Int128AddOverflow) {
    __int128_t res128 = 0;
    ErrCode err = kOk;

    __int128_t i128max = kInt128Max;
    err = safe_add_int128(res128, i128max, i128max);
    EXPECT_TRUE(!!err);

    __int128_t i128min = kInt128Min;
    err = safe_add_int128(res128, i128min, i128min);
    EXPECT_TRUE(!!err);
}

TEST_F(DecimalTest, Int256AddOverflow) {
    using namespace boost::multiprecision;
    int256_t res256 = 0;
    ErrCode err = kOk;

    int256_t i256max = kInt256Max;
    err = safe_add_int256(res256, i256max, i256max);
    EXPECT_TRUE(!!err);

    int256_t i256min = kInt256Min;
    err = safe_add_int256(res256, i256min, i256min);
    EXPECT_TRUE(!!err);

    // Non-overflow cases
    int256_t i256val = 123;
    err = safe_add_int256(res256, i256val, i256val);
    EXPECT_TRUE(!err);
}

TEST_F(DecimalTest, safe_mul_int128) {
    __int128_t res128 = 0;
    ErrCode err = kOk;

    // +  vs  +, non-overflow
    err = safe_mul_int128(res128, 123, 456);
    EXPECT_TRUE(!err);
    // +  vs  +, overflow
    err = safe_mul_int128(res128, kInt128Max, kInt128Max);
    EXPECT_TRUE(!!err);
    // +  vs  -, non-overflow
    err = safe_mul_int128(res128, 123, -456);
    EXPECT_TRUE(!err);
    // +  vs  -, overflow
    err = safe_mul_int128(res128, kInt128Max, kInt128Min);
    EXPECT_TRUE(!!err);
    // -  vs  +, non-overflow
    err = safe_mul_int128(res128, -123, 456);
    EXPECT_TRUE(!err);
    // -  vs  +, overflow
    err = safe_mul_int128(res128, kInt128Min, kInt128Max);
    EXPECT_TRUE(!!err);
    // -  vs  -, non-overflow
    err = safe_mul_int128(res128, -123, -456);
    EXPECT_TRUE(!err);
    // -  vs  -, overflow
    err = safe_mul_int128(res128, kInt128Min, kInt128Min);
    EXPECT_TRUE(!!err);
}

TEST_F(DecimalTest, safe_mul_int256) {
    using namespace boost::multiprecision;
    int256_t res256 = 0;
    ErrCode err = kOk;

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

TEST_F(DecimalTest, safe_div_int128) {
    __int128_t res128 = 0;
    ErrCode err = kOk;

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

TEST_F(DecimalTest, safe_div_int256) {
    using namespace boost::multiprecision;
    int256_t res256 = 0;
    ErrCode err = kOk;

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

TEST_F(DecimalTest, int128_to_string) {
    EXPECT_EQ(convert_int128_to_string(0), "0");
    EXPECT_EQ(convert_int128_to_string(123), "123");
    EXPECT_EQ(convert_int128_to_string(-123), "-123");
    EXPECT_EQ(convert_int128_to_string(kInt128Max), "170141183460469231731687303715884105727");
    EXPECT_EQ(convert_int128_to_string(kInt128Min), "-170141183460469231731687303715884105728");
}

TEST_F(DecimalTest, int256_to_string) {
    using namespace boost::multiprecision;
    EXPECT_EQ(convert_int256_to_string(0), "0");
    EXPECT_EQ(convert_int256_to_string(123), "123");
    EXPECT_EQ(convert_int256_to_string(-123), "-123");
    EXPECT_EQ(convert_int256_to_string(kInt256Max),
              "57896044618658097711785492504343953926634992332820282019728792003956564819967");
    EXPECT_EQ(convert_int256_to_string(kInt256Min),
              "-57896044618658097711785492504343953926634992332820282019728792003956564819968");
}

TEST_F(DecimalTest, InitializeDecimalWithEmptyString) {
    Decimal d0("");
    Decimal d1("");
    EXPECT_EQ(d0, d1);
    EXPECT_EQ(d0 + d1, d0);
    EXPECT_EQ(d0 + d1, d1);
}

TEST_F(DecimalTest, TrailingDot) {
    Decimal d0;
    ErrCode err;

    err = d0.assign("123.0");
    EXPECT_TRUE(!err);

    err = d0.assign("123.");
    EXPECT_TRUE(err);
}

TEST_F(DecimalTest, LargePartOverflow) {
    Decimal d0;
    ErrCode err;

    // Significant digits overflow
    err = d0.assign("12345678901234567890123456789012345678901.11");
    EXPECT_TRUE(err);

    // Least significant digits overflow
    err = d0.assign("11.12345678901234567890123456789012345678901");
    EXPECT_TRUE(err);
}

TEST_F(DecimalTest, InvalidCharacters) {
    Decimal d0;
    ErrCode err;

    // Invalid character in significant part.
    err = d0.assign("12345678901abc.11");
    EXPECT_TRUE(err);

    // Invalid character in least significant part.
    err = d0.assign("11.1234567014444abc");
    EXPECT_TRUE(err);
}

TEST_F(DecimalTest, DecimalAddSubResultTrailingLeastSignificantZero) {
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

// Decimal multiply as int128 overflow, but the intermediate result could be held in int256.
TEST_F(DecimalTest, DecimalMulAsInt128Overflow) {
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
        Decimal d2 = d0 * d1;  // 100000000015555555556.16049227160493815061
        EXPECT_EQ(d2.to_string(), "100000000015555555556.1604922716049382");
    }
}

TEST_F(DecimalTest, DecimalDivAsInt128Overflow) {
    {
        Decimal d0{"9999999999999999999999.22"};
        Decimal d1{"11.9999999999999999"};
        Decimal d3 = d0 / d1;  // 833333333333333340277.7127777777778356476
        EXPECT_EQ(d3.to_string(), "833333333333333340277.712778");
    }

    {
        Decimal d0{"-9999999999999999999999.22"};
        Decimal d1{"11.9999999999999999"};
        Decimal d3 = d0 / d1;  // -833333333333333340277.7127777777778356476
        EXPECT_EQ(d3.to_string(), "-833333333333333340277.712778");
    }
    {
        Decimal d0{"9999999999999999999999.22"};
        Decimal d1{"-11.9999999999999999"};
        Decimal d3 = d0 / d1;  // -833333333333333340277.7127777777778356476
        EXPECT_EQ(d3.to_string(), "-833333333333333340277.712778");
    }
    {
        Decimal d0{"9999999999999999999999.2222222222222222"};
        Decimal d1{"11.3333333333333333"};
        Decimal d3 = d0 / d1;  // 882352941176470590830.38119953863899263641
        EXPECT_EQ(d3.to_string(), "882352941176470590830.3811995386389926");
    }
    {
        Decimal d0{"-9999999999999999999999.2222222222222222"};
        Decimal d1{"11.3333333333333333"};
        Decimal d3 = d0 / d1;  // -882352941176470590830.38119953863899263641
        EXPECT_EQ(d3.to_string(), "-882352941176470590830.3811995386389926");
    }
    {
        Decimal d0{"9999999999999999999999.2222222222222222"};
        Decimal d1{"-11.3333333333333333"};
        Decimal d3 = d0 / d1;  // -882352941176470590830.38119953863899263641
        EXPECT_EQ(d3.to_string(), "-882352941176470590830.3811995386389926");
    }

    // Test 5 -> 1 carry
    {
        Decimal d0{"9999999999999999999999.9999999999999999"};
        Decimal d1{"11.1111111111111111"};
        Decimal d3 = d0 / d1;  // 900000000000000000900.000000000000000891
        EXPECT_EQ(d3.to_string(), "900000000000000000900.0000000000000009");
    }
    {
        Decimal d0{"-9999999999999999999999.9999999999999999"};
        Decimal d1{"11.1111111111111111"};
        Decimal d3 = d0 / d1;  // -900000000000000000900.000000000000000891
        EXPECT_EQ(d3.to_string(), "-900000000000000000900.0000000000000009");
    }
    {
        Decimal d0{"9999999999999999999999.9999999999999999"};
        Decimal d1{"-11.1111111111111111"};
        Decimal d3 = d0 / d1;  // -900000000000000000900.000000000000000891
        EXPECT_EQ(d3.to_string(), "-900000000000000000900.0000000000000009");
    }

    // Test trailing zero stripped.
    {
        Decimal d0{"1000000000000000000000.8888888888888883"};
        Decimal d1{"10.2222222222222222"};
        Decimal d3 = d0 / d1;  // 97826086956521739343.1871455576559550362764033
        // 97826086956521739343.1871455576559550
        //                      ~~~~16 digits~~~
        //   -> 97826086956521739343.187145557655955
        //                           ~~~15 digits~~~
        EXPECT_EQ(d3.to_string(), "97826086956521739343.187145557655955");
    }
}

TEST_F(DecimalTest, StaticCastToString) {
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

TEST_F(DecimalTest, StaticCastToDouble) {
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

TEST_F(DecimalTest, ConstexprInitializationOK) {
    {
        // Simple C string
        constexpr Decimal d0("0");
        EXPECT_EQ(d0.to_string(), "0");
    }
    {
        constexpr Decimal d0("0.1");
        EXPECT_EQ(d0.to_string(), "0.1");
    }
    {
        constexpr Decimal d0("123.1");
        EXPECT_EQ(d0.to_string(), "123.1");
    }
    {
        constexpr Decimal d0("123.666");
        EXPECT_EQ(d0.to_string(), "123.666");
    }
    {
        constexpr Decimal d0("-123.666");
        EXPECT_EQ(d0.to_string(), "-123.666");
    }

    // Leading zero truncation
    {
        constexpr Decimal d0("000.1");
        EXPECT_EQ(d0.to_string(), "0.1");
    }
    {
        constexpr Decimal d0("00.0000");
        EXPECT_EQ(d0.to_string(), "0");
    }
    {
        constexpr Decimal d0("00.11223455");
        EXPECT_EQ(d0.to_string(), "0.11223455");
    }
    {
        constexpr Decimal d0("-00.11223455");
        EXPECT_EQ(d0.to_string(), "-0.11223455");
    }
    {
        constexpr Decimal d0("-00123.11223455");
        EXPECT_EQ(d0.to_string(), "-123.11223455");
    }
    {
        constexpr Decimal d0("-0044.11223455");
        EXPECT_EQ(d0.to_string(), "-44.11223455");
    }
    {
        constexpr Decimal d0("-000999.11223455");
        EXPECT_EQ(d0.to_string(), "-999.11223455");
    }

    // trailing '0' omitted, but not those middle ones
    {
        constexpr Decimal d0("101.101");
        EXPECT_EQ(d0.to_string(), "101.101");
    }
    {
        constexpr Decimal d0("-101.101");
        EXPECT_EQ(d0.to_string(), "-101.101");
    }
    {
        constexpr Decimal d0("101.1010");
        EXPECT_EQ(d0.to_string(), "101.101");
    }
    {
        constexpr Decimal d0("-101.1010");
        EXPECT_EQ(d0.to_string(), "-101.101");
    }
    {
        constexpr Decimal d0("200.1000");
        EXPECT_EQ(d0.to_string(), "200.1");
    }
    {
        constexpr Decimal d0("-200.1000");
        EXPECT_EQ(d0.to_string(), "-200.1");
    }
    {
        constexpr Decimal d0("0.0000");
        EXPECT_EQ(d0.to_string(), "0");
    }
    {
        constexpr Decimal d0("-0.0000");
        EXPECT_EQ(d0.to_string(), "0");
    }
    // Invalid constexpr initialization. Would fail to compile.
    //   {
    //       constexpr Decimal d0("1af.123");
    //       EXPECT_EQ(d0.to_string(), "0");
    //   }
}
TEST_F(DecimalTest, ConstExprAdd) {
    {
        constexpr Decimal d0("0.12345");
        constexpr Decimal d1("0.54321");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "0.66666");
    }
    {
        constexpr Decimal d0("123.456");
        constexpr Decimal d1("543.21");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "666.666");
    }
    {
        constexpr Decimal d0("444.32");
        constexpr Decimal d1("555.123");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "999.443");
    }
    {
        constexpr Decimal d0("2421341234.133");
        constexpr Decimal d1("123123123.123");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "2544464357.256");
    }
    {
        constexpr Decimal d0("-0.12345");
        constexpr Decimal d1("-0.54321");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "-0.66666");
    }
    {
        constexpr Decimal d0("-123.456");
        constexpr Decimal d1("-543.21");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "-666.666");
    }
    {
        constexpr Decimal d0("-444.32");
        constexpr Decimal d1("-555.123");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "-999.443");
    }
    {
        constexpr Decimal d0("-2421341234.133");
        constexpr Decimal d1("-123123123.123");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "-2544464357.256");
    }
    {
        constexpr Decimal d0("-0.12345");
        constexpr Decimal d1("0.54321");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "0.41976");
    }
    {
        constexpr Decimal d0("-123.456");
        constexpr Decimal d1("543.21");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "419.754");
    }
    {
        constexpr Decimal d0("-444.32");
        constexpr Decimal d1("555.123");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "110.803");
    }
    {
        constexpr Decimal d0("-2421341234.133");
        constexpr Decimal d1("123123123.123");
        constexpr Decimal d2 = d0 + d1;
        EXPECT_EQ(d2.to_string(), "-2298218111.01");
    }
}

TEST_F(DecimalTest, ConstExprSub) {
    // {"0.12345", "0.54321", ArithOp::SUB, "-0.41976"},
    {
        constexpr Decimal d0("0.12345");
        constexpr Decimal d1("0.54321");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-0.41976");
    }
    // {"123.456", "543.21", ArithOp::SUB, "-419.754"},
    {
        constexpr Decimal d0("123.456");
        constexpr Decimal d1("543.21");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-419.754");
    }
    // {"444.32", "555.123", ArithOp::SUB, "-110.803"},
    {
        constexpr Decimal d0("444.32");
        constexpr Decimal d1("555.123");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-110.803");
    }
    // {"2421341234.133", "123123123.123", ArithOp::SUB, "2298218111.01"},
    {
        constexpr Decimal d0("2421341234.133");
        constexpr Decimal d1("123123123.123");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "2298218111.01");
    }
    // {"-0.12345", "-0.54321", ArithOp::SUB, "0.41976"},
    {
        constexpr Decimal d0("-0.12345");
        constexpr Decimal d1("-0.54321");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "0.41976");
    }
    // {"-123.456", "-543.21", ArithOp::SUB, "419.754"},
    {
        constexpr Decimal d0("-123.456");
        constexpr Decimal d1("-543.21");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "419.754");
    }
    // {"-444.32", "-555.123", ArithOp::SUB, "110.803"},
    {
        constexpr Decimal d0("-444.32");
        constexpr Decimal d1("-555.123");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "110.803");
    }
    // {"-2421341234.133", "-123123123.123", ArithOp::SUB, "-2298218111.01"},
    {
        constexpr Decimal d0("-2421341234.133");
        constexpr Decimal d1("-123123123.123");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-2298218111.01");
    }
    // {"-0.12345", "0.54321", ArithOp::SUB, "-0.66666"},
    {
        constexpr Decimal d0("-0.12345");
        constexpr Decimal d1("0.54321");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-0.66666");
    }
    // {"-123.456", "543.21", ArithOp::SUB, "-666.666"},
    {
        constexpr Decimal d0("-123.456");
        constexpr Decimal d1("543.21");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-666.666");
    }
    // {"-444.32", "555.123", ArithOp::SUB, "-999.443"},
    {
        constexpr Decimal d0("-444.32");
        constexpr Decimal d1("555.123");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-999.443");
    }
    // {"-2421341234.133", "123123123.123", ArithOp::SUB, "-2544464357.256"}
    {
        constexpr Decimal d0("-2421341234.133");
        constexpr Decimal d1("123123123.123");
        constexpr Decimal d2 = d0 - d1;
        EXPECT_EQ(d2.to_string(), "-2544464357.256");
    }
}

TEST_F(DecimalTest, ConstExprMul) {
    // {"0.12345", "0.54321", ArithOp::MUL, "0.0670592745"},
    {
        constexpr Decimal d0("0.12345");
        constexpr Decimal d1("0.54321");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "0.0670592745");
    }
    // {"123.456", "543.21", ArithOp::MUL, "67062.53376"},
    {
        constexpr Decimal d0("123.456");
        constexpr Decimal d1("543.21");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "67062.53376");
    }
    // {"444.32", "555.123", ArithOp::MUL, "246652.25136"},
    {
        constexpr Decimal d0("444.32");
        constexpr Decimal d1("555.123");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "246652.25136");
    }
    // {"2421341234.133", "123123123.123", ArithOp::MUL, "298123094892954129.157359"},
    {
        constexpr Decimal d0("2421341234.133");
        constexpr Decimal d1("123123123.123");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "298123094892954129.157359");
    }
    // {"-0.12345", "-0.54321", ArithOp::MUL, "0.0670592745"},
    {
        constexpr Decimal d0("-0.12345");
        constexpr Decimal d1("-0.54321");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "0.0670592745");
    }
    // {"-123.456", "-543.21", ArithOp::MUL, "67062.53376"},
    {
        constexpr Decimal d0("-123.456");
        constexpr Decimal d1("-543.21");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "67062.53376");
    }
    // {"-444.32", "-555.123", ArithOp::MUL, "246652.25136"},
    {
        constexpr Decimal d0("-444.32");
        constexpr Decimal d1("-555.123");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "246652.25136");
    }
    // {"-2421341234.133", "-123123123.123", ArithOp::MUL, "298123094892954129.157359"},
    {
        constexpr Decimal d0("-2421341234.133");
        constexpr Decimal d1("-123123123.123");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "298123094892954129.157359");
    }
    // {"-0.12345", "0.54321", ArithOp::MUL, "-0.0670592745"},
    {
        constexpr Decimal d0("-0.12345");
        constexpr Decimal d1("0.54321");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "-0.0670592745");
    }
    // {"-123.456", "543.21", ArithOp::MUL, "-67062.53376"},
    {
        constexpr Decimal d0("-123.456");
        constexpr Decimal d1("543.21");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "-67062.53376");
    }
    // {"-444.32", "555.123", ArithOp::MUL, "-246652.25136"},
    {
        constexpr Decimal d0("-444.32");
        constexpr Decimal d1("555.123");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "-246652.25136");
    }
    // {"-2421341234.133", "123123123.123", ArithOp::MUL, "-298123094892954129.157359"}};
    {
        constexpr Decimal d0("-2421341234.133");
        constexpr Decimal d1("123123123.123");
        constexpr Decimal d2 = d0 * d1;
        EXPECT_EQ(d2.to_string(), "-298123094892954129.157359");
    }
}

TEST_F(DecimalTest, ConstExprDiv) {
    // {"1", "3", ArithOp::DIV, "0.3333"},
    {
        constexpr Decimal d0("1");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.3333");
    }
    // {"100000", "3.33", ArithOp::DIV, "30030.03"},
    {
        constexpr Decimal d0("100000");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.03");
    }
    // {"999999", "3.33", ArithOp::DIV, "300300"},
    {
        constexpr Decimal d0("999999");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300");
    }
    // {"123456", "3.33", ArithOp::DIV, "37073.8739"},
    {
        constexpr Decimal d0("123456");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37073.8739");
    }
    // {"-1", "3", ArithOp::DIV, "-0.3333"},
    {
        constexpr Decimal d0("-1");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-0.3333");
    }
    // {"-100000", "3.33", ArithOp::DIV, "-30030.03"},
    {
        constexpr Decimal d0("-100000");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-30030.03");
    }
    // {"-999999", "3.33", ArithOp::DIV, "-300300"},
    {
        constexpr Decimal d0("-999999");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-300300");
    }
    // {"-123456", "3.33", ArithOp::DIV, "-37073.8739"},
    {
        constexpr Decimal d0("-123456");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-37073.8739");
    }
    // {"-1", "-3", ArithOp::DIV, "0.3333"},
    {
        constexpr Decimal d0("-1");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.3333");
    }
    // {"-100000", "-3.33", ArithOp::DIV, "30030.03"},
    {
        constexpr Decimal d0("-100000");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.03");
    }
    // {"-999999", "-3.33", ArithOp::DIV, "300300"},
    {
        constexpr Decimal d0("-999999");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300");
    }
    // {"-123456", "-3.33", ArithOp::DIV, "37073.8739"},
    {
        constexpr Decimal d0("-123456");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37073.8739");
    }
    // {"1.00001", "3", ArithOp::DIV, "0.333336667"},
    {
        constexpr Decimal d0("1.00001");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.333336667");
    }
    // {"100000.00001", "3.33", ArithOp::DIV, "30030.030033033"},
    {
        constexpr Decimal d0("100000.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.030033033");
    }
    // {"999999.00001", "3.33", ArithOp::DIV, "300300.000003003"},
    {
        constexpr Decimal d0("999999.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300.000003003");
    }
    // {"123456.00001", "3.33", ArithOp::DIV, "37073.873876877"},
    {
        constexpr Decimal d0("123456.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37073.873876877");
    }
    // {"-1.00001", "3", ArithOp::DIV, "-0.333336667"},
    {
        constexpr Decimal d0("-1.00001");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-0.333336667");
    }
    // {"-100000.00001", "3.33", ArithOp::DIV, "-30030.030033033"},
    {
        constexpr Decimal d0("-100000.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-30030.030033033");
    }
    // {"-999999.00001", "3.33", ArithOp::DIV, "-300300.000003003"},
    {
        constexpr Decimal d0("-999999.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-300300.000003003");
    }
    // {"-123456.00001", "3.33", ArithOp::DIV, "-37073.873876877"},
    {
        constexpr Decimal d0("-123456.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-37073.873876877");
    }
    // {"1.57565", "3", ArithOp::DIV, "0.525216667"},
    {
        constexpr Decimal d0("1.57565");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.525216667");
    }
    // {"100000.57565", "3.33", ArithOp::DIV, "30030.202897898"},
    {
        constexpr Decimal d0("100000.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.202897898");
    }
    // {"999999.57565", "3.33", ArithOp::DIV, "300300.172867868"},
    {
        constexpr Decimal d0("999999.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300.172867868");
    }
    // {"123456.57565", "3.33", ArithOp::DIV, "37074.046741742"},
    {
        constexpr Decimal d0("123456.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37074.046741742");
    }
    // {"-1.57565", "3", ArithOp::DIV, "-0.525216667"},
    {
        constexpr Decimal d0("-1.57565");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-0.525216667");
    }
    // {"-100000.57565", "3.33", ArithOp::DIV, "-30030.202897898"},
    {
        constexpr Decimal d0("-100000.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-30030.202897898");
    }
    // {"-999999.57565", "3.33", ArithOp::DIV, "-300300.172867868"},
    {
        constexpr Decimal d0("-999999.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-300300.172867868");
    }
    // {"-123456.57565", "3.33", ArithOp::DIV, "-37074.046741742"},
    {
        constexpr Decimal d0("-123456.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-37074.046741742");
    }
    // {"-1.57565", "-3", ArithOp::DIV, "0.525216667"},
    {
        constexpr Decimal d0("-1.57565");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.525216667");
    }
    // {"-100000.57565", "-3.33", ArithOp::DIV, "30030.202897898"},
    {
        constexpr Decimal d0("-100000.57565");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.202897898");
    }
    // {"-999999.57565", "-3.33", ArithOp::DIV, "300300.172867868"},
    {
        constexpr Decimal d0("-999999.57565");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300.172867868");
    }
    // {"-123456.57565", "-3.33", ArithOp::DIV, "37074.046741742"},
    {
        constexpr Decimal d0("-123456.57565");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37074.046741742");
    }
    // {"1", "-1", ArithOp::DIV, "-1"},
    {
        constexpr Decimal d0("1");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-1");
    }
    // {"100000", "-1", ArithOp::DIV, "-100000"},
    {
        constexpr Decimal d0("100000");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-100000");
    }
    // {"999999", "-1", ArithOp::DIV, "-999999"},
    {
        constexpr Decimal d0("999999");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-999999");
    }
    // {"123456", "-1", ArithOp::DIV, "-123456"},
    {
        constexpr Decimal d0("123456");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-123456");
    }
    // {"-1", "-1", ArithOp::DIV, "1"},
    {
        constexpr Decimal d0("-1");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "1");
    }
    // {"-100000", "-1", ArithOp::DIV, "100000"},
    {
        constexpr Decimal d0("-100000");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "100000");
    }
    // {"-999999", "-1", ArithOp::DIV, "999999"},
    {
        constexpr Decimal d0("-999999");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "999999");
    }
    // {"-123456", "-1", ArithOp::DIV, "123456"},
    {
        constexpr Decimal d0("-123456");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "123456");
    }
    // {"1.00001", "-1", ArithOp::DIV, "-1.00001"},
    {
        constexpr Decimal d0("1.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-1.00001");
    }
    // {"100000.00001", "-1", ArithOp::DIV, "-100000.00001"},
    {
        constexpr Decimal d0("100000.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-100000.00001");
    }
    // {"999999.00001", "-1", ArithOp::DIV, "-999999.00001"},
    {
        constexpr Decimal d0("999999.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-999999.00001");
    }
    // {"123456.00001", "-1", ArithOp::DIV, "-123456.00001"},
    {
        constexpr Decimal d0("123456.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-123456.00001");
    }
    // {"-1.00001", "-1", ArithOp::DIV, "1.00001"},
    {
        constexpr Decimal d0("-1.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "1.00001");
    }
    // {"-100000.00001", "-1", ArithOp::DIV, "100000.00001"},
    {
        constexpr Decimal d0("-100000.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "100000.00001");
    }
    // {"-999999.00001", "-1", ArithOp::DIV, "999999.00001"},
    {
        constexpr Decimal d0("-999999.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "999999.00001");
    }
    // {"-123456.00001", "-1", ArithOp::DIV, "123456.00001"},
    {
        constexpr Decimal d0("-123456.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "123456.00001");
    }
    // {"1.57565", "-1", ArithOp::DIV, "-1.57565"},
    {
        constexpr Decimal d0("1.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-1.57565");
    }
    // {"100000.57565", "-1", ArithOp::DIV, "-100000.57565"},
    {
        constexpr Decimal d0("100000.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-100000.57565");
    }
    // {"999999.57565", "-1", ArithOp::DIV, "-999999.57565"},
    {
        constexpr Decimal d0("999999.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-999999.57565");
    }
    // {"123456.57565", "-1", ArithOp::DIV, "-123456.57565"},
    {
        constexpr Decimal d0("123456.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-123456.57565");
    }
    // {"-1.57565", "-1", ArithOp::DIV, "1.57565"},
    {
        constexpr Decimal d0("-1.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "1.57565");
    }
    // {"-100000.57565", "-1", ArithOp::DIV, "100000.57565"},
    {
        constexpr Decimal d0("-100000.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "100000.57565");
    }
    // {"-999999.57565", "-1", ArithOp::DIV, "999999.57565"},
    {
        constexpr Decimal d0("-999999.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "999999.57565");
    }
    // {"-123456.57565", "-1", ArithOp::DIV, "123456.57565"},
    {
        constexpr Decimal d0("-123456.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "123456.57565");
    }
    // {"1.5756533334441", "3", ArithOp::DIV, "0.5252177778147"},
    {
        constexpr Decimal d0("1.5756533334441");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.5252177778147");
    }
    // {"30030.202898898933", "3.33", ArithOp::DIV, "9018.0789486182981982"},
    {
        constexpr Decimal d0("30030.202898898933");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "9018.0789486182981982");
    }
    // {"100000.111111111111111", "3.33", ArithOp::DIV, "30030.0633967300633967"},
    {
        constexpr Decimal d0("100000.111111111111111");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.0633967300633967");
    }
    // {"999999.111111111111111", "3.33", ArithOp::DIV, "300300.0333667000333667"},
    {
        constexpr Decimal d0("999999.111111111111111");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300.0333667000333667");
    }
    // {"123456.111111111111111", "3.33", ArithOp::DIV, "37073.9072405739072405"},
    {
        constexpr Decimal d0("123456.111111111111111");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37073.9072405739072405");
    }
    // {"1.5756533334441", "-3", ArithOp::DIV, "-0.5252177778147"},
    {
        constexpr Decimal d0("1.5756533334441");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-0.5252177778147");
    }
    // {"30030.202898898933", "-3.33", ArithOp::DIV, "-9018.0789486182981982"},
    {
        constexpr Decimal d0("30030.202898898933");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-9018.0789486182981982");
    }
    // {"100000.111111111111111", "-3.33", ArithOp::DIV, "-30030.0633967300633967"},
    {
        constexpr Decimal d0("100000.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-30030.0633967300633967");
    }
    // {"999999.111111111111111", "-3.33", ArithOp::DIV, "-300300.0333667000333667"},
    {
        constexpr Decimal d0("999999.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-300300.0333667000333667");
    }
    // {"123456.111111111111111", "-3.33", ArithOp::DIV, "-37073.9072405739072405"},
    {
        constexpr Decimal d0("123456.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "-37073.9072405739072405");
    }
    // {"-1.5756533334441", "-3", ArithOp::DIV, "0.5252177778147"},
    {
        constexpr Decimal d0("-1.5756533334441");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "0.5252177778147");
    }
    // {"-30030.202898898933", "-3.33", ArithOp::DIV, "9018.0789486182981982"},
    {
        constexpr Decimal d0("-30030.202898898933");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "9018.0789486182981982");
    }
    // {"-100000.111111111111111", "-3.33", ArithOp::DIV, "30030.0633967300633967"},
    {
        constexpr Decimal d0("-100000.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "30030.0633967300633967");
    }
    // {"-999999.111111111111111", "-3.33", ArithOp::DIV, "300300.0333667000333667"},
    {
        constexpr Decimal d0("-999999.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "300300.0333667000333667");
    }
    // {"-123456.111111111111111", "-3.33", ArithOp::DIV, "37073.9072405739072405"},
    {
        constexpr Decimal d0("-123456.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 / d1;
        EXPECT_EQ(d2.to_string(), "37073.9072405739072405");
    }
}

TEST_F(DecimalTest, ConstExprMod) {
    //=------------------------------------------
    // Transform these tests into constexpr tests
    //=------------------------------------------
    // {"1", "3", ArithOp::MOD, "1"},
    {
        constexpr Decimal d0("1");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1");
    }
    // {"100000", "3.33", ArithOp::MOD, "0.1"},
    {
        constexpr Decimal d0("100000");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.1");
    }
    // {"999999", "3.33", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("999999");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"123456", "3.33", ArithOp::MOD, "2.91"},
    {
        constexpr Decimal d0("123456");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "2.91");
    }
    // {"-1", "3", ArithOp::MOD, "-1"},
    {
        constexpr Decimal d0("-1");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1");
    }
    // {"-100000", "3.33", ArithOp::MOD, "-0.1"},
    {
        constexpr Decimal d0("-100000");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.1");
    }
    // {"-999999", "3.33", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("-999999");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-123456", "3.33", ArithOp::MOD, "-2.91"},
    {
        constexpr Decimal d0("-123456");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-2.91");
    }
    // {"-1", "-3", ArithOp::MOD, "-1"},
    {
        constexpr Decimal d0("-1");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1");
    }
    // {"-100000", "-3.33", ArithOp::MOD, "-0.1"},
    {
        constexpr Decimal d0("-100000");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.1");
    }
    // {"-999999", "-3.33", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("-999999");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-123456", "-3.33", ArithOp::MOD, "-2.91"},
    {
        constexpr Decimal d0("-123456");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-2.91");
    }
    // {"1.00001", "3", ArithOp::MOD, "1.00001"},
    {
        constexpr Decimal d0("1.00001");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.00001");
    }
    // {"100000.00001", "3.33", ArithOp::MOD, "0.10001"},
    {
        constexpr Decimal d0("100000.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.10001");
    }
    // {"999999.00001", "3.33", ArithOp::MOD, "0.00001"},
    {
        constexpr Decimal d0("999999.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"123456.00001", "3.33", ArithOp::MOD, "2.91001"},
    {
        constexpr Decimal d0("123456.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "2.91001");
    }
    // {"-1.00001", "3", ArithOp::MOD, "-1.00001"},
    {
        constexpr Decimal d0("-1.00001");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.00001");
    }
    // {"-100000.00001", "3.33", ArithOp::MOD, "-0.10001"},
    {
        constexpr Decimal d0("-100000.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.10001");
    }
    // {"-999999.00001", "3.33", ArithOp::MOD, "-0.00001"},
    {
        constexpr Decimal d0("-999999.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-123456.00001", "3.33", ArithOp::MOD, "-2.91001"},
    {
        constexpr Decimal d0("-123456.00001");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-2.91001");
    }
    // {"1.57565", "3", ArithOp::MOD, "1.57565"},
    {
        constexpr Decimal d0("1.57565");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.57565");
    }
    // {"100000.57565", "3.33", ArithOp::MOD, "0.67565"},
    {
        constexpr Decimal d0("100000.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.67565");
    }
    // {"999999.57565", "3.33", ArithOp::MOD, "0.57565"},
    {
        constexpr Decimal d0("999999.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"123456.57565", "3.33", ArithOp::MOD, "0.15565"},
    {
        constexpr Decimal d0("123456.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.15565");
    }
    // {"-1.57565", "3", ArithOp::MOD, "-1.57565"},
    {
        constexpr Decimal d0("-1.57565");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.57565");
    }
    // {"-100000.57565", "3.33", ArithOp::MOD, "-0.67565"},
    {
        constexpr Decimal d0("-100000.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.67565");
    }
    // {"-999999.57565", "3.33", ArithOp::MOD, "-0.57565"},
    {
        constexpr Decimal d0("-999999.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-123456.57565", "3.33", ArithOp::MOD, "-0.15565"},
    {
        constexpr Decimal d0("-123456.57565");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.15565");
    }
    // {"-1.57565", "-3", ArithOp::MOD, "-1.57565"},
    {
        constexpr Decimal d0("-1.57565");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.57565");
    }
    // {"-100000.57565", "-3.33", ArithOp::MOD, "-0.67565"},
    {
        constexpr Decimal d0("-100000.57565");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.67565");
    }
    // {"-999999.57565", "-3.33", ArithOp::MOD, "-0.57565"},
    {
        constexpr Decimal d0("-999999.57565");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-123456.57565", "-3.33", ArithOp::MOD, "-0.15565"},
    {
        constexpr Decimal d0("-123456.57565");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.15565");
    }
    // {"1", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("1");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"100000", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("100000");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"999999", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("999999");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"123456", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("123456");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-1", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("-1");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-100000", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("-100000");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-999999", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("-999999");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"-123456", "-1", ArithOp::MOD, "0"},
    {
        constexpr Decimal d0("-123456");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0");
    }
    // {"1.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        constexpr Decimal d0("1.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"100000.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        constexpr Decimal d0("100000.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"999999.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        constexpr Decimal d0("999999.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"123456.00001", "-1", ArithOp::MOD, "0.00001"},
    {
        constexpr Decimal d0("123456.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.00001");
    }
    // {"-1.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        constexpr Decimal d0("-1.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-100000.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        constexpr Decimal d0("-100000.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-999999.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        constexpr Decimal d0("-999999.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"-123456.00001", "-1", ArithOp::MOD, "-0.00001"},
    {
        constexpr Decimal d0("-123456.00001");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.00001");
    }
    // {"1.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        constexpr Decimal d0("1.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"100000.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        constexpr Decimal d0("100000.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"999999.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        constexpr Decimal d0("999999.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"123456.57565", "-1", ArithOp::MOD, "0.57565"},
    {
        constexpr Decimal d0("123456.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.57565");
    }
    // {"-1.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        constexpr Decimal d0("-1.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-100000.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        constexpr Decimal d0("-100000.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-999999.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        constexpr Decimal d0("-999999.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"-123456.57565", "-1", ArithOp::MOD, "-0.57565"},
    {
        constexpr Decimal d0("-123456.57565");
        constexpr Decimal d1("-1");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.57565");
    }
    // {"1.5756533334441", "3", ArithOp::MOD, "1.5756533334441"},
    {
        constexpr Decimal d0("1.5756533334441");
        constexpr Decimal d1("3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.5756533334441");
    }
    // {"30030.202898898933", "3.33", ArithOp::MOD, "0.262898898933"},
    {
        constexpr Decimal d0("30030.202898898933");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.262898898933");
    }
    // {"100000.111111111111111", "3.33", ArithOp::MOD, "0.211111111111111"},
    {
        constexpr Decimal d0("100000.111111111111111");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.211111111111111");
    }
    // {"999999.111111111111111", "3.33", ArithOp::MOD, "0.111111111111111"},
    {
        constexpr Decimal d0("999999.111111111111111");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.111111111111111");
    }
    // {"123456.111111111111111", "3.33", ArithOp::MOD, "3.021111111111111"},
    {
        constexpr Decimal d0("123456.111111111111111");
        constexpr Decimal d1("3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "3.021111111111111");
    }
    // {"1.5756533334441", "-3", ArithOp::MOD, "1.5756533334441"},
    {
        constexpr Decimal d0("1.5756533334441");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "1.5756533334441");
    }
    // {"30030.202898898933", "-3.33", ArithOp::MOD, "0.262898898933"},
    {
        constexpr Decimal d0("30030.202898898933");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.262898898933");
    }
    // {"100000.111111111111111", "-3.33", ArithOp::MOD, "0.211111111111111"},
    {
        constexpr Decimal d0("100000.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.211111111111111");
    }
    // {"999999.111111111111111", "-3.33", ArithOp::MOD, "0.111111111111111"},
    {
        constexpr Decimal d0("999999.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "0.111111111111111");
    }
    // {"123456.111111111111111", "-3.33", ArithOp::MOD, "3.021111111111111"},
    {
        constexpr Decimal d0("123456.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "3.021111111111111");
    }
    // {"-1.5756533334441", "-3", ArithOp::MOD, "-1.5756533334441"},
    {
        constexpr Decimal d0("-1.5756533334441");
        constexpr Decimal d1("-3");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-1.5756533334441");
    }
    // {"-30030.202898898933", "-3.33", ArithOp::MOD, "-0.262898898933"},
    {
        constexpr Decimal d0("-30030.202898898933");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.262898898933");
    }
    // {"-100000.111111111111111", "-3.33", ArithOp::MOD, "-0.211111111111111"},
    {
        constexpr Decimal d0("-100000.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.211111111111111");
    }
    // {"-999999.111111111111111", "-3.33", ArithOp::MOD, "-0.111111111111111"},
    {
        constexpr Decimal d0("-999999.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-0.111111111111111");
    }
    // {"-123456.111111111111111", "-3.33", ArithOp::MOD, "-3.021111111111111"},
    {
        constexpr Decimal d0("-123456.111111111111111");
        constexpr Decimal d1("-3.33");
        constexpr Decimal d2 = d0 % d1;
        EXPECT_EQ(d2.to_string(), "-3.021111111111111");
    }
}

TEST_F(DecimalTest, ConstExprCompare) {
    // {"123.001", "-432.12", CompareOp::EQ, false}, {"123.001", "-432.12", CompareOp::NE, true},
    // {"123.001", "-432.12", CompareOp::LT, false}, {"123.001", "-432.12", CompareOp::LE, false},
    // {"123.001", "-432.12", CompareOp::GT, true},  {"123.001", "-432.12", CompareOp::GE, true},
    {
        constexpr Decimal d0("123.001");
        constexpr Decimal d1("-432.12");
        constexpr bool b0 = d0 == d1;
        constexpr bool b1 = d0 != d1;
        constexpr bool b2 = d0 < d1;
        constexpr bool b3 = d0 <= d1;
        constexpr bool b4 = d0 > d1;
        constexpr bool b5 = d0 >= d1;
        EXPECT_EQ(b0, false);
        EXPECT_EQ(b1, true);
        EXPECT_EQ(b2, false);
        EXPECT_EQ(b3, false);
        EXPECT_EQ(b4, true);
        EXPECT_EQ(b5, true);
    }
}

TEST_F(DecimalTest, ConstExprCompare_2) {
    // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::EQ, false},
    // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::NE, true},
    // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::LT, false},
    // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::LE, false},
    // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::GT, true},
    // {"999999999999999999999999999.001", "432.1234567891234567", CompareOp::GE, true},
    {
        constexpr Decimal d0("999999999999999999999999999.001");
        constexpr Decimal d1("432.1234567891234567");
        constexpr bool b0 = d0 == d1;
        constexpr bool b1 = d0 != d1;
        constexpr bool b2 = d0 < d1;
        constexpr bool b3 = d0 <= d1;
        constexpr bool b4 = d0 > d1;
        constexpr bool b5 = d0 >= d1;
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
        constexpr Decimal d0("432.1234567891234567");
        constexpr Decimal d1("999999999999999999999999999.001");
        constexpr bool b0 = d0 == d1;
        constexpr bool b1 = d0 != d1;
        constexpr bool b2 = d0 < d1;
        constexpr bool b3 = d0 <= d1;
        constexpr bool b4 = d0 > d1;
        constexpr bool b5 = d0 >= d1;
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
        constexpr Decimal d0("-999999999999999999999999999.001");
        constexpr Decimal d1("-432.1234567891234567");
        constexpr bool b0 = d0 == d1;
        constexpr bool b1 = d0 != d1;
        constexpr bool b2 = d0 < d1;
        constexpr bool b3 = d0 <= d1;
        constexpr bool b4 = d0 > d1;
        constexpr bool b5 = d0 >= d1;
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
        constexpr Decimal d0("-432.1234567891234567");
        constexpr Decimal d1("-999999999999999999999999999.001");
        constexpr bool b0 = d0 == d1;
        constexpr bool b1 = d0 != d1;
        constexpr bool b2 = d0 < d1;
        constexpr bool b3 = d0 <= d1;
        constexpr bool b4 = d0 > d1;
        constexpr bool b5 = d0 >= d1;
        EXPECT_EQ(b0, false);
        EXPECT_EQ(b1, true);
        EXPECT_EQ(b2, false);
        EXPECT_EQ(b3, false);
        EXPECT_EQ(b4, true);
        EXPECT_EQ(b5, true);
    }
}
#endif

// TODO test string initialization with leading space or leading zero
}  // namespace bignum
