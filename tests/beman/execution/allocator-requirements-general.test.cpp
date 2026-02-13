// src/beman/execution/tests/allocator-requirements-general.test.cpp -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.simple_allocator;
#else
#include <beman/execution/detail/simple_allocator.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
enum class size_type {};

struct non_allocator {};
struct no_allocate {
    using value_type = int;
    auto deallocate(int*, ::std::size_t) -> void {}

    auto operator==(const no_allocate&) const -> bool = default;
};
struct no_deallocate {
    using value_type = int;
    auto allocate(::std::size_t n) -> int* { return new int[n]; }

    auto operator==(const no_deallocate&) const -> bool = default;
};

struct no_value_type {
    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int*, ::std::size_t) -> void {}

    auto operator==(const no_value_type&) const -> bool = default;
};

struct allocate_return_type_mismatch {
    using value_type = long;
    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int*, ::std::size_t) -> void {}

    auto operator==(const allocate_return_type_mismatch&) const -> bool = default;
};

struct allocate_size_type_mismatch {
    using value_type = long;
    auto allocate(::std::size_t n) -> int* { return new int[::std::size_t(n)]; }
    auto deallocate(int*, ::std::size_t) -> void {}

    auto operator==(const allocate_size_type_mismatch&) const -> bool = default;
};

struct deallocate_object_type_mismatch {
    using value_type = int;
    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(long*, ::std::size_t) -> void {}

    auto operator==(const deallocate_object_type_mismatch&) const -> bool = default;
};

struct deallocate_size_type_mismatch {
    using value_type = int;
    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int*, size_type) -> void {}

    auto operator==(const deallocate_size_type_mismatch&) const -> bool = default;
};

struct not_copy_constructible {
    using value_type = int;

    not_copy_constructible()                                                = default;
    not_copy_constructible(not_copy_constructible&&)                        = delete;
    not_copy_constructible(const not_copy_constructible&)                   = delete;
    ~not_copy_constructible()                                               = default;
    auto operator=(not_copy_constructible&&) -> not_copy_constructible      = delete;
    auto operator=(const not_copy_constructible&) -> not_copy_constructible = delete;
    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int*, ::std::size_t) -> void {}

    auto operator==(const not_copy_constructible&) const -> bool = default;
};

struct not_equality_comparable {
    using value_type = int;

    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int*, ::std::size_t) -> void {}

    auto operator==(const not_equality_comparable&) const -> bool = delete;
};

struct allocator {
    using value_type = int;

    auto allocate(::std::size_t n) -> int* { return new int[n]; }
    auto deallocate(int* object, ::std::size_t) -> void { delete[] object; }

    auto operator==(const allocator&) const -> bool = default;
};
} // namespace

TEST(allocator_requirements_general) {
    static_assert(not test_detail::simple_allocator<non_allocator>);
    static_assert(not test_detail::simple_allocator<no_allocate>);
    static_assert(not test_detail::simple_allocator<no_deallocate>);
    static_assert(not test_detail::simple_allocator<no_value_type>);
    static_assert(not test_detail::simple_allocator<allocate_return_type_mismatch>);
    static_assert(not test_detail::simple_allocator<allocate_size_type_mismatch>);
    static_assert(not test_detail::simple_allocator<deallocate_object_type_mismatch>);
    static_assert(not test_detail::simple_allocator<deallocate_size_type_mismatch>);
    static_assert(not test_detail::simple_allocator<not_copy_constructible>);
    static_assert(not test_detail::simple_allocator<not_equality_comparable>);

    static_assert(test_detail::simple_allocator<allocator>);
}
