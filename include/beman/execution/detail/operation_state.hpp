// include/beman/execution/detail/operation_state.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_OPERATION_STATE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_OPERATION_STATE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/start.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT struct operation_state_t {};

BEMAN_EXECUTION_EXPORT template <typename State>
concept operation_state =
    ::std::derived_from<typename State::operation_state_concept, ::beman::execution::operation_state_t> &&
    ::std::is_object_v<State> && requires(State& state) {
        { ::beman::execution::start(state) } noexcept;
    };
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_OPERATION_STATE
