// include/beman/execution/detail/bulk.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_BULK
#define INCLUDED_BEMAN_EXECUTION_DETAIL_BULK

#include "beman/execution/detail/callable.hpp"
#include "beman/execution/detail/get_completion_signatures.hpp"
#include "beman/execution/detail/nothrow_callable.hpp"
#include <beman/execution/detail/basic_sender.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <algorithm>
#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

#include <beman/execution/detail/suppress_push.hpp>
namespace beman::execution::detail {

struct bulk_t {

    template <class Sender, class Shape, class f>
        requires(::beman::execution::sender<Sender> && std::is_integral_v<Shape> &&
                 ::beman::execution::detail::movable_value<f>)
    auto operator()(Sender&& sndr, Shape&& shape, f&& fun) const {

        auto domain{::beman::execution::detail::get_domain_early(sndr)};

        return ::beman::execution::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(
                *this, ::beman::execution::detail::product_type<Shape, f>{shape, fun}, std::forward<Sender>(sndr)));
    }
};

template <>
struct impls_for<bulk_t> : ::beman::execution::detail::default_impls {

    static constexpr auto complete = []<class Index, class State, class Rcvr, class Tag, class... Args>(
                                         Index, State& state, Rcvr& rcvr, Tag, Args&&... args) noexcept -> void
        requires(not::std::same_as<Tag, set_value_t> ||
                 requires(State& s, Args&&... a) {
                     (s.template get<1>())(s.template get<0>(), ::std::forward<Args>(a)...);
                 })
    {
        if constexpr (std::same_as<Tag, set_value_t>) {
            auto& [shape, f] = state;

            constexpr bool nothrow = noexcept(f(auto(shape), args...));

            try {
                [&]() noexcept(nothrow) {
                    for (decltype(auto(shape)) i = 0; i < shape; i++) {
                        f(auto(i), args...);
                    }
                    Tag()(std::move(rcvr), std::forward<Args>(args)...);
                }();

            } catch (...) {

                ::beman::execution::set_error(std::move(rcvr), std::current_exception());
            }
        } else {
            Tag()(std::move(rcvr), std::forward<Args>(args)...);
        }
    };
};

template <typename T>
struct print_type;

// Specialization to trigger a compile-time error and print the type
template <typename T>
struct print_type {
    static_assert(sizeof(T) == 0, "Debugging type...");
};

/*template <typename, typename, typename, typename,typename>
struct fixed_completions_helper;
template <typename F, typename Shape,typename Sender,typename Env, typename... Args>
struct fixed_completions_helper<F, Shape,Sender, Env, completion_signatures<Args...>> {
    using completions = ::beman::execution::value_types_of_t<Sender,Env>;
    static_assert(std::is_invocable_v<F,  completions>,"Error: The function is not callable with the given arguments.");
    using type = std::conditional_t<::beman::execution::detail::nothrow_callable<F, Args ...>,
                                    completion_signatures<Args...>,
                                    completion_signatures<Args..., set_error_t(std::exception_ptr)>>;
};

template <typename F, typename Shape, typename Sender , typename Env, typename completions>
using fixed_completions = typename fixed_completions_helper<F, Shape, Sender , Env, completions>::type;*/




template <typename, typename, typename>
struct fixed_completions_helper;

template <typename F, typename Shape, typename... Args>
struct fixed_completions_helper<F, Shape, completion_signatures<Args...>> {
    //static_assert(std::is_invocable_v<F, Shape, Args...>,"Error: The function is not callable with the given arguments.");
    using type = std::conditional_t<::beman::execution::detail::nothrow_callable<F, Args ...>,
                                    completion_signatures<Args...>,
                                    completion_signatures<Args..., set_error_t(std::exception_ptr)>>;
};

template <typename F, typename Shape, typename Completions>
using fixed_completions = typename fixed_completions_helper<F, Shape, Completions>::type;

template <class Shape, class F, class Sender, class Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::
        basic_sender<::beman::execution::detail::bulk_t, ::beman::execution::detail::product_type<Shape, F>, Sender>,
    Env> {
    

    using completions = decltype(get_completion_signatures(std::declval<Sender>(), std::declval<Env>()));
    using type        = fixed_completions<F, Shape, completions>;
    //print_type<type> debug;
};

} // namespace beman::execution::detail

#include <beman/execution/detail/suppress_pop.hpp>

namespace beman::execution {

using ::beman::execution::detail::bulk_t;
inline constexpr ::beman::execution::bulk_t bulk{};

} // namespace beman::execution

#endif
