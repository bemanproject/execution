// include/beman/execution/detail/transform_sender.hpp              _*_C++_*_
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TRANSFORM_SENDER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TRANSFORM_SENDER

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.compl_domain;
import beman.execution.detail.decayed_same_as;
import beman.execution.detail.default_domain;
import beman.execution.detail.get_domain;
import beman.execution.detail.sender;
import beman.execution.detail.set_value;
import beman.execution.detail.start;
#else
#include <beman/execution/detail/compl_domain.hpp>
#include <beman/execution/detail/decayed_same_as.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/start.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

template <typename Domain, typename Tag>
struct transform_sndr_recurse {
    template <typename Sndr, typename Env>
    using domain_t = ::std::conditional_t<requires {
        ::std::declval<Domain>().transform_sender(
            ::std::declval<Tag>(), ::std::declval<Sndr>(), ::std::declval<const Env&>());
    }, Domain, ::beman::execution::default_domain>;

    template <typename Sndr, typename Env>
    using new_sndr_t =
        decltype(domain_t<Sndr, Env>().transform_sender(Tag(), ::std::declval<Sndr>(), ::std::declval<const Env&>()));

    template <typename Sndr, typename Env>
    static constexpr bool is_nothrow_new_sndr =
        noexcept(domain_t<Sndr, Env>().transform_sender(Tag(), ::std::declval<Sndr>(), ::std::declval<const Env&>()));

    template <typename Sndr, typename Env>
    using next_domain_t =
        ::std::conditional_t<::std::same_as<Tag, ::beman::execution::start_t>,
                             ::std::decay_t<::std::invoke_result_t<::beman::execution::get_domain_t, Env>>,
                             ::beman::execution::detail::compl_domain_of_t<void, new_sndr_t<Sndr, Env>, Env>>;

    constexpr transform_sndr_recurse(Domain, Tag) noexcept {}

    template <typename Sndr, typename Env>
        requires ::beman::execution::detail::decayed_same_as<new_sndr_t<Sndr, Env>, Sndr>
    auto operator()(Sndr&& sndr, const Env& env) const noexcept(is_nothrow_new_sndr<Sndr, Env>) -> decltype(auto) {
        return domain_t<Sndr, Env>().transform_sender(Tag(), ::std::forward<Sndr>(sndr), env);
    }

    template <typename Sndr, typename Env>
    auto operator()(Sndr&& sndr, const Env& env) const
        noexcept(is_nothrow_new_sndr<Sndr, Env> &&
                 ::std::is_nothrow_invocable_v<
                     ::beman::execution::detail::transform_sndr_recurse<next_domain_t<Sndr, Env>, Tag>,
                     new_sndr_t<Sndr, Env>,
                     Env>) {
        const auto next_transform =
            ::beman::execution::detail::transform_sndr_recurse{next_domain_t<Sndr, Env>(), Tag()};
        return next_transform(domain_t<Sndr, Env>().transform_sender(Tag(), ::std::forward<Sndr>(sndr), env), env);
    }
};

template <typename Sndr, typename Env>
struct is_nothrow_trans_sndr {
    using start_domain_t = ::std::decay_t<::std::invoke_result_t<::beman::execution::get_domain_t, Env>>;
    using compl_domain_t = ::beman::execution::detail::compl_domain_of_t<void, Sndr, Env>;
    using complete_t =
        ::beman::execution::detail::transform_sndr_recurse<compl_domain_t, ::beman::execution::set_value_t>;
    using starting_t = ::beman::execution::detail::transform_sndr_recurse<start_domain_t, ::beman::execution::start_t>;
    using complete_result       = ::std::invoke_result_t<const complete_t&, Sndr, const Env&>;
    using result                = ::std::invoke_result_t<const starting_t&, complete_result, const Env&>;
    static constexpr bool value = ::std::is_nothrow_invocable_v<const complete_t&, Sndr, const Env&> &&
                                  ::std::is_nothrow_invocable_v<const starting_t&, complete_result, const Env&> &&
                                  ::std::is_nothrow_constructible_v<::std::decay_t<result>, result>;
};
} // namespace beman::execution::detail

namespace beman::execution {
template <::beman::execution::sender Sndr, typename Env>
auto transform_sender(Sndr&&     sndr,
                      const Env& env) noexcept(::beman::execution::detail::is_nothrow_trans_sndr<Sndr, Env>::value)
    -> ::beman::execution::sender auto {
    const auto starting_domain   = ::beman::execution::get_domain(env);
    const auto completion_domain = ::beman::execution::detail::compl_domain(sndr, env);
    const auto starting_transform =
        ::beman::execution::detail::transform_sndr_recurse(starting_domain, ::beman::execution::start);
    const auto complete_transform =
        ::beman::execution::detail::transform_sndr_recurse(completion_domain, ::beman::execution::set_value);
    return starting_transform(complete_transform(::std::forward<Sndr>(sndr), env), env);
}
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_TRANSFORM_SENDER
