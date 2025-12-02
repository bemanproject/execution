// src/beman/execution/tests/exec-run-loop-types.test.cpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/run_loop.hpp>

#include <beman/execution/detail/inplace_stop_source.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_stop_token.hpp>
#include <beman/execution/detail/receiver_of.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>

#include <test/execution.hpp>
#include <concepts>
#include <exception>

// ----------------------------------------------------------------------------

namespace {
auto use(auto&&...) -> void {}

enum class signal_type : unsigned char { none, error, stopped, value };

struct token_env {
    test_std::inplace_stop_token token;
    auto                         query(const test_std::get_stop_token_t&) const noexcept { return this->token; }
};

struct receiver {
    using receiver_concept = test_std::receiver_t;

    signal_type*                 result;
    test_std::inplace_stop_token token;

    auto set_value() && noexcept { *result = signal_type::value; }
    auto set_error(const std::exception_ptr&) && noexcept { *result = signal_type::error; }
    auto set_stopped() && noexcept { *result = signal_type::stopped; }

    auto get_env() const noexcept -> token_env { return {this->token}; }
};

struct finish_receiver {
    test_std::run_loop* loop;
    using receiver_concept = test_std::receiver_t;

    auto set_value() && noexcept { this->loop->finish(); }
    auto set_error(const std::exception_ptr&) && noexcept { this->loop->finish(); }
    auto set_stopped() && noexcept { this->loop->finish(); }
};

auto test_run_loop_scheduler_equality() -> void {
    // p3:
    test_std::run_loop rl1;
    test_std::run_loop rl2;

    ASSERT(rl1.get_scheduler() == rl1.get_scheduler());
    ASSERT(rl2.get_scheduler() == rl2.get_scheduler());
    ASSERT(rl1.get_scheduler() != rl2.get_scheduler());
}
} // namespace

TEST(exec_run_loop_types) {
    test_std::run_loop rl;
    // p1:
    static_assert(requires {
        { rl.get_scheduler() } -> test_std::scheduler;
    });
    test_run_loop_scheduler_equality();

    // p4:
    auto scheduler{rl.get_scheduler()};
    static_assert(requires {
        { test_std::schedule(scheduler) } noexcept -> test_std::sender;
    });

    // p5:
    auto sender{test_std::schedule(scheduler)};
    struct env {};
    test_std::inplace_stop_source source{};
    struct token_env {
        test_std::inplace_stop_token token;
        auto                   query(const test_std::get_stop_token_t&) const noexcept { return this->token; }
    };
    static_assert(::std::same_as<test_std::completion_signatures<test_std::set_value_t()>,
                                 decltype(test_std::get_completion_signatures(sender, env{}))>);
    static_assert(::std::same_as<test_std::completion_signatures<test_std::set_value_t(), test_std::set_stopped_t()>,
                                 decltype(test_std::get_completion_signatures(sender, token_env{source.get_token()}))>);
    // p7:
    static_assert(test_std::receiver_of<receiver, decltype(test_std::get_completion_signatures(sender, env{}))>);
    // p7.1:
    static_assert(requires {
        { test_std::connect(sender, receiver{}) } noexcept -> test_std::operation_state;
    });
    // p7.2:
    auto e{test_std::get_env(sender)};
    static_assert(requires {
        { test_std::get_completion_scheduler<test_std::set_error_t>(e) } noexcept -> std::same_as<decltype(scheduler)>;
        {
            test_std::get_completion_scheduler<test_std::set_stopped_t>(e)
        } noexcept -> std::same_as<decltype(scheduler)>;
        { test_std::get_completion_scheduler<test_std::set_value_t>(e) } noexcept -> std::same_as<decltype(scheduler)>;
    });
    ASSERT(scheduler == test_std::get_completion_scheduler<test_std::set_error_t>(e));
    ASSERT(scheduler == test_std::get_completion_scheduler<test_std::set_stopped_t>(e));
    ASSERT(scheduler == test_std::get_completion_scheduler<test_std::set_value_t>(e));

    // p8, p9* can't be tested directly.
    signal_type                   unstopped_result{signal_type::none};
    signal_type                   stopped_result{signal_type::none};
    test_std::inplace_stop_source unstopped;
    test_std::inplace_stop_source stopped;
    stopped.request_stop();

    auto unstopped_op{test_std::connect(sender, receiver{&unstopped_result, unstopped.get_token()})};
    auto stopped_op{test_std::connect(sender, receiver{&stopped_result, stopped.get_token()})};
    auto finish_op{test_std::connect(sender, finish_receiver{&rl})};

    test_std::start(finish_op);
    test_std::start(unstopped_op);
    test_std::start(stopped_op);

    rl.run();

    ASSERT(unstopped_result == signal_type::value);
    ASSERT(stopped_result == signal_type::stopped);

    //-dk:TODO more thorough run_loop tests
}
