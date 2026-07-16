// src/beman/execution/tests/exec-parallel-scheduler.test.cpp       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <numeric>
#include <atomic>
#include <concepts>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <span>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.schedule_result_t;
#else
#include <beman/execution.hpp>
#endif

namespace {
namespace replaceability = test_std::parallel_scheduler_replacement;

struct proxy : replaceability::receiver_proxy {
    auto set_value() noexcept -> void override {}
    auto set_error(::std::exception_ptr) noexcept -> void override {}
    auto set_stopped() noexcept -> void override {}
};

struct bulk_proxy : replaceability::bulk_item_receiver_proxy {
    auto set_value() noexcept -> void override {}
    auto set_error(::std::exception_ptr) noexcept -> void override {}
    auto set_stopped() noexcept -> void override {}
    auto execute(::std::size_t, ::std::size_t) noexcept -> void override {}
};

struct backend : replaceability::parallel_scheduler_backend {
    auto schedule(replaceability::receiver_proxy&, ::std::span<::std::byte>) noexcept -> void override {}
    auto schedule_bulk_chunked(::std::size_t,
                               replaceability::bulk_item_receiver_proxy&,
                               ::std::span<::std::byte>) noexcept -> void override {}
    auto schedule_bulk_unchunked(::std::size_t,
                                 replaceability::bulk_item_receiver_proxy&,
                                 ::std::span<::std::byte>) noexcept -> void override {}
};

struct thread_pool_base : replaceability::parallel_scheduler_backend {
    struct task {
        task() = default;

        task(const task&) = delete;

        task(task&&) = delete;

        virtual ~task() = default;

        auto operator=(const task&) -> task& = delete;

        auto operator=(task&&) -> task& = delete;

        virtual auto exec() noexcept -> void = 0;
    };

    struct schedule_task : task {
        explicit schedule_task(replaceability::receiver_proxy& p) noexcept : proxy(p) {}

        auto exec() noexcept -> void override { proxy.set_value(); }

        replaceability::receiver_proxy& proxy;
    };

    struct bulk_shared_state {
        std::atomic<std::size_t> counter;
        std::exception_ptr       exception;
    };

    struct bulk_task : task {
        bulk_task(std::shared_ptr<bulk_shared_state>        counter,
                  replaceability::bulk_item_receiver_proxy& proxy,
                  std::size_t                               i,
                  std::size_t                               j) noexcept
            : shared_state(std::move(counter)), proxy(proxy), i(i), j(j) {}

        auto exec() noexcept -> void override {
            proxy.execute(i, j);
            if (shared_state->counter.fetch_sub(1uz, std::memory_order_acq_rel) == 1uz) {
                if (shared_state->exception) {
                    proxy.set_error(shared_state->exception);
                } else {
                    proxy.set_value();
                }
            }
        }

        std::shared_ptr<bulk_shared_state>        shared_state;
        replaceability::bulk_item_receiver_proxy& proxy;
        std::size_t                               i;
        std::size_t                               j;
    };

    thread_pool_base() = default;

    thread_pool_base(const thread_pool_base&) = delete;

    ~thread_pool_base() override = 0;

    auto operator=(const thread_pool_base&) = delete;

    auto shutdown() -> void {
        std::unique_lock guard{mtx};
        shutdown_requested = true;
        guard.unlock();
        cv.notify_all();
    }

    auto schedule(replaceability::receiver_proxy& proxy, ::std::span<::std::byte>) noexcept -> void override {
        try {
            auto             t = std::make_unique<schedule_task>(proxy);
            std::unique_lock guard{mtx};
            tasks.push(std::move(t));
            guard.unlock();
            cv.notify_one();
        } catch (...) {
            proxy.set_error(std::current_exception());
        }
    }

    auto schedule_bulk(std::size_t                               shape,
                       std::size_t                               chunk_length,
                       replaceability::bulk_item_receiver_proxy& proxy,
                       std::span<::std::byte>) noexcept -> void {
        const std::size_t                  chunk_count = (shape + chunk_length - 1uz) / chunk_length;
        std::shared_ptr<bulk_shared_state> shared_state;
        try {
            shared_state = std::make_shared<bulk_shared_state>(chunk_count);
        } catch (...) {
            proxy.set_error(std::current_exception());
            return;
        }
        std::unique_lock guard{mtx};
        for (std::size_t i = 0; i < chunk_count; ++i) {
            try {
                const std::size_t begin = i * chunk_length;
                const std::size_t end   = std::min(begin + chunk_length, shape);
                tasks.push(std::make_unique<bulk_task>(shared_state, proxy, begin, end));
            } catch (...) {
                const std::size_t n = chunk_count - i; // the count of `bulk_task` which are not enqueued successfully

                guard.unlock();
                if (i == 1uz) {
                    cv.notify_one();
                } else if (i > 1uz) {
                    cv.notify_all();
                }

                // happens-before `proxy.set_value()/proxy.set_error(...)` in `bulk_task::exec`
                shared_state->exception = std::current_exception();

                if (shared_state->counter.fetch_sub(n, std::memory_order_acq_rel) == n) {
                    proxy.set_error(shared_state->exception);
                }
                return;
            }
        }
        guard.unlock();
        cv.notify_all();
    }

    auto schedule_bulk_chunked(::std::size_t                             shape,
                               replaceability::bulk_item_receiver_proxy& proxy,
                               ::std::span<::std::byte>                  storage) noexcept -> void override {
        const std::size_t chunk_length = (shape + num_threads - 1uz) / num_threads;
        schedule_bulk(shape, chunk_length, proxy, storage);
    }

    auto schedule_bulk_unchunked(::std::size_t                             shape,
                                 replaceability::bulk_item_receiver_proxy& proxy,
                                 ::std::span<::std::byte>                  storage) noexcept -> void override {
        schedule_bulk(shape, 1uz, proxy, storage);
    }

  protected:
    static constexpr std::size_t      num_threads        = 4uz;
    bool                              shutdown_requested = false;
    std::mutex                        mtx;
    std::condition_variable           cv;
    std::queue<std::unique_ptr<task>> tasks;
};

thread_pool_base::~thread_pool_base() = default;

struct thread_pool_backend : thread_pool_base {
    thread_pool_backend() {
        for (std::size_t i = 0; i < num_threads; ++i) {
            workers[i] = std::thread([this]() noexcept { this->run(); });
        }
    }

    ~thread_pool_backend() override {
        shutdown();
        for (auto& worker : workers) {
            worker.join();
        }
    }

  private:
    auto run() noexcept -> void {
        while (true) {
            std::unique_lock guard{mtx};
            cv.wait(guard, [this]() noexcept { return !tasks.empty() || shutdown_requested; });
            if (shutdown_requested && tasks.empty()) {
                return;
            }
            auto task = std::move(tasks.front());
            tasks.pop();
            guard.unlock();
            task->exec();
        }
    }

    std::thread workers[num_threads];
};

// for GCC and Clang, enable -fopenmp for both compiling and linking; for MSVC, use the /openmp:llvm compiler option.
#ifdef _OPENMP
struct openmp_backend : thread_pool_base {
    openmp_backend() {
        designee = std::thread{[this]() noexcept {
#pragma omp parallel num_threads(num_threads)
            {
#pragma omp single
                {
                    while (true) {
                        std::unique_lock guard{mtx};
                        cv.wait(guard, [this]() noexcept { return !tasks.empty() || shutdown_requested; });
                        if (shutdown_requested && tasks.empty()) {
                            break;
                        }
                        auto front = std::move(tasks.front());
                        tasks.pop();
                        guard.unlock();
                        auto front_ptr = front.release();
#pragma omp task firstprivate(front_ptr)
                        {
                            std::unique_ptr<task>{front_ptr}->exec();
                        }
                    }
                }
            }
        }};
    }

    ~openmp_backend() override {
        shutdown();
        designee.join();
    }

  private:
    std::thread designee;
};
#endif

auto test_parallel_scheduler_synopsis() -> void {
    static_assert(!::std::default_initializable<test_std::parallel_scheduler>);
    static_assert(::std::copy_constructible<test_std::parallel_scheduler>);
    static_assert(::std::move_constructible<test_std::parallel_scheduler>);
    static_assert(test_std::scheduler<test_std::parallel_scheduler>);

    static_assert(::std::same_as<decltype(test_std::get_parallel_scheduler()), test_std::parallel_scheduler>);
    static_assert(::std::same_as<test_std::schedule_result_t<test_std::parallel_scheduler>,
                                 test_std::parallel_scheduler::sender>);
    static_assert(test_std::sender<test_std::parallel_scheduler::sender>);
    static_assert(::std::same_as<decltype(test_std::get_completion_signatures<test_std::parallel_scheduler::sender>()),
                                 test_std::completion_signatures<test_std::set_value_t(),
                                                                 test_std::set_error_t(::std::exception_ptr),
                                                                 test_std::set_stopped_t()>>);

    static_assert(
        noexcept(test_std::get_forward_progress_guarantee(::std::declval<const test_std::parallel_scheduler&>())));
    static_assert(::std::same_as<decltype(test_std::get_forward_progress_guarantee(
                                     ::std::declval<const test_std::parallel_scheduler&>())),
                                 test_std::forward_progress_guarantee>);
}

auto test_replaceability_synopsis() -> void {
    static_assert(::std::is_abstract_v<replaceability::receiver_proxy>);
    static_assert(::std::is_abstract_v<replaceability::bulk_item_receiver_proxy>);
    static_assert(::std::is_abstract_v<replaceability::parallel_scheduler_backend>);
    static_assert(::std::derived_from<bulk_proxy, replaceability::receiver_proxy>);
    static_assert(::std::derived_from<backend, replaceability::parallel_scheduler_backend>);
    static_assert(::std::same_as<decltype(::std::declval<proxy&>().template try_query<int>(0)), ::std::optional<int>>);
    static_assert(::std::same_as<decltype(replaceability::query_parallel_scheduler_backend()),
                                 ::std::shared_ptr<replaceability::parallel_scheduler_backend>>);
}

auto test_parallel_scheduler_schedule() -> void {
    auto sch = test_std::get_parallel_scheduler();
    {
        int i = 0;
        test_std::sync_wait(test_std::schedule(sch) | test_std::then([&i]() noexcept { i = 114514; }));
        ASSERT(i == 114514);
    }
    {
        for (auto size : {1uz, 4uz, 8uz, 16uz, 32uz}) {
            std::vector<int> vec(size);
            std::iota(vec.begin(), vec.end(), 0);

            test_std::sync_wait(
                test_std::schedule(sch) |
                test_std::bulk(test_std::par, vec.size(), [&vec](std::size_t i) noexcept { vec[i] = 2 * vec[i]; }));
            for (std::size_t i = 0; i < vec.size(); ++i) {
                ASSERT(vec[i] == 2 * static_cast<int>(i));
            }

            test_std::sync_wait(
                test_std::schedule(sch) |
                test_std::bulk(test_std::seq, vec.size(), [&vec](std::size_t i) noexcept { ++vec[i]; }));
            for (std::size_t i = 0; i < vec.size(); ++i) {
                ASSERT(vec[i] == 2 * static_cast<int>(i) + 1);
            }
        }
    }
}
} // namespace

namespace beman::execution::parallel_scheduler_replacement {
auto query_parallel_scheduler_backend() -> std::shared_ptr<parallel_scheduler_backend> {
#ifdef _OPENMP
    static auto backend = std::make_shared<::openmp_backend>();
#else
    static auto backend = std::make_shared<::thread_pool_backend>();
#endif
    return backend;
}
} // namespace beman::execution::parallel_scheduler_replacement

TEST(exec_parallel_scheduler) {
    test_parallel_scheduler_synopsis();
    test_replaceability_synopsis();
    test_parallel_scheduler_schedule();
}
