/// @file lexer.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ lexer (to allow for rudimentary syntax highlighting)
/// @version 0.1
/// @date 2024-03-11
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

#include <hyperion/platform.h>
#include <hyperion/platform/def.h>
#include <hyperion/platform/types.h>

#if HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC
_Pragma("GCC diagnostic push");
_Pragma("GCC diagnostic ignored \"-Wshadow\"");

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
_Pragma("GCC diagnostic ignored \"-Wzero-as-null-pointer-constant\"");
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG
#endif     // HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC

HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
HYPERION_IGNORE_RESERVED_MACRO_IDENTIFIERS_WARNING_START;

#include <boost/stacktrace.hpp>

HYPERION_IGNORE_RESERVED_MACRO_IDENTIFIERS_WARNING_STOP;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;

#if HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC
_Pragma("GCC diagnostic pop");
#endif // HYPERION_PLATFORM_COMPILER_IS_CLANG || HYPERION_PLATFORM_COMPILER_IS_GCC

#include <array>
#include <string_view>
#include <variant>
#include <vector>

namespace hyperion::assert::detail {

    struct Namespace { };
    struct Type { };
    struct Function { };
    struct Keyword { };
    struct String { };
    struct Numeric { };
    struct Punctuation { };

    using Identifier = std::variant<Namespace, Type, Function, Keyword>;

    struct Token {
        using Kind = std::variant<Identifier, String, Numeric, Punctuation>;
        std::string_view text = {};
        usize begin = 0;
        usize end = 0;
        Kind kind = Punctuation{};
    };

    struct Lexer {
        static constexpr auto punctuation = std::array<std::string_view, 48_usize>{
            "~",  "!",   "+",  "-",  "*",  "/",   "%",   "^",   "&",  "|",  "=", "+=",
            "-=", "*=",  "/=", "%=", "^=", "&=",  "|=",  "==",  "!=", "<",  ">", "<=",
            ">=", "<=>", "&&", "||", "<<", ">>",  "<<=", ">>=", "++", "--", "?", "::",
            ":",  "...", ".",  ".*", "->", "->*", "[",   "]",   "{",  "}",  "(", ")"};
        static constexpr auto keywords = std::array<std::string_view, 89_usize>{
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

        [[nodiscard]] static auto lex(std::string_view string) -> std::vector<Token>;
    };
} // namespace hyperion::assert::detail

#endif // HYPERION_ASSERT_DETAIL_PARSER_H
