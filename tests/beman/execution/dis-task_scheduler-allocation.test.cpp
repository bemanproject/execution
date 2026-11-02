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

#ifndef BEMAN_TSAN_ENABLED
void* operator new(std::size_t size) {
    std::abort();
    return std::malloc(size);
}
void* operator new[](std::size_t size) {
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
constexpr std::size_t big_size{40};
struct big_scheduler {
    using scheduler_concept = ex::scheduler_tag;
    std::array<char, big_size> data{};

    template <ex::receiver R>
    struct state {
        using operation_state_concept = ex::operation_state_tag;
        std::remove_reference_t<R> receiver;
        std::array<char, big_size> data{};
        state(R&& r) : receiver(std::forward<R>(r)) {}
        void start() noexcept { ex::set_value(std::move(receiver)); }
    };

    struct sender {
        using sender_concept = ex::sender_tag;
        std::array<char, big_size> data{};

        struct env {
            auto query(const ex::get_completion_scheduler_t<ex::set_value_t>&) const noexcept {
                return big_scheduler{};
            }
        };
        auto get_env() const noexcept { return env{}; }

        template <typename... Env>
        static consteval auto get_completion_signatures() {
            return ex::completion_signatures<ex::set_value_t()>{};
        }

        template <ex::receiver R>
        state<R> connect(R&& r) && {
            return state<R>{std::forward<R>(r)};
        }
    };

    auto schedule() const noexcept { return sender{}; }

    auto query(ex::get_forward_progress_guarantee_t) const noexcept {
        return ex::forward_progress_guarantee::parallel;
    }

    bool operator==(const big_scheduler&) const noexcept = default;
};

static_assert(big_size <= sizeof(big_scheduler));
static_assert(ex::sender<big_scheduler::sender>);
static_assert(ex::scheduler<big_scheduler>);

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
    ex::sync_wait(ex::starts_on(big_scheduler{}, [](std::allocator_arg_t, auto) -> ex::task<void, allocator_env> {
        auto s = co_await ex::read_env(ex::get_start_scheduler);
        co_await ex::starts_on(s, ex::just());
    }(std::allocator_arg, alloc) | ex::write_env(ex::env{ex::prop{ex::get_allocator, alloc}})));
}
