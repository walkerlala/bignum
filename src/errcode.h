#pragma once

#include <string_view>

namespace bignum {
enum ErrCodeValue : int {
        kDecimalError = -1,
        kDecimalSuccess = 0,

        kInvalidArgument,
        kDivByZero,
        kDecimalAddSubOverflow,
        kDecimalMulOverflow,
        kDecimalDivOverflow,
        kDecimalScaleOverflow,
        kDecimalValueOutOfRange,
};

inline constexpr std::string_view get_err_code_value_str(ErrCodeValue ev) {
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wswitch-enum"
        switch (ev) {
                case kDecimalError:
                        return "kDecimalError";
                case kDecimalSuccess:
                        return "kDecimalSuccess";
                case kInvalidArgument:
                        return "kInvalidArgument";
                case kDivByZero:
                        return "kDivByZero";
                case kDecimalAddSubOverflow:
                        return "kDecimalAddSubOverflow";
                case kDecimalMulOverflow:
                        return "kDecimalMulOverflow";
                case kDecimalDivOverflow:
                        return "kDecimalDivOverflow";
                case kDecimalScaleOverflow:
                        return "kDecimalScaleOverflow";
                case kDecimalValueOutOfRange:
                        return "kDecimalValueOutOfRange";
                default:
                        return "UnknownDecimalError";
        }
#pragma GCC diagnostic pop
}

class /* [[nodiscard]] */ ErrCode final {
       public:
        constexpr ErrCode() : err_(ErrCodeValue::kDecimalSuccess) {}
        constexpr ErrCode(int val) : err_(static_cast<ErrCodeValue>(val)) {}
        constexpr ErrCode(ErrCodeValue val) : err_(val) {}
        ~ErrCode() = default;

        constexpr operator bool() const { return static_cast<int>(err_) != 0; }
        constexpr operator int() const { return static_cast<int>(err_); }

        constexpr bool operator==(const ErrCode &rhs) const { return err_ == rhs.err_; }
        constexpr bool operator!=(const ErrCode &rhs) const { return err_ != rhs.err_; }

        constexpr ErrCodeValue error_code() const { return err_; }
        constexpr std::string_view error_code_str() const { return get_err_code_value_str(err_); }

       private:
        ErrCodeValue err_;
};
constexpr ErrCode kError = ErrCode{ErrCodeValue::kDecimalError};
constexpr ErrCode kSuccess = ErrCode{ErrCodeValue::kDecimalSuccess};
}  // namespace bignum

//=--------------------------------------------------------------------------=//
// Example of formatting ErrCode using fmtlib
//=--------------------------------------------------------------------------=//
// namespace fmt {
// template <>
// struct formatter<fy::ErrCode> : formatter<std::string_view> {
//     auto format(fy::ErrCode c, format_context &ctx) {
//         return formatter<std::string_view>::format(c.error_code_str(), ctx);
//     }
// };
// }  // namespace fmt
