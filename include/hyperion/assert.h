/// @file assert.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Robust C++20 runtime asserts
/// @version 0.1
/// @date 2024-10-02
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
#include <hyperion/assert/def.h>
#include <hyperion/assert/detail/parser.h>
#include <hyperion/assert/highlight.h>
#include <hyperion/assert/panic.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/platform.h>
#include <hyperion/platform/def.h>
#include <hyperion/platform/ignore.h>
#include <hyperion/source_location.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/format.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <concepts>
#include <string>
#include <string_view>
#include <utility>

/// @defgroup assert Assertions Library
/// Hyperion's runtime assertions library provides a full-featured set of
/// assertions suitable for most tasks. These assertions have a vastly expanded
/// feature-set compared to the standard `assert` from `#include <cassert>`,
/// including:
///
/// - Detailed Source Location Information: The complete source location where
/// the assertion was triggered is printed, including file, line, column (if available),
/// and enclosing function information.
/// - Expression Decomposition: The full expression passed to the assertion is
/// decomposed into individual sub-expressions, and if the assertion fails,
/// the expression is printed in both its textual form, and with the values of each
/// step in its evaluation rendered in sequence as well.
/// For example, the expression `value1 + value2 < 5` might result in the following
/// being printed:
/// @code {.cpp}
/// Where: value1 + value2 < 5
/// Evaluated To: (2 + 7) < 5
/// @endcode
/// When the result type of a leaf subexpression does not provide a formatting
/// implementation compatible with libfmt, `(NotFormattable)` is rendered instead.
/// - Backtrace: The backtrace of the callstack up to the point of the triggered
/// assertion is printed, including all available location information:
/// frame number (starting at 0, with 0 being the current frame), address,
/// function name (if available), file name (if available), and line number (if available).
/// Quality of backtrace depends on the target platform and availability of
/// debug symbols/information. Backtrace functionality relies on Boost.Stacktrace and is
/// subject to its support and limitations.
/// - Syntax Highlighting: Full syntax highlighting of the assertion error message,
/// including source locations, expressions, and function names. This uses a simple
/// single-pass implementation for tokenization, and so syntax highlighting is not
/// _guaranteed_ to be 100% accurate (for example, a type may be highlighted as a
/// namespace or vice-versa), but is reasonably accurate for the intended purpose
/// (see screenshots in the README and documentation site for examples of highlighting
/// quality). Syntax highlighting is configurable by making calls to
/// `hyperion::assert::highlight::set_color` or `hyperion::assert::highlight::set_colors`
/// at program startup. By default, highlighting uses the foreground colors of the
/// "One Dark" theme.
/// - Context Message Formatting: Assertions take an optional context message, along
/// with optional parameters for the message, using the formatting capabilities of
/// libfmt. The context message must adhere to libfmt's format string specifications
/// and the additional parameters (if any) must supply a libfmt compatible formatting
/// implementation that is in-scope.
/// - Compiler Optimization Aids: All assertions are implemented in such as way that
/// they inform the compiler that it is extremely unlikely that they will fire, which
/// may aid in code generation and optimization performed by the compiler.
///
/// hyperion::assert provides a variety of assertions and assertion-like utilities:
/// - Assumptions (`HYPERION_ASSERT_ASSUME`): Signals to the compiler that something
/// is assumed to be true. Aids in optimization and code generation. The user must
/// be able to guarantee that the assumption is always valid (otherwise, creates
/// undefined behavior).
/// - Expectations (`HYPERION_ASSERT_EXPECT` and `HYPERION_ASSERT_UNEXPECT`): Signals
/// to the compiler that something is expected to be true (or not), but that it is
/// not guaranteed. Aids in optimization and code generation. Similar to the C++20
/// `[[likely]]` and `[[unlikely]]` attributes, but is not required required to
/// be applied on a branch.
/// - Debug Assertions (`HYPERION_ASSERT_DEBUG`): Typical Debug assertion, but with
/// all of the enhancements mentioned above.
/// - Precondition Assertions (`HYPERION_ASSERT_PRECONDITION`): Used to verify a
/// precondition has been met at the beginning of a function call. Must be the first
/// statement (or sequence of statements, for multiple preconditions) in the function.
/// - Postcondition Assertions (`HYPERION_ASSERT_POSTCONDITION`): Used to verify a
/// postcondition has been fulfilled by a function's operations. May be placed at
/// any point within a function, as long as that location is at the function's
/// outer-most scope. Checked at scope exit.
/// - Fatal Assertions (`HYPERION_ASSERT_FATAL`): Used to verify that an irrecoverable
/// error has _not_ occurred.
/// - Requirement Assertions (`HYPERION_ASSERT_REQUIRE`): Used to verify that a condition
/// that is itself not an error in the state of the program, but otherwise implies that
/// the program can not successfully proceed, has _not_ occurred
/// (examples include Out of Memory or inability to access the network)
///
/// All categories except for Assumptions and Expectations provide the feature-set
/// mentioned above.
///
/// # Example
/// @code{.cpp}
///
/// auto function() -> int;
///
/// auto example() -> void {
///     auto value = 2;
///     auto some_context = some_call_that_influences_function();
///
///     HYPERION_ASSERT_DEBUG(value + function() == 42,
///                           "Example did not equal the meaning of life: {}",
///                           some_context);
/// }
/// @endcode
///
/// @headerfile hyperion/assert.h

namespace hyperion::assert::detail {

    template<typename TDecomposition>
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE constexpr auto
    create_assertion_message(std::string_view assertion_type,
                             std::string_view condition,
                             TDecomposition&& decomposition) -> std::string {
        using hyperion::assert::highlight::get_color;
        using hyperion::assert::highlight::highlight;
        using hyperion::assert::tokens::Error;
        using hyperion::assert::tokens::Punctuation;
        using hyperion::assert::detail::parser::Token;

        return fmt::format(
            R"({0} {1} {3}
    {2} {3}
    {4} {5}
                )",
            fmt::styled(assertion_type,
                        fmt::emphasis::bold
                            | fmt::fg(get_color(tokens::Kind{std::in_place_type<Error>}))),
            fmt::styled("Assertion Failed:",
                        fmt::emphasis::bold
                            | fmt::fg(get_color(tokens::Kind{std::in_place_type<Error>}))),
            fmt::styled("Where:", fmt::emphasis::bold),
            highlight(condition),
            fmt::styled("Evaluated To:", fmt::emphasis::bold),
            std::forward<TDecomposition>(decomposition));
    }

    template<typename TDecomposition>
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE constexpr auto
    create_assertion_message(std::string_view assertion_type,
                             std::string_view condition,
                             TDecomposition&& decomposition,
                             std::string_view message) -> std::string {
        using hyperion::assert::highlight::get_color;
        using hyperion::assert::highlight::highlight;
        using hyperion::assert::tokens::Error;
        using hyperion::assert::tokens::Punctuation;
        using hyperion::assert::detail::parser::Token;

        return fmt::format(
            R"({0} {1} {3}
    {2} {3}
    {4} {5}

    {6}
        {7}
                )",
            fmt::styled(assertion_type,
                        fmt::emphasis::bold
                            | fmt::fg(get_color(tokens::Kind{std::in_place_type<Error>}))),
            fmt::styled("Assertion Failed:",
                        fmt::emphasis::bold
                            | fmt::fg(get_color(tokens::Kind{std::in_place_type<Error>}))),
            fmt::styled("Where:", fmt::emphasis::bold),
            highlight(condition),
            fmt::styled("Evaluated To:", fmt::emphasis::bold),
            std::forward<TDecomposition>(decomposition),
            fmt::styled("Context Message:", fmt::emphasis::bold),
            message);
    }

    template<typename TCallable>
        requires std::invocable<TCallable>
    // NOLINTNEXTLINE(*-special-member-functions)
    struct PostConditionInvoker : TCallable {
        using TCallable::operator();
        using TCallable::operator=;
        using TCallable::TCallable;

        template<typename TFunc>
            requires std::constructible_from<TCallable, TFunc>
                && (!std::same_as<std::remove_cvref_t<TFunc>, PostConditionInvoker>)
        explicit PostConditionInvoker(TFunc&& func) noexcept(
            std::is_nothrow_constructible_v<TCallable, TFunc>)
            : TCallable{std::forward<TFunc>(func)} {
        }

        // NOLINTNEXTLINE(*-noexcept-destructor)
        ~PostConditionInvoker() noexcept(std::is_nothrow_invocable_v<TCallable>) {
            std::move (*this)();
        }
    };
    template<typename TCallable>
    PostConditionInvoker(TCallable) -> PostConditionInvoker<TCallable>;

} // namespace hyperion::assert::detail

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

/// @def HYPERION_ASSERT_ASSUME
/// @brief Signals to the compiler that a condition can be safely assumed
/// to be true at all times. Aids in optimization.
/// @param condition The condition to verify. Can be any arbitrary (sequence of)
/// expression(s), as long as the result of that (sequence of) expression(s) is
/// convertible to `bool`
/// @note Only use this if the condition can be guaranteed to be true.
/// Using this in cases where the condition is not guaranteed results in
/// undefined behavior.
/// If the condition cannot be guaranteed, instead use `HYPERION_ASSERT_EXPECT`
/// @headerfile hyperion/assert.h
/// @ingroup assert

#if HYPERION_PLATFORM_COMPILER_IS_MSVC

    #define HYPERION_ASSERT_ASSUME(condition) /** NOLINT(*-macro-usage) **/ __assume(condition)

#elif HYPERION_PLATFORM_COMPILER_IS_CLANG

    #define HYPERION_ASSERT_ASSUME(condition) /** NOLINT(*-macro-usage) **/ \
        _Pragma("GCC diagnostic push")                                      \
        _Pragma("GCC diagnostic ignored \"-Wassume\"")                      \
        __builtin_assume(condition)                                         \
        _Pragma("GCC diagnostic pop")

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
#else
    #define HYPERION_ASSERT_IGNORE_SHIFT_OP_PARENS_WARNING_START
    #define HYPERION_ASSERT_IGNORE_SHIFT_OP_PARENS_WARNING_STOP
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
        do {                                                                       \
            HYPERION_ASSERT_ASSUME(condition);                                  \
            __VA_OPT__(hyperion::ignore(__VA_ARGS__);)                          \
        } while(false)

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
        HYPERION_DETAIL_ASSERT_HANDLER(condition, "Contract Violaton:\nPre-condition" __VA_OPT__(, __VA_ARGS__))

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
        auto HYPERION_DETAIL_ASSERT_CONCAT3(  \
            _postcondition_,                                                            \
            __LINE__,                                                                   \
            __COUNTER__)                                                                \
            = hyperion::assert::detail::PostConditionInvoker{[&]() {                                                                   \
                  HYPERION_ASSERT_ASSUME(condition);                                    \
                  __VA_OPT__(hyperion::ignore(__VA_ARGS__);)                            \
              }}

#else

    #define HYPERION_ASSERT_POSTCONDITION(condition, ...) /** NOLINT(*-macro-usage) **/       \
        auto HYPERION_DETAIL_ASSERT_CONCAT3(        \
            _postcondition_,                                                                  \
            __LINE__,                                                                         \
            __COUNTER__)                                                                      \
            = hyperion::assert::detail::PostConditionInvoker{[&]() {                                                                         \
                  HYPERION_DETAIL_ASSERT_HANDLER(condition,                                   \
                                                 "Contract Violation:\nPost-condition" __VA_OPT__(, __VA_ARGS__)); \
              }}

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

#if HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES

    #define ASSUME(condition) /** NOLINT(*-macro-usage) **/ HYPERION_ASSERT_ASSUME(condition)

    #define EXPECT(condition) /** NOLINT(*-macro-usage) **/ HYPERION_ASSERT_EXPECT(condition)

    #define UNEXPECT(condition) /** NOLINT(*-macro-usage) **/ HYPERION_ASSERT_UNEXPECT(condition)

    #define ASSERT(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_ASSERT_DEBUG(condition __VA_OPT__(, __VA_ARGS__)))

    #define PRECONDITION(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_ASSERT_PRECONDITION(condition __VA_OPT__(, __VA_ARGS__)))

    #define POSTCONDITION(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_ASSERT_POSTCONDITION(condition __VA_OPT__(, __VA_ARGS__)))

    #define REQUIRE(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_ASSERT_REQUIRE(condition __VA_OPT__(, __VA_ARGS__)))

    #define ASSERT_FATAL(condition, ...) /** NOLINT(*-macro-usage) **/ \
        HYPERION_ASSERT_FATAL(condition __VA_OPT__(, __VA_ARGS__)))

#endif // HYPERION_ASSERT_DEFINE_SHORT_ASSERT_NAMES

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

#endif // HYPERION_ASSERT_H
