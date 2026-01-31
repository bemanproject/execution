// include/beman/execution/detail/get_completion_scheduler.hpp      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SCHEDULER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SCHEDULER

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/completion_tag.hpp>
#include <beman/execution/detail/decayed_same_as.hpp>
#include <beman/execution/detail/forwarding_query.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/almost_scheduler.hpp>
#include <concepts>
#include <type_traits>
#include <utility>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT template <typename Tag>
struct get_completion_scheduler_t;

template <typename Tag>
struct get_completion_scheduler_t : ::beman::execution::forwarding_query_t {
    template <typename Env>
        requires(not requires(const get_completion_scheduler_t& self, const ::std::remove_cvref_t<Env>& env) {
                    env.query(self);
                })
    auto operator()(Env&&) const noexcept =
        BEMAN_EXECUTION_DELETE("The environment needs a query(get_completion_scheduler_t) member");

    template <typename Env>
        requires(requires(const get_completion_scheduler_t& self, const ::std::remove_cvref_t<Env>& env) {
                    env.query(self);
                } && (not requires(const get_completion_scheduler_t& self, const ::std::remove_cvref_t<Env>& env) {
                     { env.query(self) } noexcept;
                 }))
    auto operator()(Env&&) const noexcept =
        BEMAN_EXECUTION_DELETE("The environment's query(get_completion_scheduler_t) has to be noexcept");

    template <typename Env>
        requires(
                    requires(const get_completion_scheduler_t& self, const ::std::remove_cvref_t<Env>& env) {
                        env.query(self);
                    } &&
                    requires(const get_completion_scheduler_t& self, const ::std::remove_cvref_t<Env>& env) {
                        { env.query(self) } noexcept;
                    } &&
                    (not requires(const get_completion_scheduler_t&                                  self,
                                  const get_completion_scheduler_t<::beman::execution::set_value_t>& value_self,
                                  const ::std::remove_cvref_t<Env>&                                  env) {
                        { env.query(self) } noexcept -> ::beman::execution::detail::almost_scheduler;
                        {
                            ::beman::execution::get_env(::beman::execution::schedule(env.query(self)))
                                .query(value_self)
                        } -> ::beman::execution::detail::decayed_same_as<decltype(env.query(self))>;
                    }))
    auto operator()(Env&&) const noexcept =
        BEMAN_EXECUTION_DELETE("The environment's query(get_completion_scheduler_t) has to return a scheduler");

    template <typename Env>
        requires requires(const get_completion_scheduler_t&                                  self,
                          const get_completion_scheduler_t<::beman::execution::set_value_t>& value_self,
                          const ::std::remove_cvref_t<Env>&                                  env) {
            { env.query(self) } noexcept -> ::beman::execution::detail::almost_scheduler;
            {
                ::beman::execution::get_env(::beman::execution::schedule(env.query(self))).query(value_self)
            } -> ::beman::execution::detail::decayed_same_as<decltype(env.query(self))>;
        }
    auto operator()(Env&& env) const noexcept {
        return ::std::as_const(env).query(*this);
    }
};

BEMAN_EXECUTION_EXPORT template <::beman::execution::detail::completion_tag Tag>
inline constexpr get_completion_scheduler_t<Tag> get_completion_scheduler{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SCHEDULER
