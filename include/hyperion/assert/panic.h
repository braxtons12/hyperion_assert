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

/// @ingroup assert
/// @{
/// @defgroup panic Runtime Panics
/// This module provides an API for triggering and handling runtime "panics".
/// "Panics" are a runtime error triggered when an irrecoverable error has occurred
/// or been detected, such as a programming bug or fatal environmental problem.
///
/// By default, panics will print their associated panic message,
/// along with syntax highlighted source location info and a backtrace, to `stderr`,
/// then trigger a breakpoint in debug builds, or a forced termination in release builds.
/// this behavior can be customized be registering a custom handler with
/// `hyperion::assert::panic::set_handler`.
///
/// # Example
/// @code{.cpp}
/// HYPERION_PANIC("A fatal error has occurred: {}", some_context_variable);
/// @endcode
/// @headerfile hyperion/assert/panic.h
/// @}

namespace hyperion::assert::panic {

    /// @brief `Handler` is the function pointer type for a valid hyperion panic handler.
    /// a `Handler` must return `void` and take the following arguments, in their presented order:
    ///
    /// - `std::string_view` `panic_message`: The pre-formatted panic message used in the
    /// invocation,
    /// - `hyperion::source_location` `location`: The source location information,
    /// - `hyperion::assert::Backtrace` `backtrace`: The backtrace of the code up to and including
    ///   the invocation of the panic
    /// @headerfile hyperion/assert/panic.h
    /// @ingroup panic
    using Handler = void (*)(const std::string_view panic_message,
                             const hyperion::source_location& location,
                             const Backtrace& backtrace) noexcept;

    /// @brief Registers the given panic handler, `handler` as the active panic handler in the
    /// program
    /// @param handler The panic handler to register
    /// @headerfile hyperion/assert/panic.h
    /// @ingroup panic
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
    set_handler(Handler handler) noexcept -> void;

    /// @brief Returns the currently active panic handler
    /// @return The active panic handler
    /// @headerfile hyperion/assert/panic.h
    /// @ingroup panic
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    get_handler() noexcept -> Handler;

    /// @brief Returns the default panic handler used by `hyperion::assert`
    /// @return The default panic handler
    /// @headerfile hyperion/assert/panic.h
    /// @ingroup panic
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    default_handler() noexcept -> Handler;

    namespace detail {
        /// @brief Executes a panic with the given context parameters
        /// @param location The source location the panic is being executed from
        /// @param backtrace The backtrace up to the point of panic
        /// @headerfile hyperion/assert/panic.h
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location, const Backtrace& backtrace) noexcept
            -> void;

        /// @brief Executes a panic with the given context parameters
        /// @param location The source location the panic is being executed from
        /// @param backtrace The backtrace up to the point of panic
        /// @param message The message the panic is triggered with
        /// @headerfile hyperion/assert/panic.h
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location,
                const Backtrace& backtrace,
                std::string_view message) noexcept -> void;

        HYPERION_IGNORE_UNUSED_TEMPLATES_WARNING_START;

        /// @brief Executes a panic with the given context parameters
        ///
        /// # Requirements
        /// - `TMessage` must be convertible to `std::string_view`
        ///
        /// # Exception Safety
        /// - May throw any exception throwable by `TMessage`'s conversion operator
        /// to `std::string_view`
        ///
        /// @param location The source location the panic is being executed from
        /// @param backtrace The backtrace up to the point of panic
        /// @param message The message the panic is triggered with
        /// @tparam TMessage The type of `message`
        /// @headerfile hyperion/assert/panic.h
        template<typename TMessage>
            requires std::convertible_to<TMessage, std::string_view>
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto execute(
            const hyperion::source_location& location,
            const Backtrace& backtrace,
            TMessage&& message) noexcept(std::is_nothrow_convertible_v<TMessage, std::string_view>)
            -> void {
            execute(location, backtrace, std::string_view{std::forward<TMessage>(message)});
        }

        // clang-format off

        /// @brief Executes a panic with the given context parameters
        ///
        /// # Exception Safety
        /// - May throw any exception throwable by formatting the given format arguments into
        /// a fully formatted string
        ///
        /// @param location The source location the panic is being executed from
        /// @param backtrace The backtrace up to the point of panic
        /// @param format_string The libfmt format string specifying how to build the panic message
        /// @param args The arguments to format into the panic message
        /// @tparam TArgs The types of the arguments to be formatted into the panic message
        /// @headerfile hyperion/assert/panic.h
        template<typename... TArgs>
            requires(fmt::is_formattable<TArgs>::value && ...)
        HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        execute(const hyperion::source_location& location,
                const Backtrace& backtrace,
                fmt::format_string<TArgs...> format_string,
                TArgs&&... args)
            noexcept(noexcept(fmt::format(format_string, std::forward<TArgs>(args)...)))
            -> void
        {
            const auto str = fmt::format(format_string, std::forward<TArgs>(args)...);
            execute(location, backtrace, std::string_view{str});
        }

        // clang-format on
        HYPERION_IGNORE_UNUSED_TEMPLATES_WARNING_STOP;
    } // namespace detail

} // namespace hyperion::assert::panic

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

/// @brief Triggers a runtime "panic"
/// "Panics" are a runtime error triggered when an irrecoverable error has occurred
/// or been detected, such as a programming bug or fatal environmental problem.
///
/// By default, panics will print their associated panic message,
/// along with syntax highlighted source location info and a backtrace, to `stderr`,
/// then trigger a breakpoint in debug builds, or a forced termination in release builds.
/// this behavior can be customized be registering a custom handler with
/// `hyperion::assert::panic::set_handler`.
///
/// # Example
/// @code{.cpp}
/// HYPERION_PANIC("A fatal error has occurred: {}", some_context_variable);
/// @endcode
/// @headerfile hyperion/assert/panic.h
/// @ingroup panic
#define HYPERION_PANIC(...) /** NOLINT(*-macro-usage) **/ \
    hyperion::assert::panic::detail::execute(             \
        hyperion::source_location::current(),             \
        hyperion::assert::Backtrace {} __VA_OPT__(, __VA_ARGS__))

#if HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES
    /// @brief Triggers a runtime "panic"
    /// "Panics" are a runtime error triggered when an irrecoverable error has occurred
    /// or been detected, such as a programming bug or fatal environmental problem.
    ///
    /// By default, panics will print their associated panic message,
    /// along with syntax highlighted source location info and a backtrace, to `stderr`,
    /// then trigger a breakpoint in debug builds, or a forced termination in release builds.
    /// this behavior can be customized be registering a custom handler with
    /// `hyperion::assert::panic::set_handler`.
    ///
    /// # Example
    /// @code{.cpp}
    /// HYPERION_PANIC("A fatal error has occurred: {}", some_context_variable);
    /// @endcode
    /// @headerfile hyperion/assert/panic.h
    /// @ingroup panic
    #define PANIC(...) /** NOLINT(*-macro-usage) **/ HYPERION_PANIC(__VA_ARGS__)
#endif // HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

#endif // HYPERION_ASSERT_PANIC_H
