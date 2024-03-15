#include <iostream>
#include "Decimal.h"

using namespace bignum;

Decimal construct_decimal_from_string() {
    auto res = Decimal("10000000000000000000000000000000000000000000000011");
    return res;
}

int main() {
    Decimal a = construct_decimal_from_string();
    double d = static_cast<double>(a);
    std::cout << "Double cast: " << d << std::endl;

    std::string s = static_cast<std::string>(a);
    std::cout << "String cast: " << s << std::endl;
    return 0;
}
