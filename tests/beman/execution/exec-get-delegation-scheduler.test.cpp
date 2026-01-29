// src/beman/execution/tests/exec-get-delegation-scheduler.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/get_delegation_scheduler.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct scheduler;

template <bool Noexcept, typename Scheduler>
struct env {
    int value{};

    auto query(const test_std::get_delegation_scheduler_t&) const noexcept(Noexcept) { return Scheduler{this->value}; }

    template <typename Tag>
    auto query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept;
};

struct scheduler {
    using scheduler_concept = test_std::scheduler_t;

    struct sender {
        using sender_concept = test_std::sender_t;
        int  value{};
        auto get_env() const noexcept -> env<true, scheduler> { return {this->value}; }
    };

    int value{};

    auto schedule() noexcept -> sender { return {this->value}; }
    auto operator==(const scheduler&) const -> bool = default;
};

template <bool Noexcept, typename Scheduler>
template <typename Tag>
auto env<Noexcept, Scheduler>::query(const test_std::get_completion_scheduler_t<Tag>&) const noexcept {
    return scheduler{this->value};
}

template <bool Expect, typename Scheduler = void>
auto test_get_delegation_scheduler(auto&& env) -> void {
    static_assert(Expect == requires { test_std::get_delegation_scheduler(env); });
    if constexpr (Expect) {
        ASSERT(17 == test_std::get_delegation_scheduler(env).value);
        // ASSERT(Scheduler{env.value} == test_std::get_delegation_scheduler(env));
    }
}
} // namespace

TEST(exec_get_delegation_scheduler) {
    static_assert(test_std::sender<scheduler::sender>);
    static_assert(test_std::scheduler<scheduler>);
    static_assert(
        std::same_as<const test_std::get_delegation_scheduler_t, decltype(test_std::get_delegation_scheduler)>);
    static_assert(test_std::forwarding_query((test_std::get_delegation_scheduler)));

    test_get_delegation_scheduler<false>(test_std::env<>{});
    test_get_delegation_scheduler<false>(env<false, scheduler>{});
    test_get_delegation_scheduler<false>(env<true, scheduler::sender>{});
    test_get_delegation_scheduler<true>(env<true, scheduler>{17});
}
