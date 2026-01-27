// include/beman/execution/detail/completion_domain.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_DOMAIN
#define INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_DOMAIN

#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/sender.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct completion_domain_undefined {};
template <typename, typename>
struct completion_domain_merge {};
template <typename T>
struct completion_domain_merge<T, T> {
    using type = T;
};
template <typename T>
struct completion_domain_merge<completion_domain_undefined, T> {
    using type = T;
};
template <typename T>
struct completion_domain_merge<T, completion_domain_undefined> {
    using type = T;
};
template <>
struct completion_domain_merge<completion_domain_undefined, completion_domain_undefined> {
    using type = completion_domain_undefined;
};

/*!
 * \brief Get a sender's completion domain or the specified default.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename Default = ::beman::execution::default_domain, typename Sender>
constexpr auto completion_domain(const Sender& sender) noexcept {

    static_assert(::beman::execution::sender<Sender>);
    constexpr auto get = []<typename Tag>(Tag, const Sender& sender) {
        if constexpr (requires {
                          ::beman::execution::get_domain(
                              ::beman::execution::get_completion_scheduler<Tag>(::beman::execution::get_env(sender)));
                      }) {
            return ::beman::execution::get_domain(
                ::beman::execution::get_completion_scheduler<Tag>(::beman::execution::get_env(sender)));
        } else {
            return completion_domain_undefined{};
        }
    };

    using type = typename completion_domain_merge<
        typename completion_domain_merge<decltype(get(::beman::execution::set_error, sender)),
                                         decltype(get(::beman::execution::set_stopped, sender))>::type,
        decltype(get(::beman::execution::set_value, sender))>::type;
    return ::std::conditional_t< ::std::same_as<type, completion_domain_undefined>, Default, type>();
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_DOMAIN
