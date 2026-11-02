// tests/beman/execution/dis-task_scheduler-allocation.test.cpp       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if 0
#include <execution>
namespace ex = std::execution;
#elif 1
#include <beman/execution.hpp>
namespace ex = beman::execution;
#else
#include <stdexec/execution.hpp>
namespace ex = stdexec;
#endif

#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <memory_resource>
#include <type_traits>
#endif

// ----------------------------------------------------------------------------

#if defined(__SANITIZE_THREAD__)
#define BEMAN_TSAN_ENABLED 1
#elif defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define BEMAN_TSAN_ENABLED 1
#endif
#endif

std::atomic<bool> new_disabled{false};

void disable_new() { new_disabled = true; }

#ifndef BEMAN_TSAN_ENABLED
void* operator new(std::size_t size) {
    if (new_disabled)
        std::abort();
    return std::malloc(size);
}
void* operator new[](std::size_t size) {
    if (new_disabled)
        std::abort();
    return std::malloc(size);
}
void operator delete(void* ptr) noexcept { std::free(ptr); }
void operator delete(void* ptr, std::size_t) noexcept { std::free(ptr); }
void operator delete[](void* ptr) noexcept { std::free(ptr); }
void operator delete[](void* ptr, std::size_t) noexcept { std::free(ptr); }
#endif

// ----------------------------------------------------------------------------

namespace {
inline std::pmr::memory_resource* malloc_resource() {
    struct resource_t : std::pmr::memory_resource {
        void* do_allocate(std::size_t bytes, std::size_t) override { return std::malloc(bytes); }
        void  do_deallocate(void* p, std::size_t, std::size_t) override { std::free(p); }
        bool  do_is_equal(const std::pmr::memory_resource& other) const noexcept override { return this == &other; }
    };
    static constinit resource_t res{};
    return &res;
}

template <typename T>
struct allocator {
    using value_type = T;
    std::pmr::memory_resource* resource{};
    allocator(std::pmr::memory_resource* r) : resource(r) {}
    template <typename U>
    allocator(const allocator<U>& other) : resource(other.resource) {}
    T*   allocate(std::size_t n) { return static_cast<T*>(resource->allocate(n * sizeof(T))); }
    void deallocate(T* p, std::size_t) noexcept { resource->deallocate(p, 0, alignof(T)); }
    bool operator==(const allocator&) const noexcept = default;
         operator std::pmr::memory_resource*() const noexcept { return resource; }
};

struct allocator_env {
    using allocator_type = std::pmr::polymorphic_allocator<std::byte>;
};
} // namespace

// ----------------------------------------------------------------------------

int main(int ac, char*[]) {
    allocator<std::byte> alloc(ac == 1 ? malloc_resource() : std::pmr::new_delete_resource());
    ex::env              env{ex::prop{ex::get_allocator, alloc}};

    ex::run_loop loop{};
    std::jthread thread([&] { loop.run(); });
    disable_new();

    ex::counting_scope scope{};

    ex::spawn(
        ex::starts_on(loop.get_scheduler(), ex::just() | ex::then([]() noexcept { std::cout << "hello, world\n"; })),
        scope.get_token(),
        env);

    ex::spawn(ex::starts_on(loop.get_scheduler(), ex::just() | ex::then([]() noexcept {
                                                      std::cout << "goodbye, world\n";
                                                  }) | ex::then([&]() noexcept { loop.finish(); })),
              scope.get_token(),
              env);

    ex::sync_wait(scope.join());
}
