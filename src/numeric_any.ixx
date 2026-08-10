module;

#include <array>
#include <bit>
#include <cmath>
#include <compare>
#include <cstring>
#include <format>
#include <functional>
#include <iosfwd>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>

export module casyyy.maths.numeric_any;
import casyyy.maths.numeric_any_parser;
// To disable some unnecessary warnings.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4002 4061 4702 4804 4146 4244)
#endif

#if __cplusplus < 202106L
#define NOT_IN_CONSTANT_EVALUATION (!::std::is_constant_evaluated())
#else
#define NOT_IN_CONSTANT_EVALUATION !consteval
#endif

// To generate types.
#define FOR_EACH_TYPES_TO(x)                                                                                           \
    x(bool, BOOL, 1) x(signed char, SIGNED_CHAR, 2) x(unsigned char, UNSIGNED_CHAR, 3) x(short, SHORT, 4)              \
        x(unsigned short, UNSIGNED_SHORT, 5) x(int, INT, 6) x(unsigned int, UNSIGNED_INT, 7) x(long, LONG, 8)          \
            x(unsigned long, UNSIGNED_LONG, 9) x(long long, LONG_LONG, 10)                                             \
                x(unsigned long long, UNSIGNED_LONG_LONG, 11) x(float, FLOAT, 12) x(double, DOUBLE, 13)                \
                    x(long double, LONG_DOUBLE, 14)
// To generate operation.
#define FOR_EACH_OPERATOR(x)                                                                                           \
    x(add, +, operator+, +=, operator+=) x(sub, -, operator-, -=, operator-=) x(mul, *, operator*, *=, operator*=)     \
        x(div, /, operator/, /=, operator/=)

namespace casyyy::maths {
// Only for arithmetic types, except the char without explict sign.
template <typename T>
concept ArithmeticWithExplictSign = ::std::is_arithmetic_v<T> && !::std::is_same_v<char, T>;
/* The casting_policy will change the behaviour of the numeric_cast.
   Three policies of casting based on these rules:
   Strict:  Allow promotions between signed integers, floating point numbers and unsigned numbers.
            Allow non-negative integers promoting to unsigned integers.
   Normal:  Allow integers convert to a floating numbers.
            Allow narrow casting from a non-negative integer to an unsigned number.
   Relaxed: Allow all casting except NaN, Inf and empty.
*/
export enum class casting_policy : unsigned char { strict, normal, relaxed };
// Type tags.
enum class numeric_types : unsigned char {
#define FUNCTION(type_name, type_enum_name, enum_value) type_enum_name = enum_value,
    FOR_EACH_TYPES_TO(FUNCTION)
#undef FUNCTION
        EMPTY = 0
};
// Type names.
inline constexpr ::std::string_view type_names[]{"empty"
#define FUNCTION(type_name, type_enum_name, enum_value) , #type_name
                                                 FOR_EACH_TYPES_TO(FUNCTION)
#undef FUNCTION
};
// To determine type tag.
template <typename T>
constexpr numeric_types types() noexcept {
    return numeric_types::EMPTY;
}
#define FUNCTION(type_name, type_enum_name, enum_value)                                                                \
    template <>                                                                                                        \
    constexpr numeric_types types<type_name>() noexcept {                                                              \
        return numeric_types::type_enum_name;                                                                          \
    }

FOR_EACH_TYPES_TO(FUNCTION)
#undef FUNCTION

export class numeric_any;
export template <ArithmeticWithExplictSign T, casting_policy Policy = casting_policy::strict>
constexpr ::std::optional<T> numeric_cast(const numeric_any&) noexcept;
// Check for a floating number isn't a NaN or inf.
constexpr bool is_normal_number(auto x) noexcept {
#if __cplusplus < 202202L
    if (x != x) return false; // Check NaN.
    constexpr auto inf = ::std::numeric_limits<decltype(x)>::infinity();
    return x != inf && x != -inf;
#else
    return ::std::isnormal(x);
#endif
}
/*
 A type-erasure-based utility class for arithmetic types.
 Support addition, subtraction, multiplication, division and comparison.
 */
export class numeric_any {
public:
    // The former declarations.
    template <ArithmeticWithExplictSign T>
    friend constexpr T unchecked_numeric_cast(const numeric_any&) noexcept;
    template <ArithmeticWithExplictSign T, casting_policy>
    friend constexpr ::std::optional<T> numeric_cast(const numeric_any&) noexcept;
    friend constexpr numeric_any bpow(const numeric_any&, unsigned) noexcept;

    constexpr numeric_any() noexcept = default;
    // Only for arithmetic types, except the char.
    constexpr numeric_any(ArithmeticWithExplictSign auto) noexcept;

    constexpr numeric_any operator-() const noexcept;
    constexpr numeric_any operator+() const noexcept {
        return *this;
    }
    constexpr numeric_any& operator++() noexcept;
    constexpr numeric_any operator++(int) noexcept;
    constexpr numeric_any& operator--() noexcept;
    constexpr numeric_any operator--(int) noexcept;
    // Operations. It will cast the inner bytes to a number first.
    // Declare first, the definitions will be placed after numeric_cast, to avoid undefined symbols.
    constexpr numeric_any& operator+=(ArithmeticWithExplictSign auto) noexcept;
    constexpr numeric_any& operator-=(ArithmeticWithExplictSign auto) noexcept;
    constexpr numeric_any& operator*=(ArithmeticWithExplictSign auto) noexcept;
    constexpr numeric_any& operator/=(ArithmeticWithExplictSign auto) noexcept;
    constexpr numeric_any& operator+=(const numeric_any&) noexcept;
    constexpr numeric_any& operator-=(const numeric_any&) noexcept;
    constexpr numeric_any& operator*=(const numeric_any&) noexcept;
    constexpr numeric_any& operator/=(const numeric_any&) noexcept;

    constexpr bool operator==(ArithmeticWithExplictSign auto) const noexcept;
    constexpr ::std::partial_ordering operator<=>(ArithmeticWithExplictSign auto) const noexcept;
    constexpr bool operator==(const numeric_any&) const noexcept;
    constexpr ::std::partial_ordering operator<=>(const numeric_any&) const noexcept;
    // Check whether inner value isn't zero. The empty will be set to false.
    explicit constexpr operator bool() const noexcept;

    constexpr numeric_any& operator=(ArithmeticWithExplictSign auto x) noexcept {
        this->reset(x);
        return *this;
    }
    // Check whether this "any" has set value.
    [[nodiscard]] constexpr bool empty() const noexcept {
        return type == numeric_types::EMPTY;
    }
    // Get meta data.
    [[nodiscard]] constexpr ::std::string_view type_name() const noexcept {
        return type_names[static_cast<unsigned>(type)];
    }
    [[nodiscard]] constexpr ::std::size_t type_size() const noexcept {
        return width;
    }
    [[nodiscard]] constexpr bool is_floating_point() const noexcept {
        return float_point;
    }
    [[nodiscard]] constexpr bool is_unsigned_number() const noexcept {
        return is_unsigned;
    }
    [[nodiscard]] constexpr bool is_nonnegative() const noexcept {
        return positive;
    }
    // Reset the inner value.
    // It will clear the inner storage first, then copying the byte in storage.
    constexpr void reset(ArithmeticWithExplictSign auto) noexcept;

    // To check types.
    template <ArithmeticWithExplictSign T>
    [[nodiscard]] constexpr bool is_same_type() const noexcept {
        return types<T>() == type;
    }
    // The function will check the inner value is able to convert to another type safely.
    template <ArithmeticWithExplictSign T>
    [[nodiscard]] constexpr bool can_safe_convert_to() const noexcept;
    // To get the inner view of the bytes.
    [[nodiscard]] constexpr ::std::span<const unsigned char> view_bytes() const noexcept {
        return ::std::span{storage_.data(), width};
    }

private:
    // Use to store all bytes of a value.
    alignas(8)::std::array<unsigned char, 16> storage_{};
    unsigned int width = 0;
    // These data use to switch the branch in operations and castings.
    numeric_types type{numeric_types::EMPTY};
    bool float_point = false;
    bool is_unsigned = false;
    bool positive    = false;
    /*
        A template in operation :
        It will set the common_type, to determine final type of value.
        Then restore the value, with type "T1", the exact or promoted type of the value.
        Finally, doing operation and reset inner storage.
    */
#define FUNCTIONS_OPERATION(name, op, op_name, op2, op2_name)                                                          \
    template <typename T1, typename T2>                                                                                \
    constexpr void casting_##name(T2 x) noexcept {                                                                     \
        using common_type = ::std::common_type_t<T1, T2>;                                                              \
        auto lhs          = unchecked_numeric_cast<T1>(*this);                                                         \
        auto result       = static_cast<common_type>(lhs) op static_cast<common_type>(x);                              \
        reset(result);                                                                                                 \
    }

    FOR_EACH_OPERATOR(FUNCTIONS_OPERATION)
#undef FUNCTIONS_OPERATION
};
// To generate operations +, -, *, /.
#define FUNCTION_TWO_OPERATION(name, op, op_name, op2, op2_name)                                                       \
    export constexpr numeric_any op_name(numeric_any x, ArithmeticWithExplictSign auto y) noexcept {                   \
        return x op2 y;                                                                                                \
    }                                                                                                                  \
    export constexpr numeric_any op_name(ArithmeticWithExplictSign auto x, numeric_any y) noexcept {                   \
        return numeric_any{x} op2 y;                                                                                   \
    }                                                                                                                  \
    export constexpr numeric_any op_name(numeric_any x, numeric_any y) noexcept {                                      \
        return x op2 y;                                                                                                \
    }

FOR_EACH_OPERATOR(FUNCTION_TWO_OPERATION)
#undef FUNCTION_TWO_OPERATION
// Based on the std::bit_cast.
#define FUNCTION_TO_CAST(type_name, type_enum_name, enum_value)                                                        \
    case numeric_types::type_enum_name: {                                                                              \
        if NOT_IN_CONSTANT_EVALUATION {                                                                                \
            type_name restore_value = 0;                                                                               \
            ::std::memcpy(&restore_value, x.storage_.data(), sizeof(type_name));                                       \
            return static_cast<T>(restore_value);                                                                      \
        } else {                                                                                                       \
            ::std::array<unsigned char, sizeof(type_name)> temp_{};                                                    \
            for (::size_t i = 0; i < sizeof(type_name); ++i)                                                           \
                temp_[i] = x.storage_[i];                                                                              \
            return static_cast<T>(::std::bit_cast<type_name, decltype(temp_)>(temp_));                                 \
        }                                                                                                              \
    }
#define FUNCTION_BINARY_EXP(type_name, type_enum_name, enum_value)                                                     \
    case numeric_types::type_enum_name: {                                                                              \
        auto result = static_cast<type_name>(1) * 1;                                                                   \
        if (n == 0) return numeric_any{result};                                                                        \
        auto restore_value = unchecked_numeric_cast<type_name>(a);                                                     \
        while (n) {                                                                                                    \
            if (n & 1) result *= restore_value;                                                                        \
            restore_value *= restore_value;                                                                            \
            n >>= 1;                                                                                                   \
        }                                                                                                              \
        return numeric_any{result};                                                                                    \
    }

constexpr numeric_any bpow(const numeric_any& a, unsigned n) noexcept {
    switch (a.type) {
        FOR_EACH_TYPES_TO(FUNCTION_BINARY_EXP)
    default:
        return a;
    }
}
// The function will just to restore the value and return the value after static_cast.
export template <ArithmeticWithExplictSign T>
constexpr T unchecked_numeric_cast(const numeric_any& x) noexcept {
    switch (x.type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_CAST)
    case numeric_types::EMPTY:
    default:
        return T{};
    }
}
#undef FUNCTION_TO_CAST
// The cases will check the Inf and NaN.
#define FUNCTION_TO_RESTORE_VALUE(type_name, type_enum_name, enum_value)                                               \
    case numeric_types::type_enum_name: {                                                                              \
        auto restore_value = unchecked_numeric_cast<type_name>(x);                                                     \
        if constexpr (::std::is_floating_point_v<type_name>) {                                                         \
            if (!is_normal_number(restore_value)) return ::std::nullopt;                                               \
            auto res = static_cast<T>(restore_value);                                                                  \
            if (!is_normal_number(restore_value)) return ::std::nullopt;                                               \
            return res;                                                                                                \
        }                                                                                                              \
        return static_cast<T>(restore_value);                                                                          \
    }

// Based on casting_policy, it will return the value after cast or std::nullopt.
export template <ArithmeticWithExplictSign T, casting_policy Policy>
[[nodiscard]] constexpr ::std::optional<T> numeric_cast(const numeric_any& x) noexcept {
    if constexpr (Policy == casting_policy::strict) {
        if (!x.can_safe_convert_to<T>()) return ::std::nullopt;
    } else if constexpr (Policy == casting_policy::normal) {
        if (x.empty()) return ::std::nullopt;
        if constexpr (::std::is_floating_point_v<T>) {
            if (x.width > sizeof(T)) return ::std::nullopt;
        } else if constexpr (::std::is_unsigned_v<T>) {
            if (x.float_point || !x.positive) return ::std::nullopt;
        } else {
            if (x.float_point || (x.is_unsigned && x.width >= sizeof(T))) return ::std::nullopt;
        }
    }
    switch (x.type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_RESTORE_VALUE)
    case numeric_types::EMPTY:
    default:
        return ::std::nullopt;
    }
}
#undef FUNCTION_TO_RESTORE_VALUE

constexpr numeric_any::numeric_any(ArithmeticWithExplictSign auto x) noexcept
    : width{sizeof(decltype(x))}, type{types<decltype(x)>()}, float_point{::std::is_floating_point_v<decltype(x)>},
      is_unsigned{::std::is_unsigned_v<decltype(x)>}, positive{x >= decltype(x){}} {
    // Base on the std::bit_cast;
    if NOT_IN_CONSTANT_EVALUATION {
        ::std::memcpy(storage_.data(), &x, sizeof(decltype(x)));
    } else {
        ::std::array<unsigned char, sizeof(decltype(x))> temp_{};
        temp_ = ::std::bit_cast<decltype(temp_), decltype(x)>(x);
        for (::std::size_t i = 0; i < sizeof(decltype(x)); ++i)
            storage_[i] = temp_[i];
    }
}

constexpr void numeric_any::reset(ArithmeticWithExplictSign auto x) noexcept {
    storage_.fill(0);
    type        = types<decltype(x)>();
    float_point = ::std::is_floating_point_v<decltype(x)>;
    width       = sizeof(decltype(x));
    is_unsigned = ::std::is_unsigned_v<decltype(x)>;
    positive    = x >= decltype(x){};
    if NOT_IN_CONSTANT_EVALUATION {
        ::std::memcpy(storage_.data(), &x, sizeof(decltype(x)));
    } else {
        ::std::array<unsigned char, sizeof(decltype(x))> temp_{};
        temp_ = ::std::bit_cast<decltype(temp_), decltype(x)>(x);
        for (::std::size_t i = 0; i < sizeof(decltype(x)); ++i)
            storage_[i] = temp_[i];
    }
}

#define FUNCTION_TO_RESTORE_VALUE(type_name, type_enum_name, enum_value)                                               \
    case numeric_types::type_enum_name: {                                                                              \
        type_name restore_value = unchecked_numeric_cast<type_name>(*this);                                            \
        return numeric_any{-restore_value};                                                                            \
    }

constexpr numeric_any numeric_any::operator-() const noexcept {
    switch (type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_RESTORE_VALUE)
    case numeric_types::EMPTY:
    default:
        return numeric_any{};
    }
}

#undef FUNCTION_TO_RESTORE_VALUE
// When inner type is bool, same as static_cast<bool>(restore - 1) or static_cast<bool>(restore + 1).
#define FUNCTION_TO_ADD(type_name, type_enum_name, enum_value)                                                         \
    case numeric_types::type_enum_name: {                                                                              \
        auto restore = unchecked_numeric_cast<type_name>(*this);                                                       \
        return *this = static_cast<type_name>(restore + static_cast<type_name>(1));                                    \
    }

#define FUNCTION_TO_SUB(type_name, type_enum_name, enum_value)                                                         \
    case numeric_types::type_enum_name: {                                                                              \
        auto restore = unchecked_numeric_cast<type_name>(*this);                                                       \
        return *this = static_cast<type_name>(restore - static_cast<type_name>(1));                                    \
    }

constexpr numeric_any& numeric_any::operator++() noexcept {
    switch (type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_ADD)
    case numeric_types::EMPTY:
    default:
        return *this;
    }
}
constexpr numeric_any numeric_any::operator++(int) noexcept {
    auto repeat = *this;
    ++*this;
    return repeat;
}
constexpr numeric_any& numeric_any::operator--() noexcept {
    switch (type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_SUB)
    case numeric_types::EMPTY:
    default:
        return *this;
    }
}
constexpr numeric_any numeric_any::operator--(int) noexcept {
    auto repeat = *this;
    --*this;
    return repeat;
}

// Check whether inner value isn't zero. The empty will be set to false.
constexpr numeric_any::operator bool() const noexcept {
    return unchecked_numeric_cast<bool>(*this);
}

template <ArithmeticWithExplictSign T>
constexpr bool numeric_any::can_safe_convert_to() const noexcept {
    if (empty()) return false;
    if (is_same_type<T>()) return true;           // Safe when the tag of the "T" is same as inner tag.
    if (type == numeric_types::BOOL) return true; // Bool is safe to cast to all types.
    if constexpr (::std::is_floating_point_v<T>) {
        return float_point && width <= sizeof(T); // Only support promotions in floating numbers.
    } else if constexpr (::std::is_unsigned_v<T>) {
        if (is_unsigned) return width <= sizeof(T); // Promotions between two unsigned numbers are safe.
        if (!float_point) {
            if (positive) return width <= sizeof(T); // Promotions from non-negative numbers are safe.
            return false;
        }
        return false;
    } else {
        // Support to promotions to a wider signed integral type.
        if (is_unsigned) return width < sizeof(T);
        // Allowing equal width converting when both signed integral numbers.
        if (!float_point) return width <= sizeof(T);
        return false;
    }
}

// Based on standard-conversions in C/C++, all operations will check the tag to determine input types.
// Then inner functions used std::common_type to determine final type of the result.
// No need to check all tags, since all integer which size smaller than sizeof(int) will be set as int.
// If one is floating point number, switch to floating point branches.
// Because of promotions.
#define FUNCTIONS_OPERATION(name, op, op_name, op2, op2_name)                                                          \
    constexpr numeric_any& numeric_any::op2_name(ArithmeticWithExplictSign auto x) noexcept {                          \
        if (empty()) return *this;                                                                                     \
        switch (type) {                                                                                                \
        case numeric_types::UNSIGNED_INT: {                                                                            \
            casting_##name<unsigned int>(x);                                                                           \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::LONG: {                                                                                    \
            casting_##name<long>(x);                                                                                   \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::UNSIGNED_LONG: {                                                                           \
            casting_##name<unsigned long>(x);                                                                          \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::LONG_LONG: {                                                                               \
            casting_##name<long long>(x);                                                                              \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::UNSIGNED_LONG_LONG: {                                                                      \
            casting_##name<unsigned long long>(x);                                                                     \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::FLOAT: {                                                                                   \
            casting_##name<float>(x);                                                                                  \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::DOUBLE: {                                                                                  \
            casting_##name<double>(x);                                                                                 \
            break;                                                                                                     \
        }                                                                                                              \
        case numeric_types::LONG_DOUBLE: {                                                                             \
            casting_##name<long double>(x);                                                                            \
            break;                                                                                                     \
        }                                                                                                              \
        default:                                                                                                       \
            casting_##name<int>(x);                                                                                    \
            break;                                                                                                     \
        }                                                                                                              \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr numeric_any& numeric_any::op2_name(const numeric_any& x) noexcept {                                      \
        if (empty() || x.empty()) return *this = numeric_any{};                                                        \
        if (type == numeric_types::LONG_DOUBLE || x.type == numeric_types::LONG_DOUBLE) {                              \
            casting_##name<long double>(unchecked_numeric_cast<long double>(x));                                       \
        } else if (type == numeric_types::DOUBLE || x.type == numeric_types::DOUBLE) {                                 \
            casting_##name<double>(unchecked_numeric_cast<double>(x));                                                 \
        } else if (type == numeric_types::FLOAT || x.type == numeric_types::FLOAT) {                                   \
            casting_##name<float>(unchecked_numeric_cast<float>(x));                                                   \
        } else if (type == numeric_types::UNSIGNED_LONG_LONG || x.type == numeric_types::UNSIGNED_LONG_LONG) {         \
            casting_##name<unsigned long long>(unchecked_numeric_cast<unsigned long long>(x));                         \
        } else if (type == numeric_types::LONG_LONG || x.type == numeric_types::LONG_LONG) {                           \
            casting_##name<long long>(unchecked_numeric_cast<long long>(x));                                           \
        } else if (type == numeric_types::UNSIGNED_LONG || x.type == numeric_types::UNSIGNED_LONG) {                   \
            casting_##name<unsigned long>(unchecked_numeric_cast<unsigned long>(x));                                   \
        } else if (type == numeric_types::LONG || x.type == numeric_types::LONG) {                                     \
            casting_##name<long>(unchecked_numeric_cast<long>(x));                                                     \
        } else if (type == numeric_types::UNSIGNED_INT || x.type == numeric_types::UNSIGNED_INT) {                     \
            casting_##name<unsigned>(unchecked_numeric_cast<unsigned>(x));                                             \
        } else {                                                                                                       \
            casting_##name<int>(unchecked_numeric_cast<int>(x));                                                       \
        }                                                                                                              \
        return *this;                                                                                                  \
    }

FOR_EACH_OPERATOR(FUNCTIONS_OPERATION)
#undef FUNCTIONS_OPERATION

// Comparison with normal number.
[[nodiscard]] constexpr bool numeric_any::operator==(ArithmeticWithExplictSign auto x) const noexcept {
    return *this <=> x == ::std::partial_ordering::equivalent;
}
[[nodiscard]] constexpr ::std::partial_ordering
numeric_any::operator<=>(ArithmeticWithExplictSign auto x) const noexcept {
    if (empty()) return ::std::partial_ordering::unordered;
    // The NaN will be checked first.
    if constexpr (::std::is_floating_point_v<decltype(x)>) {
        if (x != x) return ::std::partial_ordering::unordered; // NOLINT
        if constexpr (::std::is_same_v<decltype(x), float>) {
            if (type == numeric_types::FLOAT) return unchecked_numeric_cast<float>(*this) <=> x;
            if (type == numeric_types::DOUBLE) return unchecked_numeric_cast<double>(*this) <=> x;
        } else if constexpr (::std::is_same_v<decltype(x), double>) {
            if (type != numeric_types::LONG_DOUBLE) return unchecked_numeric_cast<double>(*this) <=> x;
        }
        return unchecked_numeric_cast<long double>(*this) <=> x;
    } else {
        if (positive != x >= decltype(x){}) return static_cast<int>(positive) <=> static_cast<int>(x >= decltype(x){});
        switch (type) {
        case numeric_types::UNSIGNED_LONG_LONG:
            return unchecked_numeric_cast<unsigned long long>(*this) <=> static_cast<unsigned long long>(x);
        case numeric_types::FLOAT:
            return unchecked_numeric_cast<float>(*this) <=> x;
        case numeric_types::DOUBLE:
            return unchecked_numeric_cast<double>(*this) <=> x;
        case numeric_types::LONG_DOUBLE:
            return unchecked_numeric_cast<long double>(*this) <=> x;
        default:
            return unchecked_numeric_cast<long long>(*this) <=> x;
        }
    }
}
// The comparison between two any is complex.
// It will check the inner tag, then restore values to do comparison.
// Also, it will do standard-conversions when restoring the inner value.
// But the tag won't change.
[[nodiscard]] constexpr ::std::partial_ordering numeric_any::operator<=>(const numeric_any& x) const noexcept {
    if (empty() || x.empty()) return ::std::partial_ordering::unordered;
    if (type == numeric_types::LONG_DOUBLE || x.type == numeric_types::LONG_DOUBLE)
        return unchecked_numeric_cast<long double>(*this) <=> unchecked_numeric_cast<long double>(x);
    if (type == numeric_types::DOUBLE || x.type == numeric_types::DOUBLE)
        return unchecked_numeric_cast<double>(*this) <=> unchecked_numeric_cast<double>(x);
    if (type == numeric_types::FLOAT || x.type == numeric_types::FLOAT)
        return unchecked_numeric_cast<float>(*this) <=> unchecked_numeric_cast<float>(x);
    // The signed will be checked first, so 12U < -1 will be false.
    if (positive != x.positive) return static_cast<int>(positive) <=> static_cast<int>(x.positive);
    if (type == numeric_types::UNSIGNED_LONG_LONG || x.type == numeric_types::UNSIGNED_LONG_LONG)
        return unchecked_numeric_cast<unsigned long long>(*this) <=> unchecked_numeric_cast<unsigned long long>(x);
    return unchecked_numeric_cast<long long>(*this) <=> unchecked_numeric_cast<long long>(x);
}
// Remind that the "empty == empty" will be false.
[[nodiscard]] constexpr bool numeric_any::operator==(const numeric_any& x) const noexcept {
    if (empty() || x.empty()) return false;
    return *this <=> x == ::std::partial_ordering::equivalent;
}

// To restore the number before output.
#define FUNCTION_OUTPUT(type_name, type_enum_name, enum_value)                                                         \
    if (x.is_same_type<type_name>()) {                                                                                 \
        auto res = unchecked_numeric_cast<type_name>(x);                                                               \
        os << res;                                                                                                     \
        return os;                                                                                                     \
    }
// Support using ostream to output.
export template <typename CharT, typename Trait = ::std::char_traits<CharT>>
::std::basic_ostream<CharT, Trait>& operator<<(::std::basic_ostream<CharT, Trait>& os, const numeric_any& x) {
    constexpr CharT empty_str[]{'e', 'm', 'p', 't', 'y', '\0'};
    FOR_EACH_TYPES_TO(FUNCTION_OUTPUT)
    os << empty_str;
    return os;
}
#undef FUNCTION_OUTPUT

// A tool function.
export constexpr numeric_any make_numeric_any(ArithmeticWithExplictSign auto x) noexcept {
    return numeric_any{x};
}
} // namespace casyyy::maths

// The inner hash will try to process every member.
#define FUNCTION_HASH(type_name, type_enum_name, enum_value)                                                           \
    if (x.is_same_type<type_name>()) {                                                                                 \
        type_name restore = unchecked_numeric_cast<type_name>(x);                                                      \
        return hash<type_name>{}(restore);                                                                             \
    }

namespace std {
// The inner hash will try to process every member.
template <>
struct hash<::casyyy::maths::numeric_any> {
    size_t operator()(const ::casyyy::maths::numeric_any& x) const noexcept {
        FOR_EACH_TYPES_TO(FUNCTION_HASH)
        return 0;
    }
};
#undef FUNCTION_HASH
#define FUNCTION_FORMAT(type_name, type_enum_name, enum_value)                                                         \
    if (x.is_same_type<type_name>()) {                                                                                 \
        type_name restore = unchecked_numeric_cast<type_name>(x);                                                      \
        if constexpr (is_same_v<CharT, wchar_t>)                                                                       \
            return vformat_to(ctx.out(), fmt, make_wformat_args(restore));                                             \
        else                                                                                                           \
            return vformat_to(ctx.out(), fmt, make_format_args(restore));                                              \
    }

#ifndef DISABLE_FORMAT_IN_NUMERIC_ANY
// Support formatter.
// It will parse the format string, and restore it when formatting.
template <typename CharT>
struct formatter<::casyyy::maths::numeric_any, CharT> {
    // To store format string for value.
    // Based on std::numeric_limits<unsigned long long>::dight10, 60 is enough.
    static constexpr unsigned FORMAT_STRING_BUFFER_SIZE = 60;

    constexpr auto parse(auto& ctx) {
        return parser_.parse(ctx);
    }

    auto format(const ::casyyy::maths::numeric_any& x, auto& ctx) const {
        CharT format_buffer[FORMAT_STRING_BUFFER_SIZE]{};
        // Restore the format stirng, and used vformat_to.
        auto finish = parser_.restore_format_string(format_buffer, format_buffer + FORMAT_STRING_BUFFER_SIZE, ctx);
        basic_string_view<CharT> fmt{format_buffer, finish};
        FOR_EACH_TYPES_TO(FUNCTION_FORMAT)
        if constexpr (is_same_v<CharT, wchar_t>) {
            return format_to(ctx.out(), L"{}", L"empty");
        }
        return format_to(ctx.out(), "{}", "empty");
    }

private:
    ::casyyy::utils::numeric_any_parser<CharT> parser_;
};
#endif
} // namespace std
#undef FUNCTION_FORMAT
#undef FOR_EACH_OPERATOR
#undef FOR_EACH_TYPES_TO
#undef NOT_IN_CONSTANT_EVALUATION

#ifdef _MSC_VER
#pragma warning(pop)
#endif