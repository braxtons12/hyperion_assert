/// @file decomposer.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Expression decomposition helper types
/// @version 0.1
/// @date 2024-03-16
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

#ifndef HYPERION_ASSERT_DETAIL_DECOMPOSER_H
#define HYPERION_ASSERT_DETAIL_DECOMPOSER_H

#include <hyperion/assert/detail/highlight.h>
#include <hyperion/assert/detail/tokens.h>
#include <hyperion/mpl/concepts.h>
#include <hyperion/mpl/type.h>
#include <hyperion/platform/def.h>

#include <fmt/format.h>

#include <concepts>
#include <sstream>
#include <string_view>
#include <utility>

namespace hyperion::assert::detail {

    template<typename TType>
    concept OutputStreamable
        = requires(const TType& type, std::stringstream& stream) { stream << type; };

    template<typename TType>
    using storage_type
        = std::conditional_t<mpl::decltype_<TType>().is_lvalue_reference()
                                 && not mpl::decltype_<TType>().is_trivially_copy_constructible(),
                             TType,
                             std::remove_cvref_t<TType>>;
    template<typename TType>
    using reference_type
        = std::conditional_t<mpl::decltype_<TType>().is_lvalue_reference(),
                             TType,
                             std::add_lvalue_reference_t<std::remove_cvref_t<TType>>>;

    template<typename TExpr>
    class UnaryExpression final {
      public:
        template<typename TType>
            requires(static_cast<bool>(mpl::decltype_<storage_type<TExpr>>().is_constructible_from(
                mpl::decltype_<TType>())))
        explicit constexpr UnaryExpression(TType&& expr) noexcept(
            mpl::decltype_<storage_type<TExpr>>().is_nothrow_constructible_from(
                mpl::decltype_<TType>()))
            : m_expr{std::forward<TType>(expr)} {
        }

        [[nodiscard]] constexpr auto expr() noexcept -> reference_type<TExpr> {
            return m_expr;
        }

      private:
        // NOLINTNEXTLINE(*-avoid-const-or-ref-data-members)
        storage_type<TExpr> m_expr;
    };

    HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;

    template<usize TSize>
        requires(TSize != 0)
    struct FixedString final {
        static constexpr auto k_size = TSize;
        // NOLINTNEXTLINE(*-c-arrays)
        char data[TSize + 1_usize] = {};

        // NOLINTNEXTLINE(*-c-arrays, *-explicit-*)
        constexpr explicit(false) FixedString(const char (&str)[TSize + 1_usize]) noexcept {
            for(auto index = static_cast<usize>(0); index < TSize; ++index) {
                // NOLINTNEXTLINE(*-pro-bounds-constant-array-index)
                data[index] = str[index];
            }
        }

        [[nodiscard]] constexpr auto size() const noexcept -> usize {
            return TSize;
        }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool {
            return false;
        }

        // NOLINTNEXTLINE(*-explicit-*)
        [[nodiscard]] constexpr explicit(false) operator std::string_view() const noexcept {
            return {data, TSize};
        }
    };
    template<usize TSize>
    // NOLINTNEXTLINE(*-c-arrays)
    FixedString(const char (&str)[TSize]) -> FixedString<TSize - 1>;

    HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;

    template<FixedString TOp>
    struct Operator;

#define HYPERION_DEFINE_OPERATOR_TYPE(oper) /** NOLINT(*-macro-usage) **/                          \
    template<>                                                                                     \
    struct Operator<#oper> final {                                                                 \
        static constexpr auto k_op = std::string_view{#oper};                                      \
                                                                                                   \
        template<typename TLhs, typename TRhs>                                                     \
        [[nodiscard]] static constexpr auto                                                        \
        do_op(TLhs&& lhs,                                                                          \
              TRhs&& rhs) noexcept(noexcept(std::forward<TLhs>(lhs) oper std::forward<TRhs>(rhs))) \
            -> decltype(std::forward<TLhs>(lhs) oper std::forward<TRhs>(rhs))                      \
            requires requires { std::forward<TLhs>(lhs) oper std::forward<TRhs>(rhs); }            \
        {                                                                                          \
            return std::forward<TLhs>(lhs) oper std::forward<TRhs>(rhs);                           \
        }                                                                                          \
                                                                                                   \
        [[nodiscard]] static constexpr auto operator_() noexcept -> std::string_view {             \
            return k_op;                                                                           \
        }                                                                                          \
    }

    HYPERION_DEFINE_OPERATOR_TYPE(+);
    HYPERION_DEFINE_OPERATOR_TYPE(-);
    HYPERION_DEFINE_OPERATOR_TYPE(*);
    HYPERION_DEFINE_OPERATOR_TYPE(/);
    HYPERION_DEFINE_OPERATOR_TYPE(%);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(<<);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(>>);
    HYPERION_DEFINE_OPERATOR_TYPE(<=>);
    HYPERION_DEFINE_OPERATOR_TYPE(<);
    HYPERION_DEFINE_OPERATOR_TYPE(<=);
    HYPERION_DEFINE_OPERATOR_TYPE(>);
    HYPERION_DEFINE_OPERATOR_TYPE(>=);
    HYPERION_DEFINE_OPERATOR_TYPE(==);
    HYPERION_DEFINE_OPERATOR_TYPE(!=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(&);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(|);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(^);
    HYPERION_DEFINE_OPERATOR_TYPE(&&);
    HYPERION_DEFINE_OPERATOR_TYPE(||);
    HYPERION_DEFINE_OPERATOR_TYPE(=);
    HYPERION_DEFINE_OPERATOR_TYPE(+=);
    HYPERION_DEFINE_OPERATOR_TYPE(-=);
    HYPERION_DEFINE_OPERATOR_TYPE(*=);
    HYPERION_DEFINE_OPERATOR_TYPE(/=);
    HYPERION_DEFINE_OPERATOR_TYPE(%=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(<<=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(>>=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(&=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(|=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_OPERATOR_TYPE(^=);

    template<>
    struct Operator<","> final {
        static constexpr auto k_op = std::string_view{","};

        template<typename TLhs, typename TRhs>
        [[nodiscard]] static constexpr auto
        do_op(TLhs&& lhs,
              TRhs&& rhs) noexcept(noexcept((std::forward<TLhs>(lhs), std::forward<TRhs>(rhs))))
            -> decltype((std::forward<TLhs>(lhs), std::forward<TRhs>(rhs)))
            requires requires { (std::forward<TLhs>(lhs), std::forward<TRhs>(rhs)); }
        {
            HYPERION_IGNORE_UNUSED_VALUES_WARNING_START;
            return (std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
            HYPERION_IGNORE_UNUSED_VALUES_WARNING_STOP;
        }

        [[nodiscard]] static constexpr auto operator_() noexcept -> std::string_view {
            return k_op;
        }
    };

#undef HYPERION_DEFINE_OPERATOR_TYPE

    template<typename TLhs, typename TRhs, FixedString TOp>
        requires requires {
            Operator<TOp>::do_op(std::declval<TLhs>().do_op(),
                                 std::declval<reference_type<TRhs>>());
        } or requires {
            Operator<TOp>::do_op(std::declval<reference_type<TLhs>>(),
                                 std::declval<reference_type<TRhs>>());
        }
    class BinaryExpression final {
      private:
        // NOLINTNEXTLINE(*-avoid-const-or-ref-data-members)
        storage_type<TLhs> m_lhs;
        // NOLINTNEXTLINE(*-avoid-const-or-ref-data-members)
        storage_type<TRhs> m_rhs;

        [[nodiscard]] static constexpr auto
        call_operator(reference_type<TLhs> lhs, reference_type<TRhs> rhs)
            requires requires { Operator<TOp>::do_op(lhs.do_op(), rhs); }
        {
            return Operator<TOp>::do_op(lhs.do_op(), rhs);
        }

        [[nodiscard]] static constexpr auto
        call_operator(reference_type<TLhs> lhs, reference_type<TRhs> rhs)
            requires requires { Operator<TOp>::do_op(lhs, rhs); }
        {
            return Operator<TOp>::do_op(lhs, rhs);
        }

      public:
        using result_type = decltype(call_operator(m_lhs, m_rhs));
        using operator_type = Operator<TOp>;
        static constexpr auto k_lhs_is_binary_expression = requires {
            typename TLhs::result_type;
            typename TLhs::operator_type;
        };
        static constexpr auto k_is_binary_expression = true;

        template<typename TArgLhs, typename TArgRhs>
            requires(static_cast<bool>(mpl::decltype_<storage_type<TLhs>>().is_constructible_from(
                        mpl::decltype_<TArgLhs>())))
                        && (static_cast<bool>(
                            mpl::decltype_<storage_type<TRhs>>().is_constructible_from(
                                mpl::decltype_<TArgRhs>())))
        constexpr BinaryExpression(TArgLhs&& lhs, TArgRhs&& rhs) noexcept
            : m_lhs{std::forward<TArgLhs>(lhs)}, m_rhs{std::forward<TArgRhs>(rhs)} {
        }

        [[nodiscard]] constexpr auto
        do_op() noexcept(noexcept(Operator<TOp>::do_op(m_lhs, m_rhs))) -> result_type
            requires(not k_lhs_is_binary_expression)
        {
            return Operator<TOp>::do_op(m_lhs, m_rhs);
        }

        [[nodiscard]] constexpr auto
        do_op() noexcept(noexcept(Operator<TOp>::do_op(m_lhs.do_op(), m_rhs))) -> result_type
            requires k_lhs_is_binary_expression
        {
            return Operator<TOp>::do_op(m_lhs.do_op(), m_rhs);
        }

        template<typename TFarRhs>
        // NOLINTNEXTLINE(misc-unconventional-assign-operator, *-assignment-signature)
        constexpr auto operator=(TFarRhs&& rhs) noexcept(
            noexcept(std::declval<BinaryExpression>().do_op()) and noexcept(
                std::declval<BinaryExpression>().do_op() = std::forward<TFarRhs>(rhs)))
            -> BinaryExpression<BinaryExpression<TLhs, TRhs, TOp>, TFarRhs, "=">
            requires requires { do_op() = std::forward<TFarRhs>(rhs); }
        {
            return {*this, std::forward<TFarRhs>(rhs)};
        }

        [[nodiscard]] constexpr auto lhs() noexcept -> reference_type<TLhs> {
            return m_lhs;
        }

        [[nodiscard]] constexpr auto rhs() noexcept -> reference_type<TRhs> {
            return m_rhs;
        }

        [[nodiscard]] constexpr auto operator_() noexcept -> std::string_view {
            return Operator<TOp>::operator_();
        }

        [[nodiscard]] constexpr auto
        expr() noexcept(noexcept(std::declval<BinaryExpression>().do_op())) -> result_type
            requires requires { std::declval<BinaryExpression>().do_op(); }
        {
            return do_op();
        }
    };

#define HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(oper) /** NOLINT(*-macro-usage) **/             \
    template<typename TLhs, typename TRhs, FixedString TOp, typename TFarRhs>                      \
    constexpr auto operator oper(BinaryExpression<TLhs, TRhs, TOp>&& lhs, TFarRhs&& rhs) noexcept( \
        noexcept(std::move(lhs).do_op() oper std::forward<TFarRhs>(rhs)))                          \
        -> BinaryExpression<BinaryExpression<TLhs, TRhs, TOp>, TFarRhs, #oper>                     \
        requires requires { std::move(lhs).do_op() oper std::forward<TFarRhs>(rhs); }              \
    {                                                                                              \
        return {std::move(lhs), std::forward<TFarRhs>(rhs)};                                       \
    }

    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(+);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(-);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(*);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(/);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(%);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(<<);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(>>);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(<=>);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(<);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(<=);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(>);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(>=);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(==);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(!=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(&);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(|);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(^);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(&&);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(||);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(+=);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(-=);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(*=);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(/=);
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(%=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(<<=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(>>=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(&=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(|=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR(^=);

    template<typename TLhs, typename TRhs, FixedString TOp, typename TFarRhs>
    constexpr auto operator,(BinaryExpression<TLhs, TRhs, TOp>&& lhs,
                             TFarRhs&& rhs) noexcept(noexcept((std::move(lhs).do_op(),
                                                               std::forward<TFarRhs>(rhs))))
        -> BinaryExpression<BinaryExpression<TLhs, TRhs, TOp>, TFarRhs, ",">
        requires requires { (std::move(lhs).do_op(), std::forward<TFarRhs>(rhs)); }
    {
        return {std::move(lhs), std::forward<TFarRhs>(rhs)};
    }

#undef HYPERION_DEFINE_BINARY_EXPRESSION_OPERATOR

    template<typename TType>
    concept IsBinaryExpression = requires { TType::k_is_binary_expression; }
                                 && requires { requires TType::k_is_binary_expression; };

    template<typename TExpr>
    class InitialExpression {
      public:
        template<typename TType>
            requires(static_cast<bool>(mpl::decltype_<storage_type<TExpr>>().is_constructible_from(
                mpl::decltype_<TType>())))
        // NOLINTNEXTLINE(*-explicit-*)
        constexpr explicit(false) InitialExpression(TType&& expr) noexcept(
            mpl::decltype_<storage_type<TExpr>>().is_noexcept_constructible_from(
                mpl::decltype_<TType>()))
            : m_expr{std::forward<TType>(expr)} {
        }

        // NOLINTNEXTLINE(*-explicit-*)
        [[nodiscard]] explicit(false) constexpr operator UnaryExpression<TExpr>() noexcept {
            return {m_expr};
        }

        [[nodiscard]] constexpr auto expr() noexcept -> reference_type<TExpr> {
            return m_expr;
        }

      private:
        // NOLINTNEXTLINE(*-avoid-const-or-ref-data-members)
        storage_type<TExpr> m_expr;
    };

#define HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(oper) /** NOLINT(*-macro-usage) **/ \
    template<typename TLhs, typename TRhs>                                              \
    constexpr auto operator oper(InitialExpression<TLhs>&& lhs, TRhs&& rhs) noexcept(   \
        noexcept(std::move(lhs).expr() oper std::forward<TRhs>(rhs)))                   \
        -> BinaryExpression<TLhs, TRhs, #oper>                                          \
        requires requires { std::move(lhs).expr() oper std::forward<TRhs>(rhs); }       \
    {                                                                                   \
        return {std::move(lhs).expr(), std::forward<TRhs>(rhs)};                        \
    }

    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(+);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(-);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(*);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(/);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(%);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(<<);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(>>);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(<=>);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(<);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(<=);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(>);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(>=);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(==);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(!=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(&);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(|);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(^);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(&&);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(||);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(+=);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(-=);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(*=);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(/=);
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(%=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(<<=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(>>=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(&=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(|=);
    // NOLINTNEXTLINE(*-signed-bitwise)
    HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR(^=);

    template<typename TLhs, typename TRhs>
    constexpr auto operator,(InitialExpression<TLhs>&& lhs,
                             TRhs&& rhs) noexcept(noexcept((std::move(lhs).expr(),
                                                            std::forward<TRhs>(rhs))))
        -> BinaryExpression<TLhs, TRhs, ",">
        requires requires { (std::move(lhs).expr(), std::forward<TRhs>(rhs)); }
    {
        return {std::move(lhs).expr(), std::forward<TRhs>(rhs)};
    }

#undef HYPERION_DEFINE_INITIAL_EXPRESSION_OPERATOR

    struct ExpressionDecomposer {
        template<typename TLhs>
        friend constexpr auto
        // NOLINTNEXTLINE(*-rvalue-reference-param-not-moved)
        operator->*([[maybe_unused]] ExpressionDecomposer && decomposer, TLhs && lhs)
            -> InitialExpression<TLhs> {
            return {std::forward<TLhs>(lhs)};
        }
    };

} // namespace hyperion::assert::detail

template<typename TExpr>
struct fmt::formatter<hyperion::assert::detail::UnaryExpression<TExpr>> {
  private:
    static inline constexpr auto k_formattable = is_formattable<TExpr>::value;

  public:
    using self = hyperion::assert::detail::UnaryExpression<TExpr>;

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    template<typename TFormatContext>
    [[nodiscard]] auto format(const self& expression, TFormatContext& context) {
        using hyperion::assert::detail::IsBinaryExpression;
        using hyperion::assert::detail::OutputStreamable;
        using hyperion::assert::detail::highlight::highlight;

        if constexpr(k_formattable) {
            fmt::format_to(context.out(), "{}", highlight(expression.expr()));
        }
        else if constexpr(OutputStreamable<TExpr>) {
            std::stringstream stream;
            stream << expression.expr();
            return fmt::format_to(context.out(), "{}", highlight(stream.str()));
        }
        else {
            return fmt::format_to(context.out(), highlight("(NotFormattable)"));
        }
    }
};

template<typename TExpr>
struct fmt::formatter<hyperion::assert::detail::InitialExpression<TExpr>> {
  public:
    using self = hyperion::assert::detail::InitialExpression<TExpr>;

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    template<typename TFormatContext>
    [[nodiscard]] auto format(self& expression, TFormatContext& context) {
        using hyperion::assert::detail::UnaryExpression;
        return fmt::format_to(context.out(), "{}", static_cast<UnaryExpression<TExpr>>(expression));
    }
};

template<typename TLhs, typename TRhs, hyperion::assert::detail::FixedString TOp>
struct fmt::formatter<hyperion::assert::detail::BinaryExpression<TLhs, TRhs, TOp>> {
  private:
    template<typename TType>
    static inline constexpr auto formattable = is_formattable<TType>::value;

  public:
    using self = hyperion::assert::detail::BinaryExpression<TLhs, TRhs, TOp>;

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    template<typename TFormatContext>
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    [[nodiscard]] auto format(self& expression, TFormatContext& context) {
        using hyperion::assert::detail::IsBinaryExpression;
        using hyperion::assert::detail::OutputStreamable;
        using hyperion::assert::detail::highlight::get_color;
        using hyperion::assert::detail::highlight::highlight;
        using hyperion::assert::detail::tokens::Punctuation;
        using hyperion::assert::detail::tokens::Token;

        if constexpr(IsBinaryExpression<TLhs>) {
            const auto punc_color = get_color(Token::Kind{std::in_place_type<Punctuation>});

            if constexpr(formattable<TRhs>) {
                return fmt::format_to(context.out(),
                                      "{}{}{} {} {}",
                                      fmt::styled('(', fmt::fg(punc_color)),
                                      expression.lhs(),
                                      fmt::styled(')', fmt::fg(punc_color)),
                                      fmt::styled(expression.operator_(), fmt::fg(punc_color)),
                                      highlight(fmt::format("{}", expression.rhs())));
            }
            else if constexpr(OutputStreamable<TRhs>) {
                std::stringstream stream;
                stream << expression.rhs();
                return fmt::format_to(context.out(),
                                      "{}{}{} {} {}",
                                      fmt::styled('(', fmt::fg(punc_color)),
                                      expression.lhs(),
                                      fmt::styled(')', fmt::fg(punc_color)),
                                      fmt::styled(expression.operator_(), fmt::fg(punc_color)),
                                      highlight(stream.str()));
            }
            else {
                return fmt::format_to(context.out(),
                                      "{}{}{} {} {}",
                                      fmt::styled('(', fmt::fg(punc_color)),
                                      expression.lhs(),
                                      fmt::styled(')', fmt::fg(punc_color)),
                                      fmt::styled(expression.operator_(), fmt::fg(punc_color)),
                                      highlight("(NotFormattable)"));
            }
        }
        else {
            // NOLINTNEXTLINE(readability-function-cognitive-complexity)
            const auto formatted = [&expression]() {
                if constexpr(formattable<TLhs> && formattable<TRhs>) {
                    if constexpr(std::convertible_to<TLhs, std::string_view>
                                 && std::convertible_to<TRhs, std::string_view>)
                    {
                        return fmt::format(R"("{}" {} "{}")",
                                           expression.lhs(),
                                           expression.operator_(),
                                           expression.rhs());
                    }
                    else if constexpr(std::convertible_to<TLhs, std::string_view>) {
                        return fmt::format(R"("{}" {} {})",
                                           expression.lhs(),
                                           expression.operator_(),
                                           expression.rhs());
                    }
                    else if constexpr(std::convertible_to<TRhs, std::string_view>) {
                        return fmt::format(R"({} {} "{}")",
                                           expression.lhs(),
                                           expression.operator_(),
                                           expression.rhs());
                    }
                    else {
                        return fmt::format("{} {} {}",
                                           expression.lhs(),
                                           expression.operator_(),
                                           expression.rhs());
                    }
                }
                if constexpr(formattable<TLhs>) {
                    if constexpr(std::convertible_to<TLhs, std::string_view>) {
                        if constexpr(OutputStreamable<TRhs>) {
                            std::stringstream stream;
                            stream << expression.rhs();
                            return fmt::format(R"("{}" {} {})",
                                               expression.lhs(),
                                               expression.operator_(),
                                               stream.str());
                        }
                        else {
                            return fmt::format(R"("{}" {} (NotFormattable))",
                                               expression.lhs(),
                                               expression.operator_());
                        }
                    }
                    else {
                        if constexpr(OutputStreamable<TRhs>) {
                            std::stringstream stream;
                            stream << expression.rhs();
                            return fmt::format("{} {} {}",
                                               expression.lhs(),
                                               expression.operator_(),
                                               stream.str());
                        }
                        else {
                            return fmt::format("{} {} (NotFormattable)",
                                               expression.lhs(),
                                               expression.operator_());
                        }
                    }
                }
                else if constexpr(formattable<TRhs>) {
                    if constexpr(std::convertible_to<TRhs, std::string_view>) {
                        if constexpr(OutputStreamable<TLhs>) {
                            std::stringstream stream;
                            stream << expression.lhs();
                            return fmt::format(R"({} {} "{}")",
                                               stream.str(),
                                               expression.operator_(),
                                               expression.rhs());
                        }
                        else {
                            return fmt::format(R"((NotFormattable) {} "{}")",
                                               expression.operator_(),
                                               expression.rhs());
                        }
                    }
                    else {
                        if constexpr(OutputStreamable<TLhs>) {
                            std::stringstream stream;
                            stream << expression.lhs();
                            return fmt::format("{} {} {}",
                                               stream.str(),
                                               expression.operator_(),
                                               expression.rhs());
                        }
                        else {
                            return fmt::format("(NotFormattable) {} {}",
                                               expression.operator_(),
                                               expression.rhs());
                        }
                    }
                }
                else if constexpr(OutputStreamable<TLhs> && OutputStreamable<TRhs>) {
                    std::stringstream stream_lhs;
                    stream_lhs << expression.lhs();
                    std::stringstream stream_rhs;
                    stream_rhs << expression.rhs();
                    return fmt::format("{} {} {}",
                                       stream_lhs.str(),
                                       expression.operator_(),
                                       stream_rhs.str());
                }
                else if constexpr(OutputStreamable<TLhs>) {
                    std::stringstream stream;
                    stream << expression.lhs();
                    return fmt::format("{} {} (NotFormattable)",
                                       stream.str(),
                                       expression.operator_());
                }
                else if constexpr(OutputStreamable<TRhs>) {
                    std::stringstream stream;
                    stream << expression.rhs();
                    return fmt::format("(NotFormattable) {} {}",
                                       expression.operator_(),
                                       stream.str());
                }
                else {
                    return fmt::format("(NotFormattable) {} (NotFormattable)",
                                       expression.operator_());
                }
            }();

            return fmt::format_to(context.out(), "{}", highlight(formatted));
        }
    }
};

#if HYPERION_ENABLE_TESTING

HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_START;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
    #include <boost/ut.hpp>
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_STOP;

    #include <string_view>

namespace hyperion::_test::assert::detail::decomposer {

    // NOLINTNEXTLINE(google-build-using-namespace)
    using namespace boost::ut;

    // NOLINTNEXTLINE(google-build-using-namespace)
    using hyperion::assert::detail::BinaryExpression;
    using hyperion::assert::detail::ExpressionDecomposer;
    using hyperion::assert::detail::InitialExpression;
    using hyperion::assert::detail::UnaryExpression;
    using hyperion::assert::detail::highlight::highlight;

    // NOLINTNEXTLINE(cert-err58-cpp)
    static const suite<"hyperion::assert::detail::decomposer"> assert_decomposer_tests = [] {
        "+"_test = [] {
            auto result = ExpressionDecomposer{}->*1 + 2;
            expect(that % result.expr() == 3);
            fmt::println(stderr, "{}", result);
        };

        "+=="_test = [] {
            auto result = ExpressionDecomposer{}->*1 + 2 == 3;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==+"_test = [] {
            auto result = ExpressionDecomposer{}->*3 == 1 + 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "-"_test = [] {
            auto result = ExpressionDecomposer{}->*1 - 2;
            expect(that % result.expr() == -1);
            fmt::println(stderr, "{}", result);
        };

        "-=="_test = [] {
            auto result = ExpressionDecomposer{}->*1 - 2 == -1;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==-"_test = [] {
            auto result = ExpressionDecomposer{}->*-1 == 1 - 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "*"_test = [] {
            auto result = ExpressionDecomposer{}->*2 * 2;
            expect(that % result.expr() == 4);
            fmt::println(stderr, "{}", result);
        };

        "*=="_test = [] {
            auto result = ExpressionDecomposer{}->*2 * 2 == 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==*"_test = [] {
            auto result = ExpressionDecomposer{}->*4 == 2 * 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "/"_test = [] {
            auto result = ExpressionDecomposer{}->*4 / 2;
            expect(that % result.expr() == 2);
            fmt::println(stderr, "{}", result);
        };

        "/=="_test = [] {
            auto result = ExpressionDecomposer{}->*4 / 2 == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==/"_test = [] {
            auto result = ExpressionDecomposer{}->*2 == 4 / 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "%"_test = [] {
            auto result = ExpressionDecomposer{}->*5_i32 % 3;
            expect(that % result.expr() == 2);
            fmt::println(stderr, "{}", result);
        };

        "%=="_test = [] {
            auto result = ExpressionDecomposer{}->*5_i32 % 3 == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==%"_test = [] {
            auto result = ExpressionDecomposer{}->*2 == 5_i32 % 3;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        _Pragma("GCC diagnostic push");
        _Pragma("GCC diagnostic ignored \"-Woverloaded-shift-op-parentheses\"");
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

        "<<"_test = [] {
            auto result = ExpressionDecomposer{}->*2 << 2;
            expect(that % result.expr() == 8_i32);
            fmt::println(stderr, "{}", result);
        };

        "<<=="_test = [] {
            auto result = ExpressionDecomposer{}->*2 << 2 == 8_i32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==<<"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*8_i32 == 2 << 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ">>"_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 >> 2;
            expect(that % result.expr() == 2);
            fmt::println(stderr, "{}", result);
        };

        ">>=="_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 >> 2 == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==>>"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*2 == 8_i32 >> 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "<=>"_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 <=> 2;
            expect(result.expr() == std::strong_ordering::greater);
            fmt::println(stderr, "{}", result);
        };

        "<=>=="_test = [] {
            auto result = ExpressionDecomposer{}->*8_i32 <=> 2 == std::strong_ordering::greater;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==<=>"_test = [] {
            auto result = ExpressionDecomposer{}->*std::strong_ordering::greater == 8_i32 <=> 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(result.expr());
            fmt::println(stderr, "{}", result);
        };

    #if HYPERION_PLATFORM_COMPILER_IS_GCC
        _Pragma("GCC diagnostic push");
        _Pragma("GCC diagnostic ignored \"-Wparentheses\"");
    #endif // HYPERION_PLATFORM_COMPILER_IS_GCC

        "<"_test = [] {
            auto result = ExpressionDecomposer{}->*2 < 4;
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "<=="_test = [] {
            auto result = ExpressionDecomposer{}->*2 < 4 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==<"_test = [] {
            auto result = ExpressionDecomposer{}->*true == 2 <= 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "<="_test = [] {
            auto result = ExpressionDecomposer{}->*2 <= 4;
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "<==="_test = [] {
            auto result = ExpressionDecomposer{}->*2 <= 4 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==<="_test = [] {
            auto result = ExpressionDecomposer{}->*true == 2 <= 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ">"_test = [] {
            auto result = ExpressionDecomposer{}->*4 > 2;
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ">=="_test = [] {
            auto result = ExpressionDecomposer{}->*4 > 2 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==>"_test = [] {
            auto result = ExpressionDecomposer{}->*true == 4 >= 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ">="_test = [] {
            auto result = ExpressionDecomposer{}->*4 >= 2;
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ">==="_test = [] {
            auto result = ExpressionDecomposer{}->*4 >= 2 == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==>="_test = [] {
            auto result = ExpressionDecomposer{}->*true == 4 >= 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "=="_test = [] {
            auto result
                = ExpressionDecomposer{}->*std::string_view{"hello"} == std::string_view{"hello"};
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "===="_test = [] {
            auto result = ExpressionDecomposer{}->*std::string_view{"hello"}
                          == std::string_view{"hello"} == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "====reversed"_test = [] {
            auto result = ExpressionDecomposer{}->*true
                          == (std::string_view{"hello"} == std::string_view{"hello"});
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "!="_test = [] {
            auto result
                = ExpressionDecomposer{}->*std::string_view{"hello"} != std::string_view{"world"};
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "!==="_test = [] {
            auto result = ExpressionDecomposer{}->*std::string_view{"hello"}
                          != std::string_view{"world"} == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==!="_test = [] {
            auto result = ExpressionDecomposer{}->*true
                          == (std::string_view{"hello"} != std::string_view{"world"});
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "&"_test = [] {
            auto result = ExpressionDecomposer{}->*0b1100_u32 & 0b1000_u32;
            expect(that % result.expr() == 0b1000_u32);
            fmt::println(stderr, "{}", result);
        };

        "&=="_test = [] {
            auto result = ExpressionDecomposer{}->*(0b1100_u32 & 0b1000_u32) == 0b1000_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==&"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*0b1000_u32 == (0b1100_u32 & 0b1000_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "|"_test = [] {
            auto result = ExpressionDecomposer{}->*0b1100_u32 | 0b0001_u32;
            expect(that % result.expr() == 0b1101_u32);
            fmt::println(stderr, "{}", result);
        };

        "|=="_test = [] {
            auto result = ExpressionDecomposer{}->*(0b1100_u32 | 0b0001_u32) == 0b1101_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==|"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*0b1101_u32 == (0b1100_u32 | 0b0001_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "^"_test = [] {
            auto result = ExpressionDecomposer{}->*0b1100_u32 ^ 0b0101_u32;
            expect(that % result.expr() == 0b1001_u32);
            fmt::println(stderr, "{}", result);
        };

        "^=="_test = [] {
            auto result = ExpressionDecomposer{}->*(0b1100_u32 ^ 0b0101_u32) == 0b1001_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==^"_test = [] {
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*0b1001_u32 == (0b1100_u32 ^ 0b0101_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "&&"_test = [] {
            auto result = ExpressionDecomposer{}->*true && false;
            expect(that % result.expr() == false);
            fmt::println(stderr, "{}", result);
        };

        "&&=="_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*(true && false) == false;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==&&"_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*false == (true && false);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "||"_test = [] {
            auto result = ExpressionDecomposer{}->*false || true;
            expect(that % result.expr() == true);
            fmt::println(stderr, "{}", result);
        };

        "||=="_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*(false || true) == true;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==||"_test = [] {
            // NOLINTNEXTLINE(readability-simplify-boolean-expr)
            auto result = ExpressionDecomposer{}->*true == (false || true);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "+="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value += 1;
            expect(that % result.expr() == 3);
            fmt::println(stderr, "{}", result);
        };

        "+==="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*(value += 1) == 3;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==+="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*3 == (value += 1);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "-="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value -= 1;
            expect(that % result.expr() == 1);
            fmt::println(stderr, "{}", result);
        };

        "-==="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*(value -= 1) == 1;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==-="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*1 == (value -= 1);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "*="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value *= 2;
            expect(that % result.expr() == 4);
            fmt::println(stderr, "{}", result);
        };

        "*==="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*(value *= 2) == 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==*="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*4 == (value *= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "/="_test = [] {
            auto value = 4;
            auto result = ExpressionDecomposer{}->*value /= 2;
            expect(that % result.expr() == 2);
            fmt::println(stderr, "{}", result);
        };

        "/==="_test = [] {
            auto value = 4;
            auto result = ExpressionDecomposer{}->*(value /= 2) == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==/="_test = [] {
            auto value = 4;
            auto result = ExpressionDecomposer{}->*2 == (value /= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "%="_test = [] {
            auto value = 5_i32;
            auto result = ExpressionDecomposer{}->*value %= 3;
            expect(that % result.expr() == 2);
            fmt::println(stderr, "{}", result);
        };

        "%==="_test = [] {
            auto value = 5_i32;
            auto result = ExpressionDecomposer{}->*(value %= 3) == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==%="_test = [] {
            auto value = 5_i32;
            auto result = ExpressionDecomposer{}->*2 == (value %= 3);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "<<="_test = [] {
            auto value = 2;
            auto result = ExpressionDecomposer{}->*value <<= 2;
            expect(that % result.expr() == 8_i32);
            fmt::println(stderr, "{}", result);
        };

        "<<==="_test = [] {
            auto value = 2;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*(value <<= 2) == 8_i32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==<<="_test = [] {
            auto value = 2;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*8_i32 == (value <<= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ">>="_test = [] {
            auto value = 8_i32;
            auto result = ExpressionDecomposer{}->*value >>= 2;
            expect(that % result.expr() == 2);
            fmt::println(stderr, "{}", result);
        };

        ">>==="_test = [] {
            auto value = 8_i32;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*(value >>= 2) == 2;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==>>="_test = [] {
            auto value = 8_i32;
            // NOLINTNEXTLINE(*-signed-bitwise)
            auto result = ExpressionDecomposer{}->*2 == (value >>= 2);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "&="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*value &= 0b0110_u32;
            expect(that % result.expr() == 0b0100_u32);
            fmt::println(stderr, "{}", result);
        };

        "&==="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*(value &= 0b0110_u32) == 0b0100_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==&="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*0b0100_u32 == (value &= 0b0110_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "|="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*value |= 0b0110_u32;
            expect(that % result.expr() == 0b1110_u32);
            fmt::println(stderr, "{}", result);
        };

        "|==="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*(value |= 0b0110_u32) == 0b1110_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==|="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*0b1110_u32 == (value |= 0b0110_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "^="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*value ^= 0b0110_u32;
            expect(that % result.expr() == 0b1010_u32);
            fmt::println(stderr, "{}", result);
        };

        "^==="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*(value ^= 0b0110_u32) == 0b1010_u32;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==^="_test = [] {
            auto value = 0b1100_u32;
            auto result = ExpressionDecomposer{}->*0b1010_u32 == (value ^= 0b0110_u32);
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        ","_test = [] {
            auto result = (ExpressionDecomposer{}->*2, 4);
            expect(that % result.expr() == 4);
            fmt::println(stderr, "{}", result);
        };

        ",=="_test = [] {
            auto result = (ExpressionDecomposer{}->*2, 4) == 4;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

        "==,"_test = [] {
            auto value = 2;
            HYPERION_IGNORE_COMMA_MISUSE_WARNING_START;
            auto result = ExpressionDecomposer{}->*4 == (value += 1, 4);
            HYPERION_IGNORE_COMMA_MISUSE_WARNING_STOP;
            static_assert(std::same_as<std::remove_cvref_t<decltype(result.expr())>, bool>);
            expect(that % value == 3);
            expect(that % result.expr());
            fmt::println(stderr, "{}", result);
        };

    #if HYPERION_PLATFORM_COMPILER_IS_GCC
        _Pragma("GCC diagnostic pop");
    #endif // HYPERION_PLATFORM_COMPILER_IS_GCC

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        _Pragma("GCC diagnostic pop");
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG
    };

} // namespace hyperion::_test::assert::detail::decomposer

#endif // HYPERION_ENABLE_TESTING

#endif // HYPERION_ASSERT_DETAIL_DECOMPOSER_H
