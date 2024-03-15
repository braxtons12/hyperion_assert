/// @file parser.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ parser (to allow for rudimentary syntax highlighting)
/// @version 0.1
/// @date 2024-03-15
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

#include <hyperion/assert/detail/def.h>
#include <hyperion/assert/detail/tokens.h>
#include <hyperion/platform/types.h>

#include <array>
#include <string_view>
#include <vector>

namespace hyperion::assert::detail::parser {

    static inline constexpr auto punctuation = std::array<std::string_view, 48_usize>{
        "~",  "!",   "+",  "-",  "*",  "/",   "%",   "^",   "&",  "|",  "=", "+=",
        "-=", "*=",  "/=", "%=", "^=", "&=",  "|=",  "==",  "!=", "<",  ">", "<=",
        ">=", "<=>", "&&", "||", "<<", ">>",  "<<=", ">>=", "++", "--", "?", "::",
        ":",  "...", ".",  ".*", "->", "->*", "[",   "]",   "{",  "}",  "(", ")"};
    static inline constexpr auto keywords
        = std::array<std::string_view, 88_usize>{"alignas",
                                                 "constinit",
                                                 "public",
                                                 "alignof",
                                                 "const_cast",
                                                 "float",
                                                 "register",
                                                 "try",
                                                 "asm",
                                                 "continue",
                                                 "for",
                                                 "reinterpret_cast",
                                                 "typedef",
                                                 "auto",
                                                 "co_await",
                                                 "friend",
                                                 "requires",
                                                 "typeid",
                                                 "bool",
                                                 "co_return",
                                                 "goto",
                                                 "return",
                                                 "typename",
                                                 "break",
                                                 "co_yield",
                                                 "if",
                                                 "short",
                                                 "union",
                                                 "case",
                                                 "decltype",
                                                 "inline",
                                                 "signed",
                                                 "unsigned",
                                                 "catch",
                                                 "default",
                                                 "int",
                                                 "sizeof",
                                                 "using",
                                                 "char",
                                                 "delete",
                                                 "long",
                                                 "static",
                                                 "virtual",
                                                 "char8_t",
                                                 "do",
                                                 "mutable",
                                                 "static_assert",
                                                 "void",
                                                 "char16_t",
                                                 "double",
                                                 "namespace",
                                                 "static_cast",
                                                 "volatile",
                                                 "char32_t",
                                                 "dynamic_cast",
                                                 "new",
                                                 "struct",
                                                 "wchar_t",
                                                 "class",
                                                 "else",
                                                 "noexcept",
                                                 "switch",
                                                 "while",
                                                 "concept",
                                                 "enum",
                                                 "template",
                                                 "const",
                                                 "explicit",
                                                 "this",
                                                 "consteval",
                                                 "export",
                                                 "private",
                                                 "thread_local",
                                                 "constexpr",
                                                 "extern",
                                                 "protected",
                                                 "throw",
                                                 "and",
                                                 "or",
                                                 "xor",
                                                 "not",
                                                 "bitand",
                                                 "bitor",
                                                 "compl",
                                                 "and_eq",
                                                 "or_eq",
                                                 "xor_eq",
                                                 "not_eq"};

    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    parse(std::string_view string) -> std::vector<tokens::Token>;
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

    // NOLINTNEXTLINE(cert-err58-cpp,readability-function-cognitive-complexity)
    static const suite<"hyperion::assert::detail::parser"> assert_parser_tests = [] {
        "function_call"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "namespaced_function_call"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "basic_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "auto_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "basic_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "auto_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "namespaced_basic_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "namespaced_auto_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "namespaced_basic_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "namespaced_auto_braced_declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "binary_operation"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "compound_operation"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "namespace declaration"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

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

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "template_type"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

            constexpr auto test_str = "std::vector<std::string>";
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
                {"vector",
                 5_usize, 11_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {
                 "<", 11_usize,
                 12_usize, Token::Kind{std::in_place_type<Punctuation>},
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
                {"string",
                 17_usize, 23_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Type>},
                 }},
                {
                 ">", 23_usize,
                 24_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "deeply_namespaced_function_call"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

            constexpr auto test_str = "hyperion::_test::assert::panic::panic_no_message()";
            const auto expected = std::vector<Token>{
                {"hyperion",
                 0_usize, 8_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 8_usize,
                 10_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"_test",
                 10_usize, 15_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 15_usize,
                 17_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"assert",
                 17_usize, 23_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 23_usize,
                 25_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"panic",
                 25_usize, 30_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 30_usize,
                 32_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"panic_no_message",
                 32_usize, 48_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "()", 48_usize,
                 50_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };

        "complicated_lambda_call_location"_test = [] {
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::parser;
            // NOLINTNEXTLINE(google-build-using-namespace)
            using namespace hyperion::assert::detail::tokens;

            // clang-format off
            constexpr auto test_str //
= "hyperion::_test::assert::panic::panic_tests::{lambda()#1}::operator()() const::{lambda()#1}::_FUN()";
            // clang-format on

            const auto expected = std::vector<Token>{
                {"hyperion",
                 0_usize, 8_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 8_usize,
                 10_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"_test",
                 10_usize, 15_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 15_usize,
                 17_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"assert",
                 17_usize, 23_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 23_usize,
                 25_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"panic",
                 25_usize, 30_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::", 30_usize,
                 32_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"panic_tests",
                 32_usize, 43_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Namespace>},
                 }},
                {
                 "::{", 43_usize,
                 46_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"lambda",
                 46_usize, 52_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "()", 52_usize,
                 54_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"#1",
                 54_usize, 56_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "}::", 56_usize,
                 59_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"operator",
                 59_usize, 67_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "()()", 67_usize,
                 71_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {
                 "const", 72_usize,
                 77_usize, Token::Kind{std::in_place_type<Keyword>},
                 },
                {
                 "::{", 77_usize,
                 80_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"lambda",
                 80_usize, 86_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "()", 86_usize,
                 88_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"#1",
                 88_usize, 90_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Variable>},
                 }},
                {
                 "}::", 90_usize,
                 93_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
                {"_FUN",
                 93_usize, 97_usize,
                 Token::Kind{
                     std::in_place_type<Identifier>,
                     Identifier{std::in_place_type<Function>},
                 }},
                {
                 "()", 97_usize,
                 99_usize, Token::Kind{std::in_place_type<Punctuation>},
                 },
            };

            const auto results = parse(test_str);

            expect(that % results.size() == expected.size());

            for(auto index = 0_usize; index < results.size(); ++index) {
                const auto& result = results.at(index);
                const auto& expectation = expected.at(index);

                expect(that % result.text == expectation.text) << "index: " << index;
                expect(that % result.begin == expectation.begin) << "index: " << index;
                expect(that % result.end == expectation.end) << "index: " << index;
                expect(that % result.kind.index() == expectation.kind.index())
                    << "index: " << index;
                if(std::holds_alternative<Identifier>(result.kind)
                   && std::holds_alternative<Identifier>(expectation.kind))
                {
                    expect(that % std::get<Identifier>(result.kind).index()
                           == std::get<Identifier>(expectation.kind).index())
                        << "index: " << index;
                }
            }
        };
    };
} // namespace hyperion::_test::assert::detail::parser

#endif // HYPERION_ENABLE_TESTING

#endif // HYPERION_ASSERT_DETAIL_PARSER_H
