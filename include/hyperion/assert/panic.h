/// @file panic.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Provides runtime panic support. A runtime panic is an error reporting
/// mechanism used to fail gracefully and report the associated error when an
/// irrecoverable error has occurred.
/// @version 0.1
/// @date 2024-03-13
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
#include <hyperion/platform.h>
#include <hyperion/platform/def.h>
#include <hyperion/source_location.h>

#include <fmt/format.h>

#include <concepts>
#include <string_view>
#include <utility>

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

/// @def HYPERION_ATTRIBUTE_COLD
/// @brief Marks a function as as a "cold" codepath. I.E., it should not be inlined,
/// primed in the instruction cache, etc.
/// @headerfile hyperion/assert/panic.h

#if HYPERION_PLATFORM_COMPILER_IS_CLANG or HYPERION_PLATFORM_COMPILER_IS_GCC
    #define HYPERION_ATTRIBUTE_COLD [[gnu::cold]]
#else
    #define HYPERION_ATTRIBUTE_COLD
#endif

/// @def HYPERION_ASSERT_DEBUG_BREAK
/// @brief Triggers a debugging break point
/// @headerfile hyperion/assert/panic.h

#if HYPERION_PLATFORM_IS_WINDOWS
    #include <intrin.h>
    #define HYPERION_ASSERT_DEBUG_BREAK() __debugbreak()
#else
    #if __has_builtin(__builtin_debugtrap)
        #define HYPERION_ASSERT_DEBUG_BREAK() /** NOLINT(*-macro-usage) **/ __builtin_debugtrap()
    #else

        #if HYPERION_PLATFORM_IS_ARCHITECTURE(HYPERION_PLATFORM_X86) \
            || HYPERION_PLATFORM_IS_ARCHITECTURE(HYPERION_PLATFORM_X86_64)

            #define HYPERION_ASSERT_DEBUG_BREAK() /** NOLINT(*-macro-usage) **/ \
                __asm__ volatile("int $0x03")

        #elif HYPERION_PLATFORM_IS_ARCHITECTURE(HYPERION_PLATFORM_ARM_V8)

            #define HYPERION_ASSERT_DEBUG_BREAK() /** NOLINT(*-macro-usage) **/ \
                __asm__ volatile(".inst 0xd4200000")

        #else // give up, use signals

            #include <signal.h>
            #define HYPERION_ASSERT_DEBUG_BREAK() /** NOLINT(*-macro-usage) **/ raise(SIGTRAP);

        #endif // HYPERION_PLATFORM_IS_ARCHITECTURE(HYPERION_PLATFORM_X86)
               // || HYPERION_PLATFORM_IS_ARCHITECTURE(HYPERION_PLATFORM_X86_64)

    #endif // __has_builtin(__builtin_debugtrap)
#endif

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

namespace hyperion::assert::panic {
    using Handler = void (*)(const std::string_view panic_message,
                             const hyperion::source_location& location,
                             const Backtrace& backtrace) noexcept;

    HYPERION_ATTRIBUTE_COLD auto set_handler(Handler handler) noexcept -> void;

    HYPERION_ATTRIBUTE_COLD [[nodiscard]] auto get_handler() noexcept -> Handler;

    HYPERION_ATTRIBUTE_COLD [[nodiscard]] auto default_handler() noexcept -> Handler;

    HYPERION_ATTRIBUTE_COLD auto
    execute(const hyperion::source_location& location, const Backtrace& backtrace) noexcept -> void;

    HYPERION_ATTRIBUTE_COLD auto execute(const hyperion::source_location& location,
                                         const Backtrace& backtrace,
                                         std::string_view message) noexcept -> void;

    HYPERION_IGNORE_UNUSED_TEMPLATES_WARNING_START;

    template<typename TArg>
    HYPERION_ATTRIBUTE_COLD static auto
    execute(const hyperion::source_location& location,
            const Backtrace& backtrace,
            TArg&& arg) noexcept(std::same_as<std::string_view, std::remove_cvref_t<TArg>>
                                 || requires { std::string_view{std::forward<TArg>(arg)}; }) -> void
        requires fmt::is_formattable<TArg>::value
                 || std::same_as<std::string_view, std::remove_cvref_t<TArg>>
                 || requires { std::string_view{std::forward<TArg>(arg)}; }
    {
        if constexpr(std::same_as<std::string_view, std::remove_cvref_t<TArg>>
                     || requires { std::string_view{std::forward<TArg>(arg)}; })
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
    HYPERION_ATTRIBUTE_COLD static auto execute(const hyperion::source_location& location,
                                                const Backtrace& backtrace,
                                                fmt::format_string<TArgs...> format_string,
                                                TArgs&&... args) -> void {
        const auto str = fmt::format(format_string, std::forward<TArgs>(args)...);
        panic::get_handler()(str, location, backtrace);
    }

    HYPERION_IGNORE_UNUSED_TEMPLATES_WARNING_STOP;

} // namespace hyperion::assert::panic

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

#define HYPERION_PANIC(...) /** NOLINT(*-macro-usage) **/ \
    hyperion::assert::panic::execute(                     \
        hyperion::source_location::current(),             \
        hyperion::assert::Backtrace {} __VA_OPT__(, __VA_ARGS__))

#if HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES
    #define PANIC(...) /** NOLINT(*-macro-usage) **/ HYPERION_PANIC(__VA_ARGS__)
#endif // HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

#if HYPERION_ENABLE_TESTING

HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_START;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
    #include <boost/ut.hpp>
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_STOP;

    #include <string>

namespace hyperion::_test::assert::panic {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    static void panic_no_message() {
        HYPERION_PANIC();
    }

    static void panic_with_message() {
        HYPERION_PANIC("With a panic Message!");
    }

    static void panic_with_formatted_message() {
        HYPERION_PANIC("With {} panic Messages!", 42);
    }

    // NOLINTNEXTLINE(cert-err58-cpp, *-avoid-non-const-global-variables)
    static std::string test_str;

    static auto test_handler(const std::string_view panic_message,
                             const hyperion::source_location& location,
                             const hyperion::assert::Backtrace& backtrace) noexcept -> void {
        if(panic_message.empty()) {
            test_str = fmt::format("panic occurred at {0}:\n\n"
                                   "Backtrace:\n{1}\n",
                                   location,
                                   backtrace);
        }
        else {
            test_str = fmt::format("panic occurred at {0}:\n\n"
                                   "{1}\n\n"
                                   "Backtrace:\n{2}\n",
                                   location,
                                   panic_message,
                                   backtrace);
        }
    }

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::assert::panic"> panic_tests = [] {
        "no_message_contents"_test = [] {
            hyperion::assert::panic::set_handler(test_handler);
            panic_no_message();
            expect(test_str.find("panic occurred at") != std::string::npos);
            expect(test_str.find("panic_no_message") != std::string::npos);
        };

        "with_message_contents"_test = [] {
            hyperion::assert::panic::set_handler(test_handler);
            panic_with_message();
            expect(test_str.find("With a panic Message!") != std::string::npos);
            expect(test_str.find("panic_with_message") != std::string::npos);
        };

        "with_formatted_message_contents"_test = [] {
            hyperion::assert::panic::set_handler(test_handler);
            panic_with_formatted_message();
            expect(test_str.find("With 42 panic Messages!") != std::string::npos);
            expect(test_str.find("panic_with_formatted_message") != std::string::npos);
        };

            // ut only supports aborts tests on UNIX-likes for the moment
    #if not HYPERION_PLATFORM_IS_WINDOWS
        "no_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] { panic_no_message(); }));
        };

        "with_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] { panic_with_message(); }));
        };

        "with_formatted_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] { panic_with_formatted_message(); }));
        };
    #endif // not HYPERION_PLATFORM_IS_WINDOWS
    };

} // namespace hyperion::_test::assert::panic

#endif // HYPERION_ENABLE_TESTING

#endif // HYPERION_ASSERT_PANIC_H
