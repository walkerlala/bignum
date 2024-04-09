# bignum
bignum is a large precision Decimal implementation in C++. It supports up-to 96 digits precision
(i.e., 96 digits in total) and up-to 30 digits scale (i.e., digits after the decimal point).

## Examples

### Simple arithmetic operations
```cpp
#include <iostream>
#include <bignum/decimal.h>

int main() {
    Decimal d1("678.90");
    Decimal d2("123.45");

    Decimal add_res = d1 + d2;
    std::cout << "d1 + d2" << add_res << std::endl;

    Decimal minus_res = d1 - d2;
    std::cout << "d1 - d2" << add_res << std::endl;

    Decimal multiply_res = d1 * d2;
    std::cout << "d1 * d2" << add_res << std::endl;

    Decimal div_res = d1 / d2;
    std::cout << "d1 / d2" << add_res << std::endl;

    Decimal mod_result =  d1 % d2;
    std::cout << "d1 % d2" << add_res << std::endl;
}
```

### Initialization interfaces (scale is auto-deduced)
```cpp
// Initialize from constant string
{
    Decimal d1("123.45");
    std::cout << "value: " << d1 << std::endl;  // output "123.45"
}

// Initialize from std::string or std::string_view
{
    std::string str = "99999.10";
    Decimal d1(str);
    std::cout << "value: " << d1 << std::endl;  // output "99999.1"
}

// Initialize from integer or __int128_t
{
    int64_t i64val = 31415926;
    Decimal d1(i64val);
    std::cout << "value: " << d1 << std::endl;  // output "31415926"

    int64_t i128val = 9999999999999999999.9;
    Decimal d2(i128val);
    std::cout << "value: " << d2 << std::endl;  // output "9999999999999999999.9"
}

// Initialize with float/double variables
{
    double dval = 3.1415926;
    Decimal d1(dval);
    std::cout << "value: " << d1 << std::endl;  // output "3.14159260000000007"
}
```

### Error handling
All operations on `Decimal` that cause overflow or error, will throw `std::runtime_error` exception
or trigger assertion by default, depending on whether the `BIGNUM_ENABLE_EXCEPTIONS` option is
turned on at compile time (which defines the `BIGNUM_ENABLE_EXCEPTIONS` macro).
`BIGNUM_ENABLE_EXCEPTIONS` is turned on by default.


There are two ways to handle error:

1. Enable exception and use `try { ... } catch { ... }` to handle error.
2. Use the expliclty error-handling interfaces.

Examples:
```cpp
{
    std::string small_str;
    for (int i = 0; i < Decimal::kMaxPrecision; i++) {
        small_str.push_back('9');
    }

    std::string large_str;
    for (int i = 0; i < Decimal::kMaxPrecision * 2; i++) {
        large_str.push_back('9');
    }

    Decimal dsmall(small_str);  // OK

    // Method 1, use exception
    Decimal dlarge;
    try {
        dlarge = Decimal(large_str);
    } catch (const std::runtime_error &err) {
        // handle Decimal exception here
    } catch (...) {
        // unknown error
    }

    // Method 2, use error-handling interfaces
    ErrCode err = dlarge.assign(large_str);
    if (err) {
        // handle error here
    }
}
```

### Cast operations:
```

```

### compile-time calculation and compile time error


## Features
- Currently only Linux platform with gcc/clang is tested. A compiler with C++20 support is required.

## Install

## Benchmark
