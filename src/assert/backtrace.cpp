/// @file backtrace.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief format implementation for `boost::stacktrace::stacktrace`
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

#include <hyperion/assert/backtrace.h>
#include <hyperion/assert/detail/cstdio_support.h>
#include <hyperion/assert/highlight.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/platform/types.h>

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

#include <cstdio>
#include <string>

namespace hyperion::assert {
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    format_backtrace(const Backtrace& backtrace, backtrace::FormatStyle style) -> std::string {

        using hyperion::operator""_usize;
        std::string output;
        // rough estimate the number of chars we need
        // roughly 100 per frame entry
        output.reserve(100_usize * backtrace.size());
        constexpr auto format_frame_to = [](std::string& str,
                                            const boost::stacktrace::frame& frame,
                                            hyperion::usize index,
                                            bool styled) {
            using hyperion::assert::highlight::get_color;
            using hyperion::assert::highlight::highlight;
            using hyperion::assert::tokens::Token;
            using hyperion::assert::tokens::Numeric;
            using hyperion::assert::tokens::String;
            using hyperion::assert::tokens::Punctuation;

            // number address [name in] [file:line]
            const auto name = frame.name();
            const auto file = frame.source_file();
            const auto line = frame.source_line();

            // if we're printing to stderr or a tty, syntax highlight the backtrace
            if(styled) {
                const auto num_color = get_color(Token::Kind{std::in_place_type<Numeric>});
                const auto str_color = get_color(Token::Kind{std::in_place_type<String>});
                const auto punc_color = get_color(Token::Kind{std::in_place_type<Punctuation>});

                str += fmt::format(
                    "{:>2}{} {}{:0>16X}",
                    fmt::styled(index, fmt::fg(num_color)),
                    fmt::styled('#', fmt::fg(punc_color)),
                    fmt::styled("0x", fmt::fg(num_color)),
                    // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
                    fmt::styled(reinterpret_cast<usize>(frame.address()), fmt::fg(num_color)));

                if(!name.empty()) {
                    str += fmt::format(" {}", highlight(name, true));
                }
                if(!file.empty()) {
                    auto line_str = line == 0 ? std::string{} :
                                                fmt::format("{}{}",
                                                            fmt::styled(':', fmt::fg(punc_color)),
                                                            fmt::styled(line, fmt::fg(num_color)));
                    auto file_str = fmt::format("\n                       {}{}{}{}",
                                                fmt::styled("in [", fmt::fg(punc_color)),
                                                fmt::styled(file, fmt::fg(str_color)),
                                                line_str,
                                                fmt::styled(']', fmt::fg(punc_color)));
                    str += file_str;
                }
                if(name.empty() && file.empty()) {
                    str += fmt::format(" {}", fmt::styled("[no info]", fmt::fg(punc_color)));
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
                    str += fmt::format("\n                      {}", file_str);
                }
                if(name.empty() && file.empty()) {
                    str += " [no info]";
                }
            }
            str += '\n';
        };

        // if we're printing to stderr or a tty, syntax highlight the backtrace
        const auto format_styled = style == backtrace::FormatStyle::Styled;
        // MSVC gets mad when we use the _usize literal operator here for some reason
        for(auto index = static_cast<usize>(0UL); index < backtrace.size(); ++index) {
            if(!backtrace[index].empty()) {
                format_frame_to(output, backtrace[index], index, format_styled);
            }
        }

        return output;
    }
} // namespace hyperion::assert
