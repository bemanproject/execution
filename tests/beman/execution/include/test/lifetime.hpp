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
    struct env {
        bool* alive;
        bool  completing{false};
        auto  query(test_std::get_completion_scheduler_t<test_std::set_value_t>) const noexcept -> lifetime_scheduler {
            return {this->alive};
        }
        ~env() { ASSERT(*this->alive || this->completing); }
        auto set_completing() noexcept -> void { this->completing = true; }
    };

    template <test_std::receiver Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_tag;
        std::remove_cvref_t<Receiver> receiver;
        auto                          start() & noexcept -> void { test_std::set_value(std::move(receiver)); }
    };
    struct sender {
        using sender_concept = test_std::sender_tag;

        using completion_signatures = test_std::completion_signatures<test_std::set_value_t()>;
        bool* alive;
        bool  completing{false};
        template <typename...>
        static consteval auto get_completion_signatures() noexcept -> completion_signatures {
            return {};
        }
        ~sender() { ASSERT(*this->alive || this->completing); }
        auto get_env() const noexcept -> env { return {this->alive}; }
        template <test_std::receiver Receiver>
        auto connect(Receiver&& r) && noexcept {
            return state<Receiver>{std::forward<Receiver>(r)};
        }
        auto set_completing() noexcept -> void { this->completing = true; }
    };

    bool* alive;
    bool  completing{false};
    ~lifetime_scheduler() { ASSERT(*this->alive || this->completing); }

    auto schedule() const noexcept -> sender { return {this->alive}; }
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
    lifetime_allocator(const lifetime_allocator<U>& other) : alive(other.alive) {}
    ~lifetime_allocator() { ASSERT(*this->alive || this->completing); }

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
    ~lifetime_stop_token() { ASSERT(*this->alive || this->completing); }
    auto stop_requested() const noexcept -> bool { return false; }
    auto stop_possible() const noexcept -> bool { return true; }
    auto set_completing() noexcept -> void { this->completing = true; }
    auto operator==(const lifetime_stop_token& other) const noexcept -> bool { return this->alive == other.alive; }
};
static_assert(test::lifetime_object<lifetime_stop_token>);
static_assert(test_std::stoppable_token<lifetime_stop_token>);

struct lifetime_receiver {
    bool* alive;
    using receiver_concept = test_std::receiver_tag;
    auto get_env() const noexcept -> lifetime_receiver { return {this->alive}; }
    auto query(test_std::get_stop_token_t) const noexcept -> lifetime_stop_token { return {this->alive}; }
    auto query(test_std::get_scheduler_t) const noexcept -> lifetime_scheduler { return {this->alive}; }
    auto query(test_std::get_delegation_scheduler_t) const noexcept -> lifetime_scheduler { return {this->alive}; }
    auto query(test_std::get_start_scheduler_t) const noexcept -> lifetime_scheduler { return {this->alive}; }
    auto query(test_std::get_allocator_t) const noexcept -> lifetime_allocator<std::byte> { return {this->alive}; }
    auto query(test_std::get_scheduler_allocator_t) const noexcept -> lifetime_allocator<std::byte> {
        return {this->alive};
    }

    template <typename... A>
    auto set_value(A&&... a) && noexcept -> void {
        [[maybe_unused]] bool b = (test::lifetime_set_completing(a) && ...);
        *this->alive            = false;
    }
    template <typename E>
    auto set_error(E&& e) && noexcept -> void {
        test::lifetime_set_completing(e);
        *this->alive = false;
    }
    auto set_stopped() && noexcept -> void { *this->alive = false; }
};
static_assert(test_std::receiver<lifetime_receiver>);

template <test_std::sender Sender>
auto lifetime(Sender&& sender) -> void {
    bool alive{true};
    auto state{test_std::connect(::std::forward<Sender>(sender), lifetime_receiver{&alive})};
    test_std::start(state);
}

template <typename Fun>
    requires requires(Fun&& f, test::lifetime_scheduler s) { f(s); }
auto lifetime(Fun&& sender) -> void {
    bool alive{true};
    auto state{test_std::connect(test_std::continues_on(test_std::just(), test::lifetime_scheduler{&alive}),
                                 test::lifetime_receiver{&alive})};
    test_std::start(state);
}
} // namespace test

// ----------------------------------------------------------------------------

#endif // INCLUDED_TESTS_BEMAN_EXECUTION_INCLUDE_TEST_LIFETIME
