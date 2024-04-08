#include "decimal.h"

#include <cstdlib>
#include <iostream>

void print_usage() {
        std::cerr << "decimal_calculator <decimal_str1> <decimal_str2> <op>" << std::endl;
}

void print_error(const std::string &errmsg) { std::cerr << errmsg << std::endl; }

bool is_error_overflow(bignum::ErrCode err) {
        if (!err) {
                return false;
        }

        auto val = err.error_code();
        return (val == bignum::ErrCodeValue::kDecimalAddSubOverflow ||
                val == bignum::ErrCodeValue::kDecimalDivOverflow ||
                val == bignum::ErrCodeValue::kDecimalMulOverflow ||
                val == bignum::ErrCodeValue::kDecimalScaleOverflow);
}
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

        bignum::Decimal res = lhs;
        bignum::ErrCode err;
        if (op == "+") {
                err = res.add(rhs);
        } else if (op == "-") {
                err = res.sub(rhs);
        } else if (op == "*") {
                err = res.mul(rhs);
        } else if (op == "/") {
                err = res.div(rhs);
        } else if (op == "%") {
                err = res.mod(rhs);
        } else {
                print_error("Unknown operation " + op);
                exit(1);
        }

        bool is_overflow = is_error_overflow(err);

        if (err) {
                if (is_overflow) {
                        print_error("Decimal calculator overflow");
                } else {
                        print_error("Decimal calculator error");
                }
                exit(1);
        }

        std::string str = static_cast<std::string>(res);
        std::cout << str;

        return 0;
}
