# bignum
bignum is a large precision Decimal implementation in C++ that supports up-to 96 digits precision
(i.e., 96 digits in total) and up-to 30 digits scale (i.e., digits after the decimal point).

## Features
- Large precision (at most 96 digits) and scale (at most 30 digits) decimal
- compile-time calculation with `constexpr` expression
- Currently only Linux platform with gcc/clang is tested. A compiler with C++20 support is required.

## Examples
### Initialization
A Decimal could be initialized from string/integer/floating point values;
precision and scale are auto-detected when initializing a Decimal object.
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

// Initialize with float/double variables (i.e., lvalue) is allowed, but initializing from
// rvalue (e.g., float/double literal) is not allowed to prevent misuse.
{
    double dval = 3.1415926;
    Decimal d1(dval);
    std::cout << "value: " << d1 << std::endl;  // output "3.14159260000000007"

    // Initialize from float/double literal would cause compile-error such as
    // 
    //    static assertion failed: Using literal floating point value to
    //    initialize Decimal is error-prone, use string literal instead,
    //    i.e., use Decimal("1.23") instead of Decimal(1.23)
    //
    // [[maybe_unused]] Decimal d2(3.1415926);
}
```

### Simple arithmetic operations
```cpp
#include <iostream>
#include <bignum/decimal.h>

int main() {
    Decimal d1("678.90");
    Decimal d2("123.45");

    std::cout << "d1=" << d1 << std::endl
              << "d2=" << d2 << std::endl
              << "d1 + d2 = " << d1 + d2 << std::endl
              << "d1 - d2 = " << d1 - d2 << std::endl
              << "d1 * d2 = " << d1 * d2 << std::endl
              << "d1 / d2 = " << d1 / d2 << std::endl
              << "d1 % d2 = " << d1 % d2 << std::endl;
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

    // Method 1, initialization overflow, use exception to handle error
    Decimal dlarge;
    try {
        dlarge = Decimal(large_str);
    } catch (const std::runtime_error &err) {
        // handle Decimal exception here
    } catch (...) {
        // unknown error
    }

    // Method 2, initialization overflow, use error-handling interfaces
    ErrCode err = dlarge.assign(large_str);
    if (err) {
        // handle error here
    }
}
```

`ErrCode` is an plain enum that could be use exchangeably with `int`:
```cpp
int err = dlarge.assign(large_str);
if (err) {
    // ...
}
```

All exception throwing interfaces have their error-handling counter-parts:
```cpp
// Initialization, constructor v.s. assign()
{
    Decimal d1("123.45");

    Decimal d2;
    ErrCode err = d2.assign("123.45");
}

// Arithmetic operations
{
    Decimal d1("678.90");
    Decimal d2("123.45");

    Decimal res1 = d1 + d2;
    ErrCode err1 = d1.add(d2);

    Decimal res2 = d1 - d2;
    ErrCode err2 = d1.sub(d2);

    // The same for:
    //  *  vs  mul()
    //  /  vs  div
    //  &  vs  mod
}
```

### Cast operations:
```cpp
Decimal d1("678.90");

// cast to string, gauranteed to succeed
{
    std::ostringstream oss;
    oss << d1;

    std::string str;
    [[maybe_unused]] ErrCode err = d1.to_string(str);

    std::string d2 = static_cast<std::string>(d1);
}

// cast to double, gauranteed to succeed
{
    double dval1;
    [[maybe_unused]] ErrCode err = d1.to_double(dval1);

    double dval2 = static_cast<double>(d1);
}

// cast to bool, gauranteed to succeed
{
    // compare to 0
    if (d1) {
    }

    bool b = static_cast<bool>(d1);
}

// cast to int64_t or __int128_t, truncate all digits after decimal points; might overflow
{
    // If overflow, exception or assertion occurs
    int64_t i64 = static_cast<int64_t>(d1);
    ErrCode err1 = d1.to_int64(i64);

    // If overflow, exception or assertion occurs
    __int128_t i128 = static_cast<__int128_t>(d1);
    ErrCode err1 = d1.to_int128(i128);
}
```

### compile-time calculation and compile time error
A Decimal could be constructed and calculated at compile time if the expression is declared as
`constexpr`:
```cpp
{
    // All calculation in the following expression is done at compile-time.
    constexpr Decimal d1("11223455.66");
    constexpr Decimal d2("23456.77");
    constexpr Decimal res = d1 + d2;

    // The following decimal string is invalid.
    // Because it is declared as constexpr, there would be compile-time error
    constexpr Decimal dinvalid("123aaa.bvb");

    // The following decimal string oveflow the maximum precision.
    // Because it is declared as constexpr, there would be compile-time error
    constexpr Decimal dmaxstrin("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999");

    // Result of the following decimal calculation overflow the maximum precision.
    // Because it is declared as constexpr, there would be compile-time error
    constexpr Decimal dlarge_1("9999999999999999999999999999999999999999999999999999999999999999999");
    constexpr Decimal dlarge_2("9999999999999999999999999999999999999999999999999999999999999999999");
    constexpr Decimal dlarge_result = dlarge_1 * dlarge_2;
}
```

## Install && Use
### Install using CMake
Compile and install bignum into `/path/to/install/dir/`:
```
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DBIGNUM_BUILD_TESTS=ON \
      -DCMAKE_INSTALL_PREFIX=/path/to/install/dir/
      /path/to/bignum/source
```

The bignum static library locates at `/path/to/install/dir/lib/libbignum.a` and its include
directory locates at `/path/to/install/dir/include`.
The install directory layout:
```
/path/to/install/dir
├── include
│   └── bignum
│       ├── assertion.h
│       ├── decimal.h
│       ├── errcode.h
│       └── gmp_wrapper.h
└── lib
    └── libbignum.a
```

Simply add `/path/to/install/dir/lib/` to your libraries search path and
`/path/to/install/dir/include` to your include files search path and compile.

By default, bignum is built as static library. To build as shared lib,
add `-DBIGNUM_BUILD_SHARED=ON` to the cmake command above.

## Benchmark
