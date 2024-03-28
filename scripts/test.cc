#include <iostream>
#include "Decimal.h"

using namespace bignum;

Decimal construct_decimal_from_string() {
    auto res = Decimal("10000000000000000000000000000000000000000000000011");
    return res;
}

int main() {
    // Cast
    {
        Decimal a = construct_decimal_from_string();
        double d = static_cast<double>(a);
        std::cout << "Double cast: " << d << std::endl;

        std::string s = static_cast<std::string>(a);
        std::cout << "String cast: " << s << std::endl;
    }

    {
        Decimal a = construct_decimal_from_string();
        Decimal b = "100000000000000000000";
        Decimal c = a + b;
        std::cout << "Addition: " << static_cast<std::string>(c) << std::endl;
    }
    return 0;
}
