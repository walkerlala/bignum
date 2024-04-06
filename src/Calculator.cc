#include "Decimal.h"

#include <cstdlib>
#include <iostream>

void print_usage() {
        std::cerr << "decimal_calculator <decimal_str1> <decimal_str2> <op>" << std::endl;
}

void print_error(const std::string &errmsg) { std::cerr << errmsg << std::endl; }

int main(int argc, char *argv[]) {
        if (argc != 4) {
                print_usage();
                exit(1);
        }

        bignum::Decimal lhs;
        if (lhs.assign(argv[1])) {
                print_error("Invalid Decimal string (arg1)");
                exit(1);
        }

        bignum::Decimal rhs;
        if (rhs.assign(argv[2])) {
                print_error("Invalid Decimal string (arg2)");
                exit(1);
        }

        std::string op = argv[3];

        bignum::Decimal res;
        if (op == "+") {
                res = lhs + rhs;
        } else if (op == "-") {
                res = lhs - rhs;
        } else if (op == "*") {
                res = lhs * rhs;
        } else if (op == "/") {
                res = lhs / rhs;
        } else if (op == "%") {
                res = lhs % rhs;
        } else {
                print_error("Unknown operation " + op);
                exit(1);
        }

        std::string str = static_cast<std::string>(res);
        std::cout << str;

        return 0;
}
