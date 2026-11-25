// include/beman/execution/detail/nested_sender_has_affine.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_NESTED_SENDER_HAS_AFFINE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_NESTED_SENDER_HAS_AFFINE

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.sender_has_affine;
#else
#include <beman/execution/detail/sender_has_affine.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender, typename Indices>
struct nested_sender_has_affine_impl;

template <typename Sender, ::std::size_t... I>
struct nested_sender_has_affine_impl<Sender, ::std::index_sequence<I...>> {
    static constexpr bool value =
        (... &&
         ::beman::execution::detail::sender_has_affine<decltype(::std::declval<Sender>().template get<I + 2uz>())>);
};

template <typename Sender>
concept nested_sender_has_affine = ::beman::execution::detail::nested_sender_has_affine_impl<
    Sender,
    ::std::make_index_sequence<::std::tuple_size_v<::std::remove_cvref_t<Sender>> - 2uz>>::value;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_NESTED_SENDER_HAS_AFFINE
