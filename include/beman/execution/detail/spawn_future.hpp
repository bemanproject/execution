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
#include <utility>
#include <variant>
#include <type_traits>
#include <tuple>
#include <exception>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
    template <typename> struct non_throwing_args_copy;
    template <typename Rc, typename... A> struct non_throwing_args_copy<Rc(A...)> {
        static constexpr bool value = (true && ... && ::std::is_nothrow_constructible_v<::std::decay_t<A>, A> );
    };
    template <typename S> inline constexpr bool non_throwing_args_copy_v{non_throwing_args_copy<S>::value};

    template <typename Completions> struct spawn_future_state_base;
    template <typename... Sigs>
    struct spawn_future_state_base<::beman::execution::completion_signatures<Sigs...>> {
        using variant_t = ::beman::execution::detail::meta::unique<
            ::std::conditional_t<
                (true && ... && non_throwing_args_copy_v<Sigs>),
                ::std::variant<::std::monostate, ::beman::execution::detail::as_tuple_t<Sigs>...>,
                ::std::variant<::std::monostate, ::std::tuple<::beman::execution::set_error_t, ::std::exception_ptr>, ::beman::execution::detail::as_tuple_t<Sigs>...>
            >
        >;

        variant_t result{};
        virtual ~spawn_future_state_base() = default;
        virtual auto complete() noexcept -> void = 0;
    };

    class spawn_future_t {
    public:
        template <::beman::execution::sender Sndr, ::beman::execution::async_scope_token Tok, typename Env>
            requires ::beman::execution::detail::queryable<::std::remove_cvref_t<Env>>
        auto operator()(Sndr&&, Tok&&, Env&&) const {
        }
        template <::beman::execution::sender Sndr, ::beman::execution::async_scope_token Tok>
        auto operator()(Sndr&& sndr, Tok&& tok) const {
            return (*this)(::std::forward<Sndr>(sndr), ::std::forward<Tok>(tok), ::beman::execution::empty_env{});
        }
    };
}

namespace beman::execution {
    using spawn_future_t = ::beman::execution::detail::spawn_future_t;
    inline constexpr spawn_future_t spawn_future{};
}

// ----------------------------------------------------------------------------

#endif
