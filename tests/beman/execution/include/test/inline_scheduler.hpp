// tests/beman/execution/include/test/inline_scheduler.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_INLINE_SCHEDULER
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_INLINE_SCHEDULER

#if 0
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/operation_state.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <test/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace test {
struct inline_scheduler {
    struct env {
        inline_scheduler query(const test_std::get_completion_scheduler_t<test_std::set_value_t>&) const noexcept {
            return {};
        }
    };
    template <test_std::receiver Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_t;
        std::remove_cvref_t<Receiver> receiver;
        void                          start() & noexcept { ::beman::execution::set_value(std::move(receiver)); }
    };
    struct sender {
        using sender_concept        = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<::beman::execution::set_value_t()>;

        env get_env() const noexcept { return {}; }
        template <test_std::receiver Receiver>
        state<Receiver> connect(Receiver&& receiver) {
            return {std::forward<Receiver>(receiver)};
        }
    };
    static_assert(test_std::sender<sender>);

    using scheduler_concept = test_std::scheduler_t;
    constexpr sender schedule() noexcept { return {}; }
    bool             operator==(const inline_scheduler&) const = default;
};
} // namespace test

// ----------------------------------------------------------------------------

#endif
