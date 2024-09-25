/// @file decomposer.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Expression decomposition helper types
/// @version 0.1
/// @date 2024-09-24
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

#include <hyperion/assert/highlight.h>
#include <hyperion/assert/tokens.h>
#include <hyperion/mpl/concepts.h>
#include <hyperion/mpl/type.h>
#include <hyperion/platform/compare.h>
#include <hyperion/platform/def.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/format.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <concepts>
#include <sstream>
#include <string_view>
#include <utility>

namespace hyperion::assert::detail {

    // clang still doesn't understand tparam tags for concepts
    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        HYPERION_IGNORE_DOCUMENTATION_WARNING_START;
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

    /// @brief A type is `OutputStreamable` if it can be serialized into an output stream
    /// @tparam TType the type to check
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<typename TType>
    concept OutputStreamable
        = requires(const TType& type, std::stringstream& stream) { stream << type; };

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        HYPERION_IGNORE_DOCUMENTATION_WARNING_STOP;
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

    /// @brief Decomposers store trivially copyable types as copies of those types,
    /// and non-trivially copyable types as references
    /// @tparam TType the type to determine how it should be stored
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<typename TType>
    using storage_type
        = std::conditional_t<mpl::decltype_<TType>().is_lvalue_reference()
                                 && not mpl::decltype_<TType>().is_trivially_copy_constructible(),
                             TType,
                             std::remove_cvref_t<TType>>;
    /// @brief Converts `TType` to the appropriate reference type for it
    /// If `TType` is a reference, maps to `TType`.
    /// Otherwise, maps to the lvalue reference type for `TType`
    /// @tparam TType The type to map to its appropriate reference type
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<typename TType>
    using reference_type
        = std::conditional_t<mpl::decltype_<TType>().is_lvalue_reference(),
                             TType,
                             std::add_lvalue_reference_t<std::remove_cvref_t<TType>>>;

    /// @brief Stores the result of a unary (operator) expression
    /// @tparam TExpr The result type of the expression
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<typename TExpr>
    class UnaryExpression final {
      public:
        /// @brief Construct a `UnaryExpression` from the result of an arbitrary unary expression
        ///
        /// # Requirements
        /// - `storage_type<TExpr>` must be constructible from `TType`
        ///
        /// @tparam TType The result type of the expression
        /// @param expr The result of the expression
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        template<typename TType>
            requires(static_cast<bool>(mpl::decltype_<storage_type<TExpr>>().is_constructible_from(
                mpl::decltype_<TType>())))
        explicit constexpr UnaryExpression(TType&& expr) noexcept(
            mpl::decltype_<storage_type<TExpr>>().is_nothrow_constructible_from(
                mpl::decltype_<TType>()))
            : m_expr{std::forward<TType>(expr)} {
        }

        /// @brief Returns the result of the constructing expression as a reference
        /// @return The result of the expression
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto expr() noexcept -> reference_type<TExpr> {
            return m_expr;
        }

      private:
        // NOLINTNEXTLINE(*-avoid-const-or-ref-data-members)
        storage_type<TExpr> m_expr;
    };

    HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;

    /// @brief `FixedString` is a statically sized, constexpr string capable of being passed
    /// as a non-type template parameter
    /// @tparam TSize The size of the string
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<usize TSize>
        requires(TSize != 0)
    struct FixedString final {
        static constexpr auto k_size = TSize;
        // NOLINTNEXTLINE(*-c-arrays)
        char data[TSize + 1_usize] = {};

        /// @brief Constructs a `FixedString` from a string literal
        /// @param str The string literal
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        // NOLINTNEXTLINE(*-c-arrays, *-explicit-*)
        constexpr explicit(false) FixedString(const char (&str)[TSize + 1_usize]) noexcept {
            for(auto index = static_cast<usize>(0); index < TSize; ++index) {
                // NOLINTNEXTLINE(*-pro-bounds-constant-array-index)
                data[index] = str[index];
            }
        }

        /// @brief Returns the size of this string
        /// @return The size
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto size() const noexcept -> usize {
            return TSize;
        }

        /// @brief Returns whether this string is empty
        /// @return Whether this is empty
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto empty() const noexcept -> bool {
            return false;
        }

        /// @brief Converts this string to a `std::string_view`
        /// @return A `std::string_view` over the contents of this string
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        // NOLINTNEXTLINE(*-explicit-*)
        [[nodiscard]] constexpr explicit(false) operator std::string_view() const noexcept {
            return {data, TSize};
        }
    };
    template<usize TSize>
    // NOLINTNEXTLINE(*-c-arrays)
    FixedString(const char (&str)[TSize]) -> FixedString<TSize - 1>;

    HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;

    /// @brief `Operator` represents and performs the binary operator matching `TOp`
    /// @tparam TOp The string representing the operator to perform
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<FixedString TOp>
    struct Operator;

/// @brief Stamps out a specialization of `Operator` for the given operator
/// @param oper The operator to specialize `Operator` for, as a raw token
/// (i.e. _not_ a string literal)
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
/// @note This macro is for mathematical and logical operators
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
#if HYPERION_PLATFORM_COMPILER_IS_MSVC
            _Pragma("warning( push )");
            _Pragma("warning( disable : 4834 )");
#else
            _Pragma("GCC diagnostic push");
            _Pragma("GCC diagnostic ignored \"-Wunused-result\"");
#endif // HYPERION_PLATFORM_COMPILER_IS_MSVC
            return (std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));
#if HYPERION_PLATFORM_COMPILER_IS_MSVC
            _Pragma("warning( pop )");
#else
            _Pragma("GCC diagnostic pop");
#endif // HYPERION_PLATFORM_COMPILER_IS_MSVC
        }

        [[nodiscard]] static constexpr auto operator_() noexcept -> std::string_view {
            return k_op;
        }
    };

#undef HYPERION_DEFINE_OPERATOR_TYPE

/// @brief Stamps out a specialization of `Operator` for the given operator
/// @param oper The operator to specialize `Operator` for, as a raw token
/// (i.e. _not_ a string literal)
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
#define HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(oper, oper_string) /** NOLINT(*-macro-usage) **/  \
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
            using namespace hyperion::platform::compare;                                           \
            return oper_string##_compare(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs));        \
        }                                                                                          \
                                                                                                   \
        [[nodiscard]] static constexpr auto operator_() noexcept -> std::string_view {             \
            return k_op;                                                                           \
        }                                                                                          \
    }

    HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(<, less_than);
    HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(<=, less_than_or_equal);
    HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(>, greater_than);
    HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(>=, greater_than_or_equal);
    HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(==, equality);
    HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE(!=, inequality);

    template<>
    struct Operator<"<=>"> final {
        static constexpr auto k_op = std::string_view{"<=>"};

        template<typename TLhs, typename TRhs>
        [[nodiscard]] static constexpr auto
        do_op(TLhs&& lhs,
              TRhs&& rhs) noexcept(noexcept(std::forward<TLhs>(lhs) <=> std::forward<TRhs>(rhs)))
            -> decltype(std::forward<TLhs>(lhs) <=> std::forward<TRhs>(rhs))
            requires requires { std::forward<TLhs>(lhs) <=> std::forward<TRhs>(rhs); }
        {
            using ret_type = decltype(std::forward<TLhs>(lhs) <=> std::forward<TRhs>(rhs));
            using namespace hyperion::platform::compare;
            if(less_than_compare(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs))) {
                return ret_type::less;
            }

            if(equality_compare(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs))) {
                return ret_type::equivalent;
            }

            if(greater_than_compare(std::forward<TLhs>(lhs), std::forward<TRhs>(rhs))) {
                return ret_type::greater;
            }

            // fallback to built in op to avoid pulling in `#include <compare>`
            return std::forward<TLhs>(lhs) <=> std::forward<TRhs>(rhs);
        }

        [[nodiscard]] static constexpr auto operator_() noexcept -> std::string_view {
            return k_op;
        }
    };

#undef HYPERION_DEFINE_COMPARISON_OPERATOR_TYPE

    /// @brief Stores the result of a binary expression
    /// @tparam TLhs The type of the left-hand argument of the expression
    /// @tparam TRhs The type of the right-hand argument of the expression
    /// @tparam TOp The binary operator used in the expression
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
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

        /// @brief Performs the binary operation on the stored arguments
        ///
        /// # Requirements
        /// - This expression is the right hand side of a prior binary expression
        ///
        /// @param lhs The left-hand side of the operation
        /// @param rhs The right-hand side of the operation
        /// @return The result of performing the binary operator on the arguments
        [[nodiscard]] static constexpr auto
        call_operator(reference_type<TLhs> lhs, reference_type<TRhs> rhs)
            requires requires { Operator<TOp>::do_op(lhs.do_op(), rhs); }
        {
            return Operator<TOp>::do_op(lhs.do_op(), rhs);
        }

        /// @brief Performs the binary operation on the stored arguments
        ///
        /// # Requirements
        /// - This expression is _NOT_ the right hand side of a prior binary expression
        ///
        /// @param lhs The left-hand side of the operation
        /// @param rhs The right-hand side of the operation
        /// @return The result of performing the binary operator on the arguments
        [[nodiscard]] static constexpr auto
        call_operator(reference_type<TLhs> lhs, reference_type<TRhs> rhs)
            requires requires { Operator<TOp>::do_op(lhs, rhs); }
                     && (not requires { Operator<TOp>::do_op(lhs.do_op(), rhs); })
        {
            return Operator<TOp>::do_op(lhs, rhs);
        }

      public:
        /// @brief The type of the result produced by performing the binary operation
        using result_type = decltype(call_operator(m_lhs, m_rhs));
        /// @brief The `Operator` specialization for `TOp`
        using operator_type = Operator<TOp>;
        /// @brief Whether this expression is the right-hand side of a prior binary expression
        static constexpr auto k_lhs_is_binary_expression = requires {
            typename TLhs::result_type;
            typename TLhs::operator_type;
        };
        static constexpr auto k_is_binary_expression = true;

        /// @brief Constructs a `BinaryExpression` from the two arguments to the binary operator
        ///
        /// # Requirements
        /// - `storage_type<TLhs>` must be constructible from `lhs`
        /// - `storage_type<TRhs>` must be constructible from `rhs`
        ///
        /// @tparam TArgLhs The type of the left-hand argument to the operator
        /// @tparam TArgRhs The type of the right-hand argument to the operator
        /// @param lhs The left-hand argument to the operator
        /// @param rhs The right-hand argument to the operator
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        template<typename TArgLhs, typename TArgRhs>
            requires(static_cast<bool>(mpl::decltype_<storage_type<TLhs>>().is_constructible_from(
                        mpl::decltype_<TArgLhs>())))
                        && (static_cast<bool>(
                            mpl::decltype_<storage_type<TRhs>>().is_constructible_from(
                                mpl::decltype_<TArgRhs>())))
        constexpr BinaryExpression(TArgLhs&& lhs, TArgRhs&& rhs) noexcept
            : m_lhs{std::forward<TArgLhs>(lhs)}, m_rhs{std::forward<TArgRhs>(rhs)} {
        }

        /// @brief Performs the binary operation
        ///
        /// # Requirements
        /// - This must _NOT_ be the right-hand side of a prior binary expression
        ///
        /// # Exception Safety
        /// - May throw any exception throwable by performing the binary operation
        /// @return The result of the binary operation
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto
        do_op() noexcept(noexcept(Operator<TOp>::do_op(m_lhs, m_rhs))) -> result_type
            requires(not k_lhs_is_binary_expression)
        {
            return Operator<TOp>::do_op(m_lhs, m_rhs);
        }

        /// @brief Performs the binary operation
        ///
        /// # Requirements
        /// - This must be the right-hand side of a prior binary expression
        ///
        /// # Exception Safety
        /// - May throw any exception throwable by performing this binary operation or any prior
        /// (according to operator precedence rules) operation in the sequence
        /// @return The result of the binary operation
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto
        do_op() noexcept(noexcept(Operator<TOp>::do_op(m_lhs.do_op(), m_rhs))) -> result_type
            requires k_lhs_is_binary_expression
        {
            return Operator<TOp>::do_op(m_lhs.do_op(), m_rhs);
        }

        /// @brief Returns a new `BinaryExpression` representing the assignment of `rhs` to the
        /// result of evaluating this `BinaryExpression`
        ///
        /// # Requirements
        /// - `rhs` must be assignable to the result of evaluating this binary expression
        ///
        /// @tparam TFarRhs the type to assign to the result of evaluating this
        /// @param rhs The value to assign to the result of evaluating this
        /// @return A binary expression representing the assignment of `rhs` to the result of
        /// evaluating this one
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        template<typename TFarRhs>
        // NOLINTNEXTLINE(misc-unconventional-assign-operator, *-assignment-signature)
        constexpr auto operator=(TFarRhs&& rhs) noexcept
            -> BinaryExpression<BinaryExpression<TLhs, TRhs, TOp>, TFarRhs, "=">
            requires requires { do_op() = std::forward<TFarRhs>(rhs); }
        {
            return {*this, std::forward<TFarRhs>(rhs)};
        }

        /// @brief Returns a reference to the left-hand side of this binary expression
        /// @return The left-hand side of this
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto lhs() noexcept -> reference_type<TLhs> {
            return m_lhs;
        }

        /// @brief Returns a reference to the right-hand side of this binary expression
        /// @return The right-hand side of this
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto rhs() noexcept -> reference_type<TRhs> {
            return m_rhs;
        }

        /// @brief Returns the operator this expression evaluates, as a `std::string_view`
        /// @return The operator this expression evaluates
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto operator_() const noexcept -> std::string_view {
            return Operator<TOp>::operator_();
        }

        /// @brief Returns the result of evaluating this expression
        ///
        /// # Exception Safety
        /// - May throw any exception throwable by evaluating this expression, or any prior
        /// expression in the sequence (according to operator precedence rules), if applicable.
        ///
        /// @return The result of evaluating this expression
        /// @ingroup decomposer
        /// @headerfile hyperion/assert/detail/decomposer.h
        [[nodiscard]] constexpr auto
        expr() noexcept(noexcept(std::declval<BinaryExpression>().do_op())) -> result_type
            requires requires { std::declval<BinaryExpression>().do_op(); }
        {
            return do_op();
        }
    };

/// @brief Stamps out an operator implementation for `BinaryExpression` for the given operator
/// @param oper The operator to implement for `BinaryExpression`, as a raw token
/// (i.e. _not_ a string literal)
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
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

    // clang still doesn't understand tparam tags for concepts
    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        HYPERION_IGNORE_DOCUMENTATION_WARNING_START;
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

    /// @brief Requires that `TType` is a `BinaryExpression` specialization
    /// @tparam TType The type to check
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
    template<typename TType>
    concept IsBinaryExpression = requires { TType::k_is_binary_expression; }
                                 && requires { requires TType::k_is_binary_expression; };

    #if HYPERION_PLATFORM_COMPILER_IS_CLANG
        HYPERION_IGNORE_DOCUMENTATION_WARNING_STOP;
    #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG

    /// @brief Represents the initial expression in an expression sequence
    /// @tparam TExpr The result type of the expression
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
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

/// @brief Stamps out an operator implementation for `InitialExpression` for the given operator
/// @param oper The operator to implement for `InitialExpression`, as a raw token
/// (i.e. _not_ a string literal)
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
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

    /// @brief Decomposes an arbitrary expression into its constituent sub-expressions via
    /// operator overloading
    /// @ingroup decomposer
    /// @headerfile hyperion/assert/detail/decomposer.h
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

/// @brief Implement `fmt::format` support for `UnaryExpression`
/// @tparam TExpr The result type of the expression
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
/// @note if `TExpr` is not `OutputStreamable` then formats to `(NotFormattable)`
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
    [[nodiscard]] auto format(const self& expression, TFormatContext& context) const {
        using hyperion::assert::detail::IsBinaryExpression;
        using hyperion::assert::detail::OutputStreamable;
        using hyperion::assert::highlight::highlight;

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

/// @brief Implement `fmt::format` support for `InitialExpression`
/// @tparam TExpr The result type of the expression
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
/// @note if `TExpr` is not `OutputStreamable` then formats to `(NotFormattable)`
template<typename TExpr>
struct fmt::formatter<hyperion::assert::detail::InitialExpression<TExpr>> {
  public:
    using self = hyperion::assert::detail::InitialExpression<TExpr>;

    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    [[nodiscard]] constexpr auto parse(format_parse_context& context) {
        return context.begin();
    }

    template<typename TFormatContext>
    [[nodiscard]] auto format(self& expression, TFormatContext& context) const {
        using hyperion::assert::detail::UnaryExpression;
        return fmt::format_to(context.out(), "{}", static_cast<UnaryExpression<TExpr>>(expression));
    }
};

/// @brief Implement `fmt::format` support for `BinaryExpression`
/// @tparam TLhs The type of the left-hand side of the expression
/// @tparam TRhs The type of the right-hand side of the expression
/// @tparam TOp The operation used in the expression, as a string literal
/// @ingroup decomposer
/// @headerfile hyperion/assert/detail/decomposer.h
/// @note If any sub-expression of this expression is not `OutputStreamable`,
/// that sub-expression will be formatted to `(NotFormattable)`
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
    [[nodiscard]] auto format(self& expression, TFormatContext& context) const {
        using hyperion::assert::detail::IsBinaryExpression;
        using hyperion::assert::detail::OutputStreamable;
        using hyperion::assert::highlight::get_color;
        using hyperion::assert::highlight::highlight;
        using hyperion::assert::tokens::Punctuation;
        using hyperion::assert::tokens::Token;

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

#endif // HYPERION_ASSERT_DETAIL_DECOMPOSER_H
