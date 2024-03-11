#include <hyperion/assert/detail/lexer.h>

#include <range/v3/algorithm.hpp>
#include <range/v3/view.hpp>

namespace hyperion::assert::detail {
    [[nodiscard]] auto Lexer::lex(std::string_view string) -> std::vector<Token> {
        constexpr auto split = [](const std::string_view& view, auto pred) noexcept {
            return view | ranges::views::split_when(pred) | ranges::views::transform([](auto rng) {
                       auto begin = rng.begin();
                       auto end = ranges::next(begin, rng.end());
                       auto sub = ranges::subrange{begin.base(), end.base()};
                       return std::string_view{*(sub.begin()), *(sub.end())};
                   });
        };

        constexpr auto is_punctuation = [](const char& elem) -> bool {
            return ranges::contains(Lexer::punctuation, std::string_view{&elem, 1});
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
            const auto begin = string.find(stripped, search_start);
            const auto end = begin + stripped.size();
            search_start = end;
            if(ranges::contains(Lexer::punctuation, stripped)) {
                results.emplace_back(stripped,
                                     begin,
                                     end,
                                     Token::Kind{std::in_place_type<Punctuation>});
            }
            else if(ranges::contains(Lexer::keywords, stripped)) {
                results.emplace_back(
                    stripped,
                    begin,
                    end,
                    Token::Kind{std::in_place_type<Identifier>, std::in_place_type<Keyword>});
            }
            else {
                const auto split_punctuation = split(stripped, not_punctuation);
                auto punc_start = 0_usize;
                for(auto punc : split_punctuation) {
                    const auto punc_begin = stripped.find(punc, punc_start);
                    const auto punc_end = punc_begin + punc.size();
                    punc_start = punc_end;
                    results.emplace_back(punc,
                                         punc_begin + search_start,
                                         punc_end + search_start,
                                         Token::Kind{std::in_place_type<Punctuation>});
                }

                const auto identifiers_or_literals = split(stripped, is_punctuation);
                auto ident_start = 0_usize;
                for(auto ident : identifiers_or_literals) {
                    const auto ident_begin = stripped.find(ident, ident_start);
                    const auto ident_end = ident_begin + ident.size();
                    ident_start = ident_end;
                    const auto is_string = ident.front() == '"' && ident.back() == '"';
                    const auto is_keyword = ranges::contains(Lexer::keywords, ident);
                    const auto is_numeric = ranges::all_of(ident, [](const char& elem) {
                        return elem >= '0' && elem <= '9';
                    });
                    results.emplace_back(
                        ident,
                        ident_begin + search_start,
                        ident_end + search_start,
                        is_string ?
                            Token::Kind{std::in_place_type<String>} :
                            (is_numeric ?
                                 Token::Kind{std::in_place_type<Numeric>} :
                                 (is_keyword ? Token::Kind{std::in_place_type<Identifier>,
                                                           std::in_place_type<Keyword>} :
                                               Token::Kind{std::in_place_type<Identifier>,
                                                           std::in_place_type<Namespace>})));
                }
            }
        }

        return results;
    }
} // namespace hyperion::assert::detail
