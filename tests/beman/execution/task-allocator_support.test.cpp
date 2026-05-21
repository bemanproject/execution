// tests/beman/execution/task-allocator_support.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <beman/execution/detail/task/allocator_support.hpp>
#include <cstddef>
#include <memory>
#include <memory_resource>
#include <new>

#ifdef _MSC_VER
#pragma warning(disable : 4291)
#endif

// ----------------------------------------------------------------------------

namespace {
struct test_resource : std::pmr::memory_resource {
    std::size_t outstanding{};

    auto do_allocate(std::size_t size, std::size_t) -> void* override {
        this->outstanding += size;
        return ::operator new(size);
    }
    auto do_deallocate(void* ptr, std::size_t size, std::size_t) -> void override {
        ::operator delete(ptr);
        this->outstanding -= size;
    }
    auto do_is_equal(const std::pmr::memory_resource& other) const noexcept -> bool override {
        auto* tr{dynamic_cast<const test_resource*>(&other)};
        return tr == this;
    }
};

struct some_data {
    double data{};
};

template <typename Allocator>
struct allocator_aware : some_data, test_detail::allocator_support<Allocator> {
    allocator_aware() : some_data() {}
};
} // namespace

// ----------------------------------------------------------------------------

TEST(allocator_support) {
    using type = allocator_aware<std::pmr::polymorphic_allocator<std::byte>>;
    [[maybe_unused]] std::unique_ptr<type> unused(new type{});

    test_resource resource{};
    ASSERT(resource.outstanding == 0u);
    type* ptr{new (std::allocator_arg, &resource) type{}};
    ASSERT(resource.outstanding != 0u);
    ptr->~type();
    ASSERT(resource.outstanding != 0u);
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
#endif
    type::operator delete(ptr, sizeof(type), std::allocator_arg, &resource);
    ASSERT(resource.outstanding == 0u);
}
