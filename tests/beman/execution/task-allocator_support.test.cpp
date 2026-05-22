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

struct allocation_counter {
    std::size_t outstanding{};
};

template <typename T>
struct tracking_allocator {
    using value_type = T;

    allocation_counter* counter{};

    explicit tracking_allocator(allocation_counter& c) noexcept : counter(&c) {}

    template <typename U>
    tracking_allocator(const tracking_allocator<U>& other) noexcept : counter(other.counter) {}

    auto allocate(std::size_t count) -> T* {
        this->counter->outstanding += count * sizeof(T);
        return static_cast<T*>(::operator new(count * sizeof(T), std::align_val_t(alignof(T))));
    }

    auto deallocate(T* ptr, std::size_t count) noexcept -> void {
        ::operator delete(ptr, std::align_val_t(alignof(T)));
        this->counter->outstanding -= count * sizeof(T);
    }

    template <typename U>
    auto operator==(const tracking_allocator<U>& other) const noexcept -> bool {
        return this->counter == other.counter;
    }
};

template <typename Allocator>
struct allocator_aware : some_data, test_detail::allocator_support<Allocator> {
    allocator_aware() : some_data() {}
};

template <typename T>
concept supports_misplaced_allocator = requires(test_resource& resource) {
    T::operator new(sizeof(T), 0, 1, std::allocator_arg, std::pmr::polymorphic_allocator<std::byte>{&resource});
};

template <typename T>
concept supports_resource_pointer_allocator = requires(test_resource& resource) {
    T::operator new(sizeof(T), std::allocator_arg, &resource);
};

template <typename Alloc>
concept constructs_pmr_byte_allocator = requires(Alloc& alloc) {
    std::pmr::polymorphic_allocator<std::byte>{alloc};
};
} // namespace

// ----------------------------------------------------------------------------

TEST(allocator_support) {
    using type = allocator_aware<std::pmr::polymorphic_allocator<std::byte>>;
    static_assert(requires { type::operator new(sizeof(type)); });
    static_assert(requires(test_resource& resource) {
        type::operator new(sizeof(type), std::allocator_arg, std::pmr::polymorphic_allocator<std::byte>{&resource});
    });
    static_assert(requires(const some_data& object, test_resource& resource) {
        type::operator new(
            sizeof(type), object, std::allocator_arg, std::pmr::polymorphic_allocator<std::byte>{&resource});
    });
    static_assert(requires(allocation_counter& counter) {
        type::operator new(sizeof(type), std::allocator_arg, tracking_allocator<int>{counter});
    });
    static_assert(not constructs_pmr_byte_allocator<tracking_allocator<int>>);
    static_assert(not supports_misplaced_allocator<type>);
    static_assert(not supports_resource_pointer_allocator<type>);

    [[maybe_unused]] std::unique_ptr<type> unused(new type{});

    test_resource resource{};
    std::pmr::polymorphic_allocator<std::byte> allocator{&resource};
    ASSERT(resource.outstanding == 0u);
    type* ptr{new (std::allocator_arg, allocator) type{}};
    ASSERT(resource.outstanding != 0u);
    ptr->~type();
    ASSERT(resource.outstanding != 0u);
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmismatched-new-delete"
#endif
    type::operator delete(ptr, sizeof(type), std::allocator_arg, allocator);
    ASSERT(resource.outstanding == 0u);

    some_data object{};
    void*     raw{type::operator new(sizeof(type), object, std::allocator_arg, allocator)};
    ASSERT(resource.outstanding != 0u);
    type::operator delete(raw, sizeof(type), object, std::allocator_arg, allocator);
    ASSERT(resource.outstanding == 0u);

    allocation_counter counter{};
    void* custom_raw{type::operator new(sizeof(type), std::allocator_arg, tracking_allocator<int>{counter})};
    ASSERT(counter.outstanding != 0u);
    type::operator delete(custom_raw, sizeof(type), std::allocator_arg, tracking_allocator<int>{counter});
    ASSERT(counter.outstanding == 0u);
}
