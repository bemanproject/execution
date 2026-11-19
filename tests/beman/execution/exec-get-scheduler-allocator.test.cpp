// tests/beman/execution/exec-get-scheduler-allocator.test.cpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct plain {};
struct sched {};

struct non_allocator {};

template <typename T>
struct allocator {
    using value_type = T;
    auto allocate(std::size_t n) -> value_type* { return static_cast<T*>(operator new(n)); }
    auto deallocate(value_type* ptr, size_t) { operator delete(ptr); }
    bool operator==(const allocator&) const = default;
};

static_assert(not test_detail::simple_allocator<non_allocator>);
static_assert(test_detail::simple_allocator<allocator<plain>>);
static_assert(test_detail::simple_allocator<allocator<sched>>);

struct with_neither {};
template <bool Noexcept, typename A = allocator<sched>>
struct with_scheduler_allocator {
    mutable std::remove_cvref_t<A> a;
    auto query(test_std::get_scheduler_allocator_t) const noexcept(Noexcept) -> A { return a; }
};
template <bool Noexcept, typename A = allocator<plain>>
struct with_allocator {
    mutable std::remove_cvref_t<A> a;
    auto                           query(test_std::get_allocator_t) const noexcept(Noexcept) -> A { return a; }
};
template <bool Noexcept1, bool Noexcept2>
struct with_both {
    auto query(test_std::get_scheduler_allocator_t) const noexcept(Noexcept1) { return allocator<sched>{}; }
    auto query(test_std::get_allocator_t) const noexcept(Noexcept2) { return allocator<plain>{}; }
};

template <bool Expect, typename Alloc = allocator<void>>
auto test_get_scheduler_allocator(const auto& env) {
    static_assert(Expect == requires { test_std::get_scheduler_allocator(env); });
    if constexpr (Expect) {
        [[maybe_unused]] auto alloc = test_std::get_scheduler_allocator(env);
        static_assert(std::same_as<Alloc, decltype(test_std::get_scheduler_allocator(env))>);
    }
}
} // namespace

// ----------------------------------------------------------------------------

TEST(exec_get_scheduler_allocator) {
    test_get_scheduler_allocator<false>(with_neither());
    test_get_scheduler_allocator<false>(with_scheduler_allocator<false>());
    test_get_scheduler_allocator<false>(with_allocator<false>());
    test_get_scheduler_allocator<false>(with_both<false, false>());

    test_get_scheduler_allocator<true, allocator<sched>>(with_scheduler_allocator<true>());
    test_get_scheduler_allocator<true, allocator<plain>>(with_allocator<true>());
    test_get_scheduler_allocator<true, allocator<sched>>(with_both<true, true>());
    test_get_scheduler_allocator<true, allocator<sched>>(with_both<true, false>());
    test_get_scheduler_allocator<true, allocator<plain>>(with_both<false, true>());

    test_get_scheduler_allocator<true, allocator<sched>>(with_scheduler_allocator<true, allocator<sched>>());
    test_get_scheduler_allocator<true, allocator<sched>&>(with_scheduler_allocator<true, allocator<sched>&>());
    test_get_scheduler_allocator<true, const allocator<sched>&>(
        with_scheduler_allocator<true, const allocator<sched>&>());
    test_get_scheduler_allocator<true, allocator<plain>>(with_allocator<true, allocator<plain>>());
    test_get_scheduler_allocator<true, allocator<plain>&>(with_allocator<true, allocator<plain>&>());
    test_get_scheduler_allocator<true, const allocator<plain>&>(with_allocator<true, const allocator<plain>&>());
}
