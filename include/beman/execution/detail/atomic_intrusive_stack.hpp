// include/beman/execution/detail/atomic_intrusive_stack.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_ATOMIC_INTRUSIVE_STACK
#define INCLUDED_BEMAN_EXECUTION_DETAIL_ATOMIC_INTRUSIVE_STACK

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <atomic>
#include <cassert>
#include <optional>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.intrusive_stack;
#else
#include <beman/execution/detail/intrusive_stack.hpp>
#endif

namespace beman::execution::detail {

template <auto Next>
class atomic_intrusive_stack;

//! @brief  This data structure is an intrusive stack that can be used in a lock-free manner.
//!
//! The stack is implemented as a singly linked list where the head is an atomic pointer to the first item in
//! the stack.
//! try_push() is a lock-free operation that tries to push an item to the stack. If the stack is empty, it
//! returns nullptr and the item is pushed to the stack.
//! This stack has a closed state, which is indicated by the head pointing to the stack itself. In this state,
//! try_push() returns std::nullopt and the stack is not modified.
//! pop_all_and_shutdown() is a lock-free operation that pops all items from the stack and returns them in a
//! queue. If the stack is empty, it returns an empty queue.
//!
//! We use this stack in the split implementation to store the listeners that are waiting for the operation to
//! complete.
//!
//! @tparam Item  The type of the item in the stack.
//! @tparam Next  The pointer to the next item in the stack.
template <class Item, Item* Item::* Next>
class atomic_intrusive_stack<Next> {
  public:
    atomic_intrusive_stack() = default;
    ~atomic_intrusive_stack() { assert(!head_ || head_ == this); }
    atomic_intrusive_stack(const atomic_intrusive_stack&)                        = delete;
    auto operator=(const atomic_intrusive_stack&) -> atomic_intrusive_stack&     = delete;
    atomic_intrusive_stack(atomic_intrusive_stack&&) noexcept                    = delete;
    auto operator=(atomic_intrusive_stack&&) noexcept -> atomic_intrusive_stack& = delete;

    //! @brief  Tries to push an item to the stack.
    //!
    //! @param item  The item to push to the stack.
    //!
    //! @return  If the stack is empty, returns nullptr and pushes the item to the stack.
    //!          If the stack is in the closed state, returns std::nullopt.
    auto try_push(Item* item) noexcept -> std::optional<Item*> {
        void* ptr = head_.load();
        if (ptr == this) {
            return std::nullopt;
        }
        item->*Next = static_cast<Item*>(ptr);
        while (!head_.compare_exchange_weak(ptr, item)) {
            if (ptr == this) {
                return std::nullopt;
            }
            item->*Next = static_cast<Item*>(ptr);
        }
        return static_cast<Item*>(ptr);
    }

    //! @brief  Tests if the stack is empty and not in the closed state.
    auto empty_and_not_shutdown() const noexcept -> bool { return head_.load() == nullptr; }

    //! @brief  Pops all items from the stack, returns them and puts this stack into the closed state.
    //!
    //! @return  If the stack is empty, returns an empty stack.
    auto pop_all_and_shutdown() noexcept -> ::beman::execution::detail::intrusive_stack<Next> {
        auto  stack = ::beman::execution::detail::intrusive_stack<Next>{};
        void* ptr   = head_.exchange(this);
        if (ptr == this) {
            return stack;
        }
        auto item   = static_cast<Item*>(ptr);
        stack.head_ = item;
        return stack;
    }

  private:
    ::std::atomic<void*> head_{nullptr};
};

} // namespace beman::execution::detail

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_ATOMIC_INTRUSIVE_STACK
