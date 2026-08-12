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
/// @cond
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
/// @endcond

namespace casyyy::maths {
/// @brief Only for arithmetic types, except the char without explict sign.
export template <typename T>
concept sign_unambiguous_arithmetic =
    ::std::is_arithmetic_v<T> && !::std::is_same_v<char, T> && !::std::is_same_v<wchar_t, T>;
/// @cond
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
template <typename>
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
// Check for a floating number isn't a NaN or inf.
constexpr bool is_normal_number(auto x) noexcept {
#if __cplusplus < 202202L
    if NOT_IN_CONSTANT_EVALUATION {
        return ::std::isnormal(x);
    } else {
        if (x != x) return false; // Check NaN. //NOLINT
        constexpr auto inf = ::std::numeric_limits<decltype(x)>::infinity();
        return x != inf && x != -inf;
    }
#else
    return ::std::isnormal(x);
#endif
}
constexpr decltype(auto) inner_abs(auto x) noexcept {
#ifdef __cpp_lib_constexpr_cmath
    return ::std::abs(x);
#else
    if NOT_IN_CONSTANT_EVALUATION {
        if constexpr (::std::is_signed_v<decltype(x)>) {
            return ::std::abs(x);
        } else {
            return x * 1;
        }
    } else {
        using T = ::std::decay_t<decltype(x)>;
        using U = decltype(T{} + 1);
        if constexpr (::std::is_signed_v<T> && ::std::is_integral_v<T>) {
            auto mask = x >> ::std::numeric_limits<T>::digits;
            return (mask + x) ^ mask;
        } else if constexpr (::std::is_unsigned_v<T>) {
            return x * 1;
        }
        auto bytes = ::std::bit_cast<::std::array<unsigned char, sizeof(U)>, U>(x);
        if constexpr (::std::endian::native == ::std::endian::little) {
            bytes.back() &= 0x7f;
        } else if constexpr (::std::endian::native == ::std::endian::big) {
            bytes.front() &= 0x7f;
        } else {
            static_assert(!::std::is_constant_evaluated(), // NOLINT
                          "In C++20, Compile-time processing of mixed-endian floating-point data is not supported.");
        }
        return ::std::bit_cast<U, decltype(bytes)>(bytes);
    }
#endif
}
/// @endcond

/// @brief Numeric conversion strategy, used for handling different types of number conversions.
/// @note The default policy is set to strict.
export enum class casting_policy : unsigned char {
    strict, /**<
             * @brief Require lossless conversion of values.
             * @details Allow promotions between signed integers, floating point numbers and unsigned numbers.
             *          Allow non-negative integers promoting to unsigned integers.
             * @note Convert integers to floating point numbers isn't allowed.
             */
    normal, /**<
             * @brief Conversion that allows some data loss.
             * @details Allow integers convert to a floating numbers.
             *          Allow narrow casting from a non-negative integer to an unsigned number.
             * @note Positive integers are only allowed to be truncated into unsigned numbers.
             */
    relaxed /**<
             * @brief The output should be a plain numeric value.
             * @details Allow all casting except NaN, Inf and empty.
             */
};

export class numeric_any;
export template <sign_unambiguous_arithmetic T, casting_policy Policy = casting_policy::strict>
constexpr ::std::optional<T> numeric_cast(const numeric_any&) noexcept;
/**
 * @brief A type-erasure-based utility class for arithmetic types.
 *        Support addition, subtraction, multiplication, division and comparison.
 * @note Only support the basic arithmetic types in C++, except char without explict sign.
 */
export class numeric_any {
public:
    // The former declarations.
    template <sign_unambiguous_arithmetic T>
    friend constexpr T unchecked_numeric_cast(const numeric_any&) noexcept;
    template <sign_unambiguous_arithmetic T, casting_policy>
    friend constexpr ::std::optional<T> numeric_cast(const numeric_any&) noexcept;
    friend constexpr numeric_any bpow(const numeric_any&, unsigned) noexcept;
    friend constexpr numeric_any abs(const numeric_any&) noexcept;

    /// @brief Construct an empty numeric_any.
    /// @note Explicitly assign before use to avoid unintended behavior from null states.
    constexpr numeric_any() noexcept = default;

    /// @brief Construct a numeric_any which is assigned by a value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explict sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    constexpr numeric_any(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Return the negative of the number inside.
    /// @return The negative of the number inside.
    /// @note Follow C++ rules for negation of fundamental types.
    constexpr numeric_any operator-() const noexcept;

    /// @brief Return the numeric_any itself.
    /// @return The numeric_any itself.
    /// @note Follow C++ rules.
    constexpr numeric_any operator+() const noexcept {
        return *this;
    }

    /// @brief Pre-increment operator.
    /// @return Reference to the incremented object.
    /// @note Follows built-in semantics, except bool.
    /// @note For boolean value, as if setting the value to static_cast<bool>(inner + 1).
    constexpr numeric_any& operator++() noexcept;

    /// @brief Post-increment operator.
    /// @return The value before increment.
    /// @note Follows built-in semantics, except bool.
    /// @note For boolean value, as if setting the value to static_cast<bool>(inner + 1).
    constexpr numeric_any operator++(int) noexcept;

    /// @brief Pre-decrement operator.
    /// @return Reference to the decreed object.
    /// @note Follows built-in semantics, except bool.
    /// @note For boolean value, as if setting the value to static_cast<bool>(inner - 1).
    constexpr numeric_any& operator--() noexcept;

    /// @brief Post-decrement operator.
    /// @return The value before decrement.
    /// @note Follows built-in semantics, except bool.
    /// @note For boolean value, as if setting the value to static_cast<bool>(inner - 1).
    constexpr numeric_any operator--(int) noexcept;

    /// @brief Do an addition assignment with a normal value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explict sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return Reference to the current value.
    /// @note Internal implicit conversion may change the stored type per C++ standard rules of arithmetic conversions.
    constexpr numeric_any& operator+=(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Do a substraction assignment with a normal value.
    /// @copydetails operator+=(sign_unambiguous_arithmetic auto)
    constexpr numeric_any& operator-=(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Do a multiplication assignment with a normal value.
    /// @copydetails operator+=(sign_unambiguous_arithmetic auto)
    constexpr numeric_any& operator*=(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Do a division assignment with a normal value.
    /// @copydetails operator+=(sign_unambiguous_arithmetic auto)
    constexpr numeric_any& operator/=(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Do an addition assignment with another numeric_any.
    /// @param x The value used to do operation.
    /// @return Reference to the current value.
    /// @note Internal implicit conversion may change the stored type per C++ standard rules of arithmetic conversions.
    constexpr numeric_any& operator+=(const numeric_any& x) noexcept;

    /// @brief Do a substraction assignment with another numeric_any.
    /// @copydetails operator+=(const numeric_any&)
    constexpr numeric_any& operator-=(const numeric_any& x) noexcept;

    /// @brief Do a multiplication assignment with another numeric_any.
    /// @copydetails operator+=(const numeric_any&)
    constexpr numeric_any& operator*=(const numeric_any& x) noexcept;

    /// @brief Do a division assignment with another numeric_any.
    /// @copydetails operator+=(const numeric_any&)
    constexpr numeric_any& operator/=(const numeric_any& x) noexcept;

    /// @brief Compare a numeric_any is equal to a normal value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explict sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return Whether a numeric_any is equal to a normal value.
    /// @note When numeric_any is empty, always return false.
    constexpr bool operator==(sign_unambiguous_arithmetic auto x) const noexcept;

    /// @brief Three-way comparison between numeric_any with a normal value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explict sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return A partial_ordering represent the result of three-way comparison.
    /// @note When numeric_any is empty, always return std::partial_ordering::unordered.
    constexpr ::std::partial_ordering operator<=>(sign_unambiguous_arithmetic auto x) const noexcept;

    /// @brief Compare a numeric_any is equal to another numeric_any.
    /// @param x The value used to do comparison.
    /// @return Whether a numeric_any is equal to a normal value.
    /// @note When left numeric_any or right numeric_any is empty, always return false.
    constexpr bool operator==(const numeric_any& x) const noexcept;

    /// @brief Three-way comparison between numeric_any with another numeric_any.
    /// @param x The value used to do comparison.
    /// @return A partial_ordering represent the result of three-way comparison.
    /// @note hen left numeric_any or right numeric_any is empty, return std::partial_ordering::unordered.
    constexpr ::std::partial_ordering operator<=>(const numeric_any& x) const noexcept;

    /// @brief Check whether inner value isn't zero.
    /// @return Whether inner value isn't zero.
    /// @note When it is empty, return false.
    explicit constexpr operator bool() const noexcept;

    /// @brief Reset the inner value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explict sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return self-referencing after reset.
    constexpr numeric_any& operator=(sign_unambiguous_arithmetic auto x) noexcept {
        this->reset(x);
        return *this;
    }

    /// @brief Check this numeric_any whether is set value or not.
    /// @return A boolean represents this numeric_any whether is set value or not.
    [[nodiscard]] constexpr bool empty() const noexcept {
        return type == numeric_types::EMPTY;
    }

    /// @brief Get the typename of the value which numeric_any store.
    /// @return A readable string view, with content representing the type stored internally.
    [[nodiscard]] constexpr ::std::string_view type_name() const noexcept {
        return type_names[static_cast<unsigned>(type)];
    }

    /// @brief Get the number of bytes used by internal storage data。
    /// @return The number of bytes used by internal storage data。
    [[nodiscard]] constexpr ::std::size_t type_size() const noexcept {
        return width;
    }

    /// @brief Check if what's stored inside is a floating point number.
    /// @return A boolean represents whether it stores a floating point number or not.
    [[nodiscard]] constexpr bool is_floating_point() const noexcept {
        return float_point;
    }

    /// @brief Check if what's stored inside is an unsigned number.
    /// @return A boolean represents whether it stores an unsigned number or not.
    [[nodiscard]] constexpr bool is_unsigned_number() const noexcept {
        return is_unsigned;
    }

    /// @brief Check if what's stored inside is a nonnegative number.
    /// @return A boolean represents whether it stores a nonnegative number or not.
    /// @note If inner value is NaN or 'empty', it will return false.
    [[nodiscard]] constexpr bool is_nonnegative() const noexcept {
        return positive;
    }

    /// @brief Reset the inner value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explict sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @note It will change the base type of the internal storage. And must offer a number.
    constexpr void reset(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Check if the given type is the type of value stored internally.
    /// @tparam T The basic arithmetic types in C++, except char without explict sign which is used to check.
    /// @return Whether the given type parameter is the type of the underlying stored value。
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    template <sign_unambiguous_arithmetic T>
    [[nodiscard]] constexpr bool is_same_type() const noexcept {
        return types<T>() == type;
    }

    /// @brief Check the inner value is able to convert to another type safely.
    /// @tparam T The basic arithmetic types in C++, except char without explict sign which is used to check.
    /// @return Whether the given type parameter can be safely converted based on metadata.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    template <sign_unambiguous_arithmetic T>
    [[nodiscard]] constexpr bool can_safe_convert_to() const noexcept;

    /// @brief Get the inner view of the bytes.
    /// @return A span which represent inner view of the bytes.
    /// @note The span is read-only. And its size is based on the number of bytes used by internal storage data。
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
    /// @cond
    /*  A template in operation :
        It will set the common_type, to determine final type of value.
        Then restore the value, with type "T1", the exact or promoted type of the value.
        Finally, doing operation and reset inner storage. */
#define FUNCTIONS_OPERATION(name, op, op_name, op2, op2_name)                                                          \
    template <typename T1, typename T2>                                                                                \
    constexpr void casting_##name(T2 x) noexcept {                                                                     \
        using common_type = ::std::common_type_t<T1, T2>;                                                              \
        auto lhs          = unchecked_numeric_cast<T1>(*this);                                                         \
        auto result       = static_cast<common_type>(lhs) op static_cast<common_type>(x);                              \
        reset(result);                                                                                                 \
    }

    FOR_EACH_OPERATOR(FUNCTIONS_OPERATION)
    /// @endcond
#undef FUNCTIONS_OPERATION
};

/** @def FUNCTION_TWO_OPERATION
 *  @brief To generate operator between numeric_any with a value or another numeric_any.
 *         Including: +, -, *, /.
 *         And used +=, -=, *=, /= to do operations.
 *  @return A new numeric_any after operations.
 */
#define FUNCTION_TWO_OPERATION(name, op, op_name, op2, op2_name)                                                       \
    export constexpr numeric_any op_name(numeric_any x, sign_unambiguous_arithmetic auto y) noexcept {                 \
        return x op2 y;                                                                                                \
    }                                                                                                                  \
    export constexpr numeric_any op_name(sign_unambiguous_arithmetic auto x, numeric_any y) noexcept {                 \
        return numeric_any{x} op2 y;                                                                                   \
    }                                                                                                                  \
    export constexpr numeric_any op_name(numeric_any x, numeric_any y) noexcept {                                      \
        return x op2 y;                                                                                                \
    }

FOR_EACH_OPERATOR(FUNCTION_TWO_OPERATION)
#undef FUNCTION_TWO_OPERATION
/// @cond
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
/// @endcond

/// @brief Calculate the integer power of a numeric_any.
/// @param a A numeric_any.
/// @param n Computed exponent. It's an unsigned number.
/// @return The result of the exponentiation.
/// @note If the internal type is narrower than int, the final type is converted to int.
export constexpr numeric_any bpow(const numeric_any& a, unsigned n) noexcept {
    switch (a.type) {
        FOR_EACH_TYPES_TO(FUNCTION_BINARY_EXP)
    default:
        return a;
    }
}
#undef FUNCTION_BINARY_EXP
/// @cond
#define FUNCTION_ABS(type_name, type_enum_name, enum_value)                                                            \
    case numeric_types::type_enum_name: {                                                                              \
        auto restore_value = unchecked_numeric_cast<type_name>(x);                                                     \
        return numeric_any{inner_abs(restore_value)};                                                                  \
    }
/// @endcond

/// @brief Compute abs.
/// @param x Input numeric_any.
/// @return A numeric_any which store the abs of the value stored in formal numeric_any.
/// @note When the numeric_any is empty, return itself.
export constexpr numeric_any abs(const numeric_any& x) noexcept {
    switch (x.type) {
        FOR_EACH_TYPES_TO(FUNCTION_ABS)
    default:
        return x;
    }
}

/// @brief Casting a numeric_any to a normal number without check.
/// @tparam T The basic arithmetic types in C++, except char without explict sign which is used to check.
/// @param x A numeric_any.
/// @return A normal value represents the value stored in a numeric_any after casting to given type.
/// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
/// @note Always return T{} when a numeric_any is empty.
export template <sign_unambiguous_arithmetic T>
constexpr T unchecked_numeric_cast(const numeric_any& x) noexcept {
    switch (x.type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_CAST)
    case numeric_types::EMPTY:
    default:
        return T{};
    }
}
#undef FUNCTION_TO_CAST
/// @cond
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
/// @endcond

/// @brief Casting a numeric_any to a normal number with check based on policy.
/// @tparam T The basic arithmetic types in C++, except char without explict sign which is used to check.
/// @tparam Policy: The casting policy which will be based on.
/// @param x A numeric_any.
/// @return A normal value represents the value stored in a numeric_any after casting to given type or null.
/// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
/// @note Always return std::nullopt when a numeric_any is empty.
export template <sign_unambiguous_arithmetic T, casting_policy Policy>
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

constexpr numeric_any::numeric_any(sign_unambiguous_arithmetic auto x) noexcept
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

constexpr void numeric_any::reset(sign_unambiguous_arithmetic auto x) noexcept {
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
/// @cond
#define FUNCTION_TO_RESTORE_VALUE(type_name, type_enum_name, enum_value)                                               \
    case numeric_types::type_enum_name: {                                                                              \
        type_name restore_value = unchecked_numeric_cast<type_name>(*this);                                            \
        return numeric_any{-restore_value};                                                                            \
    }
/// @endcond

constexpr numeric_any numeric_any::operator-() const noexcept {
    switch (type) {
        FOR_EACH_TYPES_TO(FUNCTION_TO_RESTORE_VALUE)
    case numeric_types::EMPTY:
    default:
        return numeric_any{};
    }
}

#undef FUNCTION_TO_RESTORE_VALUE
/// @cond
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
/// @endcond

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

constexpr numeric_any::operator bool() const noexcept {
    return unchecked_numeric_cast<bool>(*this);
}

template <sign_unambiguous_arithmetic T>
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
/// @cond
/* Based on standard-conversions in C/C++, all operations will check the tag to determine input types.
   No need to check all tags, because of promotions. */
#define FUNCTIONS_OPERATION(name, op, op_name, op2, op2_name)                                                          \
    constexpr numeric_any& numeric_any::op2_name(sign_unambiguous_arithmetic auto x) noexcept {                        \
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
/// @endcond
FOR_EACH_OPERATOR(FUNCTIONS_OPERATION)
#undef FUNCTIONS_OPERATION

[[nodiscard]] constexpr bool numeric_any::operator==(sign_unambiguous_arithmetic auto x) const noexcept {
    return *this <=> x == ::std::partial_ordering::equivalent;
}
[[nodiscard]] constexpr ::std::partial_ordering
numeric_any::operator<=>(sign_unambiguous_arithmetic auto x) const noexcept {
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
/* It will check the inner tag, then restore values to do comparison.
   No need to check all tags, since it won't change value. Just restore the value and do comparison。 */
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
[[nodiscard]] constexpr bool numeric_any::operator==(const numeric_any& x) const noexcept {
    if (empty() || x.empty()) return false; // Remind that the "empty == empty" will be false.
    return *this <=> x == ::std::partial_ordering::equivalent;
}
/// @cond
// To restore the number before output.
#define FUNCTION_OUTPUT(type_name, type_enum_name, enum_value)                                                         \
    if (x.is_same_type<type_name>()) {                                                                                 \
        auto res = unchecked_numeric_cast<type_name>(x);                                                               \
        os << res;                                                                                                     \
        return os;                                                                                                     \
    }
/// @endcond

/// @brief Output numeric_any to the stream.
/// @tparam CharT The character type. (e.g., char or wchar_t)
/// @tparam Traits The character traits. (default: std::char_traits<CharT>)
/// @param os The output stream reference.
/// @param x The numeric_any which will be output.
/// @return Returns the original output stream reference, enabling chained operations.
export template <typename CharT, typename Traits = ::std::char_traits<CharT>>
::std::basic_ostream<CharT, Traits>& operator<<(::std::basic_ostream<CharT, Traits>& os, const numeric_any& x) {
    constexpr CharT empty_str[]{'e', 'm', 'p', 't', 'y', '\0'};
    FOR_EACH_TYPES_TO(FUNCTION_OUTPUT)
    os << empty_str;
    return os;
}
#undef FUNCTION_OUTPUT
/// @brief A tool function to build numeric_any.
/// @param x The value used to make numeric_any.
/// @return A numeric_any store the given value.
/// @note Follow the rules in constructors.
export constexpr numeric_any make_numeric_any(sign_unambiguous_arithmetic auto x) noexcept {
    return numeric_any{x};
}
} // namespace casyyy::maths
/// @cond
// The inner hash will try to process every member.
#define FUNCTION_HASH(type_name, type_enum_name, enum_value)                                                           \
    if (x.is_same_type<type_name>()) {                                                                                 \
        type_name restore = unchecked_numeric_cast<type_name>(x);                                                      \
        return hash<type_name>{}(restore);                                                                             \
    }
/// @endcond
namespace std {
/// @brief Provide hash support.
template <>
struct hash<::casyyy::maths::numeric_any> {
    /// @cond
    size_t operator()(const ::casyyy::maths::numeric_any& x) const noexcept {
        FOR_EACH_TYPES_TO(FUNCTION_HASH)
        return 0;
    }
    ///@endcond
};
#undef FUNCTION_HASH
/// @cond
#define FUNCTION_FORMAT(type_name, type_enum_name, enum_value)                                                         \
    if (x.is_same_type<type_name>()) {                                                                                 \
        type_name restore = unchecked_numeric_cast<type_name>(x);                                                      \
        if constexpr (is_same_v<CharT, wchar_t>)                                                                       \
            return vformat_to(ctx.out(), fmt, make_wformat_args(restore));                                             \
        else                                                                                                           \
            return vformat_to(ctx.out(), fmt, make_format_args(restore));                                              \
    }
/// @endcond
#ifndef DISABLE_FORMAT_IN_NUMERIC_ANY
/**  @brief Provide formatted support.
 *   @note Unable to check in compile-time when the correct format string is used for the given argument types.
 *   @throw std::format_error When the correct format string is used for the given argument types.
 *                            Or the parser can't find your dynamic parameters.  */
template <typename CharT>
struct formatter<::casyyy::maths::numeric_any, CharT> {
    /// @cond
    // To store format string for value.
    // Based on std::numeric_limits<unsigned long long>::dight10, 60 is enough.
    static constexpr unsigned FORMAT_STRING_BUFFER_SIZE = 60;

    constexpr auto parse(auto& ctx) {
        return parser_.parse(ctx);
    }

    auto format(const ::casyyy::maths::numeric_any& x, auto& ctx) const {
        CharT format_buffer[FORMAT_STRING_BUFFER_SIZE]{};
        if (x.is_floating_point() && parser_.is_for_integer()) {
            throw ::std::format_error{"Cannot use integer format specifier with a float."};
        } else if (!x.is_floating_point() && parser_.is_for_floating_point()) {
            throw ::std::format_error{"Cannot use float format specifier with an integer."};
        }
        // Restore the format stirng, and used vformat_to.
        auto finish = parser_.restore_format_string(format_buffer, format_buffer + FORMAT_STRING_BUFFER_SIZE, ctx);
        basic_string_view<CharT> fmt{format_buffer, finish};
        FOR_EACH_TYPES_TO(FUNCTION_FORMAT)
        if constexpr (is_same_v<CharT, wchar_t>) {
            return format_to(ctx.out(), L"{}", L"empty");
        }
        return format_to(ctx.out(), "{}", "empty");
    }
    ::casyyy::utils::numeric_any_parser<CharT> parser_;
    /// @endcond
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