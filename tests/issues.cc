#include <gtest/gtest.h>
#include <iostream>

#include "decimal.h"

namespace bignum {
using namespace detail;

//=-------------------------------------------------------------------
// These are test cases that hit bugs or corner cases as time goes by
//=-------------------------------------------------------------------

TEST(IssueTest, case001) {
        Decimal d1("560523670929766645251228.275617113");
        Decimal d2(
                "6611377617686453935686898473801113298366075758159259042709446259162556239682899074"
                "564129396.41");
        ErrCode err = d1.add(d2);
        EXPECT_TRUE(!!err);
}

TEST(IssueTest, case002) {
        Decimal d1("72.67414470056563283372");
        Decimal d2("27079892846274005397.943");
        Decimal res = d1 + d2;
        EXPECT_EQ(static_cast<std::string>(res), "27079892846274005470.61714470056563283372");
}

TEST(IssueTest, case003) {
        Decimal d1(
                "8832187650537776303206930649091789177797430957939666561473539619668."
                "6598993704102329012035443262");
        Decimal d2(
                "39759521277236470205399075724298920052842085366517561818667033184705055264732608."
                "731698158");
        Decimal res = d1;
        auto err = res.mul(d2);
        EXPECT_NE(!!err, 0);
}

TEST(IssueTest, case004) {
        Decimal d1("272596762.65142969092909283373932");
        Decimal d2("21649440307694835.6265212124968757592");
        Decimal res = d1 - d2;
        EXPECT_EQ(static_cast<std::string>(res), "-21649440035098072.97509152156778292546068");
}

TEST(IssueTest, case005) {
        Decimal d1(
                "9617624838768778037183071312213963121151837210331595263316324617527022666471172."
                "6347568516234");
        Decimal d2(
                "1339920337070008752332959161147423728446106087074681074923933401534611403."
                "78411550787523642401");
        Decimal res = d1;
        auto err = res.mul(d2);
        EXPECT_NE(!!err, 0);
}

TEST(IssueTest, case006) {
        Decimal d1("-1");
        Decimal d2("0.7771527547861743124");
        Decimal res = d1 * d2;
        EXPECT_EQ(static_cast<std::string>(res), "-0.7771527547861743124");
}

TEST(IssueTest, case007) {
        Decimal d1("0.4870223912296399425");
        Decimal d2("0.7");
        Decimal res = d1 - d2;
        EXPECT_EQ(static_cast<std::string>(res), "-0.2129776087703600575");
}
}  // namespace bignum
