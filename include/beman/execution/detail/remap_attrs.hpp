// include/beman/execution/detail/remap_attrs.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_REMAP_ATTRS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_REMAP_ATTRS

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.common_domain;
import beman.execution.detail.completion_signatures_of_t;
import beman.execution.detail.env_of_t;
import beman.execution.detail.get_completion_domain;
import beman.execution.detail.get_completion_scheduler;
import beman.execution.detail.get_env;
import beman.execution.detail.hide_completion_queries;
import beman.execution.detail.indeterminate_domain;
import beman.execution.detail.meta.filter;
import beman.execution.detail.sender_in;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
import beman.execution.detail.try_query;
import beman.execution.detail.type_list;
#else
#include <beman/execution/detail/common_domain.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/get_completion_domain.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/hide_completion_queries.hpp>
#include <beman/execution/detail/indeterminate_domain.hpp>
#include <beman/execution/detail/meta_filter.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/try_query.hpp>
#include <beman/execution/detail/type_list.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief A rule making <code>Source</code> a source of <code>Output</code> for every child.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \details
 * A rule tells where the completions of an adaptor come from. It states that a child completion
 * with tag <code>source</code> may make the adaptor complete with tag <code>output</code>, i.e., that
 * <code>source</code> is one of the sources of <code>output</code>. A rule provides:
 * - <code>source</code>: the tag of the child completion.
 * - <code>output</code>: the tag the adaptor may complete with in response.
 * - <code>applies<Child, Signatures, Env...>()</code>: whether this is actually possible for a given child.
 *
 * <code>redirect_completion</code> is the rule which applies whatever the child's signatures are. It does
 * not state that the adaptor completes with <code>Output</code> whenever the child completes with <code>Source</code>.
 *
 * \tparam Source The tag of the child completion.
 * \tparam Output The tag the adaptor may complete with in response.
 * \tparam KeepsSource Whether <code>Source</code> also remains a source of itself, i.e., whether the adaptor
 *     may still complete with <code>Source</code> in response to a child completion with tag <code>Source</code>.
 */
template <typename Source, typename Output, bool KeepsSource>
struct redirect_completion {
    using source = Source;
    using output = Output;

    template <typename...>
    static consteval auto applies() noexcept -> bool {
        return true;
    }
};
} // namespace beman::execution::detail

namespace beman::execution::detail::remapped {
/*!
 * \brief Whether <code>Source</code> remains a source of itself in the presence of <code>Rule</code>.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 */
template <typename Rule, typename Source>
inline constexpr bool rule_keeps_source = true;
template <typename Source, typename Output>
inline constexpr bool
    rule_keeps_source<::beman::execution::detail::redirect_completion<Source, Output, false>, Source> = false;

/*!
 * \brief Whether <code>Rule</code> is about <code>Source</code> being a source of <code>Output</code>.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 */
template <typename Rule, typename Source, typename Output>
concept rule_matches = ::std::same_as<Source, typename Rule::source> && ::std::same_as<Output, typename Rule::output>;

/*!
 * \brief Whether <code>Rule</code> makes <code>Source</code> a source of <code>Output</code> for <code>Child</code>.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \details
 * <code>Rule::applies</code> is only instantiated for a matching rule.
 *
 * \tparam Signatures The completion signatures of <code>Child</code> in the environments <code>Env...</code>.
 */
template <typename Rule, typename Source, typename Output, typename Child, typename Signatures, typename... Env>
consteval auto rule_applies() noexcept -> bool {
    if constexpr (::beman::execution::detail::remapped::rule_matches<Rule, Source, Output>) {
        return Rule::template applies<Child, Signatures, Env...>();
    } else {
        return false;
    }
}
} // namespace beman::execution::detail::remapped

// ----------------------------------------------------------------------------

namespace beman::execution::detail::remapped {
template <typename Child, typename... Rules>
struct attrs;

/*!
 * \brief The child completion tag whose scheduler <code>Attrs</code> reports for <code>Tag</code>.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \details
 * This is <code>Tag</code> itself unless <code>Attrs</code> is a <code>remapped::attrs</code>: that answers the query
 * by asking its child, possibly using a different tag, or not at all, in which case the origin is <code>void</code>.
 */
template <typename Attrs, typename Tag, typename... Env>
struct scheduler_origin : ::std::type_identity<Tag> {};

template <typename Child, typename... Rules, typename Tag, typename... Env>
struct scheduler_origin<::beman::execution::detail::remapped::attrs<Child, Rules...>, Tag, Env...>
    : ::std::type_identity<typename ::beman::execution::detail::remapped::attrs<Child, Rules...>::
                               template scheduler_origin<Tag, Env...>> {};

template <typename Attrs, typename Tag, typename... Env>
using scheduler_origin_t = typename ::beman::execution::detail::remapped::scheduler_origin<Attrs, Tag, Env...>::type;

/*!
 * \brief The source to ask <code>Attrs</code> for the completion scheduler.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \details
 * The type is <code>void</code> unless all sources have the same, known <code>scheduler_origin</code>.
 *
 * \tparam Sources A <code>type_list</code> of the completion tags which are the sources of the output.
 */
template <typename Attrs, typename Sources, typename... Env>
struct scheduler_source : ::std::type_identity<void> {};

template <typename Attrs, typename Source, typename... Other, typename... Env>
struct scheduler_source<Attrs, ::beman::execution::detail::type_list<Source, Other...>, Env...>
    : ::std::conditional<
          !::std::is_void_v<::beman::execution::detail::remapped::scheduler_origin_t<Attrs, Source, Env...>> &&
              (::std::same_as<::beman::execution::detail::remapped::scheduler_origin_t<Attrs, Source, Env...>,
                              ::beman::execution::detail::remapped::scheduler_origin_t<Attrs, Other, Env...>> &&
               ...),
          Source,
          void> {};

/*!
 * \brief The completion domain <code>attrs</code> reports for the completion tag <code>Source</code>.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \details
 * Without an environment a missing domain is reported as <code>no_completion_domain</code>. With an
 * environment the domain of the source's completion scheduler is used instead or else
 * <code>indeterminate_domain<></code>.
 *
 * \return A default constructed object of the domain's type.
 */
template <typename Source, typename Attrs, typename... Env>
auto source_domain(const Attrs& attrs, const Env&... env) noexcept {
    if constexpr (requires { ::beman::execution::get_completion_domain<Source>(attrs, env...); }) {
        return ::beman::execution::get_completion_domain<Source>(attrs, env...);
    } else if constexpr (sizeof...(Env) == 0) {
        return ::beman::execution::detail::no_completion_domain{};
    } else if constexpr (requires {
                             ::beman::execution::get_completion_domain<::beman::execution::set_value_t>(
                                 ::beman::execution::get_completion_scheduler<Source>(attrs, env...), env...);
                         }) {
        return ::beman::execution::get_completion_domain<::beman::execution::set_value_t>(
            ::beman::execution::get_completion_scheduler<Source>(attrs, env...), env...);
    } else {
        return ::beman::execution::indeterminate_domain<>{};
    }
}

/*!
 * \brief The common completion domain <code>attrs</code> reports for all <code>Source...</code>.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \return An object of the <code>common_domain</code> of the sources' domains or <code>no_completion_domain</code>
 *     if there is no source or a source has no domain.
 */
template <typename Attrs, typename... Source, typename... Env>
auto sources_domain(const Attrs& attrs, ::beman::execution::detail::type_list<Source...>, const Env&... env) noexcept {
    constexpr bool missing =
        (::std::same_as<decltype(::beman::execution::detail::remapped::source_domain<Source>(attrs, env...)),
                        ::beman::execution::detail::no_completion_domain> ||
         ...);
    if constexpr (sizeof...(Source) == 0 || missing) {
        return ::beman::execution::detail::no_completion_domain{};
    } else {
        return ::beman::execution::detail::common_domain(
            ::beman::execution::detail::remapped::source_domain<Source>(attrs, env...)...);
    }
}

/*!
 * \brief Derives a single-child adaptor's completion attributes from the child's attributes.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \details
 * <code>Rules</code> describe where the adaptor's completions come from, see
 * <code>redirect_completion</code>. A child completion keeps its tag unless it is the
 * <code>Source</code> of a <code>redirect_completion<Source, Output, false></code>.
 *
 * A query for an output tag is answered from its sources, i.e., the child completion tags which
 * can produce that output:
 * - The sources are determined from the child's completion signatures in the supplied
 *   environment. If the signatures are not available, a query without an environment assumes
 *   that every child completion may occur and a query with an environment has no sources.
 * - The completion scheduler is the child's scheduler for a source, provided all sources have
 *   the same <code>scheduler_origin</code>: equal scheduler types do not imply equal schedulers.
 * - The completion domain is the <code>common_domain</code> of the sources' domains.
 * - The child's untagged completion domain is forwarded unless a rule turns a completion with
 *   another tag into <code>set_value</code>.
 * - Other forwarding queries are passed to the child.
 *
 * \tparam Child The type of the adaptor's child sender.
 * \tparam Rules The rules describing where the adaptor's completions come from.
 */
template <typename Child, typename... Rules>
struct attrs : ::beman::execution::detail::hide_completion_queries<::beman::execution::env_of_t<Child>> {
    using base = ::beman::execution::detail::hide_completion_queries<::beman::execution::env_of_t<Child>>;
    using base::query;

  private:
    using attrs_t = ::std::remove_cvref_t<::beman::execution::env_of_t<Child>>;

    /*!
     * \brief Whether <code>Source</code> is a source of <code>Output</code> for the adaptor.
     */
    template <typename Source, typename Output, typename... Env>
    static consteval auto is_source() noexcept -> bool {
        constexpr bool kept  = ::std::same_as<Source, Output> &&
                               (::beman::execution::detail::remapped::rule_keeps_source<Rules, Source> && ...);
        constexpr bool ruled = (::beman::execution::detail::remapped::rule_matches<Rules, Source, Output> || ...);

        if constexpr (!kept && !ruled) {
            return false;
        } else if constexpr (!::beman::execution::sender_in<Child, Env...>) {
            // Before the receiver environment is known, assume every channel may occur.
            return sizeof...(Env) == 0;
        } else {
            using signatures = ::beman::execution::completion_signatures_of_t<Child, Env...>;
            if constexpr (signatures::count_of(Source{}) == 0u) {
                return false;
            } else if constexpr (kept) {
                return true;
            } else {
                return (::beman::execution::detail::remapped::
                            rule_applies<Rules, Source, Output, Child, signatures, Env...>() ||
                        ...);
            }
        }
    }

    template <typename Output, typename... Env>
    struct sources {
        template <typename Source>
        using contributes = ::std::bool_constant<is_source<Source, Output, Env...>()>;

        using type = ::beman::execution::detail::meta::filter<
            contributes,
            ::beman::execution::detail::type_list<::beman::execution::set_value_t,
                                                  ::beman::execution::set_error_t,
                                                  ::beman::execution::set_stopped_t>>;
    };

    template <typename Output, typename... Env>
    using sources_t = typename sources<Output, Env...>::type;

    template <typename Output, typename... Env>
    using scheduler_source_t = typename ::beman::execution::detail::remapped::
        scheduler_source<attrs_t, sources_t<Output, Env...>, Env...>::type;

  public:
    /*!
     * \brief The child completion tag whose scheduler is reported for <code>Output</code>.
     * \see remapped::scheduler_origin
     */
    template <typename Output, typename... Env>
    using scheduler_origin =
        ::beman::execution::detail::remapped::scheduler_origin_t<attrs_t, scheduler_source_t<Output, Env...>, Env...>;

    /*!
     * \brief The completion scheduler for <code>Output</code>: the child's scheduler for the sources' common origin.
     * \return The scheduler or <code>void</code> if the sources don't have the same, known origin.
     */
    template <typename Output, typename... Env>
    auto query(::beman::execution::get_completion_scheduler_t<Output>, const Env&... env) const noexcept {
        using source = scheduler_source_t<Output, Env...>;
        // Without a scheduler the result is void instead of the query being ill-formed: that
        // prevents try_query from retrying without the environment.
        if constexpr (!::std::is_void_v<source> &&
                      requires { ::beman::execution::get_completion_scheduler<source>(this->child_attrs, env...); }) {
            return ::beman::execution::get_completion_scheduler<source>(this->child_attrs, env...);
        }
    }

    /*!
     * \brief The completion domain for <code>Output</code>: the common domain of its sources.
     */
    template <typename Output, typename... Env>
        requires(!::std::same_as<Output, void>)
    auto query(::beman::execution::get_completion_domain_t<Output>, const Env&... env) const noexcept {
        return ::beman::execution::detail::remapped::sources_domain(
            this->child_attrs, sources_t<Output, Env...>{}, env...);
    }

    /*!
     * \brief The child's untagged completion domain, unless a rule turns another tag into <code>set_value</code>.
     */
    template <typename... Env>
        requires((::std::same_as<typename Rules::source, typename Rules::output> ||
                  !::std::same_as<typename Rules::output, ::beman::execution::set_value_t>) &&
                 ...) &&
                requires(const attrs_t& attrs, const Env&... env) {
                    ::beman::execution::detail::try_query(attrs, ::beman::execution::get_completion_domain<>, env...);
                }
    auto query(::beman::execution::get_completion_domain_t<>, const Env&... env) const noexcept {
        return ::beman::execution::detail::try_query(
            this->child_attrs, ::beman::execution::get_completion_domain<>, env...);
    }
};
} // namespace beman::execution::detail::remapped

namespace beman::execution::detail {
/*!
 * \brief Create the completion attributes of an adaptor from its child sender.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * \tparam Rules The rules describing where the adaptor's completions come from, specified explicitly.
 * \param child The adaptor's child sender.
 * \return A <code>remapped::attrs<Child, Rules...></code> holding the child's attributes.
 */
template <typename... Rules, typename Child>
auto remap_attrs(const Child& child) noexcept {
    return ::beman::execution::detail::remapped::attrs<Child, Rules...>{::beman::execution::get_env(child)};
}
} // namespace beman::execution::detail

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_REMAP_ATTRS
