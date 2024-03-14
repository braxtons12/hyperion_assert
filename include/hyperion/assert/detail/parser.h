/// @file parser.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ parser (to allow for rudimentary syntax highlighting)
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

#ifndef HYPERION_ASSERT_DETAIL_PARSER_H
#define HYPERION_ASSERT_DETAIL_PARSER_H

#include <hyperion/platform/types.h>

#include <array>
#include <string_view>
#include <variant>
#include <vector>

namespace hyperion::assert::detail::parser {

    struct Namespace { };
    struct Type { };
    struct Function { };
    struct Keyword { };
    struct String { };
    struct Numeric { };
    struct Punctuation { };
    struct Variable { };

    using Identifier = std::variant<Namespace, Type, Function, Variable>;

    struct Token {
        using Kind = std::variant<Identifier, Keyword, String, Numeric, Punctuation>;
        std::string_view text = {};
        usize begin = 0;
        usize end = 0;
        Kind kind = Punctuation{};
    };

    static inline constexpr auto punctuation = std::array<std::string_view, 48_usize>{
        "~",  "!",   "+",  "-",  "*",  "/",   "%",   "^",   "&",  "|",  "=", "+=",
        "-=", "*=",  "/=", "%=", "^=", "&=",  "|=",  "==",  "!=", "<",  ">", "<=",
        ">=", "<=>", "&&", "||", "<<", ">>",  "<<=", ">>=", "++", "--", "?", "::",
        ":",  "...", ".",  ".*", "->", "->*", "[",   "]",   "{",  "}",  "(", ")"};
    static inline constexpr auto keywords = std::array<std::string_view, 89_usize>{
        "alignas",    "constinit",    "public",        "alignof",
        "const_cast", "float",        "register",      "try",
        "asm",        "continue",     "for",           "reinterpret_cast",
        "typedef",    "auto",         "co_await",      "friend",
        "requires",   "typeid",       "bool",          "co_return",
        "goto",       "return",       "typename",      "break",
        "co_yield",   "if",           "short",         "union",
        "case",       "decltype",     "inline",        "signed",
        "unsigned",   "catch",        "default",       "int",
        "sizeof",     "using",        "char",          "delete",
        "long",       "static",       "virtual",       "char8_t",
        "do",         "mutable",      "static_assert", "void",
        "char16_t",   "double",       "namespace",     "static_cast",
        "volatile",   "char32_t",     "dynamic_cast",  "new",
        "struct",     "wchar_t",      "class",         "else",
        "noexcept",   "switch",       "while",         "concept",
        "enum",       "template",     "const",         "explicit",
        "operator",   "this",         "consteval",     "export",
        "private",    "thread_local", "constexpr",     "extern",
        "protected",  "throw",        "and",           "or",
        "xor",        "not",          "bitand",        "bitor",
        "compl",      "and_eq",       "or_eq",         "xor_eq",
        "not_eq"};

    [[nodiscard]] auto parse(std::string_view string) -> std::vector<Token>;
} // namespace hyperion::assert::detail::parser

#if HYPERION_ENABLE_TESTING

HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_START;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
    #include <boost/ut.hpp>
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_STOP;

namespace hyperion::_test::assert::detail::parser {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::assert::detail::parser"> assert_parser_tests = [] {
        "function_call"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "function_call(value)";
            const auto expected = std::vector<Token>{
                {"function_call",
                 0_usize, 13_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "(", 13_usize,
                 14_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 14_usize, 19_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 19_usize,
                 20_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "namespaced_function_call"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "std::function_call(value)";
            const auto expected = std::vector<Token>{
                {"std",
                 0_usize, 3_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 3_usize,
                 5_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"function_call",
                 5_usize, 18_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "(", 18_usize,
                 19_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 19_usize, 24_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 24_usize,
                 25_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "basic_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "String name = function_call(value)";
            const auto expected = std::vector<Token>{
                {"String",
                 0_usize, 6_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {"name",
                 7_usize, 11_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 12_usize,
                 13_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"function_call",
                 14_usize, 27_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "(", 27_usize,
                 28_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 28_usize, 33_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 33_usize,
                 34_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "auto_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "auto name = function_call(value)";
            const auto expected = std::vector<Token>{
                {
                 "auto", 0_usize,
                 4_usize, Token::Kind{std::in_place_type<Keyword>},
                 },
                {"name",
                 5_usize, 9_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 10_usize,
                 11_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"function_call",
                 12_usize, 25_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "(", 25_usize,
                 26_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 26_usize, 31_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 31_usize,
                 32_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "basic_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "String name = AType{value}";
            const auto expected = std::vector<Token>{
                {"String",
                 0_usize, 6_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {"name",
                 7_usize, 11_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 12_usize,
                 13_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"AType",
                 14_usize, 19_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {
                 "{", 19_usize,
                 20_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 20_usize, 25_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "}", 25_usize,
                 26_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "auto_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "auto name = AType{value}";
            const auto expected = std::vector<Token>{
                {
                 "auto", 0_usize,
                 4_usize, Token::Kind{std::in_place_type<Keyword>},
                 },
                {"name",
                 5_usize, 9_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 10_usize,
                 11_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"AType",
                 12_usize, 17_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {
                 "{", 17_usize,
                 18_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 18_usize, 23_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "}", 23_usize,
                 24_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "namespaced_basic_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "std::string name = std::function_call(value)";
            const auto expected = std::vector<Token>{
                {"std",
                 0_usize, 3_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 3_usize,
                 5_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"string",
                 5_usize, 11_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {"name",
                 12_usize, 16_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 17_usize,
                 18_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"std",
                 19_usize, 22_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 22_usize,
                 24_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"function_call",
                 24_usize, 37_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "(", 37_usize,
                 38_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 38_usize, 43_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 43_usize,
                 44_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "namespaced_auto_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "auto name = std::function_call(value)";
            const auto expected = std::vector<Token>{
                {
                 "auto", 0_usize,
                 4_usize, Token::Kind{std::in_place_type<Keyword>},
                 },
                {"name",
                 5_usize, 9_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 10_usize,
                 11_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"std",
                 12_usize, 15_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 15_usize,
                 17_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"function_call",
                 17_usize, 30_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "(", 30_usize,
                 31_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 31_usize, 36_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 36_usize,
                 37_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "namespaced_basic_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "std::string name = std::AType{value}";
            const auto expected = std::vector<Token>{
                {"std",
                 0_usize, 3_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 3_usize,
                 5_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"string",
                 5_usize, 11_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {"name",
                 12_usize, 16_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 17_usize,
                 18_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"std",
                 19_usize, 22_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 22_usize,
                 24_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"AType",
                 24_usize, 29_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {
                 "{", 29_usize,
                 30_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 30_usize, 35_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "}", 35_usize,
                 36_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "namespaced_auto_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "auto name = std::AType{value}";
            const auto expected = std::vector<Token>{
                {
                 "auto", 0_usize,
                 4_usize, Token::Kind{std::in_place_type<Keyword>},
                 },
                {"name",
                 5_usize, 9_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "=", 10_usize,
                 11_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"std",
                 12_usize, 15_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 15_usize,
                 17_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"AType",
                 17_usize, 22_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {
                 "{", 22_usize,
                 23_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"value",
                 23_usize, 28_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "}", 28_usize,
                 29_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "binary_operation"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "var1 || var2";
            const auto expected = std::vector<Token>{
                {"var1",
                 0_usize, 4_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "||", 5_usize,
                 7_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"var2",
                 8_usize, 12_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "compound_operation"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "(var1 || var2) && var3";
            const auto expected = std::vector<Token>{
                {
                 "(", 0_usize,
                 1_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"var1",
                 1_usize, 5_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "||", 6_usize,
                 8_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"var2",
                 9_usize, 13_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 ")", 13_usize,
                 14_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {
                 "&&", 15_usize,
                 17_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"var3",
                 18_usize, 22_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };

        "namespace declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;

            constexpr auto test_str = "namespace hyperion::assert {}";
            const auto expected = std::vector<Token>{
                {
                 "namespace", 0_usize,
                 9_usize, Token::Kind{std::in_place_type<Keyword>},
                 },
                {"hyperion",
                 10_usize, 18_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 18_usize,
                 20_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"assert",
                 20_usize, 26_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "{}", 27_usize,
                 29_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text);
                expect(that % result.begin == expectation.begin);
                expect(that % result.end == expectation.end);
                expect(that % result.kind.index() == expectation.kind.index());
            }
        };
    };
} // namespace hyperion::_test::assert::detail::parser

#endif // HYPERION_ENABLE_TESTING

#endif // HYPERION_ASSERT_DETAIL_PARSER_H
