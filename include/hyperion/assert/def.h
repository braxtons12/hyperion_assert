/// @file def.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Implementation macro definitions
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

#ifndef HYPERION_ASSERT_DEF_H
#define HYPERION_ASSERT_DEF_H

#include <hyperion/platform.h>
#include <hyperion/platform/def.h>

/// @ingroup assert
/// @{
/// @defgroup attributes Attributes
/// Hyperion provides several atributes for guiding (or hinting to)
/// the optimizer to better meet the requirements of the user.
///
/// @headerfile hyperion/assert/def.h
/// @}

HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;

/// @def HYPERION_ATTRIBUTE_COLD
/// @brief Marks a function as a "cold" code path.
/// "cold" functions are not primed in the instruction cache,
/// branches leading to them are considered "unlikely", and grouped
/// into a section of the binary away from "hot" functions, to improve locality
/// @headerfile hyperion/assert/def.h
/// @ingroup attributes

#if HYPERION_PLATFORM_COMPILER_IS_CLANG or HYPERION_PLATFORM_COMPILER_IS_GCC
    #define HYPERION_ATTRIBUTE_COLD [[gnu::cold]]
#else
    #define HYPERION_ATTRIBUTE_COLD
#endif // HYPERION_PLATFORM_COMPILER_IS_CLANG or HYPERION_PLATFORM_COMPILER_IS_GCC

/// @def HYPERION_ATTRIBUTE_NO_INLINE
/// @brief Marks a function to not be inlined by the optimizer.
/// @headerfile hyperion/assert/def.h
/// @ingroup attributes

#if HYPERION_PLATFORM_COMPILER_IS_CLANG or HYPERION_PLATFORM_COMPILER_IS_GCC
    #define HYPERION_ATTRIBUTE_NO_INLINE [[gnu::noinline]]
#elif HYPERION_PLATFORM_COMPILER_IS_MSVC
    #define HYPERION_ATTRIBUTE_NO_INLINE __declspec(noinline)
#else
    #define HYPERION_ATTRIBUTE_NO_INLINE
#endif // HYPERION_PLATFORM_COMPILER_IS_CLANG or HYPERION_PLATFORM_COMPILER_IS_GCC

/// @def HYPERION_ASSERT_EXPECT
/// @brief Marks a condition as expected
/// @param expr the expected condition
/// @headerfile hyperion/assert/def.h
/// @ingroup assert

#if HYPERION_PLATFORM_COMPILER_IS_MSVC
    #define HYPERION_ASSERT_EXPECT(expr) /** NOLINT(*-macro-usage) **/ (expr)
#elif defined(__has_builtin)
    #if __has_builtin(__builtin_expect_with_probability)
        #define HYPERION_ASSERT_EXPECT(expr) /** NOLINT(*-macro-usage) **/ \
            __builtin_expect_with_probability((expr), true, 1)
    #elif __has_builtin(__builtin_expect)
        #define HYPERION_ASSERT_EXPECT(expr) /** NOLINT(*-macro-usage) **/ \
            __builtin_expect((expr), true)
    #else
        #define HYPERION_ASSERT_EXPECT(expr) /** NOLINT(*-macro-usage) **/ (expr)
    #endif // __has_builtin(__builtin_expect_with_probability)
#elif HYPERION_PLATFORM_COMPILER_IS_CLANG or HYPERION_PLATFORM_COMPILER_IS_GCC
    #define HYPERION_ASSERT_EXPECT(expr) /** NOLINT(*-macro-usage) **/ \
        __builtin_expect((expr), true)
#else
    #define HYPERION_ASSERT_EXPECT(expr) /** NOLINT(*-macro-usage) **/ (expr)
#endif // HYPERION_PLATFORM_COMPILER_IS_MSVC

/// @brief Marks a condition as unexpected
/// @param expr the unexpected condition
/// @headerfile hyperion/assert/def.h
/// @ingroup assert
#define HYPERION_ASSERT_UNEXPECT(expr) /** NOLINT(*-macro-usage) **/ \
    (!HYPERION_ASSERT_EXPECT(!(expr)))

/// @def HYPERION_ASSERT_DEBUG_BREAK
/// @brief Triggers a debugging break point
/// @headerfile hyperion/assert/def.h
/// @ingroup assert

#if HYPERION_PLATFORM_IS_WINDOWS
    #include <intrin.h>
    #define HYPERION_ASSERT_DEBUG_BREAK() __debugbreak()
#else
    #if __has_builtin(__builtin_debugtrap)
        #define HYPERION_ASSERT_DEBUG_BREAK() /** NOLINT(*-macro-usage) **/ __builtin_debugtrap()
    #else

        // on x86(_64) and ARM V8/64, use the respective assembly instructions to
        // cause a SIGTRAP. Otherwise, resign to `raise(SIGTRAP)` directly.
        // We don't use `__builtin_trap()` because it inherently does the wrong thing.
        // From compiler/optimizer POV, it is semantically equivalent to `abort()`.
        // On x86 it generates `SIGILL`, on ARM it maps directly to `abort()`, etc.
        // @see https://github.com/scottt/debugbreak
        // and
        // @see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=84595
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
#endif     // HYPERION_PLATFORM_IS_WINDOWS

HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;

#endif // HYPERION_ASSERT_DEF_H
