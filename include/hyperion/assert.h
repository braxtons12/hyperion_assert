/// @file assert.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Robust C++20 runtime asserts
/// @version 0.1
/// @date 2024-03-16
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

#ifndef HYPERION_ASSERT_H
#define HYPERION_ASSERT_H

#include <hyperion/assert/backtrace.h>
#include <hyperion/assert/detail/decomposer.h>
#include <hyperion/assert/detail/highlight.h>
#include <hyperion/assert/detail/parser.h>
#include <hyperion/assert/panic.h>
#include <hyperion/platform.h>
#include <hyperion/platform/def.h>
#include <hyperion/platform/ignore.h>
#include <hyperion/source_location.h>

#include <fmt/format.h>

#include <concepts>
#include <string>
#include <string_view>
#include <utility>

namespace hyperion::assert::detail {

    template<typename TDecomposition>
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE constexpr auto
    create_assertion_message(std::string_view assertion_type,
                             std::string_view condition,
                             TDecomposition&& decomposition) -> std::string {
        using hyperion::assert::detail::highlight::get_color;
        using hyperion::assert::detail::highlight::highlight;
        using hyperion::assert::detail::tokens::Error;
        using hyperion::assert::detail::tokens::Punctuation;
        using hyperion::assert::detail::tokens::Token;

        return fmt::format(
            R"({0} {1} {2}
    Where: {2}
    Evaluated To: {3}
                )",
            fmt::styled(assertion_type, fmt::fg(get_color(Token::Kind{std::in_place_type<Error>}))),
            fmt::styled("Assertion Failed:",
                        fmt::fg(get_color(Token::Kind{std::in_place_type<Error>}))),
            highlight(condition),
            std::forward<TDecomposition>(decomposition));
    }

    template<typename TDecomposition>
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE constexpr auto
    create_assertion_message(std::string_view assertion_type,
                             std::string_view condition,
                             TDecomposition&& decomposition,
                             std::string_view message) -> std::string {
        using hyperion::assert::detail::highlight::get_color;
        using hyperion::assert::detail::highlight::highlight;
        using hyperion::assert::detail::tokens::Error;
        using hyperion::assert::detail::tokens::Punctuation;
        using hyperion::assert::detail::tokens::Token;

        return fmt::format(
            R"({0} {1} {2}
    Where: {2}
    Evaluated To: {3}

    Context Message:
        {4}
                )",
            fmt::styled(assertion_type, fmt::fg(get_color(Token::Kind{std::in_place_type<Error>}))),
            fmt::styled("Assertion Failed:",
                        fmt::fg(get_color(Token::Kind{std::in_place_type<Error>}))),
            highlight(condition),
            std::forward<TDecomposition>(decomposition),
            message);
    }

    template<typename TCallable>
        requires std::invocable<TCallable>
    // NOLINTNEXTLINE(*-special-member-functions)
    struct PostConditionInvoker : TCallable {
        using TCallable::operator();

        // NOLINTNEXTLINE(*-noexcept-destructor)
        ~PostConditionInvoker() noexcept(std::is_nothrow_invocable_v<TCallable>) {
            std::move (*this)();
        }
    };

} // namespace hyperion::assert::detail

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

/// @def HYPERION_ASSERT_ASSUME
/// @brief Signals to the compiler that a condition can be safely assumed
/// to be true at all times. Aids in optimization.
/// @note Only use this if the condition can be guaranteed to be true.
/// Using this in cases where the condition is not guaranteed results in
/// undefined behavior.
/// If the condition cannot be guaranteed, instead use `HYPERION_ASSERT_EXPECT`
/// @headerfile hyperion/assert.h

#if HYPERION_PLATFORM_COMPILER_IS_MSVC

    #define HYPERION_ASSERT_ASSUME(condition) /** NOLINT(*-macro-usage) **/ __assume(condition)

#elif HYPERION_PLATFORM_COMPILER_IS_CLANG

    #define HYPERION_ASSERT_ASSUME(condition) /** NOLINT(*-macro-usage) **/ \
        __builtin_assume(condition)

#else // not HYPERION_PLATFORM_COMPILER_IS_MSVC and not HYPERION_PLATFORM_COMPILER_IS_CLANG

    #define HYPERION_ASSERT_ASSUME(condition) /** NOLINT(*-macro-usage) **/ \
        {                                                                   \
            if(HYPERION_ASSERT_UNEXPECT(!(condition))) [[unlikely]] {       \
                HYPERION_UNREACHABLE();                                     \
            }                                                               \
        }

#endif // HYPERION_PLATFORM_COMPILER_IS_MSVC

#if HYPERION_PLATFORM_COMPILER_IS_CLANG
    #define HYPERION_ASSERT_IGNORE_SHIFT_OP_PARENS_WARNING_START /** NOLINT(*-macro-usage) **/ \
        _Pragma("GCC diagnostic push")                                                         \
            _Pragma("GCC diagnostic ignored \"-Woverloaded-shift-op-parentheses\"")
    #define HYPERION_ASSERT_IGNORE_SHIFT_OP_PARENS_WARNING_STOP /** NOLINT(*-macro-usage) **/ \
        _Pragma("GCC diagnostic pop")
#endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

#define HYPERION_DETAIL_ASSERT_HANDLER(/** NOLINT(*-macro-usage) **/                          \
                                       condition,                                             \
                                       assertion_type,                                        \
                                       ...)                                                   \
    {                                                                                         \
        HYPERION_ASSERT_IGNORE_SHIFT_OP_PARENS_WARNING_START;                                 \
        auto decomposition = (hyperion::assert::detail::ExpressionDecomposer{}->*condition);  \
        HYPERION_ASSERT_IGNORE_SHIFT_OP_PARENS_WARNING_STOP;                                  \
        if(HYPERION_ASSERT_UNEXPECT(!static_cast<bool>(decomposition.expr()))) [[unlikely]] { \
            HYPERION_PANIC(hyperion::assert::detail::create_assertion_message(                \
                assertion_type,                                                               \
                #condition,                                                                   \
                decomposition __VA_OPT__(, fmt::format(__VA_ARGS__))));                       \
        }                                                                                     \
    }

/// @def HYPERION_ASSERT_DEBUG
/// @brief Typical debug assertion. Useful for sanity checking, logic verification,
/// and checking other conditions that you would like to catch early with debug builds,
/// but avoid checking in release.
///
/// In Debug builds:
/// - If `condition` evaluates to `false`:
///     - Prints a helpful error message to `stderr`, including:
///         - The source location at which the assertion was triggered
///         - The `condition` expression
///         - What `condition` evaluated to
///         - Any additional context you passed in
///         - A backtrace of the call stack up to this point
///     - Triggers a debug break point (to aid in debugging the cause of the assertion)
///     - Aborts
/// - If `condition` evaluates to `true`:
///     - Does nothing
///
/// In Release builds:
/// - This maps to `HYPERION_ASSERT_ASSUME`, aiding in code generation and optimization
///
/// @param condition The condition to verify. Can be any arbitrary (sequence of)
/// expression(s), as long as the result of that (sequence of) expression(s) is
/// convertible to `bool`
/// @param ... A message providing additional context in the case of failure can be
/// included in the error message printed to `stderr`, followed by any additional
/// arguments to be formatted into that context message.
/// Formatting is performed by libfmt, so the message must follow its format string
/// specification, and arguments must provide a libfmt-compatible formatting implementation.
///
/// @note The core mechanism of Hyperion's assertions is `HYPERION_PANIC`. All Hyperion
/// assertions eventually map down to this mechanism in the build mode that corresponds
/// with their traditional assertion strategy being active (For `HYPERION_ASSERT_DEBUG`
/// that is Debug builds. For most others, that is always).
/// This means that the exact behavior of assertion failures can be customized by
/// registering a custom Panic handler (for example if you don't want to abort,
/// or don't want to trigger a break point).
/// The contents of the error message given to `HYPERION_PANIC` are fixed, however.
/// @headerfile hyperion/assert.h
/// @ingroup assert

#if HYPERION_PLATFORM_MODE_IS_DEBUG

    #define HYPERION_ASSERT_DEBUG(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_DETAIL_ASSERT_HANDLER(condition, "Debug" __VA_OPT__(, __VA_ARGS__))

#else // not HYPERION_PLATFORM_MODE_IS_DEBUG

    #define HYPERION_ASSERT_DEBUG(condition, ...) /** NOLINT(*-macro-usage) **/ \
        {                                                                       \
            HYPERION_ASSERT_ASSUME(condition);                                  \
            __VA_OPT__(hyperion::ignore(__VA_ARGS__);)                          \
        }

#endif // HYPERION_PLATFORM_MODE_IS_DEBUG

/// @def HYPERION_ASSERT_PRECONDITION
/// @brief Pre-condition assertion. Useful for verifying that function pre-conditions
/// have been met, prior to performing any actual operations.
///
/// - If `condition` evaluates to `false`:
///     - Prints a helpful error message to `stderr`, including:
///         - The source location at which the assertion was triggered
///         - The `condition` expression
///         - What `condition` evaluated to
///         - Any additional context you passed in
///         - A backtrace of the call stack up to this point
///     - Triggers a debug break point (to aid in debugging the cause of the assertion)
///     - Aborts
/// - If `condition` evaluates to `true`:
///     - Does nothing
///
/// By default, Hyperion's contract assertions (`HYPERION_ASSERT_PRECONDITION`
/// and `HYPERION_ASSERT_POSTCONDITION`) trigger in both Debug and Release builds.
/// If `HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY` has been defined to `true`,
/// in Release builds this will map to `HYPERION_ASSERT_ASSUME`, instead.
///
/// @param condition The condition to verify. Can be any arbitrary (sequence of)
/// expression(s), as long as the result of that (sequence of) expression(s) is
/// convertible to `bool`
/// @param ... A message providing additional context in the case of failure can be
/// included in the error message printed to `stderr`, followed by any additional
/// arguments to be formatted into that context message.
/// Formatting is performed by libfmt, so the message must follow its format string
/// specification, and arguments must provide a libfmt-compatible formatting implementation.
///
/// @note The core mechanism of Hyperion's assertions is `HYPERION_PANIC`. All Hyperion
/// assertions eventually map down to this mechanism in the build mode that corresponds
/// with their traditional assertion strategy being active (For `HYPERION_ASSERT_DEBUG`
/// that is Debug builds. For most others, that is always).
/// This means that the exact behavior of assertion failures can be customized by
/// registering a custom Panic handler (for example if you don't want to abort,
/// or don't want to trigger a break point).
/// The contents of the error message given to `HYPERION_PANIC` are fixed, however.
/// @headerfile hyperion/assert.h
/// @ingroup assert

#if not HYPERION_PLATFORM_MODE_IS_DEBUG and HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY

    #define HYPERION_ASSERT_PRECONDITION(condition, ...) /** NOLINT(*-macro-usage) **/ \
        {                                                                              \
            HYPERION_ASSERT_ASSUME(condition);                                         \
            __VA_OPT__(hyperion::ignore(__VA_ARGS__);)                                 \
        }

#else

    #define HYPERION_ASSERT_PRECONDITION(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_DETAIL_ASSERT_HANDLER(condition, "Pre-condition" __VA_OPT__(, __VA_ARGS__))

#endif // not HYPERION_PLATFORM_MODE_IS_DEBUG and HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY

#define HYPERION_DETAIL_ASSERT_CONCAT(/** NOLINT(*-macro-usage) **/ \
                                      x,                            \
                                      y)                            \
    x##y
#define HYPERION_DETAIL_ASSERT_CONCAT2(/** NOLINT(*-macro-usage) **/ \
                                       x,                            \
                                       y)                            \
    HYPERION_DETAIL_ASSERT_CONCAT(x, y)
#define HYPERION_DETAIL_ASSERT_CONCAT3(/** NOLINT(*-macro-usage) **/ \
                                       x,                            \
                                       y,                            \
                                       z)                            \
    HYPERION_DETAIL_ASSERT_CONCAT2(HYPERION_DETAIL_ASSERT_CONCAT2(x, y), z)

/// @def HYPERION_ASSERT_POSTCONDITION
/// @brief Post-condition assertion. Useful for verifying that function post-conditions
/// have been met, after performing all operations.
///
/// - If `condition` evaluates to `false`:
///     - Prints a helpful error message to `stderr`, including:
///         - The source location at which the assertion was triggered
///         - The `condition` expression
///         - What `condition` evaluated to
///         - Any additional context you passed in
///         - A backtrace of the call stack up to this point
///     - Triggers a debug break point (to aid in debugging the cause of the assertion)
///     - Aborts
/// - If `condition` evaluates to `true`:
///     - Does nothing
///
/// By default, Hyperion's contract assertions (`HYPERION_ASSERT_PRECONDITION`
/// and `HYPERION_ASSERT_POSTCONDITION`) trigger in both Debug and Release builds.
/// If `HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY` has been defined to `true`,
/// in Release builds this will map to `HYPERION_ASSERT_ASSUME`, instead.
///
/// @param condition The condition to verify. Can be any arbitrary (sequence of)
/// expression(s), as long as the result of that (sequence of) expression(s) is
/// convertible to `bool`
/// @param ... A message providing additional context in the case of failure can be
/// included in the error message printed to `stderr`, followed by any additional
/// arguments to be formatted into that context message.
/// Formatting is performed by libfmt, so the message must follow its format string
/// specification, and arguments must provide a libfmt-compatible formatting implementation.
///
/// @note The core mechanism of Hyperion's assertions is `HYPERION_PANIC`. All Hyperion
/// assertions eventually map down to this mechanism in the build mode that corresponds
/// with their traditional assertion strategy being active (For `HYPERION_ASSERT_DEBUG`
/// that is Debug builds. For most others, that is always).
/// This means that the exact behavior of assertion failures can be customized by
/// registering a custom Panic handler (for example if you don't want to abort,
/// or don't want to trigger a break point).
/// The contents of the error message given to `HYPERION_PANIC` are fixed, however.
/// @headerfile hyperion/assert.h
/// @ingroup assert

#if not HYPERION_PLATFORM_MODE_IS_DEBUG and HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY

    #define HYPERION_ASSERT_POSTCONDITION(condition, ...) /** NOLINT(*-macro-usage) **/ \
        hyperion::assert::detail::PostConditionInvoker HYPERION_DETAIL_ASSERT_CONCAT3(  \
            _postcondition_,                                                            \
            __LINE__,                                                                   \
            __COUNTER__)                                                                \
            = [&]() {                                                                   \
                  HYPERION_ASSERT_ASSUME(condition);                                    \
                  __VA_OPT__(hyperion::ignore(__VA_ARGS__);)                            \
              }

#else

    #define HYPERION_ASSERT_POSTCONDITION(condition, ...) /** NOLINT(*-macro-usage) **/       \
        hyperion::assert::detail::PostConditionInvoker HYPERION_DETAIL_ASSERT_CONCAT3(        \
            _postcondition_,                                                                  \
            __LINE__,                                                                         \
            __COUNTER__)                                                                      \
            = [&]() {                                                                         \
                  HYPERION_DETAIL_ASSERT_HANDLER(condition,                                   \
                                                 "Post-condition" __VA_OPT__(, __VA_ARGS__)); \
              }

#endif // not HYPERION_PLATFORM_MODE_IS_DEBUG and HYPERION_ASSERT_CONTRACT_ASSERTIONS_DEBUG_ONLY

/// @brief General-purpose hard-requirement assertion. Useful for verifying that conditions
/// crucial to program continuation (for example, conditions for which it is not safe and/or
/// possible to continue execution when they are false) have been maintained.
///
/// - If `condition` evaluates to `false`:
///     - Prints a helpful error message to `stderr`, including:
///         - The source location at which the assertion was triggered
///         - The `condition` expression
///         - What `condition` evaluated to
///         - Any additional context you passed in
///         - A backtrace of the call stack up to this point
///     - Triggers a debug break point (to aid in debugging the cause of the assertion)
///     - Aborts
/// - If `condition` evaluates to `true`:
///     - Does nothing
///
/// @param condition The condition to verify. Can be any arbitrary (sequence of)
/// expression(s), as long as the result of that (sequence of) expression(s) is
/// convertible to `bool`
/// @param ... A message providing additional context in the case of failure can be
/// included in the error message printed to `stderr`, followed by any additional
/// arguments to be formatted into that context message.
/// Formatting is performed by libfmt, so the message must follow its format string
/// specification, and arguments must provide a libfmt-compatible formatting implementation.
///
/// @note The core mechanism of Hyperion's assertions is `HYPERION_PANIC`. All Hyperion
/// assertions eventually map down to this mechanism in the build mode that corresponds
/// with their traditional assertion strategy being active (For `HYPERION_ASSERT_DEBUG`
/// that is Debug builds. For most others, that is always).
/// This means that the exact behavior of assertion failures can be customized by
/// registering a custom Panic handler (for example if you don't want to abort,
/// or don't want to trigger a break point).
/// The contents of the error message given to `HYPERION_PANIC` are fixed, however.
/// @headerfile hyperion/assert.h
/// @ingroup assert
#define HYPERION_ASSERT_REQUIRE(condition, ...) /** NOLINT(*-macro-usage) **/ \
    HYPERION_DETAIL_ASSERT_HANDLER(condition, "Requirement" __VA_OPT__(, __VA_ARGS__))

/// @brief Fatal assertion. Useful for verifying that program-fatal conditions have
/// not occurred.
///
/// - If `condition` evaluates to `false`:
///     - Prints a helpful error message to `stderr`, including:
///         - The source location at which the assertion was triggered
///         - The `condition` expression
///         - What `condition` evaluated to
///         - Any additional context you passed in
///         - A backtrace of the call stack up to this point
///     - Triggers a debug break point (to aid in debugging the cause of the assertion)
///     - Aborts
/// - If `condition` evaluates to `true`:
///     - Does nothing
///
/// @param condition The condition to verify. Can be any arbitrary (sequence of)
/// expression(s), as long as the result of that (sequence of) expression(s) is
/// convertible to `bool`
/// @param ... A message providing additional context in the case of failure can be
/// included in the error message printed to `stderr`, followed by any additional
/// arguments to be formatted into that context message.
/// Formatting is performed by libfmt, so the message must follow its format string
/// specification, and arguments must provide a libfmt-compatible formatting implementation.
///
/// @note The core mechanism of Hyperion's assertions is `HYPERION_PANIC`. All Hyperion
/// assertions eventually map down to this mechanism in the build mode that corresponds
/// with their traditional assertion strategy being active (For `HYPERION_ASSERT_DEBUG`
/// that is Debug builds. For most others, that is always).
/// This means that the exact behavior of assertion failures can be customized by
/// registering a custom Panic handler (for example if you don't want to abort,
/// or don't want to trigger a break point).
/// The contents of the error message given to `HYPERION_PANIC` are fixed, however.
/// @headerfile hyperion/assert.h
/// @ingroup assert
#define HYPERION_ASSERT_FATAL(condition, ...) /** NOLINT(*-macro-usage) **/ \
    HYPERION_DETAIL_ASSERT_HANDLER(condition, "Fatal" __VA_OPT__(, __VA_ARGS__))

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

#if HYPERION_ENABLE_TESTING

HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_START;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
    #include <boost/ut.hpp>
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_STOP;

    #include <string>

namespace hyperion::_test::assert::assert {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    // NOLINTNEXTLINE(cert-err58-cpp, *-avoid-non-const-global-variables)
    static std::string test_str;

    static auto test_handler(const std::string_view panic_message,
                             const hyperion::source_location& location,
                             const hyperion::assert::Backtrace& backtrace) noexcept -> void {
        if(panic_message.empty()) {
            test_str = fmt::format("Panic occurred at {0}:\n\n"
                                   "Backtrace:\n{1}\n",
                                   location,
                                   backtrace);
        }
        else {
            test_str = fmt::format("Panic occurred at {0}:\n\n"
                                   "{1}\n\n"
                                   "Backtrace:\n{2}\n",
                                   location,
                                   panic_message,
                                   backtrace);
        }
    }

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::assert::assert"> assert_tests = [] {
        "no_message_contents"_test = [] {
            auto value = 2;
            auto lambda = []() {
                return 4;
            };

            hyperion::assert::panic::set_handler(test_handler);
            HYPERION_ASSERT_REQUIRE(value + lambda() == 7);
            expect(test_str.find("Panic occurred at") != std::string::npos);
            expect(test_str.find("Where:") != std::string::npos);
            expect(test_str.find("Evaluated To:") != std::string::npos);
        };

        "with_message_contents"_test = [] {
            auto value = 2;
            auto lambda = []() {
                return 4;
            };

            hyperion::assert::panic::set_handler(test_handler);
            HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with context message");
            expect(test_str.find("Panic occurred at") != std::string::npos);
            expect(test_str.find("Where:") != std::string::npos);
            expect(test_str.find("Evaluated To:") != std::string::npos);
            expect(test_str.find("with context message") != std::string::npos);
        };

        "with_formatted_message_contents"_test = [] {
            auto value = 2;
            auto lambda = []() {
                return 4;
            };

            hyperion::assert::panic::set_handler(test_handler);
            HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with {} context messages", 42);
            expect(test_str.find("Panic occurred at") != std::string::npos);
            expect(test_str.find("Where:") != std::string::npos);
            expect(test_str.find("Evaluated To:") != std::string::npos);
            expect(test_str.find("with 42 context messages") != std::string::npos);
        };

            // ut only supports aborts tests on UNIX-likes for the moment
    #if not HYPERION_PLATFORM_IS_WINDOWS
        "no_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };
                HYPERION_ASSERT_REQUIRE(value + lambda() == 7);
            }));
        };

        "with_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };
                HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with context message");
            }));
        };

        "with_formatted_message_failure"_test = [] {
            hyperion::assert::panic::set_handler(hyperion::assert::panic::default_handler());
            expect(aborts([] {
                auto value = 2;
                auto lambda = []() {
                    return 4;
                };
                HYPERION_ASSERT_REQUIRE(value + lambda() == 7, "with {} context messages", 42);
            }));
        };
    #endif // not HYPERION_PLATFORM_IS_WINDOWS
    };

} // namespace hyperion::_test::assert::assert

#endif // HYPERION_ENABLE_TESTING

#endif // HYPERION_ASSERT_H
