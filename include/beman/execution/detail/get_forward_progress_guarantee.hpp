// include/beman/execution/detail/get_forward_progress_guarantee.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_FORWARD_PROGRESS_GUARANTEE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_FORWARD_PROGRESS_GUARANTEE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/suppress_push.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.forwarding_query;
#else
#include <beman/execution/detail/forwarding_query.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {

enum class forward_progress_guarantee { concurrent, parallel, weakly_parallel };

struct get_forward_progress_guarantee_t {
    template <typename Object>
        requires requires(const Object& object, const get_forward_progress_guarantee_t& tag) { object.query(tag); }
    auto operator()(const Object& object) const noexcept -> forward_progress_guarantee {
        static_assert(::std::same_as<decltype(object.query(*this)), forward_progress_guarantee>);
        return object.query(*this);
    }

    template <typename Object>
    auto operator()(const Object&) const noexcept -> forward_progress_guarantee {
        return forward_progress_guarantee::weakly_parallel;
    }

    static constexpr auto query(const ::beman::execution::forwarding_query_t&) noexcept -> bool { return true; }
};

inline constexpr get_forward_progress_guarantee_t get_forward_progress_guarantee{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_FORWARD_PROGRESS_GUARANTEE
