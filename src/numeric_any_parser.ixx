module;

#include <charconv>
#include <format>
#include <type_traits>

export module casyyy.maths.numeric_any_parser;

namespace casyyy::utils {

export template <typename CharT>
class numeric_any_parser {
public:
    template <typename ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto begin = ctx.begin();
        auto end   = ctx.end();
        if (begin == end || *begin == static_cast<CharT>('}')) return begin;

        if (parse_align_with_character(begin) && begin == end) return begin;

        if (parse_sign(begin) && begin == end) return begin;

        if (parse_alternate_form(begin) && begin == end) return begin;

        if (parse_zero_padding(begin) && begin == end) return begin;

        if (parse_width(begin, end) && begin == end) return begin;

        if (parse_precision(begin, end) && begin == end) return begin;

        if (parse_local(begin) && begin == end) return begin;

        if (parse_base(begin) && begin == end) return begin;

        if (begin != end && *begin != static_cast<CharT>('}'))
            throw ::std::format_error{"Can't consume your input or end should be a }."};

        return begin;
    }
    CharT* restore_format_string(CharT* begin, CharT* end, auto& ctx) const {
        *begin = static_cast<CharT>('{'), ++begin;
        *begin = static_cast<CharT>(':'), ++begin;
        if (has_set_align) {
            if (fill_charactor != static_cast<CharT>(' ')) {
                *begin = fill_charactor, ++begin, *begin = align_patten, ++begin;
            } else {
                *begin = align_patten, ++begin;
            }
        }
        if (sign_flag) *begin = sign_charactor, ++begin;
        if (has_alternative) *begin = static_cast<CharT>('#'), ++begin;
        if (zero_padding) *begin = static_cast<CharT>('0'), ++begin;

        auto final_width = determine_dynamic_arg(ctx, dynamic_width, width_id, static_width);
        if (final_width != 0) {
            if constexpr (::std::is_same_v<char, CharT>) {
                begin = ::std::to_chars(begin, end, final_width).ptr;
            } else {
                char buffer[20]{};
                unsigned long long width = ::std::to_chars(buffer, buffer + 20, final_width).ptr - buffer;
                for (unsigned long long i = 0; i < width; ++i) {
                    *begin = static_cast<CharT>(buffer[i]), ++begin;
                }
            }
        }
        auto final_precision = determine_dynamic_arg(ctx, dynamic_precision, precision_id, static_precision);
        if (final_precision != 0) {
            *begin = static_cast<CharT>('.'), ++begin;
            if constexpr (::std::is_same_v<char, CharT>) {
                begin = ::std::to_chars(begin, end, final_precision).ptr;
            } else {
                char buffer[20]{};
                unsigned long long width = ::std::to_chars(buffer, buffer + 20, final_precision).ptr - buffer;
                for (unsigned long long i = 0; i < width; ++i) {
                    *begin = static_cast<CharT>(buffer[i]), ++begin;
                }
            }
        }
        if (using_locale) *begin = static_cast<CharT>('L'), ++begin;
        if (set_base) *begin = type_charactor, ++begin;
        *begin = static_cast<CharT>('}'), ++begin;
        return begin;
    }

private:
    unsigned long long determine_dynamic_arg(auto& ctx, bool has_dynamic, unsigned long long id,
                                             unsigned long long static_return) const {
        if (!has_dynamic) return static_return;
        unsigned long long dynamic = 0;
        auto get_value             = ctx.arg(id);
        if (!get_value) throw ::std::format_error("Don't input a dynamic width without input a arg.");
        ::std::visit_format_arg(
            [&dynamic](auto&& value) {
                using U = ::std::decay_t<decltype(value)>;
                if constexpr (::std::is_integral_v<U>) {
                    dynamic = static_cast<unsigned long long>(value);
                } else {
                    throw ::std::format_error("Incorrect value type of the arg.");
                }
            },
            get_value);
        return dynamic;
    }

    template <typename Iterator>
    constexpr bool parse_align_with_character(Iterator& begin) {
        Iterator next = begin;
        ++next;
        if (*next == static_cast<CharT>('^') || *next == static_cast<CharT>('>') || *next == static_cast<CharT>('<')) {
            fill_charactor = *begin;
            align_patten   = *next;
            ++(++begin);
            has_set_align = true;
        } else if (*begin == static_cast<CharT>('^') || *begin == static_cast<CharT>('>') ||
                   *begin == static_cast<CharT>('<')) {
            align_patten = *begin;
            ++begin;
            has_set_align = true;
        }
        return has_set_align;
    }
    template <typename Iterator>
    constexpr bool parse_sign(Iterator& begin) {
        if (*begin == static_cast<CharT>('-') || *begin == static_cast<CharT>('+') ||
            *begin == static_cast<CharT>(' ')) {
            sign_charactor = *begin;
            sign_flag      = true;
            ++begin;
        }
        return sign_flag;
    }
    template <typename Iterator>
    constexpr bool parse_alternate_form(Iterator& begin) {
        if (*begin == static_cast<CharT>('#')) {
            has_alternative = true;
            ++begin;
        }
        return has_alternative;
    }
    template <typename Iterator>
    constexpr bool parse_zero_padding(Iterator& begin) {
        if (*begin == static_cast<CharT>('0')) {
            if (!has_set_align) zero_padding = true;
            ++begin;
        }
        return zero_padding;
    }
    template <typename Iterator>
    constexpr bool parse_width(Iterator& begin, Iterator end) {
        if (*begin == static_cast<CharT>('{')) {
            ++begin;
            dynamic_width = true;
            if (*begin == static_cast<CharT>('}')) {
                ++begin;
                width_id = 1;
                return true;
            }
            if (*begin > static_cast<CharT>('9') || *begin < static_cast<CharT>('0'))
                throw ::std::format_error{"Is invalid arg index"};
            while (*begin <= static_cast<CharT>('9') && *begin >= static_cast<CharT>('0')) {
                width_id = width_id * 10 + (*begin - static_cast<CharT>('0'));
                ++begin;
            }
            return true;
        }

        if (*begin > static_cast<CharT>('9') || *begin < static_cast<CharT>('0')) return false;

        while (*begin <= static_cast<CharT>('9') && *begin >= static_cast<CharT>('0') && begin != end) {
            static_width = static_width * 10 + (*begin - CharT{'0'});
            ++begin;
        }
        return true;
    }
    template <typename Iterator>
    constexpr bool parse_precision(Iterator& begin, Iterator end) {
        if (*begin != static_cast<CharT>('.')) return false;
        ++begin;
        if (*begin == static_cast<CharT>('{')) {
            ++begin;
            has_precision     = true;
            dynamic_precision = true;
            if (*begin == static_cast<CharT>('}')) {
                ++begin;
                precision_id = width_id + 1;
                return true;
            }
            if (*begin > static_cast<CharT>('9') || *begin < static_cast<CharT>('0'))
                throw ::std::format_error{"Is invalid arg index"};
            while (*begin <= static_cast<CharT>('9') && *begin >= static_cast<CharT>('0')) {
                precision_id = precision_id * 10 + (*begin - static_cast<CharT>('0'));
                ++begin;
            }
            return true;
        }
        if (*begin > static_cast<CharT>('9') || *begin < static_cast<CharT>('0')) return false;
        while (*begin <= static_cast<CharT>('9') && *begin >= static_cast<CharT>('0') && begin != end) {
            has_precision    = true;
            static_precision = static_precision * 10 + (*begin - static_cast<CharT>('0'));
            ++begin;
        }
        return true;
    }
    template <typename Iterator>
    constexpr bool parse_local(Iterator& begin) {
        if (*begin == static_cast<CharT>('L')) {
            using_locale = true;
            ++begin;
        }
        return using_locale;
    }
    template <typename Iterator>
    constexpr bool parse_base(Iterator& begin) {
        CharT type = *begin;
        if (type == static_cast<CharT>('g') || type == static_cast<CharT>('e') || type == static_cast<CharT>('f') ||
            type == static_cast<CharT>('a')) {
            type_charactor = type;
            set_base       = true;
        } else if (type == static_cast<CharT>('G') || type == static_cast<CharT>('E') ||
                   type == static_cast<CharT>('F') || type == static_cast<CharT>('A')) {
            type_charactor = type;
            set_base       = true;
        } else if (type == static_cast<CharT>('x') || type == static_cast<CharT>('d') ||
                   type == static_cast<CharT>('o') || type == static_cast<CharT>('b')) {
            if (has_precision) return false;
            type_charactor = type;
            set_base       = true;
        } else if (type == static_cast<CharT>('X') || type == static_cast<CharT>('E') ||
                   type == static_cast<CharT>('O') || type == static_cast<CharT>('B')) {
            if (has_precision) return false;
            type_charactor = type;
            set_base       = true;
        }
        if (set_base) ++begin;
        return set_base;
    }
    unsigned long long static_width     = 0;
    unsigned long long static_precision = 0;
    unsigned long long width_id         = 0;
    unsigned long long precision_id     = 0;
    CharT align_patten                  = static_cast<CharT>('>');
    CharT fill_charactor                = static_cast<CharT>(' ');
    CharT type_charactor                = static_cast<CharT>(' ');
    CharT sign_charactor                = static_cast<CharT>(' ');
    bool has_set_align                  = false;
    bool sign_flag                      = false;
    bool has_alternative                = false;
    bool zero_padding                   = false;
    bool has_precision                  = false;
    bool using_locale                   = false;
    bool set_base                       = false;
    bool dynamic_width                  = false;
    bool dynamic_precision              = false;
};

} // namespace casyyy::utils

