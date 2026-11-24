// tests/beman/execution/include/test/lifetime.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_LIFETIME
#define INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_LIFETIME

#include <beman/execution/detail/common.hpp>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <exception>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.simple_allocator;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------
// This file defines test utilities for verifying the lifetime of objects
// obtained from environments. In general, these can't outlive the asynchronous
// operation and they normally need to be destroyed before completing.
// There are some issues to consider when testing these:
// - Objects which need to be stored kept (like let_*'s arguments) need to be
//   move/forwarded to meet this requirement.
// - Senders which complete synchronously with a query result normally need
//   to complete asynchronously can't be tested this way.
// - For queries returning a reference the environment needs to be kept
//   around, i.e., they also can't be tested with this utility.

namespace test {
template <typename T>
concept lifetime_object = requires(std::remove_cvref_t<T> t) { t.set_completing(); };

template <typename T>
auto lifetime_set_completing(T&& t) noexcept -> bool {
    if constexpr (lifetime_object<T>) {
        static_assert(requires { t.set_completing(); });
        t.set_completing();
    }
    return true;
}

struct lifetime_scheduler {
    using scheduler_concept = test_std::scheduler_tag;
    using scheduler_type    = decltype(test_std::run_loop{}.get_scheduler());

    struct env {
        bool*          alive;
        scheduler_type sched;
        env(bool* alive, scheduler_type sched) : alive(alive), sched(sched) {}
        env(env&& other) : alive(std::exchange(other.alive, nullptr)), sched(other.sched) {}
        ~env() { ASSERT(!this->alive || *this->alive || this->completing); }
        bool completing{false};
        auto query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept -> lifetime_scheduler {
            return {this->alive, this->sched};
        }
        auto set_completing() noexcept -> void { this->completing = true; }
    };

    struct sender {
        using sender_concept = test_std::sender_tag;

        bool*          alive;
        scheduler_type sched;
        bool           completing{false};
        template <typename, typename... E>
        static consteval auto get_completion_signatures() noexcept {
            using sender_type = decltype(test_std::schedule(std::declval<scheduler_type>()));
            if constexpr (sizeof...(E) == 0) {
                return test_std::get_completion_signatures<sender_type, test_std::env<>>();
            } else {
                return test_std::get_completion_signatures<sender_type, E...>();
            }
        }
        auto get_env() const noexcept -> env { return {this->alive, this->sched}; }
        sender(bool* alive, scheduler_type sched) : alive(alive), sched(sched) {}
        sender(sender&& other) : alive(std::exchange(other.alive, nullptr)), sched(other.sched) {}
        ~sender() { ASSERT(!this->alive || *this->alive || this->completing); }
        template <test_std::receiver Receiver>
        auto connect(Receiver&& r) && noexcept {
            return test_std::connect(test_std::schedule(this->sched), std::forward<Receiver>(r));
        }
        auto set_completing() noexcept -> void { this->completing = true; }
    };

    bool*          alive;
    scheduler_type sched;
    bool           completing{false};
    lifetime_scheduler(bool* alive, scheduler_type sched) : alive(alive), sched(sched) {}
    lifetime_scheduler(const lifetime_scheduler& other) noexcept : alive(other.alive), sched(other.sched) {}
    lifetime_scheduler(lifetime_scheduler&& other) noexcept
        : alive(std::exchange(other.alive, nullptr)), sched(other.sched) {}
    ~lifetime_scheduler() { ASSERT(!this->alive || *this->alive || this->completing); }
    lifetime_scheduler& operator=(const lifetime_scheduler& other) noexcept {
        this->alive = other.alive;
        this->sched = other.sched;
        return *this;
    }
    lifetime_scheduler& operator=(lifetime_scheduler&& other) noexcept {
        this->alive = std::exchange(other.alive, nullptr);
        this->sched = other.sched;
        return *this;
    }

    auto schedule() const noexcept -> sender { return {this->alive, this->sched}; }
    auto operator==(const lifetime_scheduler& other) const noexcept -> bool { return this->alive == other.alive; }
    auto query(test_std::get_forward_progress_guarantee_t) const noexcept -> test_std::forward_progress_guarantee {
        return test_std::forward_progress_guarantee::concurrent;
    }
    auto set_completing() noexcept -> void { this->completing = true; }
};
static_assert(test::lifetime_object<lifetime_scheduler::sender>);
static_assert(test_std::sender<lifetime_scheduler::sender>);
static_assert(test::lifetime_object<lifetime_scheduler>);
static_assert(test_std::sender_in<lifetime_scheduler::sender>);
static_assert(test_std::scheduler<lifetime_scheduler>);

template <typename T>
struct lifetime_allocator {
    using value_type = T;

    bool* alive;
    bool  completing{false};
    lifetime_allocator(bool* alive) : alive(alive) {}
    template <typename U>
    lifetime_allocator(const lifetime_allocator<U>& other) noexcept : alive(other.alive) {}
    lifetime_allocator(const lifetime_allocator& other) noexcept : alive(other.alive) {}
    lifetime_allocator(lifetime_allocator&& other) noexcept : alive(std::exchange(other.alive, nullptr)) {}
    lifetime_allocator& operator=(const lifetime_allocator& other) noexcept {
        this->alive = other.alive;
        return *this;
    }
    lifetime_allocator& operator=(lifetime_allocator&& other) noexcept {
        this->alive = std::exchange(other.alive, nullptr);
        return *this;
    }
    ~lifetime_allocator() { ASSERT(!this->alive || *this->alive || this->completing); }

    auto allocate(std::size_t n) -> value_type* {
        return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
    }
    auto deallocate(value_type* p, std::size_t) -> void { ::operator delete(p); }

    auto operator==(const lifetime_allocator& other) const noexcept -> bool { return this->alive == other.alive; }
    auto set_completing() noexcept -> void { this->completing = true; }
};
static_assert(test::lifetime_object<lifetime_allocator<int>>);
static_assert(test_detail::simple_allocator<lifetime_allocator<int>>);

struct lifetime_stop_token {
    template <typename Callback>
    struct callback_type {
        Callback callback;
        bool*    alive;
        bool     completing{false};
        callback_type(lifetime_stop_token token, Callback c) : callback(::std::move(c)), alive(token.alive) {}
        ~callback_type() { ASSERT(*this->alive || this->completing); }
        auto set_completing() noexcept -> void { this->completing = true; }
    };
    bool* alive;
    bool  completing{false};
    lifetime_stop_token(bool* alive) noexcept : alive(alive) {}
    lifetime_stop_token(const lifetime_stop_token& other) noexcept = default;
    lifetime_stop_token(lifetime_stop_token&& other) noexcept
        : alive(std::exchange(other.alive, nullptr)), completing(other.completing) {}
    ~lifetime_stop_token() { ASSERT(!this->alive || *this->alive || this->completing); }
    lifetime_stop_token& operator=(const lifetime_stop_token& other) noexcept {
        this->alive = other.alive;
        return *this;
    }
    lifetime_stop_token& operator=(lifetime_stop_token&& other) noexcept {
        this->alive      = std::exchange(other.alive, nullptr);
        this->completing = other.completing;
        return *this;
    }
    auto stop_requested() const noexcept -> bool { return false; }
    auto stop_possible() const noexcept -> bool { return true; }
    auto set_completing() noexcept -> void { this->completing = true; }
    auto operator==(const lifetime_stop_token& other) const noexcept -> bool { return this->alive == other.alive; }
};
static_assert(test::lifetime_object<lifetime_stop_token>);
static_assert(test_std::stoppable_token<lifetime_stop_token>);

struct lifetime_receiver_env {
    bool*               alive;
    test_std::run_loop& loop;
    lifetime_receiver_env(bool* alive, test_std::run_loop& loop) : alive(alive), loop(loop) {}
    lifetime_receiver_env(const lifetime_receiver_env&) noexcept = default;
    lifetime_receiver_env(lifetime_receiver_env&& other) noexcept
        : alive(std::exchange(other.alive, nullptr)), loop(other.loop) {}
    ~lifetime_receiver_env() { ASSERT(!this->alive || *this->alive); }
    auto query(test_std::get_stop_token_t) const noexcept -> lifetime_stop_token { return {this->alive}; }
    auto query(test_std::get_scheduler_t) const noexcept -> lifetime_scheduler {
        return {this->alive, this->loop.get_scheduler()};
    }
    auto query(test_std::get_delegation_scheduler_t) const noexcept -> lifetime_scheduler {
        return {this->alive, this->loop.get_scheduler()};
    }
    auto query(test_std::get_start_scheduler_t) const noexcept -> lifetime_scheduler {
        return {this->alive, this->loop.get_scheduler()};
    }
    auto query(test_std::get_allocator_t) const noexcept -> lifetime_allocator<std::byte> { return {this->alive}; }
    auto query(test_std::get_scheduler_allocator_t) const noexcept -> lifetime_allocator<std::byte> {
        return {this->alive};
    }
};

struct lifetime_receiver {
    using receiver_concept = test_std::receiver_tag;

    bool*               alive;
    test_std::run_loop& loop;

    auto get_env() const noexcept -> lifetime_receiver_env { return {this->alive, this->loop}; }
    template <typename... A>
    auto set_value(A&&... a) && noexcept -> void {
        [[maybe_unused]] bool b = (test::lifetime_set_completing(a) && ...);
        this->loop.finish();
        *this->alive = false;
    }
    template <typename E>
    auto set_error(E&& e) && noexcept -> void {
        test::lifetime_set_completing(e);
        this->loop.finish();
        *this->alive = false;
    }
    auto set_stopped() && noexcept -> void {
        this->loop.finish();
        *this->alive = false;
    }
};
static_assert(test_std::receiver<lifetime_receiver>);

template <typename Fun>
    requires requires(Fun&& f, test::lifetime_scheduler s) {
        { f(s) } -> test_std::sender;
    }
auto lifetime(Fun&& fun) -> void {
    test_std::run_loop loop;
    bool               alive{true};
    auto state{test_std::connect(std::forward<Fun>(fun)(test::lifetime_scheduler{&alive, loop.get_scheduler()}),
                                 test::lifetime_receiver{&alive, loop})};
    test_std::start(state);
    loop.run();
    ASSERT(!alive);
}

template <test_std::sender Sender>
auto lifetime(Sender&& sender) -> void {
    test::lifetime([&](test::lifetime_scheduler) { return ::std::forward<Sender>(sender); });
}

} // namespace test

// ----------------------------------------------------------------------------

#endif // INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_LIFETIME
