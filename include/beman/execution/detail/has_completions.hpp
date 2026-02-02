// include/beman/execution/detail/has_completions.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>
#include <beman/execution/detail/completion_signatures.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Receiver, typename Completions>
concept has_completions = requires(Completions* completions) {
    []<::beman::execution::detail::valid_completion_for<Receiver>... Signatures>(
        ::beman::execution::completion_signatures<Signatures...>*) {}(completions);
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS
