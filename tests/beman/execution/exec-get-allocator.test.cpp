// src/beman/execution/tests/exec-get-allocator.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct non_allocator {};
struct allocator {
    int tag{};
    using value_type = int;
    auto allocate(std::size_t n) -> int* { return new value_type[n]; }
    auto deallocate(int* ptr, std::size_t) -> void { delete[] ptr; }
    auto operator==(const allocator&) const -> bool = default;
};

struct no_allocator {
    int tag{};
};

template <bool Noexcept>
struct throwing_allocator {
    auto query(const test_std::get_allocator_t&) const noexcept(Noexcept) -> allocator { return {}; }
};

template <typename Result>
struct allocator_object {
    int  tag{};
    auto query(const test_std::get_allocator_t&) const noexcept -> Result { return Result{this->tag}; }
};

struct non_const_get_allocator {
    auto query(const test_std::get_allocator_t&) noexcept -> allocator { return {}; }
};

template <bool Expect, typename Object>
auto test_get_allocator(Object&& object) -> void {
    static_assert(Expect == requires { test_std::get_allocator(::std::forward<Object>(object)); });
}
} // namespace

TEST(exec_get_allocator) {
    static_assert(std::same_as<const test_std::get_allocator_t, decltype(test_std::get_allocator)>);
    static_assert(test_std::forwarding_query(test_std::get_allocator));

    test_get_allocator<false>(no_allocator());
    test_get_allocator<true>(throwing_allocator<true>());
    test_get_allocator<false>(throwing_allocator<false>());

    static_assert(test_detail::simple_allocator<allocator>);
    static_assert(not test_detail::simple_allocator<non_allocator>);
    test_get_allocator<true>(allocator_object<allocator>());
    test_get_allocator<false>(allocator_object<non_allocator>());

    test_get_allocator<false>(non_const_get_allocator());
    non_const_get_allocator alloc;
    test_get_allocator<false>(alloc);
    non_const_get_allocator calloc;
    test_get_allocator<false>(calloc);

    static_assert(std::same_as<allocator, decltype(test_std::get_allocator(allocator_object<allocator>()))>);
    ASSERT(43 == test_std::get_allocator(allocator_object<allocator>{43}).tag);
}
