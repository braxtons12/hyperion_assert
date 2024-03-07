/// @file source_location.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Implementation of `std::source_location` (or re-export of it,
/// if it is available)
/// @version 0.1
/// @date 2024-03-06
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

#include <hyperion/platform/def.h>
#include <hyperion/platform/types.h>

#if HYPERION_STD_LIB_HAS_SOURCE_LOCATION \
    && (not HYPERION_PLATFORM_COMPILER_IS_CLANG || __clang_major__ >= 17)

    #include <source_location>

namespace hyperion {
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

        [[nodiscard]] static consteval auto
        current(detail::SLImpl impl = {}) noexcept -> source_location {
            return {impl};
        }

      private:
        detail::SLImpl m_impl = {};
    };

} // namespace hyperion

#endif // HYPERION_STD_LIB_HAS_SOURCE_LOCATION
       // && (not HYPERION_PLATFORM_COMPILER_IS_CLANG || __clang_major__ >= 17)

#if HYPERION_ENABLE_TESTING

HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_START;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
    #include <boost/ut.hpp>
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_STOP;

    #include <string_view>

namespace hyperion::_test::source_location {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    [[nodiscard]] static constexpr auto get_location() {
        return hyperion::source_location::current();
    }

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::source_location"> source_location_tests = [] {
        "current"_test = [] {
            constexpr auto current = get_location();

            expect(current.line() == 140_u32);
            // column can be one of:
            // - The beginning of the qualified name of the call to `source_location::current`
            // (e.g. the position of "h" `hyperion::source_location::current()`)
            //      - Clang does this
            // - The beginning of the unqualified name of the call to `source_location::current`
            // (regardless of whether it is actually qualified or not)
            // (i.e. the position of the "c" in `current`)
            //      - MSVC does this
            // - The location of the opening parenthesis of the call to `source_location::current`
            //      - GCC does this or `0`
            // - `0` (the implementation doesn't support column location)
            expect(current.column() == 50_u32 || current.column() == 43_u32
                   || current.column() == 16_u32 || current.column() == 0_u32);
            const auto file_name = std::string_view{current.file_name()};

            expect(file_name.find("source_location.h") != std::string_view::npos);

            const auto function_name = std::string_view{current.function_name()};
            expect(function_name.find("get_location") != std::string_view::npos);
        };
    };

} // namespace hyperion::_test::source_location

#endif // HYPERION_ENABLE_TESTING

#endif // HYPERION_ASSERT_SOURCE_LOCATION_H
