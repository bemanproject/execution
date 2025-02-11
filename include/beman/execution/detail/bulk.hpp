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

template <typename T>
concept function_constraint = std::is_function_v<T>;

template <typename T, typename... Args>
struct function_type;

template <typename T, typename... Args>
using function_type_t = typename function_type<T, Args...>::type;

template <typename>
struct function_type_basis {};

template <typename T, typename...>
struct function_type : function_type_basis<std::remove_cv_t<T>> {};

template <function_constraint T>
struct function_type_basis<T> {
    using type = T;
};

template <function_constraint T>
struct function_type_basis<T*> {
    using type = T;
};

template <function_constraint T>
struct function_type_basis<T&> {
    using type = T;
};

template <function_constraint T>
struct function_type_basis<T&&> {
    using type = T;
};

template <function_constraint T, typename C>
struct function_type_basis<T C::*> {
    using type = T;
};

template <typename, typename...>
struct function_object_type {};

template <typename C, typename... Args>
    requires(bool(&C::operator()))
struct function_object_type<C, Args...> : function_type_basis<decltype(&C::operator())> {};

template <typename F, typename... Args>
    requires std::is_class<std::remove_cvref_t<F>>::value
struct function_type<F, Args...> : function_object_type<F, Args...> {};

template <typename>
struct function_traits;

// Specialization for function types
template <typename R, typename... Args, bool NoExcept>
struct function_traits<R(Args...) noexcept(NoExcept)> {
    // The return type
    using return_type = R;
    // A tuple containing all argument types
    using argument_types = std::tuple<Args...>;

    // The number of arguments
    static constexpr std::size_t arity = sizeof...(Args);
};

template <typename R, typename... Args, bool NoExcept>
struct function_traits<R(Args...) const noexcept(NoExcept)> {
    // The return type
    using return_type = R;
    // A tuple containing all argument types
    using argument_types = std::tuple<Args...>;
    // The number of arguments
    static constexpr std::size_t arity = sizeof...(Args);
};

template <typename F, typename FunctionType>
struct is_invocable_with; 

template <typename F, typename... Args>
struct is_invocable_with<F, std::tuple<Args...>> {
    static constexpr bool is_invocable = std::is_invocable_v<F, Args...>;
    static constexpr bool is_no_throw  = ::beman::execution::detail::nothrow_callable<F, Args...>;
};

template <typename, typename, typename>
struct fixed_completions_helper;

template <typename F, typename Shape, typename... Args>
struct fixed_completions_helper<F, Shape, completion_signatures<Args...>> {
    // get function signature
    using signature = function_type_t<F>;
    // break apart return type and args types
    using signature_helper = function_traits<signature>;
    // get arg types
    using function_args_type = signature_helper::argument_types;
    // print_type<function_args_type> debug;
    // check if function is well form
    static_assert(is_invocable_with<F, function_args_type>::is_invocable,
                  "Error: The function is not callable with the given arguments.");
    using type = std::conditional_t<is_invocable_with<F, function_args_type>::is_no_throw,
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
    // print_type<F> debug;
};

} // namespace beman::execution::detail

#include <beman/execution/detail/suppress_pop.hpp>

namespace beman::execution {

using ::beman::execution::detail::bulk_t;
inline constexpr ::beman::execution::bulk_t bulk{};

} // namespace beman::execution

#endif
