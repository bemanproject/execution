// tests/beman/execution/include/test/thread_pool.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_THREAD_POOL
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_THREAD_POOL

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace test {
struct thread_pool;
}

struct test::thread_pool {
    struct node {
        node*        next{};
        virtual void run() = 0;

        node(const node&)            = delete;
        node(node&&)                 = delete;
        node& operator=(const node&) = delete;
        node& operator=(node&&)      = delete;

      protected:
        node()  = default;
        ~node() = default;
    };

    std::mutex              mutex;
    std::condition_variable condition;
    node*                   stack{};
    bool                    stopped{false};
    std::thread             driver{[this] {
        while (std::optional<node*> n = [this] {
            std::unique_lock cerberus(mutex);
            condition.wait(cerberus, [this] { return stopped || stack; });
            return this->stack ? std::optional<node*>(std::exchange(this->stack, this->stack->next))
                                           : std::optional<node*>();
        }()) {
            (*n)->run();
        }
    }};

    thread_pool()                   = default;
    thread_pool(thread_pool&&)      = delete;
    thread_pool(const thread_pool&) = delete;
    ~thread_pool() {
        this->stop();
        this->driver.join();
    }
    thread_pool& operator=(thread_pool&&)      = delete;
    thread_pool& operator=(const thread_pool&) = delete;
    void         stop() {
        {
            std::lock_guard cerberus(this->mutex);
            stopped = true;
        }
        this->condition.notify_one();
    }

    struct scheduler {
        using scheduler_concept = test_std::scheduler_t;
        struct env {
            test::thread_pool* pool;

            template <typename T>
            scheduler query(const test_std::get_completion_scheduler_t<T>&) const noexcept {
                return {this->pool};
            }
        };
        template <typename Receiver>
        struct state final : test::thread_pool::node {
            using operation_state_concept = test_std::operation_state_t;
            std::remove_cvref_t<Receiver> receiver;
            test::thread_pool*            pool;

            template <typename R>
            state(R&& r, test::thread_pool* p) : node{}, receiver(std::forward<R>(r)), pool(p) {}
            void start() & noexcept {
                {
                    std::lock_guard cerberus(this->pool->mutex);
                    this->next = std::exchange(this->pool->stack, this);
                }
                this->pool->condition.notify_one();
            }
            void run() override { test_std::set_value(std::move(this->receiver)); }
        };
        struct sender {
            using sender_concept        = test_std::sender_t;
            using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
            test::thread_pool* pool;
            template <typename Receiver>
            state<Receiver> connect(Receiver&& receiver) {
                return state<Receiver>(std::forward<Receiver>(receiver), pool);
            }

            env get_env() const noexcept { return {this->pool}; }
        };
        test::thread_pool* pool;
        sender             schedule() { return {this->pool}; }
        bool               operator==(const scheduler&) const = default;
    };
    scheduler get_scheduler() { return {this}; }
};

static_assert(test_std::scheduler<test::thread_pool::scheduler>);

// ----------------------------------------------------------------------------

#endif
