// include/beman/execution/detail/receiver.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_RECEIVER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_RECEIVER

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT struct receiver_t {};

BEMAN_EXECUTION_EXPORT template <typename Rcvr>
concept receiver =
    ::std::derived_from<typename ::std::remove_cvref_t<Rcvr>::receiver_concept, ::beman::execution::receiver_t> &&
    requires(const ::std::remove_cvref_t<Rcvr>& rcvr) {
        { ::beman::execution::get_env(rcvr) } -> ::beman::execution::detail::queryable;
    } && ::std::move_constructible<::std::remove_cvref_t<Rcvr>> &&
    ::std::constructible_from<::std::remove_cvref_t<Rcvr>, Rcvr> && (!::std::is_final_v<::std::remove_cvref_t<Rcvr>>);
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_RECEIVER
