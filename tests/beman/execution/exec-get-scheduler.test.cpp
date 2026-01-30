// src/beman/execution/tests/exec-get-scheduler.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace {
struct scheduler {
    int  value{};
    auto operator==(const scheduler&) const -> bool = default;
};

struct env {
    int  value{};
    auto query(const test_std::get_scheduler_t&) const noexcept { return scheduler{this->value}; }
};
} // namespace

TEST(exec_get_scheduler) {
    static_assert(std::same_as<const test_std::get_scheduler_t, decltype(test_std::get_scheduler)>);
    static_assert(test_std::forwarding_query(test_std::get_scheduler));
    env  e{17};
    auto sched{test_std::get_scheduler(e)};
    static_assert(::std::same_as<scheduler, decltype(sched)>);
    ASSERT(sched == scheduler{17});
}
