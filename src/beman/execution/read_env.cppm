module;
// src/beman/execution/read_env.cppm                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/read_env.hpp>

export module beman.execution.detail.read_env;

namespace beman::execution {
export using beman::execution::read_env_t;
export using beman::execution::read_env;
} // namespace beman::execution
 
namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT
struct impls_for<::beman::execution::detail::read_env_t>;

export template <typename Query, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::read_env_t, Query>,
    Env>;
} // namespace beman::execution::detail
