// include/beman/execution/detail/task/allocator_support.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TASK_ALLOCATOR_SUPPORT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TASK_ALLOCATOR_SUPPORT

#include <concepts>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct alignas(__STDCPP_DEFAULT_NEW_ALIGNMENT__) allocator_support_allocation_unit {
    std::byte bytes[__STDCPP_DEFAULT_NEW_ALIGNMENT__];
};

static_assert(sizeof(allocator_support_allocation_unit) == __STDCPP_DEFAULT_NEW_ALIGNMENT__);
static_assert(alignof(allocator_support_allocation_unit) == __STDCPP_DEFAULT_NEW_ALIGNMENT__);

struct allocator_support_header {
    void (*deallocate)(void*, std::size_t) noexcept;
    std::size_t count;
    std::size_t palloc_offset;
};

template <typename Alloc>
using allocator_support_alloc_t = std::remove_cvref_t<Alloc>;

template <typename Alloc>
using allocator_support_palloc_t = typename std::allocator_traits<
    allocator_support_alloc_t<Alloc>>::template rebind_alloc<allocator_support_allocation_unit>;

template <typename Alloc>
concept allocator_support_allocator_like = requires { typename allocator_support_alloc_t<Alloc>::value_type; };

template <typename Allocator, typename Alloc>
concept allocator_support_allocator_arg =
    allocator_support_allocator_like<Alloc> && requires(allocator_support_alloc_t<Alloc>& alloc, std::size_t size) {
        typename allocator_support_palloc_t<Alloc>;
        typename std::allocator_traits<allocator_support_palloc_t<Alloc>>::pointer;
        requires std::same_as<typename std::allocator_traits<allocator_support_palloc_t<Alloc>>::pointer,
                              allocator_support_allocation_unit*>;
        requires(alignof(allocator_support_palloc_t<Alloc>) <= alignof(allocator_support_allocation_unit));
        allocator_support_palloc_t<Alloc>(alloc);
        std::allocator_traits<allocator_support_palloc_t<Alloc>>::allocate(
            std::declval<allocator_support_palloc_t<Alloc>&>(), size);
    };

/*!
 * \brief Utility adding allocator support to type by embedding the allocator
 * \headerfile beman/execution/task.hpp <beman/execution/task.hpp>
 *
 * To add allocator support using this class just publicly inherit from
 * allocator_support<Allocator, YourPromiseType>. This utility is probably
 * only useful for coroutine promise types.
 *
 * This struct is a massive hack, primarily support allocators for coroutines.
 * The memory for coroutines is implicitly managed and there isn't a way to
 * provide the memory directly. Instead, the promise_type can overload an
 * operator new and use a leading std::allocator_arg/allocator pair when it
 * is present. Even worse, the operator delete only gets passed a pointer to
 * delete and a size. To determine the correct allocator the operator delete
 * stores a type-erased deallocation header and the rebound allocator in the
 * allocation block.
 */
template <typename Allocator>
struct allocator_support {
    static std::size_t align_up(std::size_t value, std::size_t alignment) {
        return ((value + alignment - 1u) / alignment) * alignment;
    }

    static std::size_t header_offset(std::size_t size) {
        return allocator_support::align_up(size, alignof(allocator_support_header));
    }

    static allocator_support_header* get_header(void* ptr, std::size_t size) {
        ptr = static_cast<std::byte*>(ptr) + allocator_support::header_offset(size);
        return ::std::launder(reinterpret_cast<allocator_support_header*>(ptr));
    }

    template <typename PAlloc>
    static void deallocate_with(void* ptr, std::size_t size) noexcept {
        using palloc_traits = std::allocator_traits<PAlloc>;

        allocator_support_header* header{allocator_support::get_header(ptr, size)};
        auto*                     palloc_ptr{
            ::std::launder(reinterpret_cast<PAlloc*>(static_cast<std::byte*>(ptr) + header->palloc_offset))};
        PAlloc      palloc{*palloc_ptr};
        std::size_t count{header->count};
        palloc_ptr->~PAlloc();
        palloc_traits::deallocate(palloc, static_cast<allocator_support_allocation_unit*>(ptr), count);
    }

    template <typename Alloc>
        requires allocator_support_allocator_arg<Allocator, Alloc>
    static void* allocate(std::size_t size, Alloc alloc) {
        using palloc_t      = allocator_support_palloc_t<Alloc>;
        using palloc_traits = std::allocator_traits<palloc_t>;

        palloc_t    palloc{alloc};
        std::size_t header_offset{allocator_support::header_offset(size)};
        std::size_t palloc_offset{
            allocator_support::align_up(header_offset + sizeof(allocator_support_header), alignof(palloc_t))};
        std::size_t count{(palloc_offset + sizeof(palloc_t) + sizeof(allocator_support_allocation_unit) - 1u) /
                          sizeof(allocator_support_allocation_unit)};

        allocator_support_allocation_unit* ptr{palloc_traits::allocate(palloc, count)};
        try {
            new (static_cast<std::byte*>(static_cast<void*>(ptr)) + header_offset)
                allocator_support_header{&allocator_support::deallocate_with<palloc_t>, count, palloc_offset};
            new (static_cast<std::byte*>(static_cast<void*>(ptr)) + palloc_offset) palloc_t(palloc);
        } catch (...) {
            palloc_traits::deallocate(palloc, ptr, count);
            throw;
        }
        return ptr;
    }

    static void* operator new(std::size_t size) { return allocator_support::allocate(size, Allocator{}); }

    template <typename Alloc, typename... A>
        requires allocator_support_allocator_arg<Allocator, Alloc>
    static void* operator new(std::size_t size, std::allocator_arg_t, Alloc alloc, A&&...) {
        return allocator_support::allocate(size, alloc);
    }

    template <typename This, typename Alloc, typename... A>
        requires allocator_support_allocator_arg<Allocator, Alloc>
    static void* operator new(std::size_t size, const This&, std::allocator_arg_t, Alloc alloc, A&&...) {
        return allocator_support::allocate(size, alloc);
    }

    template <typename... A>
    static void operator delete(void* ptr, std::size_t size, const A&...) noexcept {
        allocator_support::operator delete(ptr, size);
    }
    static void operator delete(void* ptr, std::size_t size) noexcept {
        allocator_support::get_header(ptr, size)->deallocate(ptr, size);
    }
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
