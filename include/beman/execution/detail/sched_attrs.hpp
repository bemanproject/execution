// include/beman/execution/detail/sched_attrs.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHED_ATTRS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHED_ATTRS

#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_completion_scheduler.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <concepts>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {

template <typename Scheduler>
class sched_attrs {
  private:
    Scheduler sched;

  public:
    template <typename S>
        requires(not ::std::same_as<sched_attrs, ::std::remove_cvref_t<S>>)
    explicit sched_attrs(S&& s) : sched(::std::forward<S>(s)) {}

    template <typename Tag>
    auto query(const ::beman::execution::get_completion_scheduler_t<Tag>&) const noexcept {
        return this->sched;
    }

    template <typename T = bool>
        requires requires(Scheduler&& s) { s.query(::beman::execution::get_domain); }
    auto query(const ::beman::execution::get_domain_t& q, T = true) const noexcept {
        return this->sched.query(q);
    }
};

template <typename Scheduler>
sched_attrs(Scheduler&&) -> sched_attrs<::std::remove_cvref_t<Scheduler>>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
