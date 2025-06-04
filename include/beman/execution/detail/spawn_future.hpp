// include/beman/execution/detail/spawn_future.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_SPAWN_FUTURE
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_SPAWN_FUTURE

#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/async_scope_token.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/empty_env.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/as_tuple.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/prop.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/stop_when.hpp>
#include <beman/execution/detail/write_env.hpp>
#include <beman/execution/detail/inplace_stop_source.hpp>
#include <mutex>
#include <memory>
#include <utility>
#include <variant>
#include <type_traits>
#include <tuple>
#include <exception>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename>
struct non_throwing_args_copy;
template <typename Rc, typename... A>
struct non_throwing_args_copy<Rc(A...)> {
    static constexpr bool value = (true && ... && ::std::is_nothrow_constructible_v<::std::decay_t<A>, A>);
};
template <typename S>
inline constexpr bool non_throwing_args_copy_v{non_throwing_args_copy<S>::value};

template <typename Completions>
struct spawn_future_state_base;
template <typename... Sigs>
struct spawn_future_state_base<::beman::execution::completion_signatures<Sigs...>> {
    static constexpr bool has_non_throwing_args_copy = (true && ... && non_throwing_args_copy_v<Sigs>);
    using variant_t                                  = ::beman::execution::detail::meta::unique<
                                         ::std::conditional_t<has_non_throwing_args_copy,
                                                              ::std::variant<::std::monostate, ::beman::execution::detail::as_tuple_t<Sigs>...>,
                                                              ::std::variant<::std::monostate,
                                                                             ::std::tuple<::beman::execution::set_error_t, ::std::exception_ptr>,
                                                                             ::beman::execution::detail::as_tuple_t<Sigs>...>>>;

    variant_t result{};
    virtual ~spawn_future_state_base()       = default;
    virtual auto complete() noexcept -> void = 0;
};

template <typename Completions>
struct spawn_future_receiver {
    using receiver_concept = ::beman::execution::receiver_t;
    using state_t          = ::beman::execution::detail::spawn_future_state_base<Completions>;

    state_t* state{};

    template <typename... A>
    auto set_value(A&&... a) && noexcept -> void {
        this->set_complete<::beman::execution::set_value_t>(::std::forward<A>(a)...);
    }
    template <typename E>
    auto set_error(E&& e) && noexcept -> void {
        this->set_complete<::beman::execution::set_error_t>(::std::forward<E>(e));
    }
    auto set_stopped() && noexcept -> void { this->set_complete<::beman::execution::set_stopped_t>(); }

    template <typename Tag, typename... T>
    auto set_complete(T&&... t) noexcept {
        try {
            this->state->result.template emplace<::beman::execution::detail::decayed_tuple<Tag, T...>>(
                Tag(), ::std::forward<T>(t)...);
        } catch (...) {
            if constexpr (!state_t::has_non_throwing_args_copy) {
                this->state->result
                    .template emplace<::std::tuple<::beman::execution::set_error_t, ::std::exception_ptr>>(
                        ::beman::execution::set_error_t{}, ::std::current_exception());
            }
        }
        this->state->complete();
    }
};

template <::beman::execution::sender Sndr, typename Env>
using future_spawned_sender = decltype(::beman::execution::write_env(
    ::beman::execution::detail::stop_when(::std::declval<Sndr>(),
                                          ::std::declval<::beman::execution::inplace_stop_token>()),
    ::std::declval<Env>()));

template <typename Allocator,
          ::beman::execution::async_scope_token Token,
          ::beman::execution::sender            Sndr,
          typename Env>
struct spawn_future_state
    : ::beman::execution::detail::spawn_future_state_base<::beman::execution::completion_signatures_of_t<
          ::beman::execution::detail::future_spawned_sender<Sndr, Env>>> {
    using alloc_t  = typename ::std::allocator_traits<Allocator>::template rebind_alloc<spawn_future_state>;
    using traits_t = ::std::allocator_traits<alloc_t>;

    spawn_future_state(auto a, auto&&, Token tok, auto&&...) : token(tok), alloc(a) { /*-dk:TODO*/ }
    auto complete() noexcept -> void override { /*-dk:TODO*/ }
    auto abandon() noexcept -> void {
        /*-dk:TODO*/
        this->destroy();
    }
    auto destroy() noexcept -> void {
        Token tok{this->token};
        bool  assoc{this->associated};
        {
            alloc_t a{this->alloc};
            traits_t::destroy(a, this);
            traits_t::deallocate(a, this, 1u);
        }
        if (assoc) {
            tok.disassociate();
        }
    }

    ::std::mutex cerberos{};
    Token        token;
    bool         associated{false};
    alloc_t      alloc;
};

template <::beman::execution::sender Sndr, typename Ev>
auto spawn_future_get_allocator(const Sndr& sndr, const Ev& ev) {
    if constexpr (requires { ::beman::execution::get_allocator(ev); }) {
        return ::std::pair(::beman::execution::get_allocator(ev), ev);
    } else if constexpr (requires { ::beman::execution::get_allocator(::beman::execution::get_env(sndr)); }) {
        auto alloc{::beman::execution::get_allocator(::beman::execution::get_env(sndr))};
        return ::std::pair(alloc,
                           ::beman::execution::detail::join_env(
                               ::beman::execution::prop(::beman::execution::get_allocator, alloc), ev));
    } else {
        return ::std::pair(::std::allocator<void>{}, ev);
    }
}

class spawn_future_t {
  public:
    template <::beman::execution::sender Sndr, ::beman::execution::async_scope_token Tok, typename Ev>
        requires ::beman::execution::detail::queryable<::std::remove_cvref_t<Ev>>
    auto operator()(Sndr&& sndr, Tok&& tok, Ev&& ev) const {
        auto make{[&] -> decltype(auto) { //-dk:TODO while decltype(auto) instead of auto?
            return tok.wrap(::std::forward<Sndr>(sndr));
        }};
        using sndr_t = decltype(make());
        static_assert(::beman::execution::sender<Sndr>);

        auto [alloc, senv] = spawn_future_get_allocator(sndr, ev);
        using state_t = ::beman::execution::detail::spawn_future_state<decltype(alloc), Tok, sndr_t, decltype(senv)>;
        using state_alloc_t  = typename ::std::allocator_traits<decltype(alloc)>::template rebind_alloc<state_t>;
        using state_traits_t = ::std::allocator_traits<state_alloc_t>;
        state_alloc_t state_alloc(alloc);
        using deleter = decltype([](state_t* p) noexcept {
            if (p)
                p->abandon();
        });
        state_t* op{state_traits_t::allocate(state_alloc, 1u)};
        try {
            state_traits_t::construct(state_alloc, op, alloc, make(), tok, senv);
        } catch (...) {
            state_traits_t::deallocate(state_alloc, op, 1u);
            throw;
        }

        return ::beman::execution::detail::make_sender(*this, ::std::unique_ptr<state_t, deleter>{op});
    }
    template <::beman::execution::sender Sndr, ::beman::execution::async_scope_token Tok>
    auto operator()(Sndr&& sndr, Tok&& tok) const {
        return (*this)(::std::forward<Sndr>(sndr), ::std::forward<Tok>(tok), ::beman::execution::empty_env{});
    }
};
} // namespace beman::execution::detail

namespace beman::execution {
using spawn_future_t = ::beman::execution::detail::spawn_future_t;
inline constexpr spawn_future_t spawn_future{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
