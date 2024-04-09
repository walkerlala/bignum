#include <gtest/gtest.h>
#include <iostream>

#include "decimal.h"

namespace bignum {
using namespace detail;

#ifdef BIGNUM_ENABLE_EXCEPTIONS
TEST(ExceptionOrAssert, exception) {
        Decimal a("1.23");
        Decimal b("0.00");
        Decimal c;

        bool found_runtime_error = false;
        try {
                c = a / b;
        } catch (const std::runtime_error &) {
                found_runtime_error = true;
        }

        EXPECT_TRUE(found_runtime_error);
}
#else
TEST(ExceptionOrAssert, abort) {
        Decimal a("1.23");
        Decimal b("0.00");
        Decimal c;
        EXPECT_DEATH(c = a / b, "");
}
#endif
}  // namespace bignum
