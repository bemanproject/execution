// include/beman/execution/detail/simple_counting_scope.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SIMPLE_COUNTING_SCOPE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SIMPLE_COUNTING_SCOPE

#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/notify.hpp>
#include <beman/execution/detail/immovable.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <atomic>
#include <exception>
#include <mutex>
#include <utility>
#include <cstdlib>

// ----------------------------------------------------------------------------

namespace beman::execution {
class simple_counting_scope;
}

namespace beman::execution::detail {
struct simple_counting_scope_join_t {};
struct simple_counting_scope_state_base {
    ::beman::execution::simple_counting_scope*                    scope;
    ::beman::execution::detail::simple_counting_scope_state_base* next{};
    simple_counting_scope_state_base(::beman::execution::simple_counting_scope* s) : scope(s) {}
    virtual ~simple_counting_scope_state_base() = default;
    auto start() noexcept -> void;

    virtual auto complete() noexcept -> void        = 0;
    virtual auto complete_inline() noexcept -> void = 0;
};

template <typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::simple_counting_scope_join_t,
                                             ::beman::execution::simple_counting_scope*>,
    Env> {
    using type = ::beman::execution::completion_signatures<::beman::execution::set_value_t()>;
};

template <>
struct impls_for<::beman::execution::detail::simple_counting_scope_join_t>
    : ::beman::execution::detail::default_impls {
    template <typename Receiver>
    struct state : ::beman::execution::detail::simple_counting_scope_state_base {
        ::std::remove_cvref_t<Receiver>& receiver;
        using op_t = decltype(::beman::execution::connect(
            ::beman::execution::schedule(::beman::execution::get_scheduler(::beman::execution::get_env(receiver))),
            receiver));
        op_t op;
        state(::beman::execution::simple_counting_scope* s, Receiver& r)
            : ::beman::execution::detail::simple_counting_scope_state_base(s),
              receiver(r),
              op(::beman::execution::connect(::beman::execution::schedule(::beman::execution::get_scheduler(
                                                 ::beman::execution::get_env(this->receiver))),
                                             this->receiver)) {}
        auto complete() noexcept -> void override { ::beman::execution::start(this->op); }
        auto complete_inline() noexcept -> void override {
            ::beman::execution::set_value(::std::move(this->receiver));
        }
    };

    static constexpr auto get_state = []<typename Receiver>(auto&& sender, Receiver& receiver) noexcept(false) {
        auto [_, self] = sender;
        return state<Receiver>(self, receiver);
    };
    static constexpr auto start = [](::beman::execution::detail::simple_counting_scope_state_base& s, auto&) noexcept {
        s.start();
    };
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

class beman::execution::simple_counting_scope : ::beman::execution::detail::immovable {
  private:
    friend struct ::beman::execution::detail::simple_counting_scope_state_base;
    enum class state_t : unsigned char {
        unused,
        open,
        open_and_joining,
        closed,
        closed_and_joining,
        unused_and_closed,
        joined
    };

  public:
    class token;

    simple_counting_scope()                        = default;
    simple_counting_scope(simple_counting_scope&&) = delete;
    ~simple_counting_scope() {
        switch (this->state) {
        default:
            ::std::terminate();
        case state_t::unused:
        case state_t::unused_and_closed:
        case state_t::joined:
            break;
        }
    }

    auto get_token() noexcept -> token;
    auto close() noexcept -> void {
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
    auto join() noexcept {
        return ::beman::execution::detail::make_sender(::beman::execution::detail::simple_counting_scope_join_t{},
                                                       this);
    }

  private:
    friend class token;
    auto try_associate() noexcept -> bool {
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
    auto disassociate() noexcept -> void {
        {
            ::std::lock_guard lock(this->mutex);
            if (0u < --this->count)
                return;
            this->state = state_t::joined;
        }
        this->complete();
    }
    auto complete() noexcept -> void {
        auto current{[this] {
            ::std::lock_guard lock(this->mutex);
            return ::std::exchange(this->head, nullptr);
        }()};
        while (current) {
            ::std::exchange(current, current->next)->complete();
        }
    }
    ::std::mutex                         mutex;
    ::std::size_t                        count{};
    state_t                              state{state_t::unused};
    ::beman::execution::detail::simple_counting_scope_state_base* head{};
};

// ----------------------------------------------------------------------------

class beman::execution::simple_counting_scope::token {
  public:
    template <::beman::execution::sender Sender>
    auto wrap(Sender&& sender) const noexcept -> Sender&& {
        return ::std::forward<Sender>(sender);
    }

    auto try_associate() const noexcept -> bool { return this->scope->try_associate(); }
    auto disassociate() const noexcept -> void { this->scope->disassociate(); }

  private:
    friend class beman::execution::simple_counting_scope;
    explicit token(beman::execution::simple_counting_scope* scp) noexcept : scope(scp) {}
    beman::execution::simple_counting_scope* scope;
};

// ----------------------------------------------------------------------------

inline auto beman::execution::simple_counting_scope::get_token() noexcept
    -> beman::execution::simple_counting_scope::token {
    return beman::execution::simple_counting_scope::token(this);
}

auto beman::execution::detail::simple_counting_scope_state_base::start() noexcept -> void {
    switch (this->scope->state) {
    case ::beman::execution::simple_counting_scope::state_t::unused:
    case ::beman::execution::simple_counting_scope::state_t::unused_and_closed:
    case ::beman::execution::simple_counting_scope::state_t::joined:
        this->scope->state = ::beman::execution::simple_counting_scope::state_t::joined;
        this->complete_inline();
        return;
    case ::beman::execution::simple_counting_scope::state_t::open:
        this->scope->state = ::beman::execution::simple_counting_scope::state_t::open_and_joining;
        break;
    case ::beman::execution::simple_counting_scope::state_t::open_and_joining:
        break;
    case ::beman::execution::simple_counting_scope::state_t::closed:
        this->scope->state = ::beman::execution::simple_counting_scope::state_t::closed_and_joining;
        break;
    case ::beman::execution::simple_counting_scope::state_t::closed_and_joining:
        break;
    }
    ::std::lock_guard kerberos(this->scope->mutex);
    this->next = std::exchange(this->scope->head, this);
}

// ----------------------------------------------------------------------------

#endif
