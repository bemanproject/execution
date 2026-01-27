// include/beman/execution/detail/sched_env.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHED_ENV
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHED_ENV

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Scheduler>
class sched_env {
  private:
    Scheduler sched;

  public:
    template <typename S>
    explicit sched_env(S sch) : sched(::std::move(sch)) {}

    auto query(const ::beman::execution::get_scheduler_t&) const noexcept { return this->sched; }
    auto query(const ::beman::execution::get_domain_t& q) const noexcept {
        if constexpr (requires { this->sched.query(q); })
            return this->sched.query(q);
        else
            return ::beman::execution::default_domain{};
    }
};

template <typename Scheduler>
sched_env(Scheduler&&) -> sched_env<::std::remove_cvref_t<Scheduler>>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SCHED_ENV
