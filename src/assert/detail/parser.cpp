/// @file parser.cpp
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

#include <hyperion/assert/detail/parser.h>
#include <hyperion/platform/types.h>

#include <flux.hpp>

#include <optional>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace hyperion::assert::detail::parser {
    namespace {

        // NOLINTNEXTLINE(readability-function-cognitive-complexity)
        [[nodiscard]] auto lex(std::string_view string) -> std::vector<Token> {
            constexpr auto split = [](const std::string_view& view, auto pred) noexcept {
                return flux::ref(view).split(pred).map([](auto elem) {
                    return std::string_view{flux::begin(elem), flux::end(elem)};
                });
            };

            constexpr auto is_punctuation = [](const char& elem) -> bool {
                return flux::contains(parser::punctuation, std::string_view{&elem, 1});
            };

            constexpr auto not_punctuation = [is_punctuation](const char& elem) -> bool {
                return !is_punctuation(elem);
            };

            auto results = std::vector<Token>{};

            const auto whitespace_stripped = split(string, [](const char& elem) {
                return elem == ' ' || elem == '\n' || elem == '\r' || elem == '\t' || elem == '\v';
            });

            auto search_start = 0_usize;
            for(auto stripped : whitespace_stripped) {
                if(stripped.empty()) {
                    continue;
                }

                const auto begin = string.find(stripped, search_start);
                const auto end = begin + stripped.size();
                search_start = end;
                if(flux::contains(parser::punctuation, stripped)) {
                    results.emplace_back(stripped,
                                         begin,
                                         end,
                                         Token::Kind{std::in_place_type<Punctuation>});
                }
                else if(flux::contains(parser::keywords, stripped)) {
                    results.emplace_back(stripped,
                                         begin,
                                         end,
                                         Token::Kind{std::in_place_type<Keyword>});
                }
                else {
                    const auto split_punctuation = split(stripped, not_punctuation);
                    auto punc_start = 0_usize;
                    for(auto punc : split_punctuation) {
                        if(punc.empty()) {
                            continue;
                        }

                        const auto punc_begin = stripped.find(punc, punc_start);
                        const auto punc_end = punc_begin + punc.size();
                        punc_start = punc_end;
                        results.emplace_back(punc,
                                             punc_begin + begin,
                                             punc_end + begin,
                                             Token::Kind{std::in_place_type<Punctuation>});
                    }

                    const auto identifiers_or_literals = split(stripped, is_punctuation);
                    auto ident_start = 0_usize;
                    for(auto ident : identifiers_or_literals) {
                        if(ident.empty()) {
                            continue;
                        }

                        const auto ident_begin = stripped.find(ident, ident_start);
                        const auto ident_end = ident_begin + ident.size();
                        ident_start = ident_end;
                        const auto is_string = ident.front() == '"' && ident.back() == '"';
                        const auto is_keyword = flux::contains(parser::keywords, ident);
                        const auto is_numeric = flux::all(ident, [](const char& elem) {
                            return elem >= '0' && elem <= '9';
                        });
                        results.emplace_back(
                            ident,
                            ident_begin + begin,
                            ident_end + begin,
                            is_string ?
                                Token::Kind{std::in_place_type<String>} :
                                (is_numeric ?
                                     Token::Kind{std::in_place_type<Numeric>} :
                                     (is_keyword ? Token::Kind{std::in_place_type<Keyword>} :
                                                   Token::Kind{std::in_place_type<Identifier>,
                                                               std::in_place_type<Namespace>})));
                    }
                }
            }

            flux::sort(results,
                       [](const auto& lhs, const auto& rhs) { return lhs.begin < rhs.begin; });
            return results;
        }
    } // namespace

    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    [[nodiscard]] auto parse(std::string_view string) -> std::vector<Token> {
        auto tokens = lex(string);
        using Cursor = decltype(flux::first(tokens));

        auto prev_cursor = std::optional<Cursor>{};
        auto prev_prev_cursor = std::optional<Cursor>{};
        for(auto cur = flux::first(tokens); !flux::is_last(tokens, cur); flux::inc(tokens, cur)) {
            auto& token = flux::read_at(tokens, cur);
            if(std::holds_alternative<Identifier>(token.kind)) {
                if(prev_cursor.has_value()) {
                    auto& prev_token = flux::read_at(tokens, prev_cursor.value());
                    // if the previous token is a keyword, then we have a
                    // sequence of the form `Keyword variable`, `namespace Namespace`,
                    // `Keyword Type`, or `Keyword function`.
                    // For the latter two, we backtrack on the next token when we check
                    // the above "if Identifier" branch or when we start to track a pair
                    // of punctuation, respectively
                    if(std::holds_alternative<Keyword>(prev_token.kind)) {
                        if(prev_token.text == "namespace") {
                            token.kind = Identifier{std::in_place_type<Namespace>};
                        }
                        else if(prev_token.text == "auto") {
                            token.kind = Identifier{std::in_place_type<Variable>};
                        }
                        else {
                            token.kind = Identifier{std::in_place_type<Type>};
                        }
                    }
                    // if the previous token is punctuation, then we (most likely)
                    // have a sequence of the form `operator variable`.
                    // Other variations, such as `function(Type|Variable...` or
                    // `template<Type...` are also possible
                    else if(std::holds_alternative<Punctuation>(prev_token.kind)) {
                        if(prev_prev_cursor.has_value()) {
                            const auto& prev_prev_token
                                = flux::read_at(tokens, prev_prev_cursor.value());
                            if(std::holds_alternative<Keyword>(prev_prev_token.kind)
                               || prev_token.text == "(")
                            {
                                token.kind = Identifier{std::in_place_type<Type>};
                            }
                            else {
                                token.kind = Identifier{std::in_place_type<Variable>};
                            }
                        }
                        else {
                            token.kind = Identifier{std::in_place_type<Variable>};
                        }
                    }
                    // if the previous token isn't a keyword or punctuation,
                    // Then either this token is a `Variable` or we have invalid syntax.
                    else {
                        // if the previous token was an identifier and this is an
                        // identifier, then we have a sequence of `Type variable`
                        // or `Type function`
                        if(std::holds_alternative<Identifier>(prev_token.kind)) {
                            prev_token.kind = Identifier{std::in_place_type<Type>};
                        }
                        token.kind = Identifier{std::in_place_type<Variable>};
                    }
                }
                // if we haven't tracked a previous token, assume this is a namespace
                // declaration for now
                else {
                    token.kind = Identifier{std::in_place_type<Namespace>};
                }
            }
            else if(std::holds_alternative<Punctuation>(token.kind)) {
                if(prev_cursor.has_value()) {
                    auto& prev_token = flux::read_at(tokens, prev_cursor.value());
                    if(std::holds_alternative<Identifier>(prev_token.kind)) {
                        if(prev_prev_cursor.has_value()) {
                            const auto& prev_prev_token
                                = flux::read_at(tokens, prev_prev_cursor.value());
                            if(std::holds_alternative<Punctuation>(prev_prev_token.kind)) {
                                if(prev_prev_token.text == "="
                                   && (token.text == "{" || token.text == "("))
                                {
                                    prev_token.kind = Identifier{std::in_place_type<Type>};
                                }
                                else {
                                    prev_token.kind = Identifier{std::in_place_type<Variable>};
                                }
                            }
                            else if(std::holds_alternative<Keyword>(prev_prev_token.kind)
                                    && token.text == "(")
                            {
                                prev_token.kind = Identifier{std::in_place_type<Function>};
                            }
                            else if(token.text == "::") {
                                prev_token.kind = Identifier{std::in_place_type<Namespace>};
                            }
                        }
                        else if(token.text == "::") {
                            prev_token.kind = Identifier{std::in_place_type<Namespace>};
                        }
                    }
                }
            }
            prev_prev_cursor = prev_cursor;
            prev_cursor = cur;
        }
        return tokens;
    }
} // namespace hyperion::assert::detail::parser
