/// @file source_location.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Unit tests for hyperion::source_location.
/// @version 0.1
/// @date 2024-09-22
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

#ifndef HYPERION_ASSERT_SOURCE_LOCATION_TESTS_H
#define HYPERION_ASSERT_SOURCE_LOCATION_TESTS_H

#include <hyperion/source_location.h>

#include <boost/ut.hpp>

#include <string_view>

namespace hyperion::_test::source_location {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    [[nodiscard]] static constexpr auto get_location() noexcept {
        return hyperion::source_location::current();
    }

    [[nodiscard]] static constexpr auto
    make_default(hyperion::source_location loc = hyperion::source_location::current()) noexcept {
        return loc;
    }

    [[nodiscard]] static constexpr auto get_default() noexcept {
        return _test::source_location::make_default();
    }

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::source_location"> source_location_tests = [] {
        "current"_test = [] {
            constexpr auto current = get_location();

            expect(current.line() == 43_u32);
            // when `source_location::current` is used as a standalone call, column can be one of:
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

#if HYPERION_PLATFORM_IS_APPLE
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-value-range-compare"
#endif // HYPERION_PLATFORM_IS_APPLE
            const auto file_name = std::string_view{current.file_name()};
            expect(file_name.find("source_location.h") != std::string_view::npos);

            const auto function_name = std::string_view{current.function_name()};
            expect(function_name.find("get_location") != std::string_view::npos);
#if HYPERION_PLATFORM_IS_APPLE
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_IS_APPLE
        };

        "current_as_default_arg"_test = [] {
            constexpr auto current = _test::source_location::get_default();

            expect(current.line() == 52_u32);
            // when `source_location::current` is used as a default argument column can be one of:
            // - The beginning of the qualified name of the call to the function
            // (e.g. the first "_" in `test::source_location::get_default()`)
            //      - Clang does this
            // - The beginning of the unqualified name of the call to the function
            // (regardless of whether it is actually qualified or not)
            // (i.e. the position of the "g" in `get_default`)
            //      - MSVC does this
            // - The location of the opening parenthesis of the call to the function
            //      - GCC does this or `0`
            // - `0` (the implementation doesn't support column location)
            expect(current.column() == 52_u32 || current.column() == 40_u32
                   || current.column() == 16_u32 || current.column() == 0_u32);

#if HYPERION_PLATFORM_IS_APPLE
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-value-range-compare"
#endif // HYPERION_PLATFORM_IS_APPLE
            const auto file_name = std::string_view{current.file_name()};

            expect(file_name.find("source_location.h") != std::string_view::npos);

            const auto function_name = std::string_view{current.function_name()};
            expect(function_name.find("get_default") != std::string_view::npos);
#if HYPERION_PLATFORM_IS_APPLE
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_IS_APPLE
        };
    };

} // namespace hyperion::_test::source_location

#endif // HYPERION_ASSERT_SOURCE_LOCATION_TESTS_H
