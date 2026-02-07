// include/beman/execution/detail/counting_scope_join.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE_JOIN
#define INCLUDED_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE_JOIN

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.basic_sender import beman.execution.detail.completion_signatures import beman.execution.detail.connect import beman.execution.detail.counting_scope_base import beman.execution.detail.default_impls import beman.execution.detail.get_env import beman.execution.detail.get_scheduler import beman.execution.detail.impls_for import beman.execution.detail.make_sender import beman.execution.detail.receiver import beman.execution.detail.schedule import beman.execution.detail.set_value import beman.execution.detail.start
#else
#include <beman/execution/detail/basic_sender.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/counting_scope_base.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/receiver.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/start.hpp>
#endif

    // ----------------------------------------------------------------------------

    namespace beman::execution::detail { struct counting_scope_join_t { template <::beman::execution::receiver> struct state;

auto operator()(::beman::execution::detail::counting_scope_base* ptr) const {
    return ::beman::execution::detail::make_sender(*this, ptr);
}
};
inline constexpr counting_scope_join_t counting_scope_join{};

template <typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::counting_scope_join_t,
                                             ::beman::execution::detail::counting_scope_base*>,
    Env> {
    using type = ::beman::execution::completion_signatures<::beman::execution::set_value_t()>;
};

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

template <::beman::execution::receiver Receiver>
struct beman::execution::detail::counting_scope_join_t::state : ::beman::execution::detail::counting_scope_base::node {
    using op_t = decltype(::beman::execution::connect(::beman::execution::schedule(::beman::execution::get_scheduler(
                                                          ::beman::execution::get_env(::std::declval<Receiver&>()))),
                                                      ::std::declval<Receiver&>()));

    ::beman::execution::detail::counting_scope_base* scope;
    explicit state(::beman::execution::detail::counting_scope_base* s, Receiver& r)
        : scope(s),
          receiver(r),
          op(::beman::execution::connect(::beman::execution::schedule(::beman::execution::get_scheduler(
                                             ::beman::execution::get_env(this->receiver))),
                                         this->receiver)) {}
    virtual ~state() = default;

    auto complete() noexcept -> void override { ::beman::execution::start(this->op); }
    auto complete_inline() noexcept -> void override { ::beman::execution::set_value(::std::move(this->receiver)); }

    auto start() noexcept -> void { this->scope->start_node(this); }

    ::std::remove_cvref_t<Receiver>& receiver;
    op_t                             op;
};

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <>
struct impls_for<::beman::execution::detail::counting_scope_join_t> : ::beman::execution::detail::default_impls {
    struct get_state_impl {

        template <typename Receiver>
        auto operator()(auto&& sender, Receiver& receiver) const noexcept(false) {
            auto [_, self] = sender;
            return ::beman::execution::detail::counting_scope_join_t::state<Receiver>(self, receiver);
        }
    };
    static constexpr auto get_state{get_state_impl{}};
    struct start_impl {
        auto operator()(auto& s, auto&) const noexcept { s.start(); }
    };
    static constexpr auto start{start_impl{}};
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_COUNTING_SCOPE_JOIN
