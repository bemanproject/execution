// include/beman/execution/detail/stopped_as_optional.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STOPPED_AS_OPTIONAL
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STOPPED_AS_OPTIONAL

// ----------------------------------------------------------------------------

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <optional>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.child_type;
import beman.execution.detail.default_impls;
import beman.execution.detail.env;
import beman.execution.detail.forward_like;
import beman.execution.detail.fwd_env;
import beman.execution.detail.just;
import beman.execution.detail.let;
import beman.execution.detail.make_sender;
import beman.execution.detail.nested_sender_has_affine;
import beman.execution.detail.remap_attrs;
import beman.execution.detail.sender;
import beman.execution.detail.sender_adaptor_closure;
import beman.execution.detail.sender_for;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.single_sender;
import beman.execution.detail.single_sender_value_type;
import beman.execution.detail.then;
#else
#include <beman/execution/detail/child_type.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/just.hpp>
#include <beman/execution/detail/let.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/nested_sender_has_affine.hpp>
#include <beman/execution/detail/remap_attrs.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor_closure.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/single_sender.hpp>
#include <beman/execution/detail/single_sender_value_type.hpp>
#include <beman/execution/detail/then.hpp>
#endif

namespace beman::execution::detail {
struct stopped_as_optional_t : ::beman::execution::sender_adaptor_closure<stopped_as_optional_t> {
  private:
    struct value_construction_exception {
        using source = ::beman::execution::set_value_t;
        using output = ::beman::execution::set_error_t;

        template <typename Child, typename Signatures, typename Env = ::beman::execution::env<>>
        static consteval auto applies() noexcept -> bool {
            bool throwing = false;
            if constexpr (requires { typename ::beman::execution::detail::single_sender_value_type<Child, Env>; }) {
                using value_type = ::beman::execution::detail::single_sender_value_type<Child, Env>;
                Signatures::for_each([&]<typename Tag, typename... Args>(Tag (*)(Args...)) noexcept {
                    if constexpr (::std::same_as<Tag, source>) {
                        throwing |= !::std::is_nothrow_constructible_v<value_type, Args...>;
                    }
                });
            }
            return throwing;
        }
    };

  public:
    struct impls_for : ::beman::execution::detail::default_impls {
        static constexpr auto get_attrs = [](const auto&, const auto& child) noexcept {
            return ::beman::execution::detail::remap_attrs<
                ::beman::execution::detail::
                    redirect_completion<::beman::execution::set_stopped_t, ::beman::execution::set_value_t, false>,
                value_construction_exception>(child);
        };
    };

    template <::beman::execution::detail::sender_for<stopped_as_optional_t> Sndr, typename Env>
    auto transform_sender(::beman::execution::set_value_t, Sndr&& sndr, Env&&) const {
        static_assert(
            ::beman::execution::detail::single_sender<child_type<Sndr>, ::beman::execution::detail::fwd_env<Env>>,
            "sender must be a single sender");
        using value_type =
            ::beman::execution::detail::single_sender_value_type<child_type<Sndr>,
                                                                 ::beman::execution::detail::fwd_env<Env>>;
        static_assert(!::std::is_void_v<value_type>, "the value type of sender can't be empty");
        return ::beman::execution::let_stopped(
            ::beman::execution::then(
                ::std::forward<Sndr>(sndr).template get<2>(),
                []<class... Args>(Args&&... args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>) {
                    return ::std::optional<value_type>(::std::in_place, ::std::forward<Args>(args)...);
                }),
            []() noexcept { return ::beman::execution::just(::std::optional<value_type>()); });
    }

    template <::beman::execution::sender Sndr>
    auto operator()(Sndr&& sndr) const noexcept(::std::is_nothrow_constructible_v<::std::remove_cvref_t<Sndr>, Sndr>) {
        return ::beman::execution::detail::make_sender(stopped_as_optional_t{}, {}, ::std::forward<Sndr>(sndr));
    }

    template <::beman::execution::sender Sndr>
        requires ::beman::execution::detail::nested_sender_has_affine<Sndr>
    static auto affine(Sndr&& sndr) noexcept(
        noexcept(stopped_as_optional_t{}(::std::declval<::beman::execution::detail::child_type<Sndr>>().affine()))) {
        auto& [_, data, child] = sndr;
        return stopped_as_optional_t{}(::beman::execution::detail::forward_like<Sndr>(child).affine());
    }

    [[deprecated("use `sender | stopped_as_optional` instead of `sender | stopped_as_optional()`")]] auto
    operator()() const noexcept {
        return *this;
    }
};

} // namespace beman::execution::detail

namespace beman::execution {
using stopped_as_optional_t = ::beman::execution::detail::stopped_as_optional_t;
inline constexpr stopped_as_optional_t stopped_as_optional{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_STOPPED_AS_OPTIONAL
