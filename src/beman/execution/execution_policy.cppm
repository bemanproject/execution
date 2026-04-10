module;
// src/beman/execution/execution_policy.cppm                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/execution_policy.hpp>

export module beman.execution.detail.execution_policy;

namespace beman::execution {
export using beman::execution::parallel_policy;
export using beman::execution::par;

export using beman::execution::parallel_unsequenced_policy;
export using beman::execution::par_unseq;

export using beman::execution::sequenced_policy;
export using beman::execution::seq;

#if !defined(__cpp_lib_execution) || (__cpp_lib_execution >= 201902L)
export using beman::execution::unsequenced_policy;
export using beman::execution::unseq;
#endif

export using beman::execution::is_execution_policy;
export using beman::execution::is_execution_policy_v;
} // namespace beman::execution
