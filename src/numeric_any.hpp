#ifndef CY_MATH_NUMERIC_ANY_HPP_
#define CY_MATH_NUMERIC_ANY_HPP_

#include "numeric_any_parser.hpp"
#include <array>
#include <bit>
#include <compare>
#include <cstring>
#include <format>
#include <functional>
#include <iosfwd>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>

// To disable some unnecessary warnings.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4002 4061 4702 4804 4146 4244)
#endif

/// @cond
// To generate types.
#define FOR_EACH_TYPES_TO(x)                                                                                           \
    x(bool, BOOL, 0) x(signed char, SIGNED_CHAR, 1) x(unsigned char, UNSIGNED_CHAR, 2) x(short, SHORT, 3)              \
        x(unsigned short, UNSIGNED_SHORT, 4) x(int, INT, 5) x(unsigned int, UNSIGNED_INT, 6) x(long, LONG, 7)          \
            x(unsigned long, UNSIGNED_LONG, 8) x(long long, LONG_LONG, 9)                                              \
                x(unsigned long long, UNSIGNED_LONG_LONG, 10) x(float, FLOAT, 11) x(double, DOUBLE, 12)                \
                    x(long double, LONG_DOUBLE, 13)
/// @endcond

namespace cy::maths::details {
/// @cond
// Type tags.
enum class numeric_types : unsigned char {
#define FUNCTION(type_name, type_enum_name, enum_value) type_enum_name = enum_value,
    FOR_EACH_TYPES_TO(FUNCTION)
#undef FUNCTION
};
// Type names.
inline constexpr ::std::string_view type_names[]{
#define FUNCTION(type_name, type_enum_name, enum_value) #type_name,
    FOR_EACH_TYPES_TO(FUNCTION)
#undef FUNCTION
        ""};
// To determine type tag.
template <typename>
constexpr numeric_types types() noexcept {
    static_assert(false, "Isn't allow type");
    return details::numeric_types::BOOL;
}
#define FUNCTION(type_name, type_enum_name, enum_value)                                                                \
    template <>                                                                                                        \
    constexpr numeric_types types<type_name>() noexcept {                                                              \
        return details::numeric_types::type_enum_name;                                                                 \
    }

FOR_EACH_TYPES_TO(FUNCTION)
#undef FUNCTION
/// @endcond
} // namespace cy::maths::details

namespace cy::maths {
/// @brief Only for arithmetic types, except the char without explicit sign.
template <typename T>
concept sign_unambiguous_arithmetic =
    ::std::is_arithmetic_v<T> && !::std::is_same_v<char, T> && !::std::is_same_v<wchar_t, T> &&
    !::std::is_same_v<char8_t, T> && !::std::is_same_v<char16_t, T> && !::std::is_same_v<char32_t, T>;

/// @brief Numeric conversion strategy, used for handling different types of number conversions.
/// @note The default policy is set to strict.
enum class casting_policy : unsigned char { equal, strict, normal };

/**
 * @brief A type-erasure-based utility class for arithmetic types.
 *        Support addition, subtraction, multiplication, division and comparison.
 * @note Only support the basic arithmetic types in C++, except char without explicit sign.
 */
class numeric_any {
public:
    // The former declarations.
    template <sign_unambiguous_arithmetic T>
    friend constexpr T as(const numeric_any&) noexcept;

#define FUNCTIONS_TO_VISIT(type_name, type_enum_name, enum_value)                                                      \
    case details::numeric_types::type_enum_name: {                                                                     \
        auto restore = as<type_name>(x);                                                                               \
        return f(restore);                                                                                             \
    }

    /** @brief Visit the inner value of a numeric_any and apply a function to it.
     * @param x The numeric_any to be visited.
     * @param f The function to be applied to the inner value.
     * @return The result of applying the function to the inner value.
     * @note 'f' should accept a single argument of the type of the inner values.
     */
    template <typename Func, typename Any>
    friend constexpr decltype(auto) visit(Func&& f, Any&& x) noexcept {
        switch (x.type_) { FOR_EACH_TYPES_TO(FUNCTIONS_TO_VISIT) }
#ifdef _MSC_VER
        __assume(false);
#else
        __builtin_unreachable();
#endif
    }
#undef FUNCTIONS_TO_VISIT

    constexpr numeric_any() noexcept = default;

    /// @brief Construct a numeric_any which is assigned by a value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explicit sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    constexpr numeric_any(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Return the negative of the number inside.
    /// @return The negative of the number inside.
    /// @note Follow C++ rules for negation of fundamental types.
    constexpr numeric_any operator-() const noexcept {
        return visit([](auto i) { return numeric_any{-i}; }, *this);
    }

    /// @brief Return the numeric_any itself.
    /// @return The numeric_any itself.
    constexpr numeric_any operator+() const noexcept {
        return *this;
    }

    /// @brief Pre-increment operator.
    /// @return Reference to the incremented object.
    /// @note Follows built-in semantics, excpet bool, which is setting the value to (bool)(inner + 1).
    constexpr numeric_any& operator++() noexcept {
        visit([this](auto i) { this->reset(decltype(i)(i + 1)); }, *this);
        return *this;
    }

    /// @brief Post-increment operator.
    /// @return The value before increment.
    /// @note Follows built-in semantics, excpet bool, which is setting the value to (bool)(inner + 1).
    constexpr numeric_any operator++(int) noexcept {
        auto repeat = *this;
        ++*this;
        return repeat;
    }

    /// @brief Pre-decrement operator.
    /// @return Reference to the decreed object.
    /// @note Follows built-in semantics, excpet bool, which is setting the value to (bool)(inner - 1).
    constexpr numeric_any& operator--() noexcept {
        visit([this](auto i) { this->reset(decltype(i)(i - 1)); }, *this);
        return *this;
    }

    /// @brief Post-decrement operator.
    /// @return The value before decrement.
    /// @note Follows built-in semantics, excpet bool, which is setting the value to (bool)(inner - 1).
    constexpr numeric_any operator--(int) noexcept {
        auto repeat = *this;
        --*this;
        return repeat;
    }

    /// @brief Do an addition assignment with a normal value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explicit sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return Reference to the current value.
    /// @note Internal implicit conversion may change the stored type per C++ standard rules of arithmetic conversions.
    constexpr numeric_any& operator+=(sign_unambiguous_arithmetic auto x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a + b; });
    }

    /// @brief Do a substraction assignment with a normal value.
    /// @copydetails operator+=(sign_unambiguous_arithmetic auto)
    constexpr numeric_any& operator-=(sign_unambiguous_arithmetic auto x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a - b; });
    }
    /// @brief Do a multiplication assignment with a normal value.
    /// @copydetails operator+=(sign_unambiguous_arithmetic auto)
    constexpr numeric_any& operator*=(sign_unambiguous_arithmetic auto x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a * b; });
    }
    /// @brief Do a division assignment with a normal value.
    /// @copydetails operator+=(sign_unambiguous_arithmetic auto)
    constexpr numeric_any& operator/=(sign_unambiguous_arithmetic auto x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a / b; });
    }

    /// @brief Do an addition assignment with another numeric_any.
    /// @param x The value used to do operation.
    /// @return Reference to the current value.
    /// @note Internal implicit conversion may change the stored type per C++ standard rules of arithmetic conversions.
    constexpr numeric_any& operator+=(const numeric_any& x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a + b; });
    }

    /// @brief Do a substraction assignment with another numeric_any.
    /// @copydetails operator+=(const numeric_any&)
    constexpr numeric_any& operator-=(const numeric_any& x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a - b; });
    }

    /// @brief Do a multiplication assignment with another numeric_any.
    /// @copydetails operator+=(const numeric_any&)
    constexpr numeric_any& operator*=(const numeric_any& x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a * b; });
    }

    /// @brief Do a division assignment with another numeric_any.
    /// @copydetails operator+=(const numeric_any&)
    constexpr numeric_any& operator/=(const numeric_any& x) noexcept {
        return binary_operation(x, [](auto a, auto b) { return a / b; });
    }

    /// @brief Compare a numeric_any is equal to a normal value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explicit sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return Whether a numeric_any is equal to a normal value.
    [[nodiscard]] constexpr bool operator==(sign_unambiguous_arithmetic auto x) const noexcept {
        return *this <=> x == ::std::partial_ordering::equivalent;
    }

    /// @brief Three-way comparison between numeric_any with a normal value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explicit sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return A partial_ordering represent the result of three-way comparison.
    constexpr ::std::partial_ordering operator<=>(sign_unambiguous_arithmetic auto x) const noexcept;

    /// @brief Compare a numeric_any is equal to another numeric_any.
    /// @param x The value used to do comparison.
    /// @return Whether a numeric_any is equal to a normal value.
    constexpr bool operator==(const numeric_any& x) const noexcept {
        return *this <=> x == ::std::partial_ordering::equivalent;
    }

    /// @brief Three-way comparison between numeric_any with another numeric_any.
    /// @param x The value used to do comparison.
    /// @return A partial_ordering represent the result of three-way comparison.
    constexpr ::std::partial_ordering operator<=>(const numeric_any& x) const noexcept;

    /// @brief Check whether inner value isn't zero.
    /// @return Whether inner value isn't zero.
    explicit constexpr operator bool() const noexcept {
        return as<bool>(*this);
    }

    /// @brief Reset the inner value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explicit sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @return self-referencing after reset.
    constexpr numeric_any& operator=(sign_unambiguous_arithmetic auto x) noexcept {
        this->reset(x);
        return *this;
    }

    /// @brief Get the typename of the value which numeric_any store.
    /// @return A readable string view, with content representing the type stored internally.
    [[nodiscard]] constexpr ::std::string_view type_name() const noexcept {
        return details::type_names[static_cast<unsigned>(type_)];
    }

    /// @brief Get the number of bytes used by internal storage data。
    /// @return The number of bytes used by internal storage data。
    [[nodiscard]] constexpr ::std::size_t type_size() const noexcept {
        return width_;
    }

    /// @brief Check if what's stored inside is a floating point number.
    /// @return A boolean represents whether it stores a floating point number or not.
    [[nodiscard]] constexpr bool is_floating_point() const noexcept {
        return float_point_;
    }

    /// @brief Check if what's stored inside is an unsigned number.
    /// @return A boolean represents whether it stores an unsigned number or not.
    [[nodiscard]] constexpr bool is_unsigned_number() const noexcept {
        return is_unsigned_;
    }

    /// @brief Check if what's stored inside is a nonnegative number.
    /// @return A boolean represents whether it stores a nonnegative number or not.
    /// @note If inner value is NaN, it will return false.
    [[nodiscard]] constexpr bool is_nonnegative() const noexcept {
        return positive_;
    }

    /// @brief Reset the inner value.
    /// @param x Input value, must be the basic arithmetic types in C++, except char without explicit sign.
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    /// @note It will change the base type of the internal storage. And must offer a number.
    constexpr void reset(sign_unambiguous_arithmetic auto x) noexcept;

    /// @brief Check if the given type is the type of value stored internally.
    /// @tparam T The basic arithmetic types in C++, except char without explicit sign which is used to check.
    /// @return Whether the given type parameter is the type of the underlying stored value。
    /// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
    template <sign_unambiguous_arithmetic T>
    [[nodiscard]] constexpr bool is_same_type() const noexcept {
        return details::types<T>() == type_;
    }

    /// @brief Get the inner view of the bytes.
    /// @return A span which represent inner view of the bytes.
    /// @note The span is read-only. And its size is based on the number of bytes used by internal storage data。
    [[nodiscard]] constexpr ::std::span<const unsigned char> view_bytes() const noexcept {
        return ::std::span{storage_.data(), width_};
    }

private:
    // Use to store all bytes of a value.
    ::std::array<unsigned char, 16> storage_;
    unsigned int width_;
    // These data use to switch the branch in operations and castings.
    details::numeric_types type_;
    bool float_point_;
    bool is_unsigned_;
    bool positive_;

    /// @cond
    template <typename T1, typename T2, typename Func>
    constexpr numeric_any& apply(T2 x, Func&& f) noexcept {
        using common_type = ::std::common_type_t<T1, T2>;
        reset(f(as<common_type>(*this), x));
        return *this;
    }

    template <typename Func>
    constexpr numeric_any& binary_operation(auto&& x, Func&& f) noexcept {
        if constexpr (::std::is_same_v<::std::remove_cvref_t<decltype(x)>, numeric_any>) {
            switch (x.type_) {
            case details::numeric_types::UNSIGNED_INT:
                return binary_operation(as<unsigned>(x), ::std::forward<Func>(f));
            case details::numeric_types::LONG:
                return binary_operation(as<long>(x), ::std::forward<Func>(f));
            case details::numeric_types::UNSIGNED_LONG:
                return binary_operation(as<unsigned long>(x), ::std::forward<Func>(f));
            case details::numeric_types::LONG_LONG:
                return binary_operation(as<long long>(x), ::std::forward<Func>(f));
            case details::numeric_types::UNSIGNED_LONG_LONG:
                return binary_operation(as<unsigned long long>(x), ::std::forward<Func>(f));
            case details::numeric_types::FLOAT:
                return binary_operation(as<float>(x), ::std::forward<Func>(f));
            case details::numeric_types::DOUBLE:
                return binary_operation(as<double>(x), ::std::forward<Func>(f));
            case details::numeric_types::LONG_DOUBLE:
                return binary_operation(as<long double>(x), ::std::forward<Func>(f));
            default:
                return binary_operation(as<int>(x), ::std::forward<Func>(f));
            }
        } else {
            switch (type_) {
            case details::numeric_types::UNSIGNED_INT:
                return apply<unsigned int>(x, ::std::forward<Func>(f));
            case details::numeric_types::LONG:
                return apply<long>(x, ::std::forward<Func>(f));
            case details::numeric_types::UNSIGNED_LONG:
                return apply<unsigned long>(x, ::std::forward<Func>(f));
            case details::numeric_types::LONG_LONG:
                return apply<long long>(x, ::std::forward<Func>(f));
            case details::numeric_types::UNSIGNED_LONG_LONG:
                return apply<unsigned long long>(x, ::std::forward<Func>(f));
            case details::numeric_types::FLOAT:
                return apply<float>(x, ::std::forward<Func>(f));
            case details::numeric_types::DOUBLE:
                return apply<double>(x, ::std::forward<Func>(f));
            case details::numeric_types::LONG_DOUBLE:
                return apply<long double>(x, ::std::forward<Func>(f));
            default:
                return apply<int>(x, ::std::forward<Func>(f));
            }
        }
    }
    /// @endcond
};

constexpr numeric_any operator+(numeric_any x, auto&& y) noexcept {
    return x += y;
}
constexpr numeric_any operator-(numeric_any x, auto&& y) noexcept {
    return x -= y;
}
constexpr numeric_any operator*(numeric_any x, auto&& y) noexcept {
    return x *= y;
}
constexpr numeric_any operator/(numeric_any x, auto&& y) noexcept {
    return x /= y;
}
/// @cond
// Based on the std::bit_cast.
#define FUNCTION_TO_AS(type_name, type_enum_name, enum_value)                                                          \
    case details::numeric_types::type_enum_name: {                                                                     \
        if (!std::is_constant_evaluated()) {                                                                           \
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
/// @endcond
/// @brief Casting a numeric_any to a normal number without check.
/// @tparam T The basic arithmetic types in C++, except char without explicit sign which is used to check.
/// @param x A numeric_any.
/// @return A normal value represents the value stored in a numeric_any after casting to given type.
/// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
template <sign_unambiguous_arithmetic T>
constexpr T as(const numeric_any& x) noexcept {
    switch (x.type_) { FOR_EACH_TYPES_TO(FUNCTION_TO_AS) }
#ifdef _MSC_VER
    __assume(false);
#else
    __builtin_unreachable();
#endif
}
#undef FUNCTION_TO_AS

/// @brief Casting a numeric_any to a normal number with check based on policy.
/// @tparam T The basic arithmetic types in C++, except char without explicit sign which is used to check.
/// @tparam Policy: The casting policy which will be based on.
/// @param x A numeric_any.
/// @return A normal value represents the value stored in a numeric_any after casting to given type or null.
/// @note char types must be explicitly signed-qualified in constructors to avoid ambiguity.
template <sign_unambiguous_arithmetic T, casting_policy Policy = casting_policy::strict>
[[nodiscard]] constexpr ::std::optional<T> from(const numeric_any& x) noexcept {
    if (x.is_same_type<T>()) return as<T>(x);
    if constexpr (Policy == casting_policy::equal) {
        return ::std::nullopt;
    } else if constexpr (Policy == casting_policy::strict) {
        return visit(
            []<typename U>(U x) -> ::std::optional<T> {
                if constexpr (::std::is_same_v<U, bool>) {
                    return static_cast<T>(x);
                } else if constexpr (sizeof(U) > sizeof(T)) {
                    return ::std::nullopt;
                } else if constexpr (!(::std::is_integral_v<U> && ::std::is_integral_v<T>) &&
                                     !(::std::is_floating_point_v<U> && ::std::is_floating_point_v<T>)) {
                    return ::std::nullopt;
                } else if constexpr (!(::std::is_signed_v<U> && ::std::is_signed_v<T>) &&
                                     !(::std::is_unsigned_v<U> && ::std::is_unsigned_v<T>)) {
                    return ::std::nullopt;
                }
                return static_cast<T>(x);
            },
            x);
    } else {
        return visit(
            [](auto x) -> ::std::optional<T> {
                constexpr T max = ::std::numeric_limits<T>::max();
                constexpr T min = ::std::numeric_limits<T>::lowest();
                if (static_cast<long double>(x) >= static_cast<long double>(min) &&
                    static_cast<long double>(x) <= static_cast<long double>(max))
                    return static_cast<T>(x);
                return ::std::nullopt;
            },
            x);
    }
}

constexpr numeric_any::numeric_any(sign_unambiguous_arithmetic auto x) noexcept
    : storage_{}, width_{sizeof(decltype(x))}, type_{details::types<decltype(x)>()},
      float_point_{::std::is_floating_point_v<decltype(x)>}, is_unsigned_{::std::is_unsigned_v<decltype(x)>},
      positive_{x >= decltype(x){}} {
    // Base on the std::bit_cast;
    if (!std::is_constant_evaluated()) {
        ::std::memcpy(storage_.data(), &x, sizeof(decltype(x)));
    } else {
        ::std::array<unsigned char, sizeof(decltype(x))> temp{};
        temp = ::std::bit_cast<decltype(temp), decltype(x)>(x);
        for (::std::size_t i = 0; i < sizeof(decltype(x)); ++i)
            storage_[i] = temp[i];
    }
}

constexpr void numeric_any::reset(sign_unambiguous_arithmetic auto x) noexcept {
    type_        = details::types<decltype(x)>();
    float_point_ = ::std::is_floating_point_v<decltype(x)>;
    width_       = sizeof(decltype(x));
    is_unsigned_ = ::std::is_unsigned_v<decltype(x)>;
    positive_    = x >= decltype(x){};
    if (!std::is_constant_evaluated()) {
        ::std::memcpy(storage_.data(), &x, sizeof(decltype(x)));
    } else {
        ::std::array<unsigned char, sizeof(decltype(x))> temp{};
        temp = ::std::bit_cast<decltype(temp), decltype(x)>(x);
        for (::std::size_t i = 0; i < sizeof(decltype(x)); ++i)
            storage_[i] = temp[i];
    }
}

[[nodiscard]] constexpr ::std::partial_ordering
numeric_any::operator<=>(sign_unambiguous_arithmetic auto x) const noexcept {
    // The NaN will be checked first.
    if constexpr (::std::is_floating_point_v<decltype(x)>) {
        if (x != x) return ::std::partial_ordering::unordered; // NOLINT
        if constexpr (::std::is_same_v<decltype(x), float>) {
            if (type_ == details::numeric_types::FLOAT) return as<float>(*this) <=> x;
            if (type_ == details::numeric_types::DOUBLE) return as<double>(*this) <=> x;
        } else if constexpr (::std::is_same_v<decltype(x), double>) {
            if (type_ != details::numeric_types::LONG_DOUBLE) return as<double>(*this) <=> x;
        }
        return as<long double>(*this) <=> x;
    } else {
        if (positive_ != x >= decltype(x){})
            return static_cast<int>(positive_) <=> static_cast<int>(x >= decltype(x){});
        switch (type_) {
        case details::numeric_types::UNSIGNED_LONG_LONG:
            return as<unsigned long long>(*this) <=> static_cast<unsigned long long>(x);
        case details::numeric_types::FLOAT:
            return as<float>(*this) <=> x;
        case details::numeric_types::DOUBLE:
            return as<double>(*this) <=> x;
        case details::numeric_types::LONG_DOUBLE:
            return as<long double>(*this) <=> x;
        default:
            return as<long long>(*this) <=> x;
        }
    }
}
/* It will check the inner tag, then restore values to do comparison.
   No need to check all tags, since it won't change value. Just restore the value and do comparison。 */
[[nodiscard]] constexpr ::std::partial_ordering numeric_any::operator<=>(const numeric_any& x) const noexcept {
    if (type_ == details::numeric_types::LONG_DOUBLE || x.type_ == details::numeric_types::LONG_DOUBLE)
        return as<long double>(*this) <=> as<long double>(x);
    if (type_ == details::numeric_types::DOUBLE || x.type_ == details::numeric_types::DOUBLE)
        return as<double>(*this) <=> as<double>(x);
    if (type_ == details::numeric_types::FLOAT || x.type_ == details::numeric_types::FLOAT)
        return as<float>(*this) <=> as<float>(x);
    // The signed will be checked first, so 12U < -1 will be false.
    if (positive_ != x.positive_) return static_cast<int>(positive_) <=> static_cast<int>(x.positive_);
    if (type_ == details::numeric_types::UNSIGNED_LONG_LONG || x.type_ == details::numeric_types::UNSIGNED_LONG_LONG)
        return as<unsigned long long>(*this) <=> as<unsigned long long>(x);
    return as<long long>(*this) <=> as<long long>(x);
}

/// @brief Output numeric_any to the stream.
/// @tparam CharT The character type. (e.g., char or wchar_t)
/// @tparam Traits The character traits. (default: std::char_traits<CharT>)
/// @param os The output stream reference.
/// @param x The numeric_any which will be output.
/// @return Returns the original output stream reference, enabling chained operations.
template <typename CharT, typename Traits = ::std::char_traits<CharT>>
::std::basic_ostream<CharT, Traits>& operator<<(::std::basic_ostream<CharT, Traits>& os, const numeric_any& x) {
    visit([&os](auto i) { os << i; }, x);
    return os;
}

/// @brief A tool function to build numeric_any.
/// @param x The value used to make numeric_any.
/// @return A numeric_any store the given value.
/// @note Follow the rules in constructors.
constexpr numeric_any make_numeric_any(sign_unambiguous_arithmetic auto x) noexcept {
    return numeric_any{x};
}
} // namespace cy::maths

namespace std {

template <::cy::maths::sign_unambiguous_arithmetic T>
struct common_type<T, ::cy::maths::numeric_any> {
    using type = ::cy::maths::numeric_any;
};

template <::cy::maths::sign_unambiguous_arithmetic T>
struct common_type<::cy::maths::numeric_any, T> {
    using type = ::cy::maths::numeric_any;
};

/// @brief Provide hash support.
template <>
struct hash<::cy::maths::numeric_any> {
    /// @cond
    size_t operator()(const ::cy::maths::numeric_any& x) const noexcept {
        return visit([](auto i) { return hash<decltype(i)>()(i); }, x);
    }
    ///@endcond
};
/**  @brief Provide formatted support.
 *   @note Unable to check in compile-time when the correct format string is used for the given argument types.
 *   @throw std::format_error When the correct format string is used for the given argument types.
 *                            Or the parser can't find your dynamic parameters.  */
template <typename CharT>
struct formatter<::cy::maths::numeric_any, CharT> {
    /// @cond
    // To store format string for value, based on std::numeric_limits<unsigned long long>::digits10, 60 is enough.
    static constexpr unsigned FORMAT_STRING_BUFFER_SIZE = 60;

    constexpr auto parse(auto& ctx) {
        return parser_.parse(ctx);
    }

    auto format(const ::cy::maths::numeric_any& x, auto& ctx) const {
        CharT format_buffer[FORMAT_STRING_BUFFER_SIZE]{};
        if (x.is_floating_point() && parser_.is_for_integer())
            throw format_error{"Cannot use integer format specifier with a float."};
        if (!x.is_floating_point() && parser_.is_for_floating_point())
            throw format_error{"Cannot use float format specifier with an integer."};
        // Restore the format string, and format the number based on the format string.
        auto finish = parser_.restore_format_string(format_buffer, format_buffer + FORMAT_STRING_BUFFER_SIZE, ctx);
        basic_string_view<CharT> fmt{format_buffer, finish};
        if constexpr (is_same_v<CharT, wchar_t>) {
            return visit([&ctx, &fmt](auto i) { return vformat_to(ctx.out(), fmt, make_wformat_args(i)); }, x);
        } else {
            return visit([&ctx, &fmt](auto i) { return vformat_to(ctx.out(), fmt, make_format_args(i)); }, x);
        }
    }
    ::cy::utils::numeric_any_parser<CharT> parser_;
    /// @endcond
};
} // namespace std
#undef FOR_EACH_OPERATOR
#undef FOR_EACH_TYPES_TO

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif