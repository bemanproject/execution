// examples/stopping.cpp                                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// The example shows a bit of how cancellation works:
// It creates work with a sender injecting a stop token
// which gets started in a thread. To stop this thread
// the corresponding stop source is requested to stop.

#include <chrono>
#include <iostream>
#include <thread>
#include <cassert>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#include <beman/execution/stop_token.hpp>
#endif

using namespace std::chrono;
namespace ex = beman::execution;

// ----------------------------------------------------------------------------

namespace {
struct env {
    ex::inplace_stop_token token;

    explicit env(ex::inplace_stop_token t) : token(t) {} // NOLINT(hicpp-explicit-conversions)

    auto query(const ex::get_stop_token_t&) const noexcept { return this->token; }
};

template <typename Sender>
struct inject_cancel_sender {
    using sender_concept = ex::sender_t;

    template <typename Receiver>
    struct receiver {
        using receiver_concept = ex::receiver_t;

        std::remove_cvref_t<Receiver> inner_receiver;
        ex::inplace_stop_token        token{};

        auto get_env() const noexcept -> env { return env(this->token); }

        template <typename... T>
        auto set_value(T&&... t) noexcept -> void {
            ex::set_value(std::move(inner_receiver), std::forward<T>(t)...);
        }
        template <typename E>
        auto set_error(E&& e) noexcept -> void {
            ex::set_error(std::move(inner_receiver), std::forward<E>(e));
        }
        auto set_stopped() noexcept -> void { ex::set_stopped(std::move(inner_receiver)); }
    };

    template <typename E>
    auto get_completion_signatures(const E&) {
        return ex::get_completion_signatures(this->sender, env{ex::inplace_stop_token{}});
    }

    ex::inplace_stop_token token{};
    Sender                 sender;

    template <typename Receiver>
    auto connect(Receiver&& recv) && {
        return ex::connect(std::move(sender), receiver<Receiver>{std::forward<Receiver>(recv), this->token});
    }
};

template <typename S>
inject_cancel_sender(ex::inplace_stop_token, S&&) -> inject_cancel_sender<std::decay_t<S>>;

struct receiver {
    using receiver_concept = ex::receiver_t;
    auto set_value(auto&&...) noexcept -> void {}
    auto set_error(auto&&) noexcept -> void {}
    auto set_stopped() noexcept -> void {}
};
} // namespace

int main() {
    ex::inplace_stop_source source;

    std::thread t([token = source.get_token()] {
        ex::sync_wait(
            inject_cancel_sender{token, ex::read_env(ex::get_stop_token) | ex::then([](ex::inplace_stop_token tok) {
                                            while (not tok.stop_requested()) {
                                                std::cout << "sleeping\n";
                                                std::this_thread::sleep_for(10ms);
                                            }
                                        })});
    });

    // std::cin.get();
    std::this_thread::sleep_for(100ms);
    std::cout << "requesting stop\n";
    source.request_stop();

    t.join();
}
