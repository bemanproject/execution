// tests/beman/execution/task-infallible_scheduler.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/inplace_stop_source.hpp>
#endif
#include <beman/execution/detail/task/infallible_scheduler.hpp>

// ----------------------------------------------------------------------------

namespace {
template <typename Scheduler, typename... Completions>
struct sender {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<Completions...>;

    struct env {
        auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept -> Scheduler {
            return {};
        }
    };

    template <typename...>
    static consteval auto get_completion_signatures() noexcept -> completion_signatures {
        return {};
    }
    static constexpr auto get_env() noexcept -> env { return {}; }
};

template <typename... Completions>
struct scheduler {
    using scheduler_concept = test_std::scheduler_tag;

    auto schedule() const noexcept -> sender<scheduler, Completions...> { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
};

struct non_scheduler {};

struct stoppable_env {
    auto query(const test_std::get_stop_token_t&) const noexcept -> test_std::inplace_stop_token { return {}; }
};
} // namespace

// ----------------------------------------------------------------------------

TEST(task_infallible_scheduler) {
    using value_scheduler         = scheduler<test_std::set_value_t()>;
    using stoppable_scheduler     = scheduler<test_std::set_value_t(), test_std::set_stopped_t()>;
    using reversed_scheduler      = scheduler<test_std::set_stopped_t(), test_std::set_value_t()>;
    using error_scheduler         = scheduler<test_std::set_value_t(), test_std::set_error_t(int)>;
    using default_unstoppable_env = test_std::env<>;

    static_assert(test_detail::completes_with<value_scheduler, default_unstoppable_env, test_std::set_value_t()>);
    static_assert(test_detail::infallible_scheduler<value_scheduler, default_unstoppable_env>);
    static_assert(!test_detail::infallible_scheduler<stoppable_scheduler, default_unstoppable_env>);
    static_assert(test_detail::infallible_scheduler<stoppable_scheduler, stoppable_env>);
    static_assert(test_detail::infallible_scheduler<reversed_scheduler, stoppable_env>);
    static_assert(!test_detail::infallible_scheduler<error_scheduler, stoppable_env>);
    static_assert(!test_detail::infallible_scheduler<non_scheduler, stoppable_env>);
}
