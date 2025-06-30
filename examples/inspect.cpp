// examples/inspectc.pp                                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/execution.hpp>
#include "meta.hpp"
#include <iostream>
#include <sstream>
#include <typeinfo>

namespace ex = beman::execution;

// ----------------------------------------------------------------------------

namespace {
struct logger_t {
    template <ex::sender Sndr, ex::receiver Rcvr, typename Log>
    struct state {
        using operation_state_concept = ex::operation_state_t;
        using inner_t                 = decltype(ex::connect(std::declval<Sndr>(), std::declval<Rcvr>()));

        inner_t                  inner;
        std::remove_cvref_t<Log> log;
        state(Sndr&& s, Rcvr&& r, Log&& l)
            : inner(ex::connect(std::forward<Sndr>(s), std::forward<Rcvr>(r))), log(std::forward<Log>(l)) {}
        auto start() & noexcept -> void {
            this->log(meta::type<decltype(ex::get_completion_signatures(std::declval<Sndr>(),
                                                                        ex::get_env(std::declval<Rcvr>())))>::name());
            ex::start(this->inner);
        }
    };

    template <ex::sender Sndr, typename Log>
    struct sender {
        using sender_concept = ex::sender_t;

        Sndr sndr;
        Log  log;

        template <typename Env>
        auto get_completion_signatures(const Env& env) const noexcept {
            return ex::get_completion_signatures(this->sndr, env);
        }

        template <ex::receiver Receiver>
        auto connect(Receiver&& receiver) && noexcept(noexcept(ex::connect(std::move(this->sndr),
                                                                           std::forward<Receiver>(receiver)))) {
            return state<Sndr, Receiver, Log>(
                std::move(this->sndr), std::forward<Receiver>(receiver), std::move(this->log));
        }
    };

    template <ex::sender Sndr, typename Log>
    auto operator()(Sndr&& sndr, Log&& log) const {
        return sender<std::remove_cvref_t<Sndr>, std::remove_cvref_t<Log>>{std::forward<Sndr>(sndr),
                                                                           std::forward<Log>(log)};
    }
};

inline constexpr logger_t logger{};
} // namespace

// ----------------------------------------------------------------------------

int main() {
    auto log = [](std::string_view name) {
        return [name](std::string_view msg) { std::cout << name << " message='" << msg << "'\n"; };
    };

    ex::sync_wait(logger(ex::just(), log("just()")));
    ex::sync_wait(logger(ex::just() | ex::then([]() {}), log("just() | then(...)")));
    ex::sync_wait(logger(ex::just() | ex::then([]() noexcept {}), log("just() | then(...)")));
    ex::sync_wait(logger(ex::just(0, 1), log("just(0, 1)")));
    ex::sync_wait(logger(ex::just(0, 1, 2), log("just(0, 1, 2)")));
    ex::sync_wait(logger(ex::just_error(0), log("just_error(0)")) | ex::upon_error([](auto) {}));
    ex::sync_wait(logger(ex::just_stopped(), log("just_stopped()")) | ex::upon_stopped([]() {}));
}
