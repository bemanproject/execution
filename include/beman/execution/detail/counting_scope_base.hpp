// include/beman/execution/detail/counting_scope_base.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE_BASE
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE_BASE

#include <beman/execution/detail/immovable.hpp>
#include <cstddef>
#include <exception>
#include <mutex>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct counting_scope_base;
}

// ----------------------------------------------------------------------------

class beman::execution::detail::counting_scope_base : ::beman::execution::detail::immovable {
  public:
    counting_scope_base()                      = default;
    counting_scope_base(counting_scope_base&&) = delete;
    ~counting_scope_base();

    static constexpr ::std::size_t max_associations{8194u};

    auto close() noexcept -> void;

    struct node {
        virtual auto complete() noexcept -> void        = 0;
        virtual auto complete_inline() noexcept -> void = 0;
        node*        next{};
    };
    auto start_node(node*) -> void;

  protected:
    class token {
      public:
        auto try_associate() const noexcept -> bool { return this->scope->try_associate(); }
        auto disassociate() const noexcept -> void { this->scope->disassociate(); }

      protected:
        explicit token(::beman::execution::detail::counting_scope_base* s) : scope(s) {}
        ::beman::execution::detail::counting_scope_base* scope;
    };

  private:
    enum class state_t : unsigned char {
        unused,
        open,
        open_and_joining,
        closed,
        closed_and_joining,
        unused_and_closed,
        joined
    };

    auto try_associate() noexcept -> bool;
    auto disassociate() noexcept -> void;
    auto complete() noexcept -> void;
    auto add_node(node* n, ::std::lock_guard<::std::mutex>&) noexcept -> void;

    ::std::mutex mutex;
    //-dk:TODO fuse state and count and use atomic accesses
    ::std::size_t count{};
    state_t       state{state_t::unused};
    node*         head{};
};

// ----------------------------------------------------------------------------

beman::execution::detail::counting_scope_base::~counting_scope_base() {
    ::std::lock_guard kerberos(this->mutex);
    switch (this->state) {
    default:
        ::std::terminate();
    case state_t::unused:
    case state_t::unused_and_closed:
    case state_t::joined:
        break;
    }
}

auto beman::execution::detail::counting_scope_base::close() noexcept -> void {
    switch (this->state) {
    default:
        break;
    case state_t::unused:
        this->state = state_t::unused_and_closed;
        break;
    case state_t::open:
        this->state = state_t::closed;
        break;
    case state_t::open_and_joining:
        this->state = state_t::closed_and_joining;
        break;
    }
}

auto beman::execution::detail::counting_scope_base::add_node(node* n, ::std::lock_guard<::std::mutex>&) noexcept
    -> void {
    n->next = std::exchange(this->head, n);
}

auto beman::execution::detail::counting_scope_base::try_associate() noexcept -> bool {
    ::std::lock_guard lock(this->mutex);
    switch (this->state) {
    default:
        return false;
    case state_t::unused:
        this->state = state_t::open; // fall-through!
        [[fallthrough]];
    case state_t::open:
    case state_t::open_and_joining:
        ++this->count;
        return true;
    }
}
auto beman::execution::detail::counting_scope_base::disassociate() noexcept -> void {
    {
        ::std::lock_guard lock(this->mutex);
        if (0u < --this->count)
            return;
        this->state = state_t::joined;
    }
    this->complete();
}

auto beman::execution::detail::counting_scope_base::complete() noexcept -> void {
    node* current{[this] {
        ::std::lock_guard lock(this->mutex);
        return ::std::exchange(this->head, nullptr);
    }()};
    while (current) {
        ::std::exchange(current, current->next)->complete();
    }
}

auto beman::execution::detail::counting_scope_base::start_node(node* n) -> void {
    ::std::lock_guard kerberos(this->mutex);
    switch (this->state) {
    case ::beman::execution::detail::counting_scope_base::state_t::unused:
    case ::beman::execution::detail::counting_scope_base::state_t::unused_and_closed:
    case ::beman::execution::detail::counting_scope_base::state_t::joined:
        this->state = ::beman::execution::detail::counting_scope_base::state_t::joined;
        n->complete_inline();
        return;
    case ::beman::execution::detail::counting_scope_base::state_t::open:
        this->state = ::beman::execution::detail::counting_scope_base::state_t::open_and_joining;
        break;
    case ::beman::execution::detail::counting_scope_base::state_t::open_and_joining:
        break;
    case ::beman::execution::detail::counting_scope_base::state_t::closed:
        this->state = ::beman::execution::detail::counting_scope_base::state_t::closed_and_joining;
        break;
    case ::beman::execution::detail::counting_scope_base::state_t::closed_and_joining:
        break;
    }
    this->add_node(n, kerberos);
}

// ----------------------------------------------------------------------------

#endif
