// include/beman/execution/detail/state_rep.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_STATE_REP
#define INCLUDED_BEMAN_EXECUTION_DETAIL_STATE_REP

#include <beman/execution.hpp>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename C, typename Receiver>
struct state_rep {
    std::remove_cvref_t<Receiver> receiver;
    C                             context;
    template <typename R>
    state_rep(R&& r) : receiver(std::forward<R>(r)), context() {}
};
template <typename C, typename Receiver>
    requires requires { C(::beman::execution::get_env(std::declval<std::remove_cvref_t<Receiver>&>())); } &&
             (not requires(const Receiver& receiver) {
                 typename C::template env_type<decltype(::beman::execution::get_env(receiver))>;
             })
struct state_rep<C, Receiver> {
    std::remove_cvref_t<Receiver> receiver;
    C                             context;
    template <typename R>
    state_rep(R&& r) : receiver(std::forward<R>(r)), context(::beman::execution::get_env(this->receiver)) {}
};
template <typename C, typename Receiver>
    requires requires(const Receiver& receiver) {
        typename C::template env_type<decltype(::beman::execution::get_env(receiver))>;
    }
struct state_rep<C, Receiver> {
    using upstream_env = decltype(::beman::execution::get_env(std::declval<std::remove_cvref_t<Receiver>&>()));
    std::remove_cvref_t<Receiver>               receiver;
    typename C::template env_type<upstream_env> own_env;
    C                                           context;
    template <typename R>
    state_rep(R&& r)
        : receiver(std::forward<R>(r)), own_env(::beman::execution::get_env(this->receiver)), context(this->own_env) {}
};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
