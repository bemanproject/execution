// src/beman/execution/tests/exec-get-start-scheduler.test.cpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <iostream>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/get_start_scheduler.hpp>
#include <beman/execution/detail/inline_scheduler.hpp>
#include <beman/execution/detail/let.hpp>
#include <beman/execution/detail/read_env.hpp>
#include <beman/execution/detail/then.hpp>
#include <beman/execution/detail/run_loop.hpp>
#include <beman/execution/detail/starts_on.hpp>
#include <beman/execution/detail/sync_wait.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct receiver {
    using receiver_concept = test_std::receiver_tag;
    auto set_error(auto&&) && noexcept -> void {}
    auto set_stopped() && noexcept -> void {}
    auto set_value(auto&&...) && noexcept -> void {}
};

struct test_sched_sender;
struct test_sched_env;

struct test_scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    auto schedule() const -> test_sched_sender;
    auto operator==(const test_scheduler&) const -> bool = default;
    int  scheduler_id                                    = 0;
};

template <typename Rcvr>
struct test_sched_state {
    using operation_state_concept = test_std::operation_state_tag;
    auto start() noexcept { test_std::set_value(std::move(rcvr)); }
    Rcvr rcvr;
};

struct test_sched_env {
    auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept {
        return test_scheduler{scheduler_id};
    }
    int scheduler_id = 0;
};

struct test_sched_sender {
    using sender_concept = test_std::sender_tag;
    template <typename, typename...>
    static consteval auto get_completion_signatures() {
        return test_std::completion_signatures<test_std::set_value_t()>();
    }
    template <typename Rcvr>
    auto connect(Rcvr rcvr) const {
        return test_sched_state{std::move(rcvr)};
    }
    auto get_env() const noexcept { return test_sched_env{scheduler_id}; }

    int scheduler_id = 0;
};

auto test_scheduler::schedule() const -> test_sched_sender { return {scheduler_id}; }

} // namespace

TEST(exec_get_start_scheduler) {
    {
        test_scheduler sched{42};
        auto sndr = test_std::schedule(sched) |
                    test_std::let_value([]() noexcept { return test_std::read_env(test_std::get_start_scheduler); }) |
                    test_std::then([](test_scheduler sched) noexcept { ASSERT(sched == test_scheduler{42}); });
        test_std::sync_wait(std::move(sndr));
    }
    {
        test_std::sync_wait(
            test_std::schedule(test_std::inline_scheduler{}) |
            test_std::let_value([]() noexcept { return test_std::read_env(test_std::get_start_scheduler); }) |
            test_std::then([](auto sched) noexcept {
                // `sched` is the scheduler of the run_loop inside sync_wait
                static_assert(
                    std::same_as<decltype(sched), decltype(std::declval<test_std::run_loop>().get_scheduler())>);
            }));
    }
    {
        static_assert(test_std::scheduler<test_scheduler>);
        auto sched = test_scheduler{114514};
        auto [start_sched] =
            test_std::sync_wait(test_std::starts_on(sched, test_std::read_env(test_std::get_start_scheduler))).value();
        ASSERT(start_sched == sched);
    }
}
