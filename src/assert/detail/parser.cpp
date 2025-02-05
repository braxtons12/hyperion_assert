/// @file parser.cpp
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

#include <hyperion/assert/def.h>
#include <hyperion/assert/detail/parser.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/platform/types.h>

#include <flux.hpp>

#include <compare>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace hyperion::assert::detail::parser {

    namespace {

        [[nodiscard]] HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
        // NOLINTNEXTLINE(readability-function-cognitive-complexity)
        lex(std::string_view string) -> std::vector<Token> {
            constexpr auto split = [](const std::string_view& view, auto pred) noexcept {
                return flux::ref(view).split(pred).map(
                    [](auto elem) { return std::string_view{flux::begin(elem), flux::end(elem)}; });
            };

            constexpr auto is_punctuation = [](const char& elem) -> bool {
                return flux::contains(parser::punctuation, std::string_view{&elem, 1});
            };

            constexpr auto not_punctuation = [is_punctuation](const char& elem) -> bool {
                return !is_punctuation(elem);
            };

            auto results = std::vector<Token>{};
            // 5 characters per token is a reasonable average to assume to minimize
            // reallocations.
            // Found by meeting roughly in the middle between the common
            // length of an identifier or keyword (~8, anecdotally) and the average
            // length of a punctuation "token" (~ 1 - 3)
            results.reserve(string.size() / 5_usize);

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
                                         tokens::Kind{std::in_place_type<tokens::Punctuation>});
                }
                else if(flux::contains(parser::keywords, stripped)) {
                    results.emplace_back(stripped,
                                         begin,
                                         end,
                                         tokens::Kind{std::in_place_type<tokens::Keyword>});
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
                                             tokens::Kind{std::in_place_type<tokens::Punctuation>});
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
                        const auto is_numeric
                            = flux::all(ident,
                                        [](const char& elem) { return elem >= '0' && elem <= '9'; })
                              || ident.front() == '0' || ident == "true" || ident == "false";
                        results.emplace_back(
                            ident,
                            ident_begin + begin,
                            ident_end + begin,
                            [](bool _string, bool _numeric, bool _keyword) {
                                if(_string) {
                                    return tokens::Kind{std::in_place_type<tokens::String>};
                                }
                                if(_numeric) {

                                    return tokens::Kind{std::in_place_type<tokens::Numeric>};
                                }

                                if(_keyword) {

                                    return tokens::Kind{std::in_place_type<tokens::Keyword>};
                                }

                                return tokens::Kind{std::in_place_type<tokens::Identifier>,
                                                    std::in_place_type<tokens::Namespace>};
                            }(is_string, is_numeric, is_keyword));
                    }
                }
            }

            // work around flux main using three-way comparator for sort,
            // but the current release version using older STL style "less" comparator
#if FLUX_VERSION_MAJOR == 0 && FLUX_VERSION_MINOR == 4 && FLUX_VERSION_DEVEL != 1
            flux::sort(
                results,

                [](const Token& lhs, const Token& rhs) -> bool { return lhs.begin < rhs.begin; });
#else
            flux::sort(results,

                       [](const Token& lhs, const Token& rhs) -> std::weak_ordering {
                           return lhs.begin <=> rhs.begin;
                       });
#endif // FLUX_VERSION_MAJOR == 0 && FLUX_VERSION_MINOR == 4 && FLUX_VERSION_DEVEL != 1

            return results;
        }
    } // namespace

    [[nodiscard]] HYPERION_ATTRIBUTE_COLD HYPERION_ATTRIBUTE_NO_INLINE auto
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    parse(std::string_view string) -> std::vector<Token> {
        auto tokens = lex(string);

        using Cursor = decltype(flux::first(tokens));

        auto prev_cursor = std::optional<Cursor>{};
        auto prev_prev_cursor = std::optional<Cursor>{};
        for(auto cur = flux::first(tokens); !flux::is_last(tokens, cur); flux::inc(tokens, cur)) {
            auto& token = flux::read_at(tokens, cur);
            // special case "operator" because we treat it like a `Keyword`, but want it
            // highlighted as a `Function`
            if(token.text == "operator") {
                token.kind = tokens::Identifier{std::in_place_type<tokens::Function>};
            }
            else if(std::holds_alternative<tokens::Identifier>(token.kind)) {
                if(prev_cursor.has_value()) {
                    auto& prev_token = flux::read_at(tokens, prev_cursor.value());
                    // if the previous token is a keyword, then we have a
                    // sequence of the form `Keyword variable`, `namespace Namespace`,
                    // `Keyword Type`, or `Keyword function`.
                    // For the latter two, we assume `Type` for now and revert to `Function`, if
                    // necessary, in the `Punctuation` branch when we check the next token on the
                    // next iteration
                    if(std::holds_alternative<tokens::Keyword>(prev_token.kind)) {
                        if(prev_token.text == "namespace") {
                            token.kind = tokens::Identifier{std::in_place_type<tokens::Namespace>};
                        }
                        else if(prev_token.text == "auto") {
                            token.kind = tokens::Identifier{std::in_place_type<tokens::Variable>};
                        }
                        else {
                            token.kind = tokens::Identifier{std::in_place_type<tokens::Type>};
                        }
                    }
                    // if the previous token is punctuation, then we have myriad possibilities,
                    // for example: `operator Variable`, `function|constructor(Type|Variable)`,
                    // `template<Type...`, `::Type`, `::Namespace`, and `::(Type keyword)`, (often
                    // appears in `std::source_location.function_name()` when used in lambdas),
                    // are probably the most common categories of sequences
                    else if(std::holds_alternative<tokens::Punctuation>(prev_token.kind)) {
                        if(prev_token.text == "::(") {
                            token.kind = tokens::Identifier{std::in_place_type<tokens::Type>};
                        }
                        // we assume `Namespace` for now, and revert to `Function` or `Type`,
                        // if necessary, in the `Punctuation` branch, when we check the next token
                        // on the next iteration
                        else if(prev_token.text.starts_with("::")) {
                            token.kind = tokens::Identifier{std::in_place_type<tokens::Namespace>};
                        }
                        else if(prev_prev_cursor.has_value()) {
                            const auto& prev_prev_token
                                = flux::read_at(tokens, prev_prev_cursor.value());
                            if(std::holds_alternative<tokens::Keyword>(prev_prev_token.kind)
                               || prev_token.text.front() == '(')
                            {
                                token.kind = tokens::Identifier{std::in_place_type<tokens::Type>};
                            }
                            else {
                                token.kind
                                    = tokens::Identifier{std::in_place_type<tokens::Variable>};
                            }
                        }
                    }
                    // if this token is an identifier but the previous token isn't a keyword or
                    // punctuation, then either this token is a `Variable` or we have invalid
                    // syntax.
                    else {
                        // if the previous token was an identifier and this is an
                        // identifier, then we have a sequence of `Type variable`
                        // or `Type function`
                        if(std::holds_alternative<tokens::Identifier>(prev_token.kind)) {
                            prev_token.kind = tokens::Identifier{std::in_place_type<tokens::Type>};
                        }
                        // we assume `Variable` here and revert to `Function`, if necessary, in the
                        // `Punctuation` branch, when we check the next token on the next iteration
                        token.kind = tokens::Identifier{std::in_place_type<tokens::Variable>};
                    }
                }
                // if we haven't tracked a previous token, assume this is a namespace
                // declaration for now
                else {
                    token.kind = tokens::Identifier{std::in_place_type<tokens::Namespace>};
                }
            }
            else if(std::holds_alternative<tokens::Punctuation>(token.kind)) {
                if(prev_cursor.has_value()) {
                    auto& prev_token = flux::read_at(tokens, prev_cursor.value());
                    // special case operator because we treat it like a `Keyword`, but want it
                    // highlighted as a Function
                    if(prev_token.text == "operator") {
                        prev_token.kind = tokens::Identifier{std::in_place_type<tokens::Function>};
                    }
                    else if(std::holds_alternative<tokens::Identifier>(prev_token.kind)) {
                        // hack, assume that for all sequences of the form `first::second`,
                        // `first` is always a `Namespace`
                        if(token.text.starts_with("::")) {
                            prev_token.kind
                                = tokens::Identifier{std::in_place_type<tokens::Namespace>};
                        }
                        // the `token.begin == prev_token.end` here is a bit of a hack assuming
                        // that people don't but the opening brace of a namespace up against
                        // the namespace name, but do for a constructor call
                        // NOLINTNEXTLINE(bugprone-branch-clone): separate concern from text == "<"
                        else if(token.text.starts_with("{") && token.begin == prev_token.end) {
                            prev_token.kind = tokens::Identifier{std::in_place_type<tokens::Type>};
                        }
                        // hack, assume all templates are type templates and all template parameters
                        // are types
                        else if(token.text == "<"
                                || (token.text.starts_with(">")
                                    && (token.text != ">>"
                                        || !std::holds_alternative<tokens::Variable>(
                                            std::get<tokens::Identifier>(prev_token.kind)))))
                        {
                            prev_token.kind = tokens::Identifier{std::in_place_type<tokens::Type>};
                        }
                        // assume that all identifiers immediately preceding an opening paren are
                        // functions. This means that constructor calls get highlighted as
                        // function calls, but some IDEs even get this wrong, so we won't lose any
                        // sleep on that. (Constructor calls use brace-init will be highlighted
                        // as types, though. See two branches above)
                        else if(token.text.front() == '(') {
                            prev_token.kind
                                = tokens::Identifier{std::in_place_type<tokens::Function>};
                        }
                        // assume that anything else not immediately preceding an `=`, `(`, `{`,
                        // or `::` is a variable
                        else if(token.text != "=" && token.text.front() != '('
                                && token.text.front() != '{' && !token.text.starts_with("::"))
                        {
                            prev_token.kind
                                = tokens::Identifier{std::in_place_type<tokens::Variable>};
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
