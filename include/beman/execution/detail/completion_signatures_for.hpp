// include/beman/execution/detail/completion_signatures_for.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES_FOR
#define INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES_FOR

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <functional>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.get_completion_signatures;
import beman.execution.detail.sender_in;
import beman.execution.detail.tag_of_t;
#else
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/sender_in.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Tag type used to determine if completion signatures were defined.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
struct no_completion_signatures_defined_in_sender {};

/*!
 * \brief Primary template declaration for the customization of sender completion signatures.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */

template <typename Sender, typename... Env>
consteval auto get_completion_signatures_for_helper() {
#if 1
    using tag_t = ::std::remove_cvref_t<::beman::execution::tag_of_t<::std::remove_cvref_t<Sender>>>;
#else
    using tag_t = decltype([]() noexcept {
        if constexpr (requires { std::declval<Sender>().template get<0>(); })
            return decltype(std::declval<Sender>().template get<0>())();
        else
            return ::std::remove_cvref_t<::beman::execution::tag_of_t<::std::remove_cvref_t<Sender>>>();
    }());
#endif
    if constexpr (requires { tag_t::template get_completion_signatures<Sender, Env...>(); })
        return tag_t::template get_completion_signatures<Sender, Env...>();
    else
        return ::beman::execution::detail::no_completion_signatures_defined_in_sender{};
}

/*!
 * \brief Type alias used to access a senders completion signatures.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename Sender, typename... Env>
using completion_signatures_for = decltype(get_completion_signatures_for_helper<Sender, Env...>());
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES_FOR
