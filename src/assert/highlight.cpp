/// @file highlight.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ syntax highlighting
/// @version 0.1
/// @date 2024-09-24
///
/// MIT License
/// @copyright Copyright (c) 2024 Braxton Salyer <braxtonsalyer@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#include <hyperion/assert/detail/parser.h>
#include <hyperion/assert/highlight.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/mpl/value.h>
#include <hyperion/platform/def.h>
#include <hyperion/platform.h>
#include <hyperion/platform/types.h>

#include <flux.hpp>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <cassert>
#include <string>
#include <string_view>
#include <unordered_map>

namespace hyperion::assert::highlight {

    namespace {
        [[nodiscard]] constexpr auto
        to_string(const tokens::Token::Kind& kind) noexcept -> std::string_view {
            using namespace std::string_view_literals;
            if(std::holds_alternative<tokens::Punctuation>(kind)) {
                return "Punctuation"sv;
            }

            if(std::holds_alternative<tokens::Keyword>(kind)) {
                return "Keyword"sv;
            }

            if(std::holds_alternative<tokens::String>(kind)) {
                return "String"sv;
            }

            if(std::holds_alternative<tokens::Numeric>(kind)) {
                return "Numeric"sv;
            }

            if(std::holds_alternative<tokens::Error>(kind)) {
                return "Error"sv;
            }

            if(const auto* ident = std::get_if<tokens::Identifier>(&kind); ident != nullptr) {
                if(std::holds_alternative<tokens::Namespace>(*ident)) {
                    return "Namespace"sv;
                }

                if(std::holds_alternative<tokens::Type>(*ident)) {
                    return "Type"sv;
                }

                if(std::holds_alternative<tokens::Function>(*ident)) {
                    return "Function"sv;
                }

                if(std::holds_alternative<tokens::Variable>(*ident)) {
                    return "Variable"sv;
                }
            }

            HYPERION_UNREACHABLE();
        }

        [[nodiscard]] auto
        get_colors() -> std::unordered_map<std::string_view, Color>& {
            using namespace std::string_view_literals;
            static auto colors = std::unordered_map<std::string_view, Color>{
                {"Punctuation"sv, RgbColor{0x9daaaa_u32}},
                {    "Keyword"sv, RgbColor{0xc67ada_u32}},
                {     "String"sv, RgbColor{0x83a76e_u32}},
                {    "Numeric"sv, RgbColor{0xd29767_u32}},
                {  "Namespace"sv, RgbColor{0x00997b_u32}},
                {       "Type"sv, RgbColor{0xdbba75_u32}},
                {   "Function"sv, RgbColor{0x61afef_u32}},
                {   "Variable"sv, RgbColor{0x9daaaa_u32}},
                {      "Error"sv, RgbColor{0xc65156_u32}},
            };

            return colors;
        }
    } // namespace

    [[nodiscard]] auto set_color(const tokens::Token::Kind& kind, Color color) {
        get_colors()[to_string(kind)] = color;
    }

    // NOLINTNEXTLINE(*-rvalue-reference-param-not-moved)
    [[nodiscard]] auto set_color(tokens::Token::Kind&& kind, Color color) {
        get_colors()[to_string(kind)] = color;
    }

    [[nodiscard]] auto set_color(const Highlight& _highlight) {
        get_colors()[to_string(_highlight.kind)] = _highlight.color;
    }

    // NOLINTNEXTLINE(*-rvalue-reference-param-not-moved)
    [[nodiscard]] auto set_color(Highlight&& _highlight) {
        get_colors()[to_string(_highlight.kind)] = _highlight.color;
    }

    [[nodiscard]] auto set_colors(const std::vector<Highlight>& colors) {
        auto& _colors = get_colors();
        flux::for_each(colors, [&_colors](const auto& _highlight) {
            _colors[to_string(_highlight.kind)] = _highlight.color;
        });
    }

    [[nodiscard]] auto set_colors(std::vector<Highlight>&& colors) {
        auto& _colors = get_colors();
        flux::for_each(std::move(colors), [&_colors](auto&& _highlight) {
            _colors[to_string(_highlight.kind)] = _highlight.color;
        });
    }

    [[nodiscard]] auto
    get_color(const tokens::Token::Kind& kind) noexcept -> Color {
        return get_colors()[to_string(kind)];
    }

    namespace {
        using hyperion::mpl::operator""_value;

        template<mpl::MetaValue TCurrent,
                 mpl::MetaValue TStep = decltype(TCurrent{} / mpl::Value<2_usize>{}),
                 mpl::MetaValue TBound = decltype(TCurrent{} * mpl::Value<2_usize>{})>
            requires(static_cast<bool>(TCurrent{} > TStep{}))
                    && (static_cast<bool>(TCurrent{} <= TBound{}))
                    && (static_cast<bool>(TCurrent{} != 0))
        struct indexed_call {
            template<typename TFunction>
            // NOLINTNEXTLINE(misc-no-recursion)
            static constexpr auto call(usize desired, TFunction&& func) -> std::string {
                using hyperion::mpl::MetaValue;

                if(desired == TCurrent{}) {
                    return std::forward<TFunction>(func)(TCurrent{});
                }

                constexpr auto next_step = []() {
                    if constexpr(TStep{} / 2_value == 0_value && TCurrent{} > 2_value
                                 && TCurrent{} < TBound{} - 2_value)
                    {
                        return 1_value;
                    }
                    else {
                        return TStep{} / 2_value;
                    }
                }();

                // don't recurse endlessly if we're not getting anywhere
                if constexpr(TStep{} == 0_value && next_step == 0_value) {
                    HYPERION_UNREACHABLE();
                }
                else {

                    if(desired < TCurrent{}) {
                        return indexed_call<decltype(TCurrent{} - TStep{}),
                                            decltype(next_step),
                                            TBound>::call(desired, std::forward<TFunction>(func));
                    }

                    constexpr auto next = []() {
                        if constexpr((TCurrent{} + TStep{}) < TBound{}) {
                            return TCurrent{} + TStep{};
                        }
                        else {
                            return TBound{};
                        }
                    }();

                    return indexed_call<decltype(next), decltype(next_step), TBound>::call(
                        desired,
                        std::forward<TFunction>(func));
                }
            }
        };
    } // namespace

    [[nodiscard]] auto highlight(std::string_view str, bool for_backtrace) -> std::string {
        auto tokens = detail::parser::parse(str);

        if(tokens.empty()) {
            return std::string{str};
        }

        auto last_index = 0_usize;
        auto fmt_string = std::string{};
        fmt_string.reserve(str.size());
        flux::for_each(tokens, [&last_index, &fmt_string, &str](const auto& token) {
            if(token.begin != last_index) {
                fmt_string += str.substr(last_index, token.begin - last_index);
            }

            fmt_string += "{}";
            last_index = token.end;
        });

        const auto get_data = [&str](const auto& token) {
            return std::make_pair(str.substr(token.begin, token.end - token.begin),
                                  fmt::fg(get_color(token.kind)));
        };

        constexpr auto style = [](const auto& pair) {
            return fmt::styled(pair.first, pair.second);
        };

        if(tokens.size() == 1_usize && for_backtrace) {
            tokens.front().kind = tokens::Identifier{std::in_place_type<tokens::Function>};
            return fmt::format(fmt::runtime(fmt_string), style(get_data(tokens.front())));
        }

        using hyperion::mpl::MetaValue;

        const auto unpack = [&tokens, &get_data, &style, &fmt_string](MetaValue auto size) {
            auto seq = flux::adjacent_map<decltype(size)::value>(
                std::move(tokens),
                [&fmt_string, &get_data, &style](const auto&... _tokens) {
                    return fmt::format(fmt::runtime(fmt_string), style(get_data(_tokens))...);
                });

            assert(flux::size(seq) == 1_usize);
            return flux::read_at(seq, flux::first(seq));
        };

        return indexed_call<mpl::Value<100_usize>>::call(tokens.size(), unpack);
    }
} // namespace hyperion::assert::highlight
