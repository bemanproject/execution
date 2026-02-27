// include/beman/execution/detail/has_completions.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures;
import beman.execution.detail.valid_completion_for;
#else
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

template <typename Receiver, ::beman::execution::detail::valid_completion_for<Receiver>... Signatures>
auto has_completions_test(::beman::execution::completion_signatures<Signatures...>*) noexcept -> void {}

template <typename Receiver, typename Completions>
concept has_completions =
    requires(Completions* completions) { ::beman::execution::detail::has_completions_test<Receiver>(completions); };
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS
