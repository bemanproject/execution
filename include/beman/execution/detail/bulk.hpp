// include/beman/execution/detail/bulk.hpp                         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_BULK
#define INCLUDED_BEMAN_EXECUTION_DETAIL_BULK

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <algorithm>
#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.basic_sender;
import beman.execution.detail.completion_signatures;
import beman.execution.detail.completion_signatures_for;
import beman.execution.detail.default_impls;
import beman.execution.detail.get_completion_signatures;
import beman.execution.detail.get_domain_early;
import beman.execution.detail.impls_for;
import beman.execution.detail.make_sender;
import beman.execution.detail.meta.combine;
import beman.execution.detail.meta.unique;
import beman.execution.detail.movable_value;
import beman.execution.detail.product_type;
import beman.execution.detail.sender;
import beman.execution.detail.sender_adaptor;
import beman.execution.detail.sender_adaptor_closure;
import beman.execution.detail.set_error;
import beman.execution.detail.set_value;
import beman.execution.detail.transform_sender;
#else
#include <beman/execution/detail/basic_sender.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/get_completion_signatures.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/meta_combine.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#endif

namespace beman::execution::detail {

template <typename, typename, typename>
struct fixed_completions_helper;

template <typename F, typename Shape, typename... Args>
struct fixed_completions_helper<F, Shape, completion_signatures<Args...>> {

    template <typename, typename>
    struct may_throw;
    template <typename XF, typename Tag, typename... XArgs>
    struct may_throw<XF, Tag(XArgs...)> {
        static constexpr bool value =
            std::same_as<Tag, ::beman::execution::set_value_t> && !::std::is_nothrow_invocable<XF, Shape, XArgs...>();
    };
    template <typename XF, typename... Sigs>
    struct may_throw<XF, completion_signatures<Sigs...>> {
        static constexpr bool value = (false || ... || may_throw<XF, Sigs>::value);
    };

    using type = std::conditional_t<!may_throw<F, Args...>::value,
                                    completion_signatures<Args...>,
                                    completion_signatures<Args..., set_error_t(std::exception_ptr)>>;
};

struct bulk_t : ::beman::execution::sender_adaptor_closure<bulk_t> {

    template <class Shape, class f>
        requires(std::is_integral_v<Shape> && ::beman::execution::detail::movable_value<f>)
    auto operator()(Shape&& shape, f&& fun) const {
        return beman::execution::detail::sender_adaptor{*this, std::forward<Shape>(shape), std::forward<f>(fun)};
    }

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

  private:
    template <typename F, typename Shape, typename Completions>
    using fixed_completions = typename fixed_completions_helper<F, Shape, Completions>::type;
    template <typename, typename>
    struct get_signatures;
    template <class Shape, class F, class Sender, class Env>
    struct get_signatures<::beman::execution::detail::basic_sender<::beman::execution::detail::bulk_t,
                                                                   ::beman::execution::detail::product_type<Shape, F>,
                                                                   Sender>,
                          Env> {
        using completions = decltype(::beman::execution::get_completion_signatures(std::declval<Sender>(), std::declval<Env>()));
        using type        = ::beman::execution::detail::meta::unique<
                   ::beman::execution::detail::meta::combine<fixed_completions<F, Shape, completions>>>;
    };

  public:
    template <typename Sender, typename... Env>
    static consteval auto get_completion_signatures() {
        return typename get_signatures<std::remove_cvref_t<Sender>, Env...>::type{};
    }
};

template <>
struct impls_for<bulk_t> : ::beman::execution::detail::default_impls {

    struct complete_impl {
        template <class Index, class Shape, class Fun, class Rcvr, class Tag, class... Args>
            requires(!::std::same_as<Tag, set_value_t> || std::is_invocable_v<Fun, Shape, Args...>)
        auto operator()(Index,
                        ::beman::execution::detail::product_type<Shape, Fun>& state,
                        Rcvr&                                                 rcvr,
                        Tag,
                        Args&&... args) const noexcept -> void {
            if constexpr (std::same_as<Tag, set_value_t>) {
                auto& [shape, f] = state;

                using s_type = std::remove_cvref_t<decltype(shape)>;

                constexpr bool nothrow = noexcept(f(s_type(shape), args...));

                try {
                    [&]() noexcept(nothrow) {
                        for (decltype(s_type(shape)) i = 0; i < shape; i++) {
                            f(s_type(i), args...);
                        }
                        Tag()(std::move(rcvr), std::forward<Args>(args)...);
                    }();

                } catch (...) {
                    if constexpr (not nothrow) {
                        ::beman::execution::set_error(std::move(rcvr), std::current_exception());
                    }
                }
            } else {
                Tag()(std::move(rcvr), std::forward<Args>(args)...);
            }
        }
    };
    static constexpr auto complete{complete_impl{}};
};

} // namespace beman::execution::detail

#include <beman/execution/detail/suppress_pop.hpp>

namespace beman::execution {

using bulk_t = ::beman::execution::detail::bulk_t;
inline constexpr ::beman::execution::bulk_t bulk{};

} // namespace beman::execution

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_BULK
