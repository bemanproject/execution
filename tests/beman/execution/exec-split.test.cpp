// src/beman/execution/tests/split.test.cpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <chrono>
#include <future>
#include <list>
#include <thread>
#include <variant>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/meta_combine.hpp>
#include <beman/execution/detail/split.hpp>
#include <beman/execution/detail/type_list.hpp>
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {

struct timed_scheduler_t : beman::execution::scheduler_t {};

class some_thread_pool {
  public:
    some_thread_pool()                                       = default;
    some_thread_pool(const some_thread_pool&)                = delete;
    some_thread_pool& operator=(const some_thread_pool&)     = delete;
    some_thread_pool(some_thread_pool&&) noexcept            = delete;
    some_thread_pool& operator=(some_thread_pool&&) noexcept = delete;

    ~some_thread_pool() {
        std::lock_guard lock(mutex_);
        for (auto& future : futures_) {
            future.get();
        }
    }

    template <class Fn>
    auto schedule(Fn&& fn) -> void {
        std::lock_guard lock(mutex_);
        futures_.emplace_back(std::async(std::launch::async, std::forward<Fn>(fn)));
    }

  private:
    std::mutex                   mutex_;
    std::list<std::future<void>> futures_;
};

struct some_thread_pool_scheduler {
    using scheduler_concept = timed_scheduler_t;

    template <class Receiver>
    struct timed_operation {
        using operation_state_concept = beman::execution::operation_state_t;

        Receiver                              receiver_;
        some_thread_pool&                     pool_;
        std::chrono::steady_clock::time_point deadline_;

        void start() noexcept try {
            pool_.schedule([this] {
                std::this_thread::sleep_until(deadline_);
                beman::execution::set_value(std::move(receiver_));
            });
        } catch (...) {
            beman::execution::set_error(std::move(receiver_), std::current_exception());
        }
    };

    struct timed_sender {
        using sender_concept = beman::execution::sender_t;

        using completion_signatures =
            beman::execution::completion_signatures<beman::execution::set_value_t(),
                                                    beman::execution::set_error_t(std::exception_ptr)>;

        some_thread_pool*                     pool_;
        std::chrono::steady_clock::time_point deadline_;

        struct env {
            some_thread_pool* pool_;

            auto query(beman::execution::get_completion_scheduler_t<beman::execution::set_value_t>) const noexcept
                -> some_thread_pool_scheduler {
                return some_thread_pool_scheduler{*pool_};
            }
        };

        auto get_env() const noexcept { return env{pool_}; }

        template <beman::execution::receiver_of<completion_signatures> Receiver>
        auto connect(Receiver receiver) const noexcept -> timed_operation<Receiver> {
            return timed_operation<Receiver>{std::move(receiver), *pool_, deadline_};
        }
    };

    explicit some_thread_pool_scheduler(some_thread_pool& pool) : pool_(&pool) {}

    auto now() const noexcept -> std::chrono::steady_clock::time_point { return std::chrono::steady_clock::now(); }

    auto schedule_at(std::chrono::steady_clock::time_point deadline) const noexcept -> timed_sender {
        return {pool_, deadline};
    }

    auto schedule_after(std::chrono::nanoseconds duration) const noexcept {
        return beman::execution::let_value(beman::execution::just(),
                                           [this, duration] { return schedule_at(now() + duration); });
    }

    auto schedule() const noexcept -> timed_sender { return schedule_at(now()); }

    friend bool operator==(const some_thread_pool_scheduler&, const some_thread_pool_scheduler&) noexcept = default;

    some_thread_pool* pool_;
};

struct NonCopyable {
    NonCopyable()                                  = default;
    NonCopyable(NonCopyable&&) noexcept            = default;
    NonCopyable& operator=(NonCopyable&&) noexcept = default;
    ~NonCopyable()                                 = default;
    NonCopyable(const NonCopyable&)                = delete;
    NonCopyable& operator=(const NonCopyable&)     = delete;
};

void test_destroy_unused_split() {
    auto just               = beman::execution::just(NonCopyable{});
    auto split              = beman::execution::split(std::move(just));
    using split_sender_type = decltype(split);
    static_assert(beman::execution::sender_in<split_sender_type, test_std::env<>>);
}

void test_destroy_two_unused_split() {
    auto just               = beman::execution::just(NonCopyable{});
    auto split              = beman::execution::split(std::move(just));
    auto copy               = split;
    using split_sender_type = decltype(copy);
    static_assert(beman::execution::sender_in<split_sender_type, test_std::env<>>);
}

using test_detail::type_list;
using test_detail::meta::combine;

template <class... Args>
using to_set_value_t = type_list<beman::execution::set_value_t(Args...)>;

void test_completion_sigs_and_sync_wait_on_split() {
    auto just                        = beman::execution::just(NonCopyable{});
    auto split                       = beman::execution::split(std::move(just));
    using split_sender               = std::decay_t<decltype(split)>;
    using expected_value_completions = type_list<beman::execution::set_value_t(const NonCopyable&)>;
    using value_completions =
        beman::execution::value_types_of_t<split_sender, test_std::env<>, to_set_value_t, combine>;
    static_assert(std::same_as<value_completions, expected_value_completions>);

    auto eat_completion = beman::execution::then(split, [&](const NonCopyable&) {});
    ASSERT(beman::execution::sync_wait(eat_completion));
}

void test_two_sync_waits_on_one_split() {
    auto        just    = beman::execution::just(NonCopyable{});
    auto        split   = beman::execution::split(std::move(just));
    const void* pointer = nullptr;
    auto        save_pointer =
        beman::execution::then(split, [&](const NonCopyable& obj) { pointer = static_cast<const void*>(&obj); });
    auto assert_pointer = beman::execution::then(
        split, [&](const NonCopyable& obj) { ASSERT(pointer == static_cast<const void*>(&obj)); });
    ASSERT(beman::execution::sync_wait(save_pointer));
    ASSERT(beman::execution::sync_wait(assert_pointer));
}

void test_completion_from_another_thread() {
    using namespace std::chrono_literals;
    auto context   = some_thread_pool{};
    auto scheduler = some_thread_pool_scheduler{context};
    auto split     = beman::execution::split(scheduler.schedule_after(1ms));
    auto return_42 = beman::execution::then(split, [] { return 42; });
    auto result    = beman::execution::sync_wait(return_42);
    ASSERT(scheduler == scheduler); // avoid a warning about the required op== being unused
    ASSERT(result.has_value());
    if (result.has_value()) {
        auto [val] = *result;
        ASSERT(val == 42);
    }
}

void test_multiple_completions_from_other_threads() {
    using namespace std::chrono_literals;
    auto context = some_thread_pool{};
    auto result  = [&] {
        auto scheduler   = some_thread_pool_scheduler{context};
        auto deadline    = scheduler.now() + 20ms;
        auto split       = beman::execution::split(scheduler.schedule_at(deadline));
        auto resched     = beman::execution::continues_on(split, scheduler);
        auto return_42   = beman::execution::then(resched, [] { return 42; });
        auto three_times = beman::execution::when_all(return_42, return_42, return_42);
        return beman::execution::sync_wait(three_times);
    }();
    ASSERT(result.has_value());
    if (result.has_value()) {
        auto [val0, val1, val2] = *result;
        ASSERT(val0 == 42);
        ASSERT(val1 == 42);
        ASSERT(val2 == 42);
    }
}
} // namespace

TEST(exec_split) {
    test_destroy_unused_split();
    test_destroy_two_unused_split();
    test_completion_sigs_and_sync_wait_on_split();
    test_two_sync_waits_on_one_split();
    test_completion_from_another_thread();
    test_multiple_completions_from_other_threads();
}
