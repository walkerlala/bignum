# bignum
bignum is a large precision Decimal implementation in C++ that supports up-to 96 digits precision
(i.e., 96 digits in total) and up-to 30 digits scale (i.e., digits after the decimal point).

***This package is currently being benchmark and NOT released yet***

## Features
- Large precision (at most 96 digits) and scale (at most 30 digits) decimal
- Optimized for speed;
- Each `Decimal` object occupies fixed 64 bytes; no dynamic allocation;
- compile-time calculation with `constexpr` expression

Currently only Linux platform with gcc/clang is tested. A compiler with C++20 support is required.

## Initialization
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

    __int128_t i128val = 9999999999999999999;
    Decimal d2(i128val);
    std::cout << "value: " << d2 << std::endl;  // output "9999999999999999999"
}

// Initializing from float/double lvalue (e.g., float/double variables) is allowed.
// To prevent misuse, initializing from rvalue (e.g., float/double literal) is not allowed.
{
    double dval = 3.1415926;
    Decimal d1(dval);
    std::cout << "value: " << d1 << std::endl;  // output "3.14159260000000007"

    // Initialize from float/double literal would cause compile-error such as
    // 
    //    " static assertion failed: Using literal floating point value to
    //      initialize Decimal is error-prone, use string literal instead,
    //      i.e., use Decimal("1.23") instead of Decimal(1.23) "
    //
    // [[maybe_unused]] Decimal d2(3.1415926);
}
```

Initializing from literal float/double is intentionally prohibitted by default to prevent misuse,
as using a float literal might cause unexpected precision loss,
e.g., a float initialization `Decimal(3.1415926)` would result int `3.14159260000000007`,
instead of the original  `3.1415926`.
Notice that initialization with literal could always be `constexpr`, and a float literal
initialization `constexpr Decimal(1.23)` has
the same performance as a string literal initialization `constexpr Decimal("1.23")` because
these initializations are both done at compile time only. So users should prefer and use
literal string initialization.

Users who truely want literal float/double initialization could turn on the
`BIGNUM_ENABLE_LITERAL_FLOAT_CONSTRUCTOR` cmake option, which defines the
`BIGNUM_ENABLE_LITERAL_FLOAT_CONSTRUCTOR` macro at compile time.
`BIGNUM_ENABLE_LITERAL_FLOAT_CONSTRUCTOR` is turned off by default.

## Arithmetic operations
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

Note that even if initializing from float/double literals is not allowed, arithmetic operations
with float/double literals is allowed:
```cpp
#include <iostream>
#include <bignum/decimal.h>

int main() {
    Decimal d1("678.90");
    Decimal d2 = d1 + 3.1415926;
    std::cout << "d2=" << d2 << std::endl;  // Output "d2=682.04159260000000007"
}
```

## Overflow and rounding behavior
- Initialization that overflow the maximum precision (i.e., 96 digits in total) or
maximum scale (i.e., 30 digits after the decimal point) would result in overflow error.
No rounding happens in constructor or `assign()` interfaces.

- Multiplication might increase the scale, and if theoretical result scale is larger than
`Decimal::kMaxScale`, it is truncated and rounded to `Decimal::kMaxScale`.

- Scale of division result is equal to `dividend_scale + Decimal::kDivIncreaseScale`.
If `dividend_scale + Decimal::kDivIncreaseScale` is larger than `Decimal::kMaxScale`,
result scale is rounded to `Decimal::kMaxScale`.

- All rounding is done using the "round-half-up" rule and round away from zero.

- Casting a `Decimal` object into integer (e.g, int64, int128) truncates all least significant
digits and no rounding would happen, i.e., `static_cast<int64_t>(Decimal("123.6"))  == 123`.

## Error handling
There are 3 ways to handle error while using Decimal.

1. Use the explicit error-handling interfaces which return `ErrCode`. These interfaces are
   guaranteed to not throwing any exception.

2. Define the `BIGNUM_ENABLE_EXCEPTIONS` macro and use `try { ... } catch { ... }` to handle
   `DecimalError` exception (which is derived from `std::runtime_error`).
   If using CMake, user could turned on the `BIGNUM_ENABLE_EXCEPTIONS` option to define the macro.
   The `BIGNUM_ENABLE_EXCEPTIONS` option is turned on by default.

3. undefine the `BIGNUM_ENABLE_EXCEPTIONS` macro (just turn off the cmake option), and use the
   interface that does not return `ErrCode`. If overflow or error happens while using these
   interfaces, internal assertion will be triggered, which call `std::abort()` to crash the program.

Examples:
```cpp
std::string small_str;
for (int i = 0; i < Decimal::kMaxPrecision; i++) {
    small_str.push_back('9');
}

std::string large_str;
for (int i = 0; i < Decimal::kMaxPrecision * 2; i++) {
    large_str.push_back('9');
}

Decimal dsmall(small_str);  // OK

// Method 1, initialization overflow, use the interface without ErrCode return.
//
// If BIGNUM_ENABLE_EXCEPTIONS is defined, exception will be thrown at error;
// if BIGNUM_ENABLE_EXCEPTIONS is not defined, assertion will be triggered.
Decimal dlarge;
try {
    dlarge = Decimal(large_str);
} catch (const DecimalError &err) {
    // handle Decimal exception here
} catch (...) {
    // unknown error
}

// Method 2, initialization overflow, use error-handling interfaces
ErrCode err = dlarge.assign(large_str);
if (err) {
    // handle error here
}
```

`ErrCode` is an plain enum that could be use exchangeably with `int`:
```cpp
int err = dlarge.assign(large_str);
if (err) {
    // ...
}
```
By defining the `BIGNUM_ERROR_NODISCARD` macro (simply turn on the `BIGNUM_ERROR_NODISCARD` cmake
option), `ErrCode` is marked with the C++ `[[nodiscard]]` attribute and all user code must check
for all returned `ErrCode` object. Explicitly checking all error would make user code more robust.
The `BIGNUM_ERROR_NODISCARD` is NOT defined by default (the cmake option is NOT turned on by default)


All exception throwing interfaces have their error-handling counterparts:
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

All interfaces are documented at the *SYNOPSIS* section below.

## Cast operations:
```cpp
Decimal d1("678.90");

// cast to string, gauranteed to succeed;
// requires explicit cast;
{
    std::ostringstream oss;
    oss << d1;

    std::string str1 = d1.to_string();
    std::string str2 = static_cast<std::string>(d1);
}

// cast to double, gauranteed to succeed
// requires explicit cast;
{
    double dval1 = d1.to_double();
    double dval2 = static_cast<double>(d1);
}

// cast to bool, gauranteed to succeed;
// implicit cast is allowed;
{
    // compare to 0
    if (d1) {
    }

    bool b1 = d1.to_bool();
    bool b2 = static_cast<bool>(d1);
}

// cast to int64_t or __int128_t, truncate all digits after decimal points; might overflow
{
    // overflow will cause exception or assertion
    int64_t i64 = static_cast<int64_t>(d1);
    __int128_t i128 = static_cast<__int128_t>(d1);

    // overflow will cause exception or assertion
    ErrCode err1 = d1.to_int64(i64);
    ErrCode err2 = d1.to_int128(i128);
}
```

## compile-time calculation and compile time error
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

## Synopsis of Decimal class
Public members of the `Decimal` class are listed for quick reference.
Some implementation details are hidden.
More details could be found at comments in `decimal.h` header file.
```cpp
class Decimal final {
    public:
        // Maximum decimal digits in total
        constexpr static int32_t kMaxPrecision = 96;

        // Maximum decimal digits after the decimal points
        constexpr static int32_t kMaxScale = 30;

        // Every division increase current scale by 4 until max scale is reached.
        // e.g., a 1.7 / 11 => 0.154545454545... => 0.15455
        // (scale increase from 1 to 5, with rounding).
        //
        // Intermediate result might be increased to maximum of 30+4=34,
        // and then after the calculation, it is decreased to scale 30 by rounding.
        constexpr static int32_t kDivIncreaseScale = 4;

       public:
        // default initialization to 0
        constexpr Decimal();

        // Construction using integral values, i.e., (u)int8_t,..,(u)int64_t,__(u)int128_t
        template <IntegralType U>
        constexpr Decimal(U i);

#ifndef BIGNUM_ENABLE_LITERAL_FLOAT_CONSTRUCTOR
        // Construction using floating point value (lvalue).
        //
        // This interface would throw exception or trigger assertion on overflow.
        // User who want explicit error handling should use the assign(..) interfaces.
        template <FloatingPointType U>
        constexpr Decimal(U &v);

        template <FloatingPointType U>
        constexpr Decimal(U &&) = delete;

#else

        template <FloatingPointType U>
        constexpr Decimal(U v);
#endif

        // Construction using string value.
        //
        // This interface would throw exception or trigger assertion on overflow or error.
        // User who want explicit error handling should use the assign(..) interfaces.
        constexpr Decimal(std::string_view sv);
        constexpr Decimal(const char *s);

        constexpr Decimal(const Decimal &rhs);
        constexpr Decimal(Decimal &&rhs);
        constexpr Decimal &operator=(const Decimal &rhs);
        constexpr Decimal &operator=(Decimal &&rhs);

        ~Decimal() = default;

        //=--------------------------------------------------------
        // Assignment/conversion operators.
        // Return error code instead of exception/assertion.
        //=--------------------------------------------------------
        template <IntegralType U>
        constexpr ErrCode assign(U i) noexcept;

#ifndef BIGNUM_ENABLE_LITERAL_FLOAT_CONSTRUCTOR
        template <FloatingPointType U>
        ErrCode assign(U &f) noexcept;
#else
        template <FloatingPointType U>
        ErrCode assign(U f) noexcept;
#endif

        constexpr ErrCode assign(std::string_view sv) noexcept;

        //=--------------------------------------------------------
        // Cast operators.
        //=--------------------------------------------------------
        std::string to_string() const noexcept;
        explicit operator std::string() const noexcept;

        constexpr double to_double() const noexcept;
        explicit constexpr operator double() const noexcept;

        constexpr bool to_bool() const noexcept;
        constexpr operator bool() const noexcept;

        constexpr ErrCode to_int64(int64_t &i) const noexcept;
        explicit constexpr operator int64_t() const;

        constexpr ErrCode to_int128(__int128_t &i) const noexcept;
        explicit constexpr operator __int128_t();

        //=----------------------------------------------------------
        // getters && setters
        //=----------------------------------------------------------
        constexpr int32_t get_scale() const { return m_scale; }
        constexpr bool is_negative() const;

        //=-=--------------------------------------------------------
        // operator +=
        //=-=--------------------------------------------------------
        constexpr ErrCode add(const Decimal &rhs) noexcept;
        constexpr Decimal &operator+=(const Decimal &rhs);
        constexpr Decimal &operator+=(double f);

        //=-=--------------------------------------------------------
        // operator +
        //=-=--------------------------------------------------------
        constexpr Decimal operator+(const Decimal &rhs) const;
        constexpr Decimal operator+(double f) const;

        //=-=--------------------------------------------------------
        // operator -=
        //=-=--------------------------------------------------------
        constexpr ErrCode sub(const Decimal &rhs) noexcept;
        constexpr Decimal &operator-=(const Decimal &rhs);
        constexpr Decimal &operator-=(double f);

        //=-=--------------------------------------------------------
        // operator -
        //=-=--------------------------------------------------------
        constexpr Decimal operator-(const Decimal &rhs) const;
        constexpr Decimal operator-(double f) const;
        constexpr Decimal operator-() const;

        //=----------------------------------------------------------
        // operator *=
        //=----------------------------------------------------------
        constexpr ErrCode mul(const Decimal &rhs) noexcept;
        constexpr Decimal &operator*=(const Decimal &rhs);
        constexpr Decimal &operator*=(double f);

        //=-=--------------------------------------------------------
        // operator *
        //=-=--------------------------------------------------------
        constexpr Decimal operator*(const Decimal &rhs) const;
        constexpr Decimal operator*(double f) const;

        //=----------------------------------------------------------
        // operator /=
        //=----------------------------------------------------------
        constexpr ErrCode div(const Decimal &rhs) noexcept;
        constexpr Decimal &operator/=(const Decimal &rhs);
        constexpr Decimal &operator/=(double f);

        //=-=--------------------------------------------------------
        // operator /
        //=-=--------------------------------------------------------
        constexpr Decimal operator/(const Decimal &rhs) const;
        constexpr Decimal operator/(double f) const;

        //=----------------------------------------------------------
        // operator %=
        //=----------------------------------------------------------
        constexpr ErrCode mod(const Decimal &rhs) noexcept;
        constexpr Decimal &operator%=(const Decimal &rhs);
        constexpr Decimal &operator%=(double f);

        //=-=--------------------------------------------------------
        // operator %
        //=-=--------------------------------------------------------
        constexpr Decimal operator%(const Decimal &rhs) const;
        constexpr Decimal operator%(double f) const;

        //=--------------------------------------------------------
        // Comparison operators.
        //=--------------------------------------------------------
        constexpr bool operator==(const Decimal &rhs) const;
        constexpr bool operator<(const Decimal &rhs) const;
        constexpr bool operator<=(const Decimal &rhs) const;
        constexpr bool operator!=(const Decimal &rhs) const;
        constexpr bool operator>(const Decimal &rhs) const;
        constexpr bool operator>=(const Decimal &rhs) const;

        constexpr bool operator==(double f) const;
        constexpr bool operator<(double f) const;
        constexpr bool operator<=(double f) const;

        constexpr bool operator!=(double f) const { return !(*this == f); }
        constexpr bool operator>(double f) const { return !(*this <= f); }
        constexpr bool operator>=(double f) const { return !(*this < f); }
};
```

Each `Decimal` object occupies 64 bytes.
