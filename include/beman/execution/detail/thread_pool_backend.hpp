// include/beman/execution/detail/thread_pool_backend.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_THREAD_POOL_BACKEND
#define INCLUDED_BEMAN_EXECUTION_DETAIL_THREAD_POOL_BACKEND

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.parallel_scheduler_replacement;
import beman.execution.detail.unreachable;
#else
#include <beman/execution/detail/parallel_scheduler_replacement.hpp>
#include <beman/execution/detail/unreachable.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
class thread_pool_backend_base
    : public ::beman::execution::parallel_scheduler_replacement::parallel_scheduler_backend {
  protected:
    struct task {
        task() noexcept : next(nullptr) {}

        task(const task&) = delete;

        task(task&&) = delete;

        virtual ~task() = default;

        auto operator=(const task&) -> task& = delete;

        auto operator=(task&&) -> task& = delete;

        virtual auto exec(::std::pmr::polymorphic_allocator<>) noexcept -> void = 0;

        task* next;
    };

    struct schedule_task : task {
        explicit schedule_task(::beman::execution::parallel_scheduler_replacement::receiver_proxy& p) noexcept
            : proxy(p) {}

        auto exec(::std::pmr::polymorphic_allocator<> alloc) noexcept -> void override {
            auto& proxy_ref = proxy;
            alloc.delete_object(this);
            proxy_ref.set_value();
        }

        ::beman::execution::parallel_scheduler_replacement::receiver_proxy& proxy;
    };

    struct bulk_task : task {
        struct shared_state_type {
            ::std::span<bulk_task>       tasks;
            ::std::atomic<::std::size_t> counter;
        };

        bulk_task(::std::shared_ptr<shared_state_type>                                          counter,
                  ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                  ::std::size_t                                                                 i,
                  ::std::size_t                                                                 j) noexcept
            : shared_state(::std::move(counter)), proxy(proxy), i(i), j(j) {}

        auto exec(::std::pmr::polymorphic_allocator<> alloc) noexcept -> void override {
            proxy.execute(i, j);
            if (shared_state->counter.fetch_sub(1uz, ::std::memory_order_acq_rel) == 1uz) {
                auto& proxy_ref = proxy;
                ::std::ranges::destroy(shared_state->tasks);
                alloc.deallocate_object(shared_state->tasks.data(), shared_state->tasks.size());
                proxy_ref.set_value();
            }
        }

        ::std::shared_ptr<shared_state_type>                                          shared_state;
        ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy;
        ::std::size_t                                                                 i;
        ::std::size_t                                                                 j;
    };

  public:
    thread_pool_backend_base() = default;

    thread_pool_backend_base(const thread_pool_backend_base&) = delete;

    thread_pool_backend_base(thread_pool_backend_base&&) = delete;

    ~thread_pool_backend_base() override = default;

    auto operator=(const thread_pool_backend_base&) -> thread_pool_backend_base& = delete;

    auto operator=(thread_pool_backend_base&&) -> thread_pool_backend_base& = delete;

    auto shutdown() -> void {
        ::std::unique_lock guard{mtx};
        shutdown_requested = true;
        guard.unlock();
        cv.notify_all();
    }

    auto schedule(::beman::execution::parallel_scheduler_replacement::receiver_proxy& proxy,
                  ::std::span<::std::byte>) noexcept -> void override {
        try {
            ::std::unique_lock                  guard{mtx};
            ::std::pmr::polymorphic_allocator<> alloc{&mempool};
            auto*                               t = alloc.new_object<schedule_task>(proxy);
            if (tasks_end == nullptr) {
                tasks_begin = t;
            } else {
                tasks_end->next = t;
            }
            tasks_end = t;
            guard.unlock();
            cv.notify_one();
        } catch (...) {
            proxy.set_error(::std::current_exception());
        }
    }

    auto schedule_bulk(::std::size_t                                                                 shape,
                       ::std::size_t                                                                 chunk_length,
                       ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                       ::std::span<::std::byte> storage) noexcept -> void {
        if (shape == 0uz) {
            schedule(proxy, storage);
            return;
        }

        const ::std::size_t                             chunk_count = (shape + chunk_length - 1uz) / chunk_length;
        ::std::pmr::polymorphic_allocator<>             alloc{&mempool};
        ::std::shared_ptr<bulk_task::shared_state_type> shared_state;
        bulk_task*                                      batch = nullptr;
        try {
            batch        = alloc.allocate_object<bulk_task>(chunk_count);
            shared_state = ::std::allocate_shared<bulk_task::shared_state_type>(
                alloc, ::std::span(batch, chunk_count), chunk_count);
        } catch (...) {
            if (batch) {
                alloc.deallocate_object(batch, chunk_count);
            }
            proxy.set_error(::std::current_exception());
            return;
        }

        bulk_task* prev = nullptr;
        for (::std::size_t i = 0; i < chunk_count; ++i) {
            const ::std::size_t begin = i * chunk_length;
            const ::std::size_t end   = ::std::min(begin + chunk_length, shape);
            // NOLINTBEGIN(*-pointer-arithmetic-on-polymorphic-object, *-ctr56-cpp)
            ::std::construct_at(batch + i, shared_state, proxy, begin, end);
            if (prev) {
                prev->next = &batch[i];
            }
            prev = &batch[i];
            // NOLINTEND(*-pointer-arithmetic-on-polymorphic-object, *-ctr56-cpp)
        }

        ::std::unique_lock guard{mtx};
        if (tasks_end == nullptr) {
            tasks_begin = batch;
        } else {
            tasks_end->next = batch;
        }
        tasks_end = prev;
        guard.unlock();
        cv.notify_all();
    }

    auto schedule_bulk_chunked(::std::size_t                                                                 shape,
                               ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                               ::std::span<::std::byte> storage) noexcept -> void override {
        const ::std::size_t chunk_length = (shape + num_threads - 1uz) / num_threads;
        schedule_bulk(shape, chunk_length, proxy, storage);
    }

    auto schedule_bulk_unchunked(::std::size_t                                                                 shape,
                                 ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                                 ::std::span<::std::byte> storage) noexcept -> void override {
        schedule_bulk(shape, 1uz, proxy, storage);
    }

  protected:
    inline static ::std::size_t                          num_threads = ::std::thread::hardware_concurrency();
    inline static ::std::pmr::synchronized_pool_resource mempool;
    bool                                                 shutdown_requested = false;
    ::std::mutex                                         mtx;
    ::std::condition_variable                            cv;
    task*                                                tasks_begin = nullptr;
    task*                                                tasks_end   = nullptr;
};

struct thread_pool_backend : ::beman::execution::detail::thread_pool_backend_base {
    explicit thread_pool_backend(::std::in_place_t) : workers(num_threads, &mempool) {}

    thread_pool_backend() : thread_pool_backend(::std::in_place) {
        for (::std::size_t i = 0; i < num_threads; ++i) {
            workers[i] = ::std::thread([this]() noexcept { this->run(); });
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
            ::std::unique_lock guard{mtx};
            cv.wait(guard, [this]() noexcept { return tasks_begin != nullptr || shutdown_requested; });
            if (shutdown_requested && tasks_begin == nullptr) {
                return;
            }
            auto task   = tasks_begin;
            tasks_begin = task->next;
            if (tasks_begin == nullptr) {
                tasks_end = nullptr;
            }
            task->next = nullptr;
            guard.unlock();
            task->exec(&mempool);
        }
    }

    ::std::pmr::vector<::std::thread> workers;
};

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // #ifdef INCLUDED_BEMAN_EXECUTION_DETAIL_THREAD_POOL_BACKEND
