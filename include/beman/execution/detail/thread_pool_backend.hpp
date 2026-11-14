// include/beman/execution/detail/thread_pool_backend.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_THREAD_POOL_BACKEND
#define INCLUDED_BEMAN_EXECUTION_DETAIL_THREAD_POOL_BACKEND

#include <cassert>
#include <beman/execution/detail/common.hpp>
#if BEMAN_EXECUTION_WITH_GCD_SUPPORT && __has_include(<dispatch/dispatch.h>)
#include <dispatch/dispatch.h>
#endif
#if defined(_WIN32) && __has_include(<windows.h>)
#include <windows.h>
#include <threadpoolapiset.h>
#endif
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <exception>
#include <memory>
#include <mutex>
#include <new>
#include <span>
#include <system_error>
#include <thread>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.parallel_scheduler_replacement;
import beman.execution.detail.psched_bulk_sender;
#else
#include <beman/execution/detail/parallel_scheduler_replacement.hpp>
#include <beman/execution/detail/psched_bulk_sender.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
class thread_pool_backend_base
    : public ::beman::execution::parallel_scheduler_replacement::parallel_scheduler_backend {
  protected:
    struct task_base {
        task_base() noexcept : next(nullptr) {}

        task_base(const task_base&) = delete;

        task_base(task_base&&) = delete;

        virtual ~task_base() = default;

        auto operator=(const task_base&) -> task_base& = delete;

        auto operator=(task_base&&) -> task_base& = delete;

        virtual auto exec(int ec = 0) noexcept -> void = 0;

        task_base* next;
    };

    struct schedule_task : task_base {
        explicit schedule_task(::beman::execution::parallel_scheduler_replacement::receiver_proxy& p) noexcept
            : proxy(p) {}

        auto exec(int ec) noexcept -> void override {
            auto& proxy_ref = proxy;
            ::std::destroy_at(this);
            if (ec) [[unlikely]] {
                proxy_ref.set_error(::std::make_exception_ptr(::std::system_error(ec, ::std::system_category())));
            } else {
                proxy_ref.set_value();
            }
        }

        ::beman::execution::parallel_scheduler_replacement::receiver_proxy& proxy;
    };

    // `schedule_task` is small enough to fit directly into the pre-allocated storage provided by `schedule()`
    static_assert(sizeof(schedule_task) <= psched_storage_size && alignof(schedule_task) <= psched_storage_alignment);

    struct single_bulk_task : task_base {
        single_bulk_task(::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& p,
                         ::std::size_t                                                                 shape,
                         bool one_by_one) noexcept
            : proxy(p), shape(shape), one_by_one(one_by_one) {}

        auto exec(int ec) noexcept -> void override {
            if (ec == 0) [[likely]] {
                if (one_by_one) {
                    for (::std::size_t i = 0; i < shape; ++i) {
                        proxy.execute(i, i + 1uz);
                    }
                } else {
                    proxy.execute(0uz, shape);
                }
            }
            auto& proxy_ref = proxy;
            ::std::destroy_at(this);
            if (ec) [[unlikely]] {
                proxy_ref.set_error(::std::make_exception_ptr(::std::system_error(ec, ::std::system_category())));
            } else {
                proxy_ref.set_value();
            }
        }

        ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy;
        ::std::size_t                                                                 shape;
        bool                                                                          one_by_one;
    };

    // `single_bulk_task` is small enough to fit directly into the pre-allocated storage provided by
    // `schedule_bulk_chunked()`/`schedule_bulk_unchunked()`
    static_assert(sizeof(single_bulk_task) <= psched_storage_size &&
                  alignof(single_bulk_task) <= psched_storage_alignment);

    struct batched_bulk_task : task_base {
        struct cookie_type {
            cookie_type(batched_bulk_task* head, ::std::size_t chunk_count, bool one_by_one) noexcept
                : head(head), chunk_count(chunk_count), one_by_one(one_by_one), ref_count(chunk_count) {}
            batched_bulk_task* head;
            ::std::size_t      chunk_count;
            bool               one_by_one;
            int                ec = 0;
            ::std::size_t      ref_count;
        };

        batched_bulk_task(cookie_type*                                                                  cookie,
                          ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                          ::std::size_t                                                                 i,
                          ::std::size_t                                                                 j) noexcept
            : cookie(cookie), proxy(proxy), i(i), j(j) {}

        auto exec(int ec) noexcept -> void override {
            if (ec) [[unlikely]] {
                if (next == nullptr) {
                    cookie->ec = ec;
                }
            } else {
                if (cookie->one_by_one) {
                    for (::std::size_t k = i; k < j; ++k) {
                        proxy.execute(k, k + 1uz);
                    }
                } else {
                    proxy.execute(i, j);
                }
            }

            ::std::atomic_ref<::std::size_t> ref_count{cookie->ref_count};
            if (ref_count.fetch_sub(1uz, ::std::memory_order_acq_rel) == 1uz) {
                auto       head        = cookie->head;
                const auto chunk_count = cookie->chunk_count;
                auto       error       = ::std::error_code{cookie->ec, ::std::system_category()};
                auto&      proxy_ref   = proxy;
                ::std::destroy_at(cookie);
                ::std::destroy_n(head, chunk_count);
                ::operator delete(
                    head, chunk_count * sizeof(batched_bulk_task), ::std::align_val_t{alignof(batched_bulk_task)});
                if (error) [[unlikely]] {
                    proxy_ref.set_error(::std::make_exception_ptr(::std::system_error{error}));
                } else {
                    proxy_ref.set_value();
                }
            }
        }

        cookie_type*                                                                  cookie;
        ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy;
        ::std::size_t                                                                 i;
        ::std::size_t                                                                 j;
    };

    // The cookie of a batch lives in the pre-allocated storage too, so a batch costs exactly one allocation:
    // the chunk array itself.
    static_assert(sizeof(batched_bulk_task::cookie_type) <= psched_storage_size &&
                  alignof(batched_bulk_task::cookie_type) <= psched_storage_alignment);

  public:
    thread_pool_backend_base() = default;

    thread_pool_backend_base(const thread_pool_backend_base&) = delete;

    thread_pool_backend_base(thread_pool_backend_base&&) = delete;

    ~thread_pool_backend_base() override = default;

    auto operator=(const thread_pool_backend_base&) -> thread_pool_backend_base& = delete;

    auto operator=(thread_pool_backend_base&&) -> thread_pool_backend_base& = delete;

    auto schedule(::beman::execution::parallel_scheduler_replacement::receiver_proxy& proxy,
                  ::std::span<::std::byte> storage) noexcept -> void override {
        submit(::std::construct_at(reinterpret_cast<schedule_task*>(storage.data()), proxy)); // nothrow!
    }

    auto schedule_bulk_chunked(::std::size_t                                                                 shape,
                               ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                               ::std::span<::std::byte> storage) noexcept -> void override {
        schedule_bulk(shape, false, proxy, storage);
    }

    auto schedule_bulk_unchunked(::std::size_t                                                                 shape,
                                 ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                                 ::std::span<::std::byte> storage) noexcept -> void override {
        schedule_bulk(shape, true, proxy, storage);
    }

  protected:
    [[nodiscard]] static auto num_threads() noexcept -> ::std::size_t {
        static const ::std::size_t count = (::std::max)(1u, ::std::thread::hardware_concurrency());
        return count;
    }

  private:
    auto schedule_bulk(::std::size_t                                                                 shape,
                       bool                                                                          one_by_one,
                       ::beman::execution::parallel_scheduler_replacement::bulk_item_receiver_proxy& proxy,
                       ::std::span<::std::byte> storage) noexcept -> void {
        if (shape == 0uz) {
            schedule(proxy, storage);
            return;
        }

        const ::std::size_t threads      = num_threads();
        const ::std::size_t chunk_length = shape / threads + (shape % threads != 0uz);
        const ::std::size_t chunk_count  = shape / chunk_length + (shape % chunk_length != 0uz);
        try {
            if (chunk_count == 1uz) {
                submit(::std::construct_at(
                    reinterpret_cast<single_bulk_task*>(storage.data()), proxy, shape, one_by_one));
            } else {
                auto head = static_cast<batched_bulk_task*>(::operator new(
                    chunk_count * sizeof(batched_bulk_task), ::std::align_val_t{alignof(batched_bulk_task)}));
                // NOLINTBEGIN(*-reinterpret-cast, *-pointer-arithmetic-on-polymorphic-object, *-ctr56-cpp)
                auto cookie = ::std::construct_at(
                    reinterpret_cast<batched_bulk_task::cookie_type*>(storage.data()), head, chunk_count, one_by_one);

                batched_bulk_task* prev = nullptr;
                for (::std::size_t i = 0; i < chunk_count; ++i) {
                    const ::std::size_t begin = i * chunk_length;
                    const ::std::size_t end   = begin + (::std::min)(chunk_length, shape - begin);
                    auto                task  = ::std::construct_at(head + i, cookie, proxy, begin, end);
                    if (prev) {
                        prev->next = task;
                    }
                    prev = task;
                }
                // NOLINTEND(*-reinterpret-cast, *-pointer-arithmetic-on-polymorphic-object, *-ctr56-cpp)
                submit(head, chunk_count);
            }
        } catch (...) {
            proxy.set_error(::std::current_exception());
        }
    }

    virtual auto submit(task_base* t, ::std::size_t n = 1uz) noexcept -> void = 0;
};

struct simple_thread_pool_backend : ::beman::execution::detail::thread_pool_backend_base {
    explicit simple_thread_pool_backend(::std::in_place_t) : workers(::new ::std::thread[num_threads()]) {}

    simple_thread_pool_backend() : simple_thread_pool_backend(::std::in_place) {
        for (auto& worker : ::std::span(workers.get(), num_threads())) {
            worker = ::std::thread(&simple_thread_pool_backend::run, this);
        }
    }

    ~simple_thread_pool_backend() override {
        shutdown();
        for (auto& worker : ::std::span(workers.get(), num_threads())) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

  private:
    auto shutdown() noexcept -> void {
        ::std::unique_lock guard{mtx};
        shutdown_requested = true;
        guard.unlock();
        cv.notify_all();
    }

    auto submit(task_base* t, ::std::size_t n) noexcept -> void override {
        ::std::unique_lock guard{mtx};
        for (::std::size_t i = 0; i < n; ++i) {
            if (auto prev_back = ::std::exchange(back, t)) {
                prev_back->next = t;
            } else {
                front = t;
            }
            t = t->next;
        }
        assert(t == nullptr);
        guard.unlock();
        if (n == 1uz) {
            cv.notify_one();
        } else {
            cv.notify_all();
        }
    }

    [[nodiscard]] auto pop_front() noexcept -> task_base* {
        ::std::unique_lock guard{mtx};
        cv.wait(guard, [this] { return front != nullptr || shutdown_requested; });
        if (front == back) {
            back = nullptr;
        }
        return front ? ::std::exchange(front, front->next) : nullptr;
    }

    auto run() noexcept -> void {
        while (auto task = pop_front()) {
            task->exec();
        }
    }

    bool                               shutdown_requested = false;
    ::std::mutex                       mtx;
    ::std::condition_variable          cv;
    task_base*                         front = nullptr;
    task_base*                         back  = nullptr;
    ::std::unique_ptr<::std::thread[]> workers;
};

#if BEMAN_EXECUTION_WITH_GCD_SUPPORT && __has_include(<dispatch/dispatch.h>)
struct gcd_thread_pool_backend : ::beman::execution::detail::thread_pool_backend_base {
  private:
    auto submit(task_base* t, ::std::size_t n) noexcept -> void override {
        auto queue = ::dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        for (::std::size_t i = 0; i < n; ++i) {
            auto* next = t->next;
            ::dispatch_async_f(queue, t, &run);
            t = next;
        }
        assert(t == nullptr);
    }

    static auto run(void* ctx) noexcept -> void { static_cast<task_base*>(ctx)->exec(); }
};
#endif

#if defined(_WIN32) && __has_include(<windows.h>)
struct win_thread_pool_backend : ::beman::execution::detail::thread_pool_backend_base {
  private:
    auto submit(task_base* t, ::std::size_t n) noexcept -> void override {
        int ec = 0;
        for (::std::size_t i = 0; i < n; ++i) {
            auto* next = t->next;
            if (ec) [[unlikely]] {
                t->exec(ec);
            } else if (!::TrySubmitThreadpoolCallback(&run, t, nullptr)) {
                ec = static_cast<int>(::GetLastError());
                t->exec(ec);
            }
            t = next;
        }
        assert(t == nullptr);
    }

    static auto CALLBACK run(::PTP_CALLBACK_INSTANCE, void* ctx) noexcept -> void {
        static_cast<task_base*>(ctx)->exec();
    }
};
#endif

#if BEMAN_EXECUTION_WITH_GCD_SUPPORT && __has_include(<dispatch/dispatch.h>)
using thread_pool_backend = gcd_thread_pool_backend;
#elif defined(_WIN32) && __has_include(<windows.h>)
using thread_pool_backend = win_thread_pool_backend;
#else
using thread_pool_backend = simple_thread_pool_backend;
#endif

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // #ifdef INCLUDED_BEMAN_EXECUTION_DETAIL_THREAD_POOL_BACKEND
