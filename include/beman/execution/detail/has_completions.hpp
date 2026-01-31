// include/beman/execution/detail/has_completions.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/valid_completion_for.hpp>
#include <beman/execution/detail/completion_signatures.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
#if not defined(__clang__)
// The version of clang current (2024-09-01) installed on my Mac crashes
// with this code - thus, there is a work-around.
template <typename Receiver, typename Completions>
concept has_completions = requires(Completions* completions) {
    []<::beman::execution::detail::valid_completion_for<Receiver>... Signatures>(
        ::beman::execution::completion_signatures<Signatures...>*) {}(completions);
};
#else
template <typename, typename>
struct has_completions_aux;
template <typename Receiver, typename... Signature>
struct has_completions_aux<Receiver, ::beman::execution::completion_signatures<Signature...>> {
    static constexpr bool value =
        (::beman::execution::detail::valid_completion_for<Signature, Receiver> && ... && true);
};

template <typename Receiver, typename Completions>
concept has_completions = has_completions_aux<Receiver, Completions>::value;
#endif
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_HAS_COMPLETIONS
