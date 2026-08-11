module;
// src/beman/execution/product_type.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define BEMAN_EXECUTION_DETAIL_PRODUCT_TYPE_CPPM
#include <beman/execution/detail/product_type.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <tuple>
#endif

export module beman.execution.detail.product_type;

namespace beman::execution::detail {
export using beman::execution::detail::is_product_type;
export using beman::execution::detail::is_product_type_c;
export using beman::execution::detail::product_type;

} // namespace beman::execution::detail

namespace std {
template <typename... T>
struct tuple_size<::beman::execution::detail::product_type<T...>>
    : ::std::integral_constant<std::size_t, ::beman::execution::detail::product_type<T...>::size()> {};

template <::std::size_t I, typename... T>
struct tuple_element<I, ::beman::execution::detail::product_type<T...>> {
    using type =
        ::std::decay_t<decltype(::std::declval<::beman::execution::detail::product_type<T...>>().template get<I>())>;
};
} // namespace std
