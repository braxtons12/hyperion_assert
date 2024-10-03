/// @file tokens.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Token definitions for Rudimentary C++ parsing (to allow for rudimentary syntax
/// highlighting)
/// @version 0.1
/// @date 2024-10-01
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

#ifndef HYPERION_ASSERT_TOKENS_H
#define HYPERION_ASSERT_TOKENS_H

#include <hyperion/platform/types.h>

#include <string_view>
#include <variant>

/// @ingroup tokens
/// @{
/// @defgroup tokens Syntax Highlighting Tokens
/// This module defines the possible C++ token kinds that could make up
/// a tokenized fragment of C++ code.
/// @headerfile hyperion/assert/tokens.h
/// @}

namespace hyperion::assert::tokens {

    /// @brief The token represents a namespace identifier
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Namespace { };
    /// @brief The token represents a type identifier
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Type { };
    /// @brief The token represents a function identifier
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Function { };
    /// @brief The token represents a variable identifier
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Variable { };
    /// @brief The token represents a keyword
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Keyword { };
    /// @brief The token represents a string literal
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct String { };
    /// @brief The token represents a numeric literal
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Numeric { };
    /// @brief The token represents punctuation
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Punctuation { };
    /// @brief The token represents an error
    ///
    /// This is not specific C++ syntax, but instead for
    /// highlighting a particular sequence using an error-signaling color
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    struct Error { };

    /// @brief The token represents an identifier
    /// (a namespace, type, function, or variable)
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    using Identifier = std::variant<Namespace, Type, Function, Variable>;

    /// @brief The represents the type of token a token is
    /// @headerfile hyperion/assert/tokens.h
    /// @ingroup tokens
    using Kind = std::variant<Identifier, Keyword, String, Numeric, Punctuation, Error>;

} // namespace hyperion::assert::tokens

#endif // HYPERION_ASSERT_TOKENS_H
