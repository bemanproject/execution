// include/beman/execution/detail/bulk.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SPLIT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SPLIT

#include "beman/execution/detail/basic_sender.hpp"
#include "beman/execution/detail/callable.hpp"
#include "beman/execution/detail/completion_signatures_for.hpp"
#include "beman/execution/detail/get_domain_early.hpp"
#include "beman/execution/detail/make_sender.hpp"
#include "beman/execution/detail/movable_value.hpp"
#include "beman/execution/detail/product_type.hpp"
#include "beman/execution/detail/sender.hpp"
#include "beman/execution/detail/set_error.hpp"
#include "beman/execution/detail/transform_sender.hpp"
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <algorithm>
#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

namespace beman::execution::detail {

struct bulk_t {

    /*

  decltype((sndr)) does not satisfy sender, or
  Shape does not satisfy integral, or
  decltype((f)) does not satisfy movable-value,
     */
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
                 ::beman::execution::detail::callable<
                     Tag,
                     Args...> /* expression f(auto(shape), args...) is well-formed. dont know if this is ok */)
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
struct completion_signatures_for<
    ::beman::execution::detail::
        basic_sender<::beman::execution::detail::bulk_t, ::beman::execution::detail::product_type<Shape, f>, Sender>,
    Env> {};

} // namespace beman::execution::detail

namespace beman::execution {

using ::beman::execution::detail::bulk_t;
inline constexpr ::beman::execution::bulk_t bulk{};

} // namespace beman::execution

#endif
