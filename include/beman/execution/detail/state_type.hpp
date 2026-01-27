// include/beman/execution/detail/state_type.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STATE_TYPE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STATE_TYPE

#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/call_result_t.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender, typename Receiver>
using state_type = ::std::decay_t< ::beman::execution::detail::call_result_t<
    decltype(::beman::execution::detail::impls_for< ::beman::execution::tag_of_t<Sender> >::get_state),
    Sender,
    Receiver&> >;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_STATE_TYPE
