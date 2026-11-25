// include/beman/execution/detail/default_impls.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_DEFAULT_IMPLS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_DEFAULT_IMPLS

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.allocator_aware_move;
import beman.execution.detail.callable;
import beman.execution.detail.env;
import beman.execution.detail.forward_like;
import beman.execution.detail.fwd_env;
import beman.execution.detail.get_allocator;
import beman.execution.detail.get_env;
import beman.execution.detail.product_type;
import beman.execution.detail.sender_decompose;
import beman.execution.detail.start;
#else
#include <beman/execution/detail/allocator_aware_move.hpp>
#include <beman/execution/detail/callable.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <beman/execution/detail/start.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Helper type providing default implementations for basic_sender
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 */
struct default_impls {
    struct get_attrs_impl {
        auto operator()(const auto&, const auto&... child) const noexcept -> decltype(auto) {
            if constexpr (1 == sizeof...(child))
                return (::beman::execution::detail::fwd_env(::beman::execution::get_env(child)), ...);
            else
                return ::beman::execution::env<>{};
        }
    };
    static constexpr auto get_attrs = get_attrs_impl{};
    struct get_env_impl {
        auto operator()(auto, auto&, const auto& receiver) const noexcept -> decltype(auto) {
            return ::beman::execution::detail::fwd_env(::beman::execution::get_env(receiver));
        }
    };
    static constexpr auto get_env = get_env_impl{};
    struct get_state_impl {
        template <typename Sender>
        static constexpr bool has_data_member = requires(Sender& sender) {
            sender.size();
            sender.template get<1>();
        };

        template <typename Sender>
        static auto get_data(Sender&& sender) noexcept -> decltype(auto) {
            if constexpr (has_data_member<Sender>)
                return sender.template get<1>();
            else
                return ::beman::execution::detail::get_sender_data(::std::forward<Sender>(sender)).data;
        }

        // Returns a reference to the data or a prvalue made with the receiver's allocator; copying/moving from a
        // reference is accounted for by basic_state.
        template <typename Sender, typename Receiver>
        auto operator()(Sender&& sender, Receiver& receiver) const
            noexcept(noexcept(::beman::execution::detail::allocator_aware_move(
                ::beman::execution::detail::forward_like<Sender>(get_data(::std::declval<Sender>())), receiver)))
                -> decltype(auto) {
            auto&& data{get_data(::std::forward<Sender>(sender))};

            return ::beman::execution::detail::allocator_aware_move(
                ::beman::execution::detail::forward_like<Sender>(data), receiver);
        }
    };
    static constexpr auto get_state = get_state_impl{};
    struct start_impl {
        auto operator()(auto&, auto&, auto&... ops) const noexcept -> void { (::beman::execution::start(ops), ...); }
    };
    static constexpr auto start = start_impl{};
    struct complete_impl {
        template <typename Index, typename Receiver, typename Tag, typename... Args>
            requires ::beman::execution::detail::callable<Tag, Receiver, Args...>
        auto operator()(Index, auto&, Receiver& receiver, Tag, Args&&... args) const noexcept -> void {
            static_assert(Index::value == 0);
            Tag()(::std::move(receiver), ::std::forward<Args>(args)...);
        }
    };
    static constexpr auto complete = complete_impl{};

    template <typename Sender, typename... Env>
    static consteval auto check_types() -> void {}
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_DEFAULT_IMPLS
