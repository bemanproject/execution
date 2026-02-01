// examples/when_all-cancel.cpp                                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <cassert>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#include <beman/execution/stop_token.hpp>
#endif

namespace ex = beman::execution;

// ----------------------------------------------------------------------------

namespace {
struct env {
    ex::inplace_stop_source* source;
    auto                     query(const ex::get_stop_token_t&) const noexcept -> ex::inplace_stop_token {
        std::cout << "query\n";
        return this->source->get_token();
    }
};

struct receiver {
    using receiver_concept = ex::receiver_t;
    ex::inplace_stop_source* source;

    auto set_value() && noexcept -> void { std::cout << "set_value\n"; }
    auto set_error(auto&&) && noexcept -> void { std::cout << "set_error\n"; }
    auto set_stopped() && noexcept -> void { std::cout << "set_stopped\n"; }

    auto get_env() const noexcept { return env{this->source}; }
};
static_assert(ex::receiver<receiver>);

struct await_stop {
    using sender_concept        = ex::sender_t;
    using completion_signatures = ex::completion_signatures<ex::set_value_t(), ex::set_stopped_t()>;

    template <ex::receiver Receiver>
    struct state {
        struct stop_t {
            state* st;
            auto   operator()() const noexcept -> void {
                auto local_st{this->st};
                local_st->callback.reset();
                std::cout << "await_stop stopping\n";
                ex::set_stopped(std::move(local_st->receiver));
                std::cout << "await_stop stopping done\n";
            }
        };
        using operation_state_concept = ex::operation_state_t;
        using token_t                 = decltype(ex::get_stop_token(ex::get_env(std::declval<Receiver>())));
        using callback_t              = ex::stop_callback_for_t<token_t, stop_t>;

        Receiver                  receiver;
        std::optional<callback_t> callback{};

        auto start() & noexcept -> void {
            callback.emplace(ex::get_stop_token(ex::get_env(this->receiver)), stop_t{this});
        }
    };

    template <ex::receiver Receiver>
    auto connect(Receiver&& receiver) -> state<std::remove_cvref_t<Receiver>> {
        return {::std::forward<Receiver>(receiver)};
    }
};
static_assert(ex::sender_in<await_stop>);
static_assert(ex::operation_state<await_stop::state<receiver>>);

template <ex::sender Sender>
struct eager {
    using sender_concept        = ex::sender_t;
    using completion_signatures = ex::completion_signatures<ex::set_value_t(), ex::set_stopped_t()>;

    Sender sender;

    template <ex::receiver Receiver>
    struct state {
        using operation_state_concept = ex::operation_state_t;
        struct receiver {
            using receiver_concept = ex::receiver_t;
            state* st;
            auto   set_value() && noexcept -> void { ex::set_value(std::move(st->outer_receiver)); }
            template <typename E>
            auto set_error(E&& e) && noexcept -> void {
                ex::set_error(std::move(st->outer_receiver), std::forward<E>(e));
            }
            auto set_stopped() && noexcept -> void {
                st->inner_state.reset();
                ex::set_stopped(std::move(st->outer_receiver));
            }

            auto get_env() const noexcept -> env { return ex::get_env(st->outer_receiver); }
        };
        using inner_state_t = decltype(ex::connect(std::declval<Sender>(), std::declval<receiver>()));

        struct helper {
            inner_state_t st;
            template <typename S, typename R>
            helper(S&& s, R&& r) : st(ex::connect(std::forward<S>(s), std::forward<R>(r))) {}
        };

        Receiver              outer_receiver;
        std::optional<helper> inner_state;

        template <typename R, typename S>
        state(R&& r, S&& s)
            : outer_receiver(std::forward<R>(r)), inner_state(std::in_place, std::forward<S>(s), receiver{this}) {}
        auto start() & noexcept -> void {
            if (this->inner_state) {
                ex::start((*this->inner_state).st);
            } else {
                assert(this->inner_state);
            }
        }
    };
    template <ex::receiver Receiver>
    auto connect(Receiver&& receiver) {
        return state<std::remove_cvref_t<Receiver>>(std::forward<Receiver>(receiver), std::move(this->sender));
    }
};
template <ex::sender Sender>
eager(Sender&&) -> eager<std::remove_cvref_t<Sender>>;
// static_assert(ex::sender_in<eager>);
// static_assert(ex::operation_state<eager::state<receiver>>);
} // namespace

auto main() -> int {
    auto s{eager{ex::when_all(await_stop{})}};

    ex::inplace_stop_source source{};
    auto                    op{ex::connect(s, receiver{&source})};
    std::cout << "start\n";
    ex::start(op);
    std::cout << "started\n";
    source.request_stop();
    std::cout << "done\n";
}
