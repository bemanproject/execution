// include/beman/execution/detail/affine_on.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_AFFINE_ON
#define INCLUDED_BEMAN_EXECUTION_DETAIL_AFFINE_ON

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/suppress_push.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.env;
import beman.execution.detail.forward_like;
import beman.execution.detail.fwd_env;
import beman.execution.detail.get_domain_early;
import beman.execution.detail.get_scheduler;
import beman.execution.detail.get_stop_token;
import beman.execution.detail.join_env;
import beman.execution.detail.make_sender;
import beman.execution.detail.never_stop_token;
import beman.execution.detail.prop;
import beman.execution.detail.schedule_from;
import beman.execution.detail.scheduler;
import beman.execution.detail.sender;
import beman.execution.detail.sender_adaptor;
import beman.execution.detail.sender_adaptor_closure;
import beman.execution.detail.sender_for;
import beman.execution.detail.sender_has_affine_on;
import beman.execution.detail.tag_of_t;
import beman.execution.detail.transform_sender;
import beman.execution.detail.write_env;
#else
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/never_stop_token.hpp>
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/schedule_from.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/sender_has_affine_on.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/write_env.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Ev>
struct affine_on_env {
    Ev   ev_;
    auto query(const ::beman::execution::get_stop_token_t&) const noexcept -> ::beman::execution::never_stop_token {
        return ::beman::execution::never_stop_token();
    }
    template <typename Q>
    auto query(const Q& q) const noexcept -> decltype(q(this->ev_)) {
        return q(this->ev_);
    }
};
template <typename Ev>
affine_on_env(const Ev&) -> affine_on_env<Ev>;

/**
 * @brief The affine_on_t struct is a sender adaptor closure that transforms a sender
 *        to complete on the scheduler obtained from the receiver's environment.
 *
 * This adaptor implements scheduler affinity to adapt a sender to complete on the
 * scheduler obtained the receiver's environment. The get_scheduler query is used
 * to obtain the scheduler on which the sender gets started.
 */
struct affine_on_t : ::beman::execution::sender_adaptor_closure<affine_on_t> {
    /**
     * @brief Adapt a sender with affine_on.
     *
     * @tparam Sender The deduced type of the sender to be transformed.
     * @param sender The sender to be transformed.
     * @return An adapted sender to complete on the scheduler it was started on.
     */
    template <::beman::execution::sender Sender>
    auto operator()(Sender&& sender) const {
        return ::beman::execution::detail::transform_sender(
            ::beman::execution::detail::get_domain_early(sender),
            ::beman::execution::detail::make_sender(
                *this, ::beman::execution::env<>{}, ::std::forward<Sender>(sender)));
    }

    /**
     * @brief Overload for creating a sender adaptor from affine_on.
     *
     * @return A sender adaptor for the affine_on_t.
     */
    auto operator()() const { return ::beman::execution::detail::sender_adaptor{*this}; }

    /**
     * @brief affine_on is implemented by transforming it into a use of schedule_from.
     *
     * The constraints ensure that the environment provides a scheduler which is
     * infallible and, thus, can be used to guarantee completion on the correct
     * scheduler.
     *
     * The implementation first tries to see if the child sender's tag has a custom
     * affine_on implementation. If it does, that is used. Otherwise, the default
     * implementation gets a scheduler from the environment and uses schedule_from
     * to adapt the sender to complete on that scheduler.
     *
     * @tparam Sender The type of the sender to be transformed.
     * @tparam Env The type of the environment providing the scheduler.
     * @param sender The sender to be transformed.
     * @param env The environment providing the scheduler.
     * @return A transformed sender that is affined to the scheduler.
     */
    template <::beman::execution::sender Sender, typename Env>
        requires ::beman::execution::detail::sender_for<Sender, affine_on_t> && requires(const Env& env) {
            { ::beman::execution::get_scheduler(env) } -> ::beman::execution::scheduler;
            { ::beman::execution::schedule(::beman::execution::get_scheduler(env)) } -> ::beman::execution::sender;
            {
                ::beman::execution::get_completion_signatures(
                    ::beman::execution::schedule(::beman::execution::get_scheduler(env)),
                    ::beman::execution::detail::join_env(
                        ::beman::execution::env{::beman::execution::prop{
                            ::beman::execution::get_stop_token, ::beman::execution::never_stop_token{}, {}}},
                        env))
            } -> ::std::same_as<::beman::execution::completion_signatures<::beman::execution::set_value_t()>>;
        }
    static auto transform_sender(Sender&& sender, const Env& ev) {
        [[maybe_unused]] auto& [tag, data, child] = sender;
        using child_tag_t = ::beman::execution::tag_of_t<::std::remove_cvref_t<decltype(child)>>;

        if constexpr (::beman::execution::detail::nested_sender_has_affine_on<Sender, Env>) {
            constexpr child_tag_t t{};
            return t.affine_on(::beman::execution::detail::forward_like<Sender>(child), ev);
        } else {
            return ::beman::execution::write_env(
                ::beman::execution::schedule_from(
                    ::beman::execution::get_scheduler(ev),
                    ::beman::execution::write_env(::beman::execution::detail::forward_like<Sender>(child), ev)),
                beman::execution::detail::affine_on_env(ev));
        }
    }
};

} // namespace beman::execution::detail

namespace beman::execution {
/**
 * @brief affine_on is a CPO, used to adapt a sender to complete on the scheduler
 *      it got started on which is derived from get_scheduler on the receiver's environment.
 */
using affine_on_t = beman::execution::detail::affine_on_t;
inline constexpr affine_on_t affine_on{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_AFFINE_ON
