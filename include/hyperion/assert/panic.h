/// @file panic.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Provides runtime panic support. A runtime panic is an error reporting
/// mechanism used to fail gracefully and report the associated error when an
/// irrecoverable error has occurred.
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

#ifndef HYPERION_ASSERT_PANIC_H
#define HYPERION_ASSERT_PANIC_H

#include <hyperion/assert/backtrace.h>
#include <hyperion/assert/detail/def.h>
#include <hyperion/platform.h>
#include <hyperion/platform/def.h>
#include <hyperion/source_location.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/core.h>
#include <fmt/format.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC


#include <concepts>
#include <string_view>
#include <type_traits>
#include <utility>

namespace hyperion::assert::panic {

    using Handler = void (*)(const std::string_view panic_message,
                             const hyperion::source_location& location,
                             const Backtrace& backtrace) noexcept;

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
    set_handler(Handler handler) noexcept -> void;

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    get_handler() noexcept -> Handler;

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    default_handler() noexcept -> Handler;

    namespace detail {
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location, const Backtrace& backtrace) noexcept -> void;

        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location,
                const Backtrace& backtrace,
                std::string_view message) noexcept -> void;

        HYPERION_IGNORE_UNUSED_TEMPLATES_WARNING_START;

        template<typename TArg>
            requires fmt::is_formattable<TArg>::value
                     || std::same_as<std::string_view, std::remove_cvref_t<TArg>>
                     || std::convertible_to<TArg, std::string_view>
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location,
                const Backtrace& backtrace,
                TArg&& arg) noexcept(std::same_as<std::string_view, std::remove_cvref_t<TArg>>
                                     || std::is_nothrow_convertible_v<TArg, std::string_view>) -> void {
            if constexpr(std::same_as<std::string_view, std::remove_cvref_t<TArg>>
                         || std::convertible_to<TArg, std::string_view>)
            {
                panic::get_handler()(std::string_view{std::forward<TArg>(arg)}, location, backtrace);
            }
            else {
                const auto str = fmt::format("{}", std::forward<TArg>(arg));
                panic::get_handler()(str, location, backtrace);
            }
        }

        template<typename... TArgs>
            requires(fmt::is_formattable<TArgs>::value && ...)
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location,
                const Backtrace& backtrace,
                fmt::format_string<TArgs...> format_string,
                TArgs&&... args) -> void {
            const auto str = fmt::format(format_string, std::forward<TArgs>(args)...);
            panic::get_handler()(str, location, backtrace);
        }

        HYPERION_IGNORE_UNUSED_TEMPLATES_WARNING_STOP;
    }

} // namespace hyperion::assert::panic

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

#define HYPERION_PANIC(...) /** NOLINT(*-macro-usage) **/ \
    hyperion::assert::panic::detail::execute(                     \
        hyperion::source_location::current(),             \
        hyperion::assert::Backtrace {} __VA_OPT__(, __VA_ARGS__))

#if HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES
    #define PANIC(...) /** NOLINT(*-macro-usage) **/ HYPERION_PANIC(__VA_ARGS__)
#endif // HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

#endif // HYPERION_ASSERT_PANIC_H
