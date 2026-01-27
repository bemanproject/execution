// include/beman/execution/detail/when_all_with_variant.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_WHEN_ALL_WITH_VARIANT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_WHEN_ALL_WITH_VARIANT

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/when_all.hpp>
#include <beman/execution/detail/into_variant.hpp>

#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct when_all_with_variant_t {
    // template <::beman::execution::detail::sender_for<when_all_with_variant_t> Sender>
    template <::beman::execution::sender Sender>
    auto transform_sender(Sender&& sender, auto&&...) const noexcept {
        return ::std::forward<Sender>(sender).apply([](auto&&, auto&&, auto&&... child) {
            return ::beman::execution::when_all(
                ::beman::execution::into_variant(::beman::execution::detail::forward_like<Sender>(child))...);
        });
    }

    template <::beman::execution::sender... Sender>
    auto operator()(Sender&&... sender) const {
        using domain_t =
            typename ::std::common_type_t<decltype(::beman::execution::detail::get_domain_early(sender))...>;
        return ::beman::execution::transform_sender(
            domain_t{}, ::beman::execution::detail::make_sender(*this, {}, ::std::forward<Sender>(sender)...));
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
BEMAN_EXECUTION_EXPORT using when_all_with_variant_t = ::beman::execution::detail::when_all_with_variant_t;
BEMAN_EXECUTION_EXPORT inline constexpr ::beman::execution::when_all_with_variant_t when_all_with_variant{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_WHEN_ALL_WITH_VARIANT
