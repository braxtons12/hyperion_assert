/// @file decomposer.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Unit tests main for hyperion::assert::detail::decomposer
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

#ifndef HYPERION_ASSERT_DECOMPOSER_TESTS_H
#define HYPERION_ASSERT_DECOMPOSER_TESTS_H

#include <hyperion/assert/detail/decomposer.h>

#include <boost/ut.hpp>

#include <iostream>
#include <string_view>

namespace hyperion::_test::assert::detail::decomposer {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    // NOLINTNEXTLINE(google-build-using-namespace)
    using hyperion::assert::detail::BinaryExpression;
    using hyperion::assert::detail::ExpressionDecomposer;
    using hyperion::assert::detail::InitialExpression;
    using hyperion::assert::detail::UnaryExpression;

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::assert::detail::decomposer"> assert_decomposer_tests = [] {
        std::cerr << "Running decomposer tests" << std::endl;
        "+"_test = [] {
            auto result = ExpressionDecomposer{}->*1 + 2;
            expect(that % result.expr() == 3);
        };

        "+=="_test = [] {
            auto result = ExpressionDecomposer{}->*1 + 2 == 3;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==+"_test = [] {
            auto result = ExpressionDecomposer{}->*3 == 1 + 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "-"_test = [] {
            auto result = ExpressionDecomposer{}->*1 - 2;
            expect(that % result.expr() == -1);
        };

        "-=="_test = [] {
            auto result = ExpressionDecomposer{}->*1 - 2 == -1;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==-"_test = [] {
            auto result = ExpressionDecomposer{}->*-1 == 1 - 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "*"_test = [] {
            auto result = ExpressionDecomposer{}->*2 * 2;
            expect(that % result.expr() == 4);
        };

        "*=="_test = [] {
            auto result = ExpressionDecomposer{}->*2 * 2 == 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==*"_test = [] {
            auto result = ExpressionDecomposer{}->*4 == 2 * 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "/"_test = [] {
            auto result = ExpressionDecomposer{}->*4 / 2;
            expect(that % result.expr() == 2);
        };

        "/=="_test = [] {
            auto result = ExpressionDecomposer{}->*4 / 2 == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==/"_test = [] {
            auto result = ExpressionDecomposer{}->*2 == 4 / 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "%"_test = [] {
            auto result = ExpressionDecomposer{}->*5_i32 % 3;
            expect(that % result.expr() == 2);
        };

        "%=="_test = [] {
            auto result = ExpressionDecomposer{}->*5_i32 % 3 == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==%"_test = [] {
            auto result = ExpressionDecomposer{}->*2 == 5_i32 % 3;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        _Pragma("GCC diagnostic push");
        _Pragma("GCC diagnostic ignored \"-Woverloaded-shift-op-parentheses\"");
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

        "<<"_test = [] {
            auto result = ExpressionDecomposer{}->*2 << 2;
            expect(that % result.expr() == 8_i32);
        };

        "<<=="_test = [] {
            auto result = ExpressionDecomposer{}->*2 << 2 == 8_i32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==<<"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*8_i32 == 2 << 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        ">>"_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 >> 2;
            expect(that % result.expr() == 2);
        };

        ">>=="_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 >> 2 == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==>>"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*2 == 8_i32 >> 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "<=>"_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 <=> 2;
            expect(result.expr() == std::strong_ordering::greater);
        };

        "<=>=="_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 <=> 2 == std::strong_ordering::greater;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(result.expr());
        };

        "==<=>"_test = [] {
            auto result = ExpressionDecomposer{}->*std::strong_ordering::greater == 8_i32 <=> 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(result.expr());
        };

    #if HYPERION_PLATFORM_COMPILER_IS_GCC
        _Pragma("GCC diagnostic push");
        _Pragma("GCC diagnostic ignored \"-Wparentheses\"");
    #endif // HYPERION_PLATFORM_COMPILER_IS_GCC

        "<"_test = [] {
            auto result = ExpressionDecomposer{}->*2 < 4;
            expect(that % result.expr());
        };

        "<=="_test = [] {
            auto result = ExpressionDecomposer{}->*2 < 4 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==<"_test = [] {
            auto result = ExpressionDecomposer{}->*true == 2 <= 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "<="_test = [] {
            auto result = ExpressionDecomposer{}->*2 <= 4;
            expect(that % result.expr());
        };

        "<==="_test = [] {
            auto result = ExpressionDecomposer{}->*2 <= 4 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==<="_test = [] {
            auto result = ExpressionDecomposer{}->*true == 2 <= 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        ">"_test = [] {
            auto result = ExpressionDecomposer{}->*4 > 2;
            expect(that % result.expr());
        };

        ">=="_test = [] {
            auto result = ExpressionDecomposer{}->*4 > 2 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==>"_test = [] {
            auto result = ExpressionDecomposer{}->*true == 4 >= 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        ">="_test = [] {
            auto result = ExpressionDecomposer{}->*4 >= 2;
            expect(that % result.expr());
        };

        ">==="_test = [] {
            auto result = ExpressionDecomposer{}->*4 >= 2 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==>="_test = [] {
            auto result = ExpressionDecomposer{}->*true == 4 >= 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "=="_test = [] {
            auto result
                = ExpressionDecomposer{}->*std::string_view{"hello"} == std::string_view{"hello"};
            expect(that % result.expr());
        };

        "===="_test = [] {
            auto result = ExpressionDecomposer{}->*std::string_view{"hello"}
                          == std::string_view{"hello"} == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "====reversed"_test = [] {
            auto result = ExpressionDecomposer{}->*true
                          == (std::string_view{"hello"} == std::string_view{"hello"});
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "!="_test = [] {
            auto result
                = ExpressionDecomposer{}->*std::string_view{"hello"} != std::string_view{"world"};
            expect(that % result.expr());
        };

        "!==="_test = [] {
            auto result = ExpressionDecomposer{}->*std::string_view{"hello"}
                          != std::string_view{"world"} == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==!="_test = [] {
            auto result = ExpressionDecomposer{}->*true
                          == (std::string_view{"hello"} != std::string_view{"world"});
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "&"_test = [] {
            auto result = ExpressionDecomposer{}->*0b1100_u32 & 0b1000_u32;
            expect(that % result.expr() == 0b1000_u32);
        };

        "&=="_test = [] {
            auto result = ExpressionDecomposer{}->*(0b1100_u32 & 0b1000_u32) == 0b1000_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==&"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*0b1000_u32 == (0b1100_u32 & 0b1000_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "|"_test = [] {
            auto result = ExpressionDecomposer{}->*0b1100_u32 | 0b0001_u32;
            expect(that % result.expr() == 0b1101_u32);
        };

        "|=="_test = [] {
            auto result = ExpressionDecomposer{}->*(0b1100_u32 | 0b0001_u32) == 0b1101_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==|"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*0b1101_u32 == (0b1100_u32 | 0b0001_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "^"_test = [] {
            auto result = ExpressionDecomposer{}->*0b1100_u32 ^ 0b0101_u32;
            expect(that % result.expr() == 0b1001_u32);
        };

        "^=="_test = [] {
            auto result = ExpressionDecomposer{}->*(0b1100_u32 ^ 0b0101_u32) == 0b1001_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==^"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*0b1001_u32 == (0b1100_u32 ^ 0b0101_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "&&"_test = [] {
            auto result = ExpressionDecomposer{}->*true && false;
            expect(that % result.expr() == false);
        };

        "&&=="_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*(true && false) == false;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==&&"_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*false == (true && false);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "||"_test = [] {
            auto result = ExpressionDecomposer{}->*false || true;
            expect(that % result.expr() == true);
        };

        "||=="_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*(false || true) == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==||"_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*true == (false || true);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "+="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value += 1;
            expect(that % result.expr() == 3);
        };

        "+==="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*(value += 1) == 3;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==+="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*3 == (value += 1);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "-="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value -= 1;
            expect(that % result.expr() == 1);
        };

        "-==="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*(value -= 1) == 1;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==-="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*1 == (value -= 1);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "*="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value *= 2;
            expect(that % result.expr() == 4);
        };

        "*==="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*(value *= 2) == 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==*="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*4 == (value *= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "/="_test = [] {
            auto value = 4;
            auto result = ExpressionDecomposer{}->*value /= 2;
            expect(that % result.expr() == 2);
        };

        "/==="_test = [] {
            auto value = 4;
            auto result = ExpressionDecomposer{}->*(value /= 2) == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==/="_test = [] {
            auto value = 4;
            auto result = ExpressionDecomposer{}->*2 == (value /= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "%="_test = [] {
            auto value = 5_i32;
            auto result = ExpressionDecomposer{}->*value %= 3;
            expect(that % result.expr() == 2);
        };

        "%==="_test = [] {
            auto value = 5_i32;
            auto result = ExpressionDecomposer{}->*(value %= 3) == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==%="_test = [] {
            auto value = 5_i32;
            auto result = ExpressionDecomposer{}->*2 == (value %= 3);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "<<="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value <<= 2;
            expect(that % result.expr() == 8_i32);
        };

        "<<==="_test = [] {
            auto value = 2;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*(value <<= 2) == 8_i32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==<<="_test = [] {
            auto value = 2;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*8_i32 == (value <<= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        ">>="_test = [] {
            auto value = 8_i32;
            auto result = ExpressionDecomposer{}->*value >>= 2;
            expect(that % result.expr() == 2);
        };

        ">>==="_test = [] {
            auto value = 8_i32;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*(value >>= 2) == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==>>="_test = [] {
            auto value = 8_i32;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*2 == (value >>= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "&="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*value &= 0b0110_u32;
            expect(that % result.expr() == 0b0100_u32);
        };

        "&==="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*(value &= 0b0110_u32) == 0b0100_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==&="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*0b0100_u32 == (value &= 0b0110_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "|="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*value |= 0b0110_u32;
            expect(that % result.expr() == 0b1110_u32);
        };

        "|==="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*(value |= 0b0110_u32) == 0b1110_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==|="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*0b1110_u32 == (value |= 0b0110_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "^="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*value ^= 0b0110_u32;
            expect(that % result.expr() == 0b1010_u32);
        };

        "^==="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*(value ^= 0b0110_u32) == 0b1010_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==^="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*0b1010_u32 == (value ^= 0b0110_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        ","_test = [] {
            auto result = (ExpressionDecomposer{}->*2, 4);
            expect(that % result.expr() == 4);
        };

        ",=="_test = [] {
            auto result = (ExpressionDecomposer{}->*2, 4) == 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
        };

        "==,"_test = [] {
            auto value = 2;
            HYPERION_IGNORE_COMMA_MISUSE_WARNING_START;
            auto result = ExpressionDecomposer{}->*4 == (value += 1, 4);
            HYPERION_IGNORE_COMMA_MISUSE_WARNING_STOP;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % value == 3);
            expect(that % result.expr());
        };

    #if HYPERION_PLATFORM_COMPILER_IS_GCC
        _Pragma("GCC diagnostic pop");
    #endif // HYPERION_PLATFORM_COMPILER_IS_GCC

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        _Pragma("GCC diagnostic pop");
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG
        std::cerr << "Finished running decomposer tests" << std::endl;
    };

} // namespace hyperion::_test::assert::detail::decomposer

#endif // HYPERION_ASSERT_DECOMPOSER_TESTS_H
