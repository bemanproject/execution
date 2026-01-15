// include/beman/execution/detail/read_env.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_READ_ENV
#define INCLUDED_BEMAN_EXECUTION_DETAIL_READ_ENV

#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <exception>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct read_env_t {
    auto operator()(auto&& query) const { return ::beman::execution::detail::make_sender(*this, query); }
    template <::beman::execution::sender Sender>
    static auto affine_on(Sender&& sndr, const auto&) noexcept {
        return ::std::forward<Sender>(sndr);
    }
};

template <>
struct impls_for<::beman::execution::detail::read_env_t> : ::beman::execution::detail::default_impls {
    static constexpr auto start = [](auto query, auto& receiver) noexcept -> void {
        try {
            auto env{::beman::execution::get_env(receiver)};
            ::beman::execution::set_value(::std::move(receiver), query(env));
        } catch (...) {
            ::beman::execution::set_error(::std::move(receiver), ::std::current_exception());
        }
    };
};

template <typename Query, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::read_env_t, Query>,
    Env> {
    using set_value_type =
        ::beman::execution::set_value_t(decltype(::std::declval<Query>()(::std::as_const(::std::declval<Env>()))));
    using set_error_type = ::beman::execution::set_error_t(::std::exception_ptr);
    using type           = ::std::conditional_t<noexcept(::std::declval<Query>()(::std::declval<const Env&>())),
                                                ::beman::execution::completion_signatures<set_value_type>,
                                                ::beman::execution::completion_signatures<set_value_type, set_error_type>>;
};
} // namespace beman::execution::detail

namespace beman::execution {
using read_env_t = beman::execution::detail::read_env_t;
inline constexpr read_env_t read_env{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
