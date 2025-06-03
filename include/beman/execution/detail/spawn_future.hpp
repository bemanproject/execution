// include/beman/execution/detail/spawn_future.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_SPAWN_FUTURE
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_SPAWN_FUTURE

#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/async_scope_token.hpp>
#include <beman/execution/detail/empty_env.hpp>
#include <beman/execution/detail/queryable.hpp>
#include <beman/execution/detail/as_tuple.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
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

class spawn_future_t {
  public:
    template <::beman::execution::sender Sndr, ::beman::execution::async_scope_token Tok, typename Env>
        requires ::beman::execution::detail::queryable<::std::remove_cvref_t<Env>>
    auto operator()(Sndr&&, Tok&&, Env&&) const {}
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
