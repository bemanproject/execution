module;
// src/beman/execution/product_type.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/product_type.hpp>

export module beman.execution.detail.product_type;

namespace beman::execution::detail {
export using beman::execution::detail::is_product_type;
export using beman::execution::detail::is_product_type_c;
export using beman::execution::detail::product_type;

} // namespace beman::execution::detail
namespace std {
export template <typename T>
    requires ::beman::execution::detail::is_product_type_c<T>
struct tuple_size<T>;

export template <::std::size_t I, typename T>
    requires ::beman::execution::detail::is_product_type_c<T>
struct tuple_element<I, T>;
} // namespace std
