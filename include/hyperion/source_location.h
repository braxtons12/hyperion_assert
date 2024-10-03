/// @file source_location.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Implementation of `std::source_location` (or re-export of it,
/// if it is available)
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

#ifndef HYPERION_ASSERT_SOURCE_LOCATION_H
#define HYPERION_ASSERT_SOURCE_LOCATION_H

#include <hyperion/assert/def.h>
#include <hyperion/platform.h>
#include <hyperion/platform/def.h>
#include <hyperion/platform/types.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/format.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC


#include <string>

/// @defgroup source_location Source Location
/// Hyperion provides an alternative implementation of `std::source_location`
/// as `hyperion::source_location`, for users whose target standard library
/// implementation doesn't yet support `std::source_location`
/// (or their implementation still has bugs), and re-exports `std::source_location`
/// under the hyperion namespace for those that do support it (bug-free).
///
/// It also provides a libfmt format specialization for `hyperion::source_location`
/// (either source of implementation), formatting in the way typically found in assertions
/// (i.e. `[file_name|line_number:column_number]: function_name` )
///
/// @headerfile hyperion/source_location.h

#if HYPERION_STD_LIB_HAS_SOURCE_LOCATION \
    && (not HYPERION_PLATFORM_COMPILER_IS_CLANG or __clang_major__ >= 16)

    #include <source_location>

namespace hyperion {
    /// @brief Re-export of `std::source_location` as `hyperion::source_location`
    /// @ingroup source_location
    /// @headerfile hyperion/source_location.h
    using std::source_location;
} // namespace hyperion

#else

namespace hyperion {

    #define HYPERION_BUILTIN_LINE() __builtin_LINE()

    #if HYPERION_PLATFORM_COMPILER_IS_MSVC

        #define HYPERION_BUILTIN_COLUMN()   __builtin_COLUMN()
        #define HYPERION_BUILTIN_FILE()     __builtin_FILE()
        #define HYPERION_BUILTIN_FUNCTION() __builtin_FUNCSIG()

    #else

        #if __has_builtin(__builtin_COLUMN)
            #define HYPERION_BUILTIN_COLUMN() __builtin_COLUMN()
        #else
            #define HYPERION_BUILTIN_COLUMN() 0
        #endif // __has_builtin(__builtin_COLUMN)

        #if __has_builtin(__builtin_FILE_NAME)
            #define HYPERION_BUILTIN_FILE() __builtin_FILE_NAME()
        #else
            #define HYPERION_BUILTIN_FILE() __builtin_FILE()
        #endif // __has_builtin(__builtin_FILE_NAME)

        #if __has_builtin(__builtin_FUNCSIG)
            #define HYPERION_BUILTIN_FUNCTION() __builtin_FUNCSIG()
        #else
            #define HYPERION_BUILTIN_FUNCTION() __builtin_FUNCTION()
        #endif // __has_builtin(__builtin_FUNCSIG)

    #endif // HYPERION_PLATFORM_COMPILER_IS_MSVC

    namespace detail {
        struct SLImpl {
            u32 line = HYPERION_BUILTIN_LINE();
            u32 column = HYPERION_BUILTIN_COLUMN();
            const char* file = HYPERION_BUILTIN_FILE();
            const char* function = HYPERION_BUILTIN_FUNCTION();
        };
    } // namespace detail

    /// @brief Stand-in, standard-compliant implementation of `std::source_location`,
    /// for those standard library implementations that don't yet have a bug-free implementation.
    ///
    /// Requires that the compiler level support for the pre-requisite intrinsics exists:
    /// - `__builtin_FILE()` or `__builtin_FILE_NAME()`
    /// - `__builtin_FUNCSIG()` or `__builtin_FUNCTION()`
    /// - `__builtin_LINE()`
    /// - If `__builtin_COLUMN()` is not supported, column number will be reported as `0`
    ///
    /// @ingroup source_location
    /// @headerfile hyperion/source_location.h
    class source_location {
      private:
        constexpr source_location(detail::SLImpl impl) noexcept : m_impl{impl} {
        }

      public:
        constexpr source_location() noexcept = default;
        constexpr source_location(const source_location&) noexcept = default;
        constexpr source_location(source_location&&) noexcept = default;
        constexpr auto operator=(const source_location&) noexcept -> source_location& = default;
        constexpr auto operator=(source_location&&) noexcept -> source_location& = default;
        constexpr ~source_location() noexcept = default;

        [[nodiscard]] constexpr auto line() const noexcept -> u32 {
            return m_impl.line;
        }

        [[nodiscard]] constexpr auto column() const noexcept -> u32 {
            return m_impl.column;
        }

        [[nodiscard]] constexpr auto file_name() const noexcept -> const char* {
            return m_impl.file;
        }

        [[nodiscard]] constexpr auto function_name() const noexcept -> const char* {
            return m_impl.function;
        }

        [[nodiscard]] static
    #if HYPERION_PLATFORM_COMPILER_IS_CLANG && __clang_major__ < 16
            constexpr
    #else
            consteval
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG && __clang_major__ < 16
            auto
            current(detail::SLImpl impl = {}) noexcept -> source_location {
            return {impl};
        }

      private:
        detail::SLImpl m_impl = {};
    };

} // namespace hyperion

#endif // HYPERION_STD_LIB_HAS_SOURCE_LOCATION
       // && (not HYPERION_PLATFORM_COMPILER_IS_CLANG || __clang_major__ >= 16)

namespace hyperion::assert::detail {

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    format_source_location(const hyperion::source_location& location) -> std::string;

} // namespace hyperion::assert::detail

/// @brief libfmt format specialization for `hyperion::source_location`
/// @ingroup source_location
/// @headerfile hyperion/source_location.h
template<>
struct fmt::formatter<hyperion::source_location> {
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] inline constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    template<typename TFormatContext>
    [[nodiscard]] inline auto
    format(const hyperion::source_location& location, TFormatContext& context) const {
        return fmt::format_to(context.out(),
                              "[{}|{}:{}]: {}",
                              location.file_name(),
                              location.line(),
                              location.column(),
                              location.function_name());
    }
};

#endif // HYPERION_ASSERT_SOURCE_LOCATION_H
