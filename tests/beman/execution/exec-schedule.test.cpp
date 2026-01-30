// src/beman/execution/tests/exec-schedule.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/schedule.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>
#include <utility>

// ----------------------------------------------------------------------------

namespace {
struct non_scheduler {};

struct sender {
    using sender_concept = test_std::sender_t;
    int  value{};
    auto operator==(const sender&) const -> bool = default;
};

template <bool Noexcept, typename Sender>
struct scheduler {
    int  value{};
    auto schedule() noexcept(Noexcept) { return Sender{this->value}; }
};

struct rvalue_scheduler {
    int  value{};
    auto schedule() && noexcept { return sender{this->value}; }
};

template <bool Expect, typename Scheduler>
auto test_schedule(Scheduler&& sched) {
    static_assert(Expect == requires { test_std::schedule(::std::forward<Scheduler>(sched)); });
    if constexpr (Expect) {
        static_assert(noexcept(test_std::schedule(std::forward<Scheduler>(sched))) ==
                      noexcept(std::forward<Scheduler>(sched).schedule()));
        auto value{sched.value};
        auto s = test_std::schedule(std::forward<Scheduler>(sched));
        ASSERT(s.value == value);
    }
}
} // namespace

TEST(exec_schedule) {
    static_assert(std::same_as<const test_std::schedule_t, decltype(test_std::schedule)>);
    static_assert(test_std::sender<sender>);
    test_schedule<false>(non_scheduler{});
    test_schedule<false>(scheduler<true, int>{17});
    test_schedule<true>(scheduler<false, sender>{17});
    test_schedule<true>(scheduler<true, sender>{17});
    test_schedule<true>(rvalue_scheduler{17});
}
