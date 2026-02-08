module;
// src/beman/execution/write_env.cppm                                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_HAS_MODULES
#define BEMAN_HAS_MODULES
#endif
#include <beman/execution/detail/write_env.hpp>

export module beman.execution.detail.write_env;

namespace beman::execution {
export using beman::execution::write_env_t;
export using beman::execution::write_env;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT struct impls_for<write_env_t>;
export template <typename NewEnv, typename Child, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::write_env_t, NewEnv, Child>,
    Env>;
} // namespace beman::execution::detail
