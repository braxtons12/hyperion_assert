/// @file highlight.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ syntax highlighting
/// @version 0.1
/// @date 2024-09-20
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


#include <string>
#include <string_view>
#include <vector>

namespace hyperion::assert::highlight {

    struct Highlight {
        tokens::Token::Kind kind;
        fmt::detail::color_type color;
    };

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    highlight(std::string_view str, bool for_backtrace = false) -> std::string;

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(const tokens::Token::Kind& kind, fmt::detail::color_type color);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(tokens::Token::Kind&& kind, fmt::detail::color_type color);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(const Highlight& _highlight);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_color(Highlight&& _highlight);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_colors(const std::vector<Highlight>& colors);
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    set_colors(std::vector<Highlight>&& colors);

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    get_color(const tokens::Token::Kind& kind) noexcept -> fmt::detail::color_type;

} // namespace hyperion::assert::highlight

#endif
