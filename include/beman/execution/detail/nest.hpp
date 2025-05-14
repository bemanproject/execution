// include/beman/execution/detail/nest.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_NEST
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_NEST

#include <beman/execution/detail/async_scope_token.hpp>
#include <beman/execution/detail/sender.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
    struct nest_t {
        template <::beman::execution::sender Sender, ::beman::execution::async_scope_token Token>
        auto operator()(Sender&&, Token&&) const {
        }
    };
    inline constexpr nest_t nest{};
}

// ----------------------------------------------------------------------------

#endif
