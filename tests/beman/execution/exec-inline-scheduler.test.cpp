// tests/beman/execution/exec-inline-scheduler.test.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <type_traits>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {

struct test_receiver {
    using receiver_concept = test_std::receiver_tag;
    bool& called;
    auto  set_value() && noexcept -> void { called = true; }
    auto  get_env() const noexcept { return test_std::env{}; }
};

struct custom_scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    int id{};

    struct state {
        using operation_state_concept = test_std::operation_state_tag;
        auto start() noexcept -> void {}
    };
    struct env {
        int  id{};
        auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept {
            return custom_scheduler{id};
        }
    };
    struct sender {
        using sender_concept = test_std::sender_tag;
        int                   id{};
        static consteval auto get_completion_signatures() {
            return test_std::completion_signatures<test_std::set_value_t()>();
        }
        template <typename R>
        auto connect(R&&) const {
            return state{};
        }
        auto get_env() const noexcept { return env{id}; }
    };
    auto schedule() const { return sender{id}; }
    auto operator==(const custom_scheduler&) const -> bool = default;
};

struct custom_domain {
    auto operator==(const custom_domain&) const -> bool = default;
};

struct sched_with_domain {
    using scheduler_concept = test_std::scheduler_tag;

    struct state {
        using operation_state_concept = test_std::operation_state_tag;
        auto start() noexcept -> void {}
    };
    struct sender {
        using sender_concept = test_std::sender_tag;
        static consteval auto get_completion_signatures() {
            return test_std::completion_signatures<test_std::set_value_t()>();
        }
        template <typename R>
        auto connect(R&&) const {
            return state{};
        }
    };
    auto schedule() const { return sender{}; }
    auto operator==(const sched_with_domain&) const -> bool = default;
    auto query(test_std::get_domain_t) const noexcept { return custom_domain{}; }
};

// Receiver whose env provides a scheduler
struct sched_env_receiver {
    using receiver_concept = test_std::receiver_tag;
    bool& called;

    struct env {
        auto query(test_std::get_scheduler_t) const noexcept { return custom_scheduler{42}; }
    };
    auto set_value() && noexcept -> void { called = true; }
    auto get_env() const noexcept { return env{}; }
};

struct domain_env_receiver {
    using receiver_concept = test_std::receiver_tag;
    bool& called;

    struct env {
        auto query(test_std::get_scheduler_t) const noexcept { return sched_with_domain{}; }
    };
    auto set_value() && noexcept -> void { called = true; }
    auto get_env() const noexcept { return env{}; }
};

auto test_scheduler_concept() {
    static_assert(test_std::scheduler<test_std::inline_scheduler>);
    static_assert(test_std::scheduler<const test_std::inline_scheduler>);
    static_assert(test_std::scheduler<test_std::inline_scheduler&>);
    static_assert(test_std::scheduler<const test_std::inline_scheduler&>);
}

auto test_schedule_returns_sender() {
    auto sndr = test_std::schedule(test_std::inline_scheduler{});
    static_assert(test_std::sender<decltype(sndr)>);

    // Completion signatures: set_value_t()
    using sigs = test_std::completion_signatures<test_std::set_value_t()>;
    static_assert(std::same_as<sigs, typename decltype(sndr)::completion_signatures>);
}

auto test_equality() {
    test_std::inline_scheduler s1;
    test_std::inline_scheduler s2;

    // All inline_schedulers are equal
    ASSERT(s1 == s2);
    ASSERT(!(s1 != s2));
}

auto test_copyable() {
    static_assert(std::copyable<test_std::inline_scheduler>);

    test_std::inline_scheduler s1;
    test_std::inline_scheduler s2 = s1;
    ASSERT(s1 == s2);
}

auto test_connect_and_start() {
    bool called = false;
    auto sndr   = test_std::schedule(test_std::inline_scheduler{});
    auto op     = test_std::connect(sndr, test_receiver{called});
    static_assert(test_std::operation_state<decltype(op)>);
    ASSERT(!called);
    test_std::start(op);
    ASSERT(called);
}

auto test_sender_env_is_inline_attrs() {
    auto sndr  = test_std::schedule(test_std::inline_scheduler{});
    auto attrs = test_std::get_env(sndr);
    static_assert(std::same_as<decltype(attrs), test_detail::inline_attrs<test_std::set_value_t>>);
}

auto test_inline_attrs_completion_scheduler() {
    auto attrs = test_detail::inline_attrs<test_std::set_value_t>{};

    struct test_env {
        auto query(test_std::get_scheduler_t) const noexcept { return custom_scheduler{42}; }
    };

    auto sched = attrs.query(test_std::get_completion_scheduler<test_std::set_value_t>, test_env{});
    static_assert(std::same_as<decltype(sched), custom_scheduler>);
    ASSERT(sched == custom_scheduler{42});
}

auto test_inline_attrs_completion_domain() {
    auto attrs = test_detail::inline_attrs<test_std::set_value_t>{};

    struct domain_env {
        auto query(test_std::get_domain_t) const noexcept { return custom_domain{}; }
    };

    auto dom = attrs.query(test_std::get_completion_domain<test_std::set_value_t>, domain_env{});
    static_assert(std::same_as<decltype(dom), custom_domain>);
}

auto test_connect_with_scheduler_env() {
    bool called = false;
    auto sndr   = test_std::schedule(test_std::inline_scheduler{});
    auto op     = test_std::connect(sndr, sched_env_receiver{called});
    ASSERT(!called);
    test_std::start(op);
    ASSERT(called);
}

auto test_get_completion_scheduler_with_env() {
    auto sndr  = test_std::schedule(test_std::inline_scheduler{});
    auto attrs = test_std::get_env(sndr);

    struct test_env {
        auto query(test_std::get_scheduler_t) const noexcept { return custom_scheduler{7}; }
    };

    auto sched = test_std::get_completion_scheduler<test_std::set_value_t>(attrs, test_env{});
    static_assert(std::same_as<decltype(sched), custom_scheduler>);
    ASSERT(sched == custom_scheduler{7});
}

auto test_get_completion_domain_with_env() {
    auto sndr  = test_std::schedule(test_std::inline_scheduler{});
    auto attrs = test_std::get_env(sndr);

    struct test_env {
        auto query(test_std::get_domain_t) const noexcept { return custom_domain{}; }
    };

    auto dom = test_std::get_completion_domain<test_std::set_value_t>(attrs, test_env{});
    static_assert(std::same_as<decltype(dom), custom_domain>);
}

} // namespace

TEST(exec_inline_scheduler) {
    test_scheduler_concept();
    test_schedule_returns_sender();
    test_equality();
    test_copyable();
    test_connect_and_start();
    test_sender_env_is_inline_attrs();
    test_inline_attrs_completion_scheduler();
    test_inline_attrs_completion_domain();
    test_connect_with_scheduler_env();
    test_get_completion_scheduler_with_env();
    test_get_completion_domain_with_env();
}
