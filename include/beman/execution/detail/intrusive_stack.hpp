// include/beman/execution/detail/intrusive_stack.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_INTRUSIVE_STACK
#define INCLUDED_BEMAN_EXECUTION_DETAIL_INTRUSIVE_STACK

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <cassert>
#include <utility>
#endif

namespace beman::execution::detail {

template <auto Next>
class intrusive_stack;

//! @brief  This data structure is an intrusive stack that is not thread-safe.
template <class Item, Item* Item::* Next>
class intrusive_stack<Next> {
  public:
    intrusive_stack() = default;

    explicit intrusive_stack(Item* head) noexcept : head_{head} {}

    //! @brief  Pushes an item to the stack.
    auto push(Item* item) noexcept -> void { item->*Next = std::exchange(head_, item); }

    //! @brief  Pops one item from the stack.
    //!
    //! @return  The item that was popped from the stack, or nullptr if the stack is empty.
    auto pop() noexcept -> Item* {
        if (head_) {
            auto item = head_;
            head_     = std::exchange(item->*Next, nullptr);
            return item;
        }
        return nullptr;
    }

    //! @brief  Tests if the stack is empty.
    auto empty() const noexcept -> bool { return !head_; }

  private:
    Item* head_{nullptr};
};

} // namespace beman::execution::detail

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_INTRUSIVE_STACK
