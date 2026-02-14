// tests/beman/execution/exec-task-scheduler.test.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/task_scheduler.hpp>
#include <beman/execution/execution.hpp>
#include <test/execution.hpp>

// ----------------------------------------------------------------------------

namespace {
    struct test_sender;

    struct test_scheduler {
        using scheduler_concept = beman::execution::scheduler_t;

        int id{};

        explicit test_scheduler(int i) : id(i) {}

        auto operator==(const test_scheduler&) const noexcept -> bool = default;

        auto schedule() -> test_sender;
    };

    struct test_sender {
        using sender_concept = beman::execution::sender_t;

        test_scheduler sched_;

        template <typename Env>
        auto get_completion_signatures(Env&&) const noexcept {
            using stop_token_type = beman::execution::stop_token_of_t<Env>;
            if constexpr (beman::execution::unstoppable_token<stop_token_type>) {
                return beman::execution::completion_signatures<
                    beman::execution::set_value_t()>{};
            } else {
                return beman::execution::completion_signatures<
                    beman::execution::set_value_t(),
                    beman::execution::set_stopped_t()>{};
            }
        }

        template <typename Receiver>
        struct operation {
            Receiver rcvr_;
            using operation_state_concept = beman::execution::operation_state_t;
            void start() & noexcept {
                beman::execution::set_value(std::move(rcvr_));
            }
        };

        template <beman::execution::receiver Receiver>
        auto connect(Receiver&& rcvr) && {
            return operation<std::decay_t<Receiver>>{std::forward<Receiver>(rcvr)};
        }

        struct env {
            test_scheduler sched_;
            auto query(beman::execution::get_completion_scheduler_t<beman::execution::set_value_t>) const noexcept
                -> test_scheduler {
                return sched_;
            }
        };

        auto get_env() const noexcept -> env;
    };

    inline auto test_scheduler::schedule() -> test_sender { return test_sender{*this}; }
    inline auto test_sender::get_env() const noexcept -> env {
        return env{sched_};
    }

}

TEST(exec_task_scheduler){
    using namespace beman::execution;

    static_assert(scheduler<test_scheduler>);
    test_scheduler sched(1);
    auto sender = test_std::schedule(sched);
    ASSERT(sched == test_std::get_completion_scheduler<test_std::set_value_t>(test_std::get_env(sender)));
    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t()>,
                               decltype(test_std::get_completion_signatures(sender, test_std::env<>{}))>);
    test_std::inplace_stop_source stop_source;
    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(), test_std::set_stopped_t()>,
                               decltype(test_std::get_completion_signatures(sender, test_std::env{test_std::prop(test_std::get_stop_token, stop_source.get_token())}))>);

    static_assert(test_std::scheduler<test_detail::task_scheduler>);
#if 0
    test_std::task_scheduler sched1;
    test_std::task_scheduler sched2(sched1);
    test_std::task_scheduler sched3(std::move(sched1));
    test_std::task_scheduler sched4;
    sched4 = sched2;
    test_std::task_scheduler sched5;
    sched5 = std::move(sched3);

    // Verify that wrapping the same test_scheduler preserves equality.
    task_scheduler a(test_scheduler(1));
    task_scheduler b(test_scheduler(1));
    task_scheduler c(test_scheduler(2));
    ASSERT(a == b);
    ASSERT(!(a == c));

    // Copy preserves identity.
    task_scheduler d(a);
    ASSERT(d == a);

    // Assignment preserves identity.
    task_scheduler e;
    e = c;
    ASSERT(e == c);
    ASSERT(!(e == a));
#endif

    return EXIT_SUCCESS;
}
