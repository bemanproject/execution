// src/beman/execution/tests/exec-get-compl-sched.test.cpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif
#include <concepts>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace {
struct tag {};

template <typename>
struct env;

template <typename Tag>
struct sender {
    using sender_concept = test_std::sender_t;
    auto get_env() const noexcept -> env<Tag> { return {}; }
};

template <typename Tag>
struct scheduler {
    using scheduler_concept = test_std::scheduler_t;
    int  value{};
    auto operator==(const scheduler&) const -> bool = default;
    auto schedule() noexcept -> sender<Tag> { return {}; }
};

template <typename Tag>
struct env {
    int  value{};
    auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept {
        return scheduler<Tag>{this->value + 1};
    }
    auto query(const test_std::get_completion_scheduler_t<test_std::set_error_t>&) const noexcept {
        return scheduler<test_std::set_error_t>{this->value + 2};
    }
    auto query(const test_std::get_completion_scheduler_t<test_std::set_stopped_t>&) const noexcept {
        return scheduler<test_std::set_stopped_t>{this->value + 3};
    }
    template <typename T>
    auto query(const test_std::get_completion_scheduler_t<T>&) const noexcept {
        return scheduler<T>{this->value};
    }
};

template <bool Expect, typename Tag, typename Env>
auto test_tag(Env&& env) -> void {
    static_assert(Expect == requires { test_std::get_completion_scheduler<Tag>(env); });
}
} // namespace

TEST(exec_get_compl_sched) {
    static_assert(test_std::sender<sender<test_std::set_error_t>>);
    static_assert(test_std::sender<sender<test_std::set_stopped_t>>);
    static_assert(test_std::sender<sender<test_std::set_value_t>>);
    static_assert(test_std::scheduler<scheduler<test_std::set_error_t>>);
    static_assert(test_std::scheduler<scheduler<test_std::set_stopped_t>>);
    static_assert(test_std::scheduler<scheduler<test_std::set_value_t>>);

    static_assert(std::same_as<const test_std::get_completion_scheduler_t<test_std::set_error_t>,
                               decltype(test_std::get_completion_scheduler<test_std::set_error_t>)>);
    static_assert(std::same_as<const test_std::get_completion_scheduler_t<test_std::set_stopped_t>,
                               decltype(test_std::get_completion_scheduler<test_std::set_stopped_t>)>);
    static_assert(std::same_as<const test_std::get_completion_scheduler_t<test_std::set_value_t>,
                               decltype(test_std::get_completion_scheduler<test_std::set_value_t>)>);
    static_assert(test_std::forwarding_query(test_std::get_completion_scheduler<test_std::set_error_t>));
    static_assert(test_std::forwarding_query(test_std::get_completion_scheduler<test_std::set_stopped_t>));
    static_assert(test_std::forwarding_query(test_std::get_completion_scheduler<test_std::set_value_t>));

    env<test_std::set_value_t> e{17};
    test_tag<true, test_std::set_error_t>(e);
    test_tag<true, test_std::set_stopped_t>(e);
    test_tag<true, test_std::set_value_t>(e);
    test_tag<false, tag>(e);

    static_assert(::std::same_as<decltype(test_std::get_completion_scheduler<test_std::set_error_t>(e)),
                                 scheduler<test_std::set_error_t>>);
    static_assert(::std::same_as<decltype(test_std::get_completion_scheduler<test_std::set_stopped_t>(e)),
                                 scheduler<test_std::set_stopped_t>>);
    static_assert(::std::same_as<decltype(test_std::get_completion_scheduler<test_std::set_value_t>(e)),
                                 scheduler<test_std::set_value_t>>);
    ASSERT(test_std::get_completion_scheduler<test_std::set_error_t>(e) == scheduler<test_std::set_error_t>{19});
    ASSERT(test_std::get_completion_scheduler<test_std::set_stopped_t>(e) == scheduler<test_std::set_stopped_t>{20});
    ASSERT(test_std::get_completion_scheduler<test_std::set_value_t>(e) == scheduler<test_std::set_value_t>{18});
}
