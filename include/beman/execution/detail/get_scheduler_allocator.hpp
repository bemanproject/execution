// include/beman/execution/detail/get_scheduler_allocator.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_GET_SCHEDULER_ALLOCATOR
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_GET_SCHEDULER_ALLOCATOR

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/suppress_push.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.forwarding_query;
import beman.execution.detail.simple_allocator;
#else
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/simple_allocator.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct get_scheduler_allocator_t {
    template <typename Object>
        requires(requires(const Object& object, const get_scheduler_allocator_t& tag) {
            { object.query(tag) } noexcept -> ::beman::execution::detail::simple_allocator<>;
        })
    auto operator()(Object&& object) const noexcept -> decltype(auto) {
        return ::std::as_const(object).query(*this);
    }
    template <typename Object>
        requires((not requires(const Object& object, const get_scheduler_allocator_t& tag) {
                     { object.query(tag) } noexcept -> ::beman::execution::detail::simple_allocator<>;
                 }) && (requires(const Object& object, const ::beman::execution::get_allocator_t& tag) {
                     { object.query(tag) } noexcept -> ::beman::execution::detail::simple_allocator<>;
                 }))
    auto operator()(Object&& object) const noexcept -> decltype(auto) {
        return ::std::as_const(object).query(::beman::execution::get_allocator);
    }

    constexpr auto query(const ::beman::execution::forwarding_query_t&) const noexcept -> bool { return true; }
};
} // namespace beman::execution::detail

namespace beman::execution {
using get_scheduler_allocator_t = ::beman::execution::detail::get_scheduler_allocator_t;
inline constexpr get_scheduler_allocator_t get_scheduler_allocator{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

// ----------------------------------------------------------------------------

#endif
