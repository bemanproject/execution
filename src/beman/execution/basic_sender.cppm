module;
// src/beman/execution/basic_sender.cppm                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <tuple>
#include <type_traits>
#include <beman/execution/detail/basic_sender.hpp>

export module beman.execution.detail.basic_sender;

namespace beman::execution::detail {
export using beman::execution::detail::basic_sender;
} // namespace beman::execution::detail

#if not defined(__GNUC__) || defined(__clang__)
namespace std {
export template <typename Tag, typename Data, typename... Child>
struct tuple_size<::beman::execution::detail::basic_sender<Tag, Data, Child...>>
    : ::std::integral_constant<std::size_t, 2u + sizeof...(Child)> {};

export template <::std::size_t I, typename... T>
struct tuple_element<I, ::beman::execution::detail::basic_sender<T...>> {
    using type =
        ::std::decay_t<decltype(::std::declval<::beman::execution::detail::basic_sender<T...>>().template get<I>())>;
};
} // namespace std
#endif
