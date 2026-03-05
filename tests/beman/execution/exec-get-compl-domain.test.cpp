// tests/beman/execution/exec-get-compl-domain.test.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#include <cstddef>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.get_completion_domain;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.operation_state;
import beman.execution.detail.sender;
import beman.execution.detail.scheduler;
import beman.execution.detail.scheduler_t;
import beman.execution.detail.get_env;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.schedule;
import beman.execution.detail.get_completion_signatures;
import beman.execution.detail.completion_signatures;
#else
#include <beman/execution/detail/get_completion_domain.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/scheduler_t.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
    struct throwing_domain {
        throwing_domain() noexcept(false) = default;
    };
    template <std::size_t>
    struct domain {
        int value{};
        auto operator==(const domain&) const -> bool = default;
    };

    template <std::size_t> struct test_env {};

template <bool Value, typename CPO>
void test_get_completion_domain_template() {
    static_assert(Value == requires { beman::execution::get_completion_domain<CPO>; });
}

template <typename Tag, typename Q = Tag>
auto test_get_completion_domain_tag() {
    struct queryable_tag {
        auto query(test_std::get_completion_domain_t<Q>, test_env<0>) const { return domain<0>{42}; }
        auto query(test_std::get_completion_domain_t<Q>, test_env<1>) const { return domain<1>{42}; }
        auto query(test_std::get_completion_domain_t<Q>) const { return domain<2>{42}; }
        auto query(test_std::get_completion_domain_t<Q>, test_env<3>) const { return throwing_domain{}; }
    };

    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(queryable_tag{}, test_env<0>{})), domain<0>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(queryable_tag{}, test_env<0>{}) == domain<0>{});
    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(queryable_tag{}, test_env<1>{})), domain<1>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(queryable_tag{}, test_env<1>{}) == domain<1>{});
    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(queryable_tag{})), domain<2>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(queryable_tag{}) == domain<2>{});
    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(queryable_tag{}, test_env<2>{})), domain<2>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(queryable_tag{}, test_env<2>{}) == domain<2>{});
}

    template <typename Q>
    struct scheduler {
        using scheduler_concept = test_std::scheduler_t;
        struct state {
            using operation_state_concept = test_std::operation_state_t;
             auto start() noexcept {}
        };
        struct env {
            auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept { return scheduler{}; }
        };
        struct sender {
            using sender_concept = test_std::sender_t;
            static consteval auto get_completion_signatures() { return test_std::completion_signatures<test_std::set_value_t()>(); }
            auto connect(auto&&) const { return state{}; }
            auto get_env() const noexcept { return env{}; }  
        };
        auto schedule() const { return sender{}; }

        bool operator==(const scheduler&) const = default;

        auto query(test_std::get_completion_domain_t<Q>, test_env<0>) const { return domain<0>{42}; }
        auto query(test_std::get_completion_domain_t<Q>, test_env<1>) const { return domain<1>{42}; }
        auto query(test_std::get_completion_domain_t<Q>) const { return domain<2>{42}; }
        auto query(test_std::get_completion_domain_t<Q>, test_env<3>) const { return throwing_domain{}; }
    };
    static_assert(beman::execution::scheduler<scheduler<void>>);

template <typename Tag, typename Q = Tag>
auto test_get_completion_domain_tag_via_scheduler() {
    struct initial_queryable{
            auto query(test_std::get_completion_scheduler_t<Q>, test_env<0>) const noexcept { return scheduler<Q>{}; }
            auto query(test_std::get_completion_scheduler_t<Q>, test_env<1>) const noexcept { return scheduler<Q>{}; }
            auto query(test_std::get_completion_scheduler_t<Q>) const noexcept { return scheduler<Q>{}; }
            auto query(test_std::get_completion_scheduler_t<Q>, test_env<2>) const noexcept { return scheduler<Q>{}; }
    };

    test_std::get_completion_scheduler<Tag>(initial_queryable{});
    initial_queryable().query(test_std::get_completion_scheduler<Tag>, test_env<0>{});
    //-dk:TODO test_std::get_completion_scheduler<Tag>(initial_queryable{}, test_env<0>{});

    //static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(initial_queryable{}, test_env<0>{})), domain<0>>);
    #if 0
    ASSERT(beman::execution::get_completion_domain<Tag>(initial_queryable{}, test_env<0>{}) == domain<0>{});
    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(initial_queryable{}, test_env<1>{})), domain<1>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(initial_queryable{}, test_env<1>{}) == domain<1>{});
    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(initial_queryable{})), domain<2>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(initial_queryable{}) == domain<2>{});
    static_assert(std::same_as<decltype(beman::execution::get_completion_domain<Tag>(initial_queryable{}, test_env<2>{})), domain<2>>);
    ASSERT(beman::execution::get_completion_domain<Tag>(initial_queryable{}, test_env<2>{}) == domain<2>{});
    #endif
}
} // namespace

TEST(exec_with_awaitable_senders) {
    test_get_completion_domain_template<true, void>();
    test_get_completion_domain_template<true, test_std::set_error_t>();
    test_get_completion_domain_template<true, test_std::set_stopped_t>();
    test_get_completion_domain_template<true, test_std::set_value_t>();
    //-dk:TODO test compilation failure: test_get_completion_domain_template<false, int>();

    test_get_completion_domain_tag<void>();
    test_get_completion_domain_tag<void, test_std::set_value_t>();
    test_get_completion_domain_tag<test_std::set_error_t>();
    test_get_completion_domain_tag<test_std::set_stopped_t>();
    test_get_completion_domain_tag<test_std::set_value_t>();

    test_get_completion_domain_tag_via_scheduler<test_std::set_value_t>();
}
