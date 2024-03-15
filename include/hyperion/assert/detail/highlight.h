/// @file highlight.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ syntax highlighting
/// @version 0.1
/// @date 2024-03-15
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

#ifndef HYPERION_ASSERT_DETAIL_HIGHLIGHT_H
#define HYPERION_ASSERT_DETAIL_HIGHLIGHT_H

#include <hyperion/assert/detail/tokens.h>

#include <fmt/color.h>

#include <string>
#include <string_view>
#include <vector>

namespace hyperion::assert::detail::highlight {

    struct Highlight {
        tokens::Token::Kind kind;
        fmt::detail::color_type color;
    };

    [[nodiscard]] auto highlight(std::string_view str) -> std::string;

    [[nodiscard]] auto set_color(const tokens::Token::Kind& kind, fmt::detail::color_type color);
    [[nodiscard]] auto set_color(tokens::Token::Kind&& kind, fmt::detail::color_type color);
    [[nodiscard]] auto set_color(const Highlight& _highlight);
    [[nodiscard]] auto set_color(Highlight&& _highlight);
    [[nodiscard]] auto set_colors(const std::vector<Highlight>& colors);
    [[nodiscard]] auto set_colors(std::vector<Highlight>&& colors);

    [[nodiscard]] auto
    get_color(const tokens::Token::Kind& kind) noexcept -> fmt::detail::color_type;

} // namespace hyperion::assert::detail::highlight

#endif
