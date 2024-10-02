/// @file highlight.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ syntax highlighting
/// @version 0.1
/// @date 2024-10-01
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
/// syntax highlighting, powered by libfmt styling functionality, as well functionality
/// to syntax highlight arbitrary strings containing C++ code in a libfmt-compatible way.
///
/// # Example
/// @code{.cpp}
/// constexpr auto keyword_color = hyperion::assert::highlight::Color(0xC67ADA_u32);
/// constexpr auto keyword_highlight = hyperion::assert::highlight::Highlight {
///     .kind = hyperion::assert::tokens::Keyword{},
///     .color = keyword_color,
/// };
/// hyperion::assert::highlight::register_highlight(keyword_highlight);
/// highlighted = hyperion::assert::highlight::highlight("auto my_cpp_code = 42;");
/// @endcode
/// @headerfile hyperion/assert/highlight.h
/// @}

namespace hyperion::assert::highlight {

    /// @brief `RgbColor` represents a red-green-blue color, typically defined as
    /// a six-digit hexadecimal number (e.g. `0x61AFEF`)
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    struct RgbColor {
        /// @brief Constructs an `RgbColor` from a six-digit hex number
        /// @param hex the six-digit hex number representing
        /// the red, green, and blue color values
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        explicit(false) constexpr RgbColor(const u32 hex) noexcept
            : red{static_cast<u8>((hex >> 16_u32) & 0xFF_u32)},
              green{static_cast<u8>((hex >> 8_u32) & 0xFF_u32)},
              blue{static_cast<u8>(hex & 0xFF_u32)} {
        }
        /// @brief Constructs an `RgbColor` from a libfmt `fmt::color` type
        /// @param color the color
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        explicit(false) constexpr RgbColor(const fmt::color color) noexcept
            : RgbColor{static_cast<u32>(color)} {
        }
        /// @brief Constructs an `RgbColor` from individual red, green, and blue color values
        /// @param _red the red value
        /// @param _green the green value
        /// @param _blue the blue value
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        constexpr RgbColor(const u8 _red, const u8 _green, const u8 _blue) noexcept
            : red{_red}, green{_green}, blue{_blue} {
        }

        /// @brief Implicit conversion operator for `RgbColor` to its numeric representation
        /// @return the numeric representation of this `RgbColor`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        // NOLINTNEXTLINE(*-explicit-constructor)
        [[nodiscard]] explicit(false) constexpr operator u32() const noexcept {
            return static_cast<u32>((red << 16_u32) | (green << 8_u32)) | (blue);
        }

        /// @brief Implicit conversion operator for `RgbColor` to the corresponding libfmt
        /// `fmt::detail::color_type` representation
        /// @return the libfmt representation of this `RgbColor`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        // NOLINTNEXTLINE(*-explicit-constructor)
        [[nodiscard]] explicit(false) constexpr operator fmt::detail::color_type() const noexcept {
            return fmt::detail::color_type{static_cast<u32>(*this)};
        }

        /// @brief three-way comparison operator for `RgbColor`s
        /// @return how this `RgbColor` orders relative to `other`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        constexpr auto operator<=>(const RgbColor& other) const noexcept = default;
        /// @brief equality comparison operator for `RgbColor`s
        /// @return whether this `RgbColor` is equal to `other`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        constexpr auto operator==(const RgbColor& other) const noexcept -> bool = default;

        u8 red = 0xFF_u8;
        u8 green = 0xFF_u8;
        u8 blue = 0xFF_u8;
    };

    /// @brief `Color` represents either a red-green-blue color value (i.e. an `RgbColor`),
    /// typically defined as a six-digit hexadecimal number (e.g. `0x61AFEF`), OR a libfmt
    /// terminal emulator color value (i.e. `fmt::terminal_color`)
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    class Color {
      public:
        /// @brief Default constructs a `Color` as pure white
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        constexpr Color() noexcept = default;
        /// @brief Constructs a `Color` as the given `RgbColor` value
        /// @param color The `RgbColor` value to construct this `Color` as
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        explicit(false) constexpr Color(const RgbColor color) noexcept : m_value{color} {
        }
        /// @brief Constructs a `Color` as the given libfmt `fmt::terminal_color` value
        /// @param color The `fmt::terminal_color` value to construct this `Color` as
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        explicit(false) constexpr Color(const fmt::terminal_color color) noexcept
            : m_is_term{true}, m_value{color} {
        }
        /// @brief Constructs a `Color` as the given red-blue-green color
        /// @param rgb_hex The red-blue-green color to construct this `Color` as,
        /// represented as a 6-digit hexadecimal number
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        explicit(false) constexpr Color(const u32 rgb_hex) noexcept : m_value{RgbColor{rgb_hex}} {
        }

        /// @brief Returns whether this `Color` represents a terminal emulator color
        /// @return whether this `Color` represents a libfmt terminal emulator color,
        /// i.e. a `fmt::terminal_color`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        [[nodiscard]] constexpr auto is_term_color() const noexcept {
            return m_is_term;
        }

        /// @brief Returns the libfmt terminal color this `Color` represents,
        /// if it represents one
        /// @return the `fmt::terminal_color` this `Color` represents, or `std::nullopt`
        /// if it doesn't represent an `fmt::terminal_color`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        [[nodiscard]] constexpr auto
        term_color() const noexcept -> std::optional<fmt::terminal_color> {
            if(m_is_term) {
                return m_value.term_color;
            }

            return {};
        }

        /// @brief Returns whether this `Color` represents an `RgbColor`
        /// @return whether this `Color` represents an `RgbColor`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        [[nodiscard]] constexpr auto is_rgb_color() const noexcept {
            return m_is_term;
        }

        /// @brief Returns the `RgbColor` this `Color` represents, if it represents one
        /// @return the `RgbColor` this `Color` represents, or `std::nullopt`
        /// if it doesn't represent an `RgbColor`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        [[nodiscard]] constexpr auto rgb_color() const noexcept -> std::optional<RgbColor> {
            if(m_is_term) {
                return {};
            }

            return m_value.rgb_color;
        }

        /// @brief implicit conversion operator for `Color` to the corresponding
        /// libfmt `fmt::detail::color_type` representation
        /// @return the libfmt color type representation of this `Color`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        // NOLINTNEXTLINE(*-explicit-constructor)
        [[nodiscard]] explicit(false) constexpr operator fmt::detail::color_type() const noexcept {
            if(m_is_term) {
                return {m_value.term_color};
            }

            return m_value.rgb_color;
        }

        /// @brief three-way comparison operator for `Color`s
        ///
        /// When `this` and `other` represent different types of colors
        /// (i.e. `this->is_term_color() != other.is_term_color()`),
        /// `this` is defined to compare as less than `other` when both of
        /// `this->is_term_color()` and `other.is_rgb_color()` are true,
        /// and vice versa
        ///
        /// @return how this `Color` orders relative to `other`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        constexpr auto operator<=>(const Color& other) const noexcept -> std::weak_ordering {
            if(m_is_term && other.m_is_term) {
                return m_value.term_color <=> other.m_value.term_color;
            }

            if(m_is_term) {
                return std::weak_ordering::less;
            }

            if(other.m_is_term) {
                return std::weak_ordering::greater;
            }

            return m_value.rgb_color <=> other.m_value.rgb_color;
        }

        /// @brief equality comparison operator for `Color`s
        ///
        /// When `this` and `other` represent the same type of color
        /// (i.e. `this->is_term_color() == other.is_term_color()`),
        /// returns the result of directly comparing the color values
        /// they represent. Otherwise, returns `false`.
        ///
        /// @return whether this `Color` is equal to `other`
        /// @headerfile hyperion/assert/highlight.h
        /// @ingroup highlight
        constexpr auto operator==(const Color& other) const noexcept -> bool {
            if(m_is_term && other.m_is_term) {
                return m_value.term_color == other.m_value.term_color;
            }

            if(!m_is_term && !other.m_is_term) {
                return m_value.rgb_color == other.m_value.rgb_color;
            }

            return false;
        }

      private:
        bool m_is_term = false;
        union storage {
            explicit(false) constexpr storage(const RgbColor color) noexcept : rgb_color{color} {
            }
            explicit(false) constexpr storage(const fmt::terminal_color color) noexcept
                : term_color{color} {
            }

            RgbColor rgb_color = 0xFFFFFF_u32;
            fmt::terminal_color term_color;
        };
        storage m_value = RgbColor{0xFFFFFF_u32};
    };

    /// @brief `Highlight` represents the syntax highlighting configuration
    /// for a specific token kind
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    struct Highlight {
        tokens::Kind kind;
        Color color;
    };

    /// @brief Highlights the given string in a way compatible with libfmt's
    /// styling functionality, according to the currently configured highlighting style,
    /// and returns the result as a new `std::string`
    /// @param str the string to do syntax highlighting on
    /// @param first_token_is_function Whether the first token in the given string
    /// is known to represent a function (to prevent it from being highlighted as a variable)
    /// @return The syntax highlighted string
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    highlight(std::string_view str, bool first_token_is_function = false) -> std::string;

    /// @brief Registers the given syntax highlighting setting in `hyperion::assert`'s
    /// syntax highlighting configuration
    /// @param _highlight The syntax highlighting setting to register
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    register_highlight(const Highlight& _highlight);
    /// @brief Registers the given syntax highlighting setting in `hyperion::assert`'s
    /// syntax highlighting configuration
    /// @param _highlight The syntax highlighting setting to register
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    register_highlight(Highlight&& _highlight);
    /// @brief Registers the given syntax highlighting settings in `hyperion::assert`'s
    /// syntax highlighting configuration
    /// @param highlights The syntax highlighting settings to register
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    register_highlights(const std::vector<Highlight>& highlights);
    /// @brief Registers the given syntax highlighting settings in `hyperion::assert`'s
    /// syntax highlighting configuration
    /// @param highlights The syntax highlighting settings to register
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    register_highlights(std::vector<Highlight>&& highlights);

    /// @brief Returns the syntax highlighting color currently registered in
    /// `hyperion::assert`'s syntax highlighting configuration
    /// @param kind The token kind to get the registered highlight color for
    /// @return The registered color syntax highlight color for `kind`
    /// @headerfile hyperion/assert/highlight.h
    /// @ingroup highlight
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    get_color(const tokens::Kind& kind) noexcept -> Color;

} // namespace hyperion::assert::highlight

#endif
