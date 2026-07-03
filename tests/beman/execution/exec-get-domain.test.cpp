// src/beman/execution/tests/exec-get-domain.test.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct domain {
    int value{};
};

struct no_get_domain {};

template <bool Noexcept>
struct non_const_get_domain {
    auto query(const test_std::get_domain_t&) noexcept(Noexcept) -> domain { return {}; }
};

template <bool Noexcept, typename Result>
struct has_get_domain {
    int            value{};
    constexpr auto query(const test_std::get_domain_t&) const noexcept(Noexcept) -> Result { return {this->value}; }
};

struct overloaded_get_domain {
    auto query(const test_std::get_domain_t&) const noexcept -> domain { return {}; }
    auto query(const test_std::get_domain_t&) noexcept -> void = delete;
};

template <typename Result, typename Object>
auto test_get_domain(Object&& object) {
    if constexpr (requires { test_std::get_domain(object); }) {
        static_assert(std::same_as<Result, decltype(test_std::get_domain(object))>);
    }
}

struct sched_domain {};

struct test_sched_sender;
struct test_sched_env;

struct test_scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    auto schedule() const -> test_sched_sender;
    auto operator==(const test_scheduler&) const -> bool = default;
    auto query(test_std::get_completion_domain_t<test_std::set_value_t>) const noexcept { return sched_domain{}; }
};

struct test_sched_state {
    using operation_state_concept = test_std::operation_state_tag;
    auto start() noexcept {}
};

struct test_sched_env {
    auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept { return test_scheduler{}; }
};

struct test_sched_sender {
    using sender_concept = test_std::sender_tag;
    static consteval auto get_completion_signatures() {
        return test_std::completion_signatures<test_std::set_value_t()>();
    }
    template <typename R>
    auto connect(R&&) const {
        return test_sched_state{};
    }
    auto get_env() const noexcept { return test_sched_env{}; }
};

inline auto test_scheduler::schedule() const -> test_sched_sender { return {}; }

static_assert(test_std::scheduler<test_scheduler>);

// Env that has get_scheduler but no get_domain
struct env_with_scheduler {
    auto query(test_std::get_scheduler_t) const noexcept { return test_scheduler{}; }
};
} // namespace

TEST(exec_get_domain) {
    static_assert(std::same_as<const test_std::get_domain_t, decltype(test_std::get_domain)>);

    static_assert(test_std::forwarding_query(test_std::get_domain));

    test_get_domain<test_std::default_domain>(no_get_domain{});               // falling back to `default_domain`
    test_get_domain<test_std::default_domain>(non_const_get_domain<true>{});  // falling back to `default_domain`
    test_get_domain<test_std::default_domain>(non_const_get_domain<false>{}); // falling back to `default_domain`
    test_get_domain<domain>(has_get_domain<true, domain>{42});
    test_get_domain<domain>(has_get_domain<false, domain>{42});
    test_get_domain<domain>(overloaded_get_domain{});

    static_assert(42 == test_std::get_domain(has_get_domain<true, domain>{42}).value);

    test_get_domain<sched_domain>(env_with_scheduler{});
}
