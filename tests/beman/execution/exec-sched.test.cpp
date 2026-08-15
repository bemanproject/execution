// src/beman/execution/tests/exec-sched.test.cpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/infallible_scheduler.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct bad_env {
    auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept { return 0; }
};

template <typename Scheduler>
struct env {
    auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept {
        return Scheduler{};
    }
};

struct infallible_env {};
struct stoppable_env {
    auto query(test_std::get_stop_token_t) const noexcept { return test_std::inplace_stop_token{}; }
};
struct fallible_env {
    auto query(test_std::get_stop_token_t) const noexcept { return test_std::inplace_stop_token{}; }
};

template <typename Env>
struct sender {
    using sender_concept = test_std::sender_tag;
    template <typename, typename... Ev>
    static consteval auto get_completion_signatures() noexcept {
        if constexpr (sizeof...(Ev) == 0 ||
                      (false || ... ||
                       test_std::unstoppable_token<decltype(test_std::get_stop_token(std::declval<Ev>()))>)) {
            return test_std::completion_signatures<test_std::set_value_t()>();
        } else if constexpr (sizeof...(Ev) == 1 && (false || ... || std::same_as<fallible_env, Ev>)) {
            return test_std::completion_signatures<test_std::set_value_t(), test_std::set_error_t(int)>();
        } else {
            return test_std::completion_signatures<test_std::set_value_t(), test_std::set_stopped_t()>();
        }
    }
    auto get_env() const noexcept { return Env{}; }
};

struct no_scheduler_concept {
    auto schedule() -> sender<env<no_scheduler_concept>> { return {}; }
    auto operator==(const no_scheduler_concept&) const -> bool = default;
};

struct not_queryable {
    using scheduler_concept = test_std::scheduler_tag;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }
    not_queryable()                                        = default;
    not_queryable(const not_queryable&)                    = default;
    not_queryable(not_queryable&&)                         = default;
    ~not_queryable()                                       = delete;
    auto operator=(const not_queryable&) -> not_queryable& = default;
    auto operator=(not_queryable&&) -> not_queryable&      = default;
    auto schedule() -> sender<env<not_queryable>> { return {}; }
    auto operator==(const not_queryable&) const -> bool = default;
};

struct no_schedule {
    using scheduler_concept = test_std::scheduler_tag;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }
    auto operator==(const no_schedule&) const -> bool = default;
};

struct not_equality_comparable {
    using scheduler_concept = test_std::scheduler_tag;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }
    auto schedule() -> sender<env<not_equality_comparable>> { return {}; }
};

struct not_copy_constructible {
    using scheduler_concept = test_std::scheduler_tag;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }
    not_copy_constructible(const not_copy_constructible&)                    = delete;
    not_copy_constructible(not_copy_constructible&&)                         = default;
    ~not_copy_constructible()                                                = default;
    auto operator=(const not_copy_constructible&) -> not_copy_constructible& = delete;
    auto operator=(not_copy_constructible&&) -> not_copy_constructible&      = default;
    auto schedule() -> sender<env<not_copy_constructible>> { return {}; }
    auto operator==(const not_copy_constructible&) const -> bool = default;
};

struct scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }
    auto schedule() -> sender<env<scheduler>> { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
};

struct indirect_completion_scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept {
        return test_std::forward_progress_guarantee::weakly_parallel;
    }
    auto schedule() -> sender<env<scheduler>> { return {}; }
    auto operator==(const indirect_completion_scheduler&) const -> bool = default;
};

template <bool Expect, typename Signal, typename Result, typename Env>
auto test_get_completion_scheduler(Result&& result, Env&& env) -> void {
    static_assert(Expect == requires { test_std::get_completion_scheduler<Signal>(env); });
    if constexpr (Expect) {
        static_assert(::std::same_as<std::remove_cvref_t<Result>,
                                     std::remove_cvref_t<decltype(test_std::get_completion_scheduler<Signal>(env))>>);
        ASSERT(result == test_std::get_completion_scheduler<Signal>(env));
    }
}

template <bool Expect, typename Scheduler>
auto test_scheduler() -> void {
    static_assert(Expect == test_std::scheduler<Scheduler>);
}

auto test_infallible_scheduler() -> void {
    static_assert(test_detail::infallible_scheduler<scheduler, infallible_env>);
    static_assert(test_detail::infallible_scheduler<scheduler, stoppable_env>);
    static_assert(not test_detail::infallible_scheduler<scheduler, fallible_env>);
}
} // namespace

TEST(exec_sched) {
    static_assert(test_std::sender<sender<env<scheduler>>>);
    static_assert(std::same_as<env<scheduler>, decltype(test_std::get_env(sender<env<scheduler>>{}))>);

    test_get_completion_scheduler<false, test_std::set_value_t>(0, bad_env{});

    test_scheduler<false, int>();
    test_scheduler<false, no_scheduler_concept>();
    test_scheduler<false, not_queryable>();
    test_scheduler<false, no_schedule>();
    test_scheduler<false, not_equality_comparable>();
    test_scheduler<false, not_copy_constructible>();
    test_scheduler<true, indirect_completion_scheduler>();
    test_scheduler<true, scheduler>();
    test_infallible_scheduler();
}
