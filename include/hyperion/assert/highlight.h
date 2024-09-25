/// @file highlight.h
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

#ifndef HYPERION_ASSERT_HIGHLIGHT_H
#define HYPERION_ASSERT_HIGHLIGHT_H

#include <hyperion/assert/detail/def.h>
#include <hyperion/assert/tokens.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/color.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <optional>
#include <string>
#include <string_view>
#include <vector>

/// @ingroup assert
/// @{
/// @defgroup highlight Syntax highlighting and highlight configuration
/// This module provides an API for setting and querying `hyperion::assert`'s
/// syntax highlighting, as well functionality to syntax highlight arbitrary strings
/// containing C++ code.
///
/// # Example
/// @code{.cpp}
/// // calculate the backtrace up to this point
/// auto backtrace = hyperion::Backtrace{};
/// // no syntax highlighting
/// fmt::print(stdout, "{}", backtrace);
/// // with syntax highlighting
/// fmt::print(stdout,
///            "{}",
///            hyperion::format_backtrace(backtrace,
///                                       hyperion::backtrace::FormatStyle::Styled));
/// @endcode
/// @headerfile hyperion/mpl/list.h
/// @}

namespace hyperion::assert::highlight {

    struct RgbColor {
        explicit(false) constexpr RgbColor(const u32 hex) noexcept
            : red{static_cast<u8>((hex >> 16_u32) & 0xFF_u32)},
              green{static_cast<u8>((hex >> 8_u32) & 0xFF_u32)},
              blue{static_cast<u8>(hex & 0xFF_u32)} {
        }
        explicit(false) constexpr RgbColor(fmt::color color) noexcept
            : RgbColor{static_cast<u32>(color)} {
        }
        constexpr RgbColor(const u8 _red, const u8 _green, const u8 _blue) noexcept
            : red{_red}, green{_green}, blue{_blue} {
        }

        // NOLINTNEXTLINE(*-explicit-constructor)
        [[nodiscard]] explicit(false) constexpr operator u32() const noexcept {
            return static_cast<u32>((red << 16_u32) | (green << 8_u32)) | (blue);
        }

        [[nodiscard]] explicit(false) constexpr operator fmt::detail::color_type() const noexcept {
            return fmt::detail::color_type{static_cast<u32>(*this)};
        }

        u8 red = 0x61_u8;
        u8 green = 0xAF_u8;
        u8 blue = 0xEF_u8;
    };

    class Color {
      public:
        constexpr Color() noexcept = default;
        explicit(false) constexpr Color(RgbColor color) noexcept : value{color} {
        }
        explicit(false) constexpr Color(fmt::terminal_color color) noexcept
            : is_term{true}, value{color} {
        }

        [[nodiscard]] constexpr auto is_term_color() const noexcept {
            return is_term;
        }

        [[nodiscard]] constexpr auto rgb_color() const noexcept -> std::optional<RgbColor> {
            if(is_term) {
                return {};
            }

            return value.rgb_color;
        }

        [[nodiscard]] constexpr auto
        term_color() const noexcept -> std::optional<fmt::terminal_color> {
            if(is_term) {
                return value.term_color;
            }

            return {};
        }

        [[nodiscard]] explicit(false) constexpr operator fmt::detail::color_type() const noexcept {
            if(is_term) {
                return {value.term_color};
            }

            return value.rgb_color;
        }

      private:
        bool is_term = false;
        union storage {
            explicit(false) constexpr storage(RgbColor color) noexcept : rgb_color{color} {
            }
            explicit(false) constexpr storage(fmt::terminal_color color) noexcept
                : term_color{color} {
            }

            RgbColor rgb_color = 0x61AFEF_u32;
            fmt::terminal_color term_color;
        };
        storage value = RgbColor{0x61AFEF_u32};
    };

    struct Highlight {
        tokens::Token::Kind kind;
        Color color;
    };

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    highlight(std::string_view str, bool for_backtrace = false) -> std::string;

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(const tokens::Token::Kind& kind, Color color);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(tokens::Token::Kind&& kind, Color color);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(const Highlight& _highlight);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(Highlight&& _highlight);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_colors(const std::vector<Highlight>& colors);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_colors(std::vector<Highlight>&& colors);

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    get_color(const tokens::Token::Kind& kind) noexcept -> Color;

} // namespace hyperion::assert::highlight

#endif
