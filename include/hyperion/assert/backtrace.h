/// @file backtrace.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief format implementation for `boost::stacktrace::stacktrace`
/// @version 0.1
/// @date 2024-03-08
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

#ifndef HYPERION_ASSERT_BACKTRACE_H
#define HYPERION_ASSERT_BACKTRACE_H

#include <hyperion/platform.h>
#include <hyperion/platform/def.h>

#if HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif // HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC

#include <boost/stacktrace.hpp>

#if HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC
#pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC

HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;

#include <fmt/format.h>

HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;

namespace hyperion::assert {
    /// @brief `Backtrace` provides a full backtrace of the current call stack at
    /// the point of its constructor invocation
    /// @headerfile hyperion/assert/backtrace.h
    using Backtrace = boost::stacktrace::stacktrace;
} // namespace hyperion::assert

/// @brief Specialization of `fmt::formatter` to provide `fmt` support for `hyperion::Backtrace`
/// @headerfile hyperion/assert/backtrace.h
template<>
struct fmt::formatter<hyperion::assert::Backtrace> {
    [[nodiscard]] constexpr auto
    // NOLINTNEXTLINE(*-convert-member-functions-to-static)
    parse(format_parse_context& context) noexcept {
        return context.begin();
    }

    template<typename TFormatContext>
    [[nodiscard]] constexpr auto
    format(const hyperion::assert::Backtrace& trace, TFormatContext& context) {
        return fmt::format_to(context.out(), "{}", to_string(trace));
    }
};

#endif // HYPERION_ASSERT_BACKTRACE_H
