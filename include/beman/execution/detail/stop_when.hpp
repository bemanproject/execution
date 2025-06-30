// include/beman/execution/detail/stop_when.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_STOP_WHEN
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_STOP_WHEN

#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/stoppable_token.hpp>
#include <beman/execution/detail/unstoppable_token.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/stop_callback_for_t.hpp>
#include <type_traits>
#include <optional>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
inline constexpr struct stop_when_t {

    template <::beman::execution::sender Sndr, ::beman::execution::stoppable_token Tok>
    struct sender;

    template <::beman::execution::sender Sndr, ::beman::execution::stoppable_token Tok>
    auto operator()(Sndr&& sndr, Tok&& tok) const noexcept {
        if constexpr (::beman::execution::unstoppable_token<Tok>) {
            return ::std::forward<Sndr>(sndr);
        } else {
            return sender<Sndr, Tok>(*this, ::std::forward<Tok>(tok), ::std::forward<Sndr>(sndr));
        }
    }
} stop_when{};
} // namespace beman::execution::detail

template <::beman::execution::sender Sndr, ::beman::execution::stoppable_token Tok>
struct beman::execution::detail::stop_when_t::sender {
    using sender_concept = ::beman::execution::sender_t;

    stop_when_t               stop_when{};
    std::remove_cvref_t<Tok>  tok;
    std::remove_cvref_t<Sndr> sndr;

    template <::beman::execution::receiver Rcvr>
    struct state {
        using operation_state_concept = ::beman::execution::operation_state_t;
        using rcvr_t                  = ::std::remove_cvref_t<Rcvr>;
        using token1_t                = ::std::remove_cvref_t<Tok>;
        using token2_t =
            decltype(::beman::execution::get_stop_token(::beman::execution::get_env(::std::declval<rcvr_t>())));

        struct cb_t {
            ::beman::execution::inplace_stop_source& source;
            auto                                     operator()() const noexcept { this->source.request_stop(); }
        };
        struct base_state {
            rcvr_t                                  rcvr;
            ::beman::execution::inplace_stop_source source{};
        };
        struct env {
            base_state* st;
            auto        query(const ::beman::execution::get_stop_token_t&) const noexcept {
                return this->st->source.get_token();
            }
            template <typename Q, typename... A>
                requires requires(const Q& q, A&&... a, const rcvr_t& r) {
                    q(::beman::execution::get_env(r), ::std::forward<A>(a)...);
                }
            auto query(const Q& q, A&&... a) const noexcept {
                return q(::beman::execution::get_env(this->st->rcvr), ::std::forward<A>(a)...);
            }
        };

        struct receiver {
            using receiver_concept = ::beman::execution::receiver_t;
            base_state* st;

            auto get_env() const noexcept -> env { return env{this->st}; }
            template <typename... A>
            auto set_value(A&&... a) const noexcept -> void {
                ::beman::execution::set_value(::std::move(this->st->rcvr), ::std::forward<A>(a)...);
            }
            template <typename E>
            auto set_error(E&& e) const noexcept -> void {
                ::beman::execution::set_error(::std::move(this->st->rcvr), ::std::forward<E>(e));
            }
            auto set_stopped() const noexcept -> void { ::beman::execution::set_stopped(::std::move(this->st->rcvr)); }
        };
        using inner_state_t =
            decltype(::beman::execution::connect(::std::declval<Sndr>(), ::std::declval<receiver>()));

        token1_t                                                               tok;
        base_state                                                             base;
        std::optional<::beman::execution::stop_callback_for_t<token1_t, cb_t>> cb1;
        std::optional<::beman::execution::stop_callback_for_t<token2_t, cb_t>> cb2;
        inner_state_t                                                          inner_state;

        template <::beman::execution::sender S, ::beman::execution::stoppable_token T, ::beman::execution::receiver R>
        state(S&& s, T&& t, R&& r)
            : tok(::std::forward<T>(t)),
              base{::std::forward<R>(r)},
              inner_state(::beman::execution::connect(::std::forward<S>(s), receiver(&this->base))) {}

        auto start() & noexcept {
            this->cb1.emplace(this->tok, cb_t(this->base.source));
            this->cb2.emplace(::beman::execution::get_stop_token(::beman::execution::get_env(this->base.rcvr)),
                              cb_t(this->base.source));
            ::beman::execution::start(this->inner_state);
        }
    };

    template <typename E>
    auto get_completion_signatures(const E& e) const noexcept {
        return ::beman::execution::get_completion_signatures(this->sndr, e);
    }
    template <::beman::execution::receiver Rcvr>
    auto connect(Rcvr&& rcvr) && -> state<Rcvr> {
        return state<Rcvr>{std::move(this->sndr), ::std::move(this->tok), ::std::forward<Rcvr>(rcvr)};
    }
};
// ----------------------------------------------------------------------------

#endif
