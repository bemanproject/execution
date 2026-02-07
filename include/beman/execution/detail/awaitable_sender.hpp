// include/beman/execution/detail/awaitable_sender.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_AWAITABLE_SENDER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_AWAITABLE_SENDER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <coroutine>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.env_of_t import beman.execution.detail.single_sender
#else
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/single_sender.hpp>
#endif

    namespace beman::execution::detail { template <class Sndr, class Promise> concept awaitable_sender = ::beman::execution::detail::single_sender <Sndr, ::beman::execution::env_of_t<Promise>> && requires(Promise & prom) { { prom.unhandled_stopped() }->::std::convertible_to <::std::coroutine_handle<>>;
};
} // namespace beman::execution::detail

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_AWAITABLE_SENDER
