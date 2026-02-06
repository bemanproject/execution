// src/beman/execution/tests/exec-starts-on.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/starts_on.hpp>
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct non_scheduler {};
struct non_sender {};

struct scheduler {
    struct env {
        auto query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept -> scheduler {
            return {};
        }
    };
    struct sender {
        template <typename Receiver>
        struct state {
            using operation_state_concept = test_std::operation_state_t;
            std::remove_cvref_t<Receiver> receiver;
            auto start() & noexcept -> void { test_std::set_value(::std::move(this->receiver)); }
        };
        using sender_concept        = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
        auto get_env() const noexcept -> env { return {}; }
        template <test_std::receiver Receiver>
        auto connect(Receiver&& receiver) -> state<Receiver> {
            return {std::forward<Receiver>(receiver)};
        }
    };
    using scheduler_concept = test_std::scheduler_t;
    auto schedule() -> sender { return {}; }
    auto operator==(const scheduler&) const -> bool = default;
};
struct sender {
    using sender_concept        = test_std::sender_t;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;

    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        std::remove_cvref_t<Receiver> receiver;
        auto                          start() & noexcept -> void { test_std::set_value(::std::move(this->receiver)); }
    };
    template <test_std::receiver Receiver>
    auto connect(Receiver&& receiver) -> state<Receiver> {
        return {std::forward<Receiver>(receiver)};
    }
};

template <bool Expect, typename Scheduler, typename Sender>
auto test_constraints(Scheduler&& scheduler, Sender&& sender) {
    static_assert(Expect == requires { test_std::starts_on(scheduler, sender); });
    static_assert(Expect == requires {
        test_std::starts_on(::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender));
    });
}

template <typename Scheduler, typename Sender>
auto test_use(Scheduler&& scheduler, Sender&& sender) -> void {
    auto s{test_std::starts_on(::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender))};
    test::use(s);
    // test::check_type<void>(s);
    // test::check_type<void>(test_std::get_completion_signatures(s, test_std::test_std::env<>{}));
    test_std::sync_wait(std::move(s));
}
} // namespace

TEST(exec_starts_on) {
    static_assert(std::same_as<const test_std::starts_on_t, decltype(test_std::starts_on)>);
    static_assert(not test_std::scheduler<non_scheduler>);
    static_assert(test_std::scheduler<scheduler>);
    static_assert(not test_std::sender<non_sender>);
    static_assert(test_std::sender<sender>);

    test_constraints<false>(non_scheduler{}, non_sender{});
    test_constraints<false>(non_scheduler{}, sender{});
    test_constraints<false>(scheduler{}, non_sender{});
    test_constraints<true>(scheduler{}, sender{});

    test_use(scheduler{}, sender{});
}
