#include <gtest/gtest.h>
#include <iostream>

#include "Decimal.h"

namespace bignum {
using namespace detail;

//=-------------------------------------------------------------------
// These are test cases that hit bugs or corner cases as time goes by
//=-------------------------------------------------------------------

class IssueTest : public ::testing::Test {};

TEST_F(IssueTest, case001) {
        Decimal d1("4533527415768002964173054920367325040031456305595549715.3");
        Decimal d2("239488530950790143412839555444765933.2163809400115095694428491613");
        Decimal d3 = d1 + d2;

        EXPECT_EQ(static_cast<std::string>(d3),
                  "4533527415768002964412543451318115183444295861040315648.516380940011509569");
}

TEST_F(IssueTest, case002) {
        Decimal d1("35272101902216212706414636703984778.46138620280095");
        Decimal d2("8338229360.8856451124078");
        Decimal d3 = d1 * d2;

        EXPECT_EQ(static_cast<std::string>(d3),
                  "294106875701209638510679238005211694343641861.30841008980345536995862741");
}
}  // namespace bignum
