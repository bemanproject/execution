// include/beman/execution/detail/bulk.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_BULK
#define INCLUDED_BEMAN_EXECUTION_DETAIL_BULK

#include "beman/execution/detail/error_types_of_t.hpp"
#include "beman/execution/detail/meta_combine.hpp"
#include "beman/execution/detail/meta_unique.hpp"
#include "beman/execution/detail/set_stopped.hpp"
#include "beman/execution/detail/value_types_of_t.hpp"
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

template <class Shape, class f, class Sender, class Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::
        basic_sender<::beman::execution::detail::bulk_t, ::beman::execution::detail::product_type<Shape, f>, Sender>,
    Env> {

    // Creates a completion signature for set_value_t`
    template <class... Args>
    using make_value_completions =
        ::beman::execution::completion_signatures<::beman::execution::set_value_t(const std::decay_t<Args>&...)>;

    // Creates a completion signature for set_error_t
    template <class... Args>
    using make_error_completions =
        ::beman::execution::completion_signatures<::beman::execution::set_error_t(const std::decay_t<Args>&)...>;

    // Retrieves the value completion signatures from the Sender using Env, 
    // then applies `make_value_completions` to format them and merges all signatures.
    using value_completions = ::beman::execution::
        value_types_of_t<Sender, Env, make_value_completions, ::beman::execution::detail::meta::combine>;

    // Retrieves the error completion signatures from the Sender using Env, 
    // then applies make_error_completions to format them.
    using error_completions = ::beman::execution::error_types_of_t<Sender, Env, make_error_completions>;

    using fixed_completions =
        ::beman::execution::completion_signatures<::beman::execution::set_stopped_t(),
                                                  ::beman::execution::set_error_t(std::exception_ptr)>;

    
    using type = ::beman::execution::detail::meta::unique<
        ::beman::execution::detail::meta::combine<fixed_completions, value_completions, error_completions>>;
};

} // namespace beman::execution::detail

#include <beman/execution/detail/suppress_pop.hpp>

namespace beman::execution {

using ::beman::execution::detail::bulk_t;
inline constexpr ::beman::execution::bulk_t bulk{};

} // namespace beman::execution

#endif
