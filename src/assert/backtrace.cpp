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
#include <hyperion/assert/detail/cstdio_support.h>
#include <hyperion/assert/detail/highlight.h>
#include <hyperion/platform/types.h>

#include <fmt/format.h>

#include <cstdio>
#include <string>

namespace hyperion::assert {
    [[nodiscard]] auto format_backtrace(const Backtrace& backtrace, const int desc) -> std::string {

        using hyperion::operator""_usize;
        std::string output;
        // rough estimate the number of chars we need
        // roughly 100 per frame entry
        output.reserve(100_usize * backtrace.size());
        constexpr auto format_frame_to = [](std::string& str,
                                            const boost::stacktrace::frame& frame,
                                            hyperion::usize index,
                                            const int _desc) {
            using hyperion::assert::detail::highlight::get_color;
            using hyperion::assert::detail::tokens::Token;
            using hyperion::assert::detail::tokens::Numeric;
            using hyperion::assert::detail::tokens::String;

            // number address [name in] [file:line]
            const auto name = frame.name();
            const auto file = frame.source_file();
            const auto line = frame.source_line();

            // if we're printing to stderr or a tty, syntax highlight the backtrace
            if(detail::cstdio_support::isatty(_desc)
               || _desc == detail::cstdio_support::fileno(stderr))
            {
                str += fmt::format(
                    "{:>2}# {}{:0>16X}",
                    fmt::styled(index,
                                fmt::fg(get_color(Token::Kind{std::in_place_type<Numeric>}))),
                    fmt::styled("0x", fmt::fg(get_color(Token::Kind{std::in_place_type<Numeric>}))),
                    // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
                    fmt::styled(reinterpret_cast<usize>(frame.address()),
                                fmt::fg(get_color(Token::Kind{std::in_place_type<Numeric>}))));

                if(!name.empty()) {
                    str += fmt::format(" {}", hyperion::assert::detail::highlight::highlight(name));
                }
                if(!file.empty()) {
                    auto line_str
                        = line == 0 ? std::string{} :
                                      fmt::format(":{}",
                                                  fmt::styled(line,
                                                              fmt::fg(get_color(Token::Kind{
                                                                  std::in_place_type<Numeric>}))));
                    auto file_str = fmt::format(
                        " in [{}{}]",
                        fmt::styled(file,
                                    fmt::fg(get_color(Token::Kind{std::in_place_type<String>}))),
                        line_str);
                    // for some reason, fmt isn't padding here when we try to do so w/ the format
                    // specifier, so we do it manually
                    str += fmt::format("\n                      {}", file_str);
                }
            }
            else {
                str += fmt::format("{:>2}# 0x{:0>16X}",
                                   index,
                                   // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
                                   reinterpret_cast<usize>(frame.address()));

                if(!name.empty()) {
                    str += fmt::format(" {}", name);
                }
                if(!file.empty()) {
                    auto line_str = line == 0 ? std::string{} : fmt::format(":{}", line);
                    auto file_str = fmt::format(" in [{}{}]", file, line_str);
                    // for some reason, fmt isn't padding here when we try to do so w/ the format
                    // specifier, so we do it manually
                    str += fmt::format("\n                      {}", file_str);
                }
            }
            str += '\n';
        };

        for(auto index = 0_usize; index < backtrace.size(); ++index) {
            if(!backtrace[index].empty()) {
                format_frame_to(output, backtrace[index], index, desc);
            }
        }

        return output;
    }
} // namespace hyperion::assert
