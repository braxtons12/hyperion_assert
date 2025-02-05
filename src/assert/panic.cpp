/// @file panic.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Provides runtime panic support. A runtime panic is an error reporting
/// mechanism used to fail gracefully and report the associated error when an
/// irrecoverable error has occurred.
/// @version 0.1.1
/// @date 2025-01-18
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
#include <hyperion/assert/def.h>
#include <hyperion/assert/detail/parser.h>
#include <hyperion/assert/highlight.h>
#include <hyperion/assert/panic.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/source_location.h>

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

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <string_view>

namespace hyperion::assert::detail {

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    format_source_location(const hyperion::source_location& location) -> std::string {
        using hyperion::assert::detail::parser::Token;
        using hyperion::assert::highlight::get_color;
        using hyperion::assert::highlight::highlight;
        using hyperion::assert::tokens::Numeric;
        using hyperion::assert::tokens::Punctuation;
        using hyperion::assert::tokens::String;

        const auto str_color = get_color(tokens::Kind{std::in_place_type<String>});
        const auto num_color = get_color(tokens::Kind{std::in_place_type<Numeric>});
        const auto punc_color = get_color(tokens::Kind{std::in_place_type<Punctuation>});

        return fmt::format("{}{}{}{}{}{}{} {}",
                           fmt::styled('[', fmt::fg(punc_color)),
                           fmt::styled(location.file_name(), fmt::fg(str_color)),
                           fmt::styled('|', fmt::fg(punc_color)),
                           fmt::styled(location.line(), fmt::fg(num_color)),
                           fmt::styled(':', fmt::fg(punc_color)),
                           fmt::styled(location.column(), fmt::fg(num_color)),
                           fmt::styled("]:", fmt::fg(punc_color)),
                           highlight(location.function_name(), true));
    }

} // namespace hyperion::assert::detail

namespace hyperion::assert::panic {

    namespace detail {
#if HYPERION_PLATFORM_COMPILER_IS_GCC && __GNUC__ >= 14
    #define NORETURN
#else
    #define NORETURN [[noreturn]]
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC && __GNUC__ >= 14

        namespace {
            HYPERION_IGNORE_INVALID_NORETURN_WARNING_START;

            NORETURN HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
            default_handler(const std::string_view panic_message,
                            const hyperion::source_location& location,
                            const Backtrace& backtrace) noexcept -> void {
                using hyperion::assert::detail::parser::Token;
                using hyperion::assert::highlight::get_color;
                using hyperion::assert::tokens::Error;

                if(panic_message.empty()) {
                    fmt::print(stderr,
                               "{0} {1}:\n\n"
                               "{2}\n{3}\n",
                               fmt::styled("Panic occurred at",
                                           fmt::emphasis::bold
                                               | fmt::fg(get_color(
                                                   tokens::Kind{std::in_place_type<Error>}))),
                               assert::detail::format_source_location(location),
                               fmt::styled("Backtrace:", fmt::emphasis::bold),
                               hyperion::assert::format_backtrace(backtrace,
                                                                  backtrace::FormatStyle::Styled));
                }
                else {
                    fmt::print(stderr,
                               "{0} {1}:\n\n"
                               "{2}\n\n"
                               "{3}\n{4}\n",
                               fmt::styled("Panic occurred at",
                                           fmt::emphasis::bold
                                               | fmt::fg(get_color(
                                                   tokens::Kind{std::in_place_type<Error>}))),
                               assert::detail::format_source_location(location),
                               panic_message,
                               fmt::styled("Backtrace:", fmt::emphasis::bold),
                               hyperion::assert::format_backtrace(backtrace,
                                                                  backtrace::FormatStyle::Styled));
                }
#if HYPERION_PLATFORM_MODE_IS_DEBUG
                HYPERION_ASSERT_DEBUG_BREAK();
#else
                std::abort();
#endif // HYPERION_PLATFORM_MODE_IS_DEBUG
            }

            HYPERION_IGNORE_INVALID_NORETURN_WARNING_STOP;

            std::atomic<panic::Handler> s_handler // NOLINT(*-avoid-non-const-global-variables)
                = &default_handler;
        } // namespace

#undef NORETURN

    } // namespace detail

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
    set_handler(Handler handler) noexcept -> void {
        detail::s_handler.store(handler, std::memory_order_release);
    }

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    get_handler() noexcept -> Handler {
        return detail::s_handler.load(std::memory_order_acquire);
    }

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    default_handler() noexcept -> Handler {
        return &detail::default_handler;
    }

    namespace detail {
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location, const Backtrace& backtrace) noexcept
            -> void {
            panic::get_handler()("", location, backtrace);
        }

        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location,
                const Backtrace& backtrace,
                std::string_view message) noexcept -> void {
            panic::get_handler()(message, location, backtrace);
        }
    } // namespace detail
} // namespace hyperion::assert::panic
