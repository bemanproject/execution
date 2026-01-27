// include/beman/execution/detail/get_domain.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <utility>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
struct get_domain_t {
    template <typename Object>
        requires(not requires(Object&& object, const get_domain_t& tag) {
                    ::std::forward<Object>(object).query(tag);
                }) && (not requires(Object&& object, const get_domain_t& tag) { ::std::as_const(object).query(tag); })
    auto operator()(Object&&) const noexcept = BEMAN_EXECUTION_DELETE("object needs a query(get_domain_t) overload");
    template <typename Object>
        requires(not requires(Object&& object, const get_domain_t& tag) { ::std::as_const(object).query(tag); })
    auto
    operator()(Object&&) const noexcept = BEMAN_EXECUTION_DELETE("query(get_domain_t) overload needs to be const");
    template <typename Object>
        requires(not requires(Object&& object, const get_domain_t& tag) {
                    { ::std::as_const(object).query(tag) } noexcept;
                })
    auto
    operator()(Object&&) const noexcept = BEMAN_EXECUTION_DELETE("query(get_domain_t) overload needs to be noexcept");

    template <typename Object>
    constexpr auto operator()(Object&& object) const noexcept {
        return ::std::as_const(object).query(*this);
    }
    constexpr auto query(const ::beman::execution::forwarding_query_t&) const noexcept -> bool { return true; }
};

inline constexpr get_domain_t get_domain{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_DOMAIN
