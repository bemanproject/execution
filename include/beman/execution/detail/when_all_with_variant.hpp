// include/beman/execution/detail/when_all_with_variant.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_WHEN_ALL_WITH_VARIANT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_WHEN_ALL_WITH_VARIANT

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.forward_like;
import beman.execution.detail.into_variant;
import beman.execution.detail.make_sender;
import beman.execution.detail.sender;
import beman.execution.detail.sender_for;
import beman.execution.detail.set_value;
import beman.execution.detail.when_all;
#else
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/into_variant.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/when_all.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct when_all_with_variant_t {
    template <::beman::execution::detail::sender_for<when_all_with_variant_t> Sender, typename Env>
    auto transform_sender(::beman::execution::set_value_t, Sender&& sender, const Env&) const noexcept {
        return ::std::forward<Sender>(sender).apply([](auto&&, auto&&, auto&&... child) {
            return ::beman::execution::when_all(
                ::beman::execution::into_variant(::beman::execution::detail::forward_like<Sender>(child))...);
        });
    }

    template <::beman::execution::sender... Sender>
    auto operator()(Sender&&... sender) const {
        return ::beman::execution::detail::make_sender(*this, {}, ::std::forward<Sender>(sender)...);
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
using when_all_with_variant_t = ::beman::execution::detail::when_all_with_variant_t;
inline constexpr ::beman::execution::when_all_with_variant_t when_all_with_variant{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_WHEN_ALL_WITH_VARIANT
