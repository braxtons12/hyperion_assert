/// @file backtrace.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief format implementation for `boost::stacktrace::stacktrace`
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

#include <hyperion/assert/backtrace.h>
#include <hyperion/assert/detail/highlight.h>
#include <hyperion/platform/types.h>

#include <fmt/format.h>

#include <cassert>
#include <string>

namespace hyperion::assert {

    [[nodiscard]] auto format_backtrace(const Backtrace& backtrace) -> std::string {
        using hyperion::operator""_usize;
        std::string output;
        // rough estimate the number of chars we need
        // roughly 100 per frame entry
        output.reserve(100_usize * backtrace.size());
        constexpr auto format_frame_to = [](std::string& str,
                                            const boost::stacktrace::frame& frame,
                                            hyperion::usize index) {
            using hyperion::assert::detail::highlight::get_color;
            using hyperion::assert::detail::tokens::Token;
            using hyperion::assert::detail::tokens::Numeric;
            using hyperion::assert::detail::tokens::String;

            // number address [name in] [file:line]
            const auto name = frame.name();
            const auto file = frame.source_file();
            const auto line = frame.source_line();

            str += fmt::format(
                "{}# {}",
                fmt::styled(index, fmt::fg(get_color(Token::Kind{std::in_place_type<Numeric>}))),
                fmt::styled(frame.address(),
                            fmt::fg(get_color(Token::Kind{std::in_place_type<Numeric>}))));

            if(!name.empty()) {
                str += fmt::format(" {}", hyperion::assert::detail::highlight::highlight(name));
            }
            if(!file.empty()) {
                str += fmt::format(
                    " in {}",
                    fmt::styled(file, fmt::fg(get_color(Token::Kind{std::in_place_type<String>}))));
            }
            if(line != 0) {
                str += fmt::format(
                    "{}",
                    fmt::styled(line,
                                fmt::fg(get_color(Token::Kind{std::in_place_type<Numeric>}))));
            }
            str += '\n';
        };

        for(auto index = 0_usize; index < backtrace.size(); ++index) {
            if(!backtrace[index].empty()) {
                format_frame_to(output, backtrace[index], index);
            }
        }

        return output;
    }
} // namespace hyperion::assert
