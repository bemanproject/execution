// src/beman/execution26/tests/exec-let.pass.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/let.hpp>
#include <beman/execution26/detail/just.hpp>
#include <beman/execution26/detail/then.hpp>
#include <test/execution.hpp>
#include <cstdlib>
#include <concepts>
#include <iostream>

// ----------------------------------------------------------------------------

namespace
{
    struct receiver {
        using receiver_concept = test_std::receiver_t;
        auto set_error(auto&&) && noexcept -> void {}
        auto set_stopped() && noexcept -> void {}
        auto set_value(auto&&...) && noexcept -> void {}
    };
    template <typename... Sigs>
    struct test_sender
    {
        using sender_concept = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<Sigs...>;

        struct state {
            using operation_state_concept = test_std::operation_state_t;
            auto start() & noexcept -> void {}
        };
        auto connect(auto&&) -> state { return {}; }
    };

    auto test_let_value()
    {
        auto s0{test_std::let_value(test_std::just(),
            []{ return test_std::just() | test_std::then([]{ std::cout << ">>executing let_value sender<<\n"; }); })};
        auto s1{test_std::just() | test_std::let_value([]{ return test_std::just(); })};
        static_assert(test_std::sender<decltype(s0)>);
        static_assert(test_std::sender<decltype(s1)>);

        static_assert(test_std::sender<test_sender<test_std::set_value_t()>>);
        static_assert(test_std::operation_state<test_sender<test_std::set_value_t()>::state>);
        auto s2{
            test_sender<
                test_std::set_value_t(),
                test_std::set_value_t(int),
                test_std::set_value_t(int&),
                test_std::set_error_t(int),
                test_std::set_stopped_t()
            >()
            | test_std::let_value([](auto&&...){ return test_std::just(); })
        };
        static_assert(test_std::sender<decltype(s2)>);

        auto state2{test_std::connect(s2, receiver{})};
        test::use(state2);
        test_std::start(state2);

        auto state0{test_std::connect(s0, receiver{})};
        test::use(state0);
        test_std::start(state0);
        auto state1{test_std::connect(s1, receiver{})};
        test::use(state1);
        test_std::start(state1);
    }
}

// ----------------------------------------------------------------------------

auto main() -> int
{
    static_assert(std::same_as<test_std::let_error_t const, decltype(test_std::let_error)>);
    static_assert(std::same_as<test_std::let_stopped_t const, decltype(test_std::let_stopped)>);
    static_assert(std::same_as<test_std::let_value_t const, decltype(test_std::let_value)>);

    test_let_value();

    return EXIT_SUCCESS;
}