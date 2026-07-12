// include/beman/execution/detail/exit_scope_sender_of_t.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_EXIT_SCOPE_SENDER_OF_T
#define INCLUDED_BEMAN_EXECUTION_DETAIL_EXIT_SCOPE_SENDER_OF_T

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures_of_t;
import beman.execution.detail.completion_signatures;
import beman.execution.detail.enter_scope_sender;
import beman.execution.detail.exit_scope_sender_in;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/enter_scope_sender.hpp>
#include <beman/execution/detail/exit_scope_sender_in.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {

namespace detail {

/// `true` if `Sig` is a value completion signature, i.e. `set_value_t(Args...)`.
template <typename Sig>
inline constexpr bool is_value_completion_sig = false;
template <typename... Args>
inline constexpr bool is_value_completion_sig<::beman::execution::set_value_t(Args...)> = true;

/// Finds the type T from the unique unary set_value_t(T) in a signature list.
/// Not defined (substitution failure) if there are zero or two or more value signatures,
/// or if the sole value signature is not unary.
template <typename... Sigs>
struct single_value_sender;

template <typename T, typename... Rest>
    requires(!(::beman::execution::detail::is_value_completion_sig<Rest> || ...))
struct single_value_sender<::beman::execution::set_value_t(T), Rest...> {
    using type = T;
};

template <typename First, typename... Rest>
    requires(!::beman::execution::detail::is_value_completion_sig<First>)
struct single_value_sender<First, Rest...> : ::beman::execution::detail::single_value_sender<Rest...> {};

/// Helper that extracts the exit scope sender type from a list of completion signatures.
template <typename Signatures, typename... Env>
struct extract_exit_scope_sender;

template <typename... Sigs, typename... Env>
    requires requires { typename ::beman::execution::detail::single_value_sender<Sigs...>::type; } &&
             ::beman::execution::
                 exit_scope_sender_in<typename ::beman::execution::detail::single_value_sender<Sigs...>::type, Env...>
struct extract_exit_scope_sender<::beman::execution::completion_signatures<Sigs...>, Env...> {
    using type = typename ::beman::execution::detail::single_value_sender<Sigs...>::type;
};

} // namespace detail

/*!
 * \brief Represents the type of exit scope sender which an asynchronous operation formed from
 *        `Sender` in `Env...` yield on completion.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 *
 * \details
 * This looks at the completion signatures of `Sender` in `Env...` and finds the unique value completion signature.
 * If there is no value completion signature, or if there are two or more value completion signatures, this type is not
 * defined.
 */
template <::beman::execution::enter_scope_sender Sender, typename... Env>
using exit_scope_sender_of_t = typename ::beman::execution::detail::
    extract_exit_scope_sender<::beman::execution::completion_signatures_of_t<Sender, Env...>, Env...>::type;
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
