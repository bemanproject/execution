// include/beman/execution/detail/basic_state.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_BASIC_STATE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_BASIC_STATE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#include <beman/execution/detail/state_type.hpp>
#include <exception>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Class template used to present the receiver and operation state for library senders.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename Sender, typename Receiver>
struct basic_state {
    basic_state(Sender&& sender, Receiver&& rcvr) noexcept(true)
        : receiver(::std::move(rcvr)),
          state(::beman::execution::detail::impls_for<::beman::execution::tag_of_t<Sender>>::get_state(
              ::std::forward<Sender>(sender), this->receiver)) {}

    Receiver                                                 receiver;
    ::beman::execution::detail::state_type<Sender, Receiver> state;
};
template <typename Sender, typename Receiver>
basic_state(Sender&&, Receiver&&) -> basic_state<Sender&&, Receiver>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_BASIC_STATE
