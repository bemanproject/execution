// examples/intro-1-hello-world.cpp                                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/execution.hpp>
#include <beman/execution/detail/suppress_push.hpp>
#include <expected>
#include <iostream>
#include <string>
#include <tuple>
#include <cinttypes>

namespace ex = ::beman::execution;
using namespace std::string_literals;

enum class success : std::uint8_t { one };
enum class failure : std::uint8_t { fail_one };

struct expected_to_channel_t {
    template <typename Receiver>
    struct own_receiver {
        using receiver_concept = ex::receiver_t;
        Receiver* receiver;
        template <typename Value, typename Error>
        auto set_value(std::expected<Value, Error>&& exp) noexcept -> void {
            if (exp) {
                std::cout << "received an expected with value from child/upstream\n" << std::flush;
                ex::set_value(std::move(*receiver), exp.value_or(Value{}));
                std::cout << "set_value\n" << std::flush;
            } else {
                std::cout << "received an expected with error from child/upstream\n";
                ex::set_error(std::move(*receiver), exp.error_or(Error{}));
            }
        }
        template <typename Error>
        auto set_error(Error&& error) noexcept -> void {
            std::cout << "received an error from child/upstream";
            ex::set_error(std::move(*receiver), std::forward<Error>(error));
        }
        auto set_stopped() noexcept -> void {
            std::cout << "received an cancelletion from child/upstream";
            ex::set_stopped(std::move(*receiver));
        }
    };
    template <ex::sender CSender, ex::receiver Receiver>
    struct state {
        using operation_state_concept = ex::operation_state_t;
        using child_state_t = decltype(ex::connect(std::declval<CSender>(), std::declval<own_receiver<Receiver>>()));
        Receiver      parent_receiver;
        child_state_t child_state;
        template <ex::sender S, ex::receiver R>
        state(S&& child_sender, R&& parent_receiver)
            : parent_receiver(std::forward<R>(parent_receiver)),
              child_state(ex::connect(std::forward<S>(child_sender), own_receiver<Receiver>{&this->parent_receiver})) {
        }
        void start() & noexcept {
            std::cout << "starting execpted_to_channel\n";
            ex::start(child_state);
        }
    };

    template <ex::sender CSender>
    struct sender {
        using sender_concept = ex::sender_t;
        // value_types_of<CSender....> -> set_value_t(std::expected<T, E>)
        // -> completion_signatures<set_value_t(T), set_error_t(E)>
        // -> + error_type_of<CSender...>
        // -> + sends_stopped<CSender...> -> set_stopped_t()
        // -> unique
        using completion_signatures = ex::completion_signatures<ex::set_value_t(success), ex::set_error_t(failure)>;
        CSender child_sender;

        template <ex::receiver Receiver>
        state<CSender, std::remove_cvref_t<Receiver>> connect(Receiver&& receiver) && {
            return {std::move(this->child_sender), std::forward<Receiver>(receiver)};
        }
        template <ex::receiver Receiver>
        state<CSender, std::remove_cvref_t<Receiver>> connect(Receiver&& receiver) const& {
            return {this->child_sender, std::forward<Receiver>(receiver)};
        }
    };

    template <ex::sender CSender>
    sender<std::remove_cvref_t<CSender>> operator()(CSender&& child_sender) const {
        return {std::forward<CSender>(child_sender)};
    }
    auto operator()() const { return ex::detail::sender_adaptor{*this}; }
};
inline constexpr expected_to_channel_t expected_to_channel{};

int main() {
    ex::sync_wait(ex::just(std::expected<success, failure>(success::one)) | expected_to_channel() |
                  ex::then([](success) noexcept { std::cout << "success\n"; }) |
                  ex::upon_error([](failure) noexcept { std::cout << "fail\n"; }));
}
