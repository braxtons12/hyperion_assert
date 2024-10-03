/// @file parser.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Rudimentary C++ parser (to allow for rudimentary syntax highlighting)
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

#ifndef HYPERION_ASSERT_DETAIL_PARSER_H
#define HYPERION_ASSERT_DETAIL_PARSER_H

#include <hyperion/assert/def.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/platform/types.h>

#include <array>
#include <string_view>
#include <vector>

namespace hyperion::assert::detail::parser {

    /// @brief Punctuation and operator tokens
    /// @headerfile hyperion/assert/detail/parser.h
    /// @ingroup parser
    static inline constexpr auto punctuation = std::array<std::string_view, 49_usize>{
        "~",  "!",  "+",   "-",  "*",   "/",   "%",  "^",  "&", "|",  "=",  "+=",  "-=",
        "*=", "/=", "%=",  "^=", "&=",  "|=",  "==", "!=", "<", ">",  "<=", ">=",  "<=>",
        "&&", "||", "<<",  ">>", "<<=", ">>=", "++", "--", "?", "::", ":",  "...", ".",
        ".*", "->", "->*", "[",  "]",   "{",   "}",  "(",  ")", ";"};
    /// @brief Keyword tokens
    /// @headerfile hyperion/assert/detail/parser.h
    /// @ingroup parser
    static inline constexpr auto keywords = std::array<std::string_view, 88_usize>{
        "alignas",
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
        "not_eq",
    };

    /// @brief `Token` represents a C++ token and its location within an associated string
    /// @headerfile hyperion/assert/detail/parser.h
    /// @ingroup parser
    struct Token {
        std::string_view text = {};
        usize begin = 0;
        usize end = 0;
        tokens::Kind kind = tokens::Punctuation{};
    };

    /// @brief Parses the given string into a sequence of tokens
    /// @param string the string to parse
    /// @return The parsed tokens
    /// @headerfile hyperion/assert/detail/parser.h
    /// @ingroup parser
    HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE [[nodiscard]] auto
    parse(std::string_view string) -> std::vector<Token>;
} // namespace hyperion::assert::detail::parser

#endif // HYPERION_ASSERT_DETAIL_PARSER_H
