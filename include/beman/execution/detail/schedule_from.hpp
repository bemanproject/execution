// include/beman/execution/detail/schedule_from.hpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULE_FROM
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULE_FROM

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <exception>
#include <type_traits>
#include <tuple>
#include <utility>
#include <variant>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.as_tuple;
import beman.execution.detail.basic_sender;
import beman.execution.detail.child_type;
import beman.execution.detail.completion_signatures;
import beman.execution.detail.completion_signatures_for;
import beman.execution.detail.completion_signatures_of_t;
import beman.execution.detail.connect;
import beman.execution.detail.connect_result_t;
import beman.execution.detail.decayed_tuple;
import beman.execution.detail.default_domain;
import beman.execution.detail.default_impls;
import beman.execution.detail.env_of_t;
import beman.execution.detail.error_types_of_t;
import beman.execution.detail.fwd_env;
import beman.execution.detail.get_completion_signatures;
import beman.execution.detail.get_domain;
import beman.execution.detail.get_env;
import beman.execution.detail.impls_for;
import beman.execution.detail.join_env;
import beman.execution.detail.make_sender;
import beman.execution.detail.meta.combine;
import beman.execution.detail.meta.prepend;
import beman.execution.detail.meta.to;
import beman.execution.detail.meta.transform;
import beman.execution.detail.meta.unique;
import beman.execution.detail.query_with_default;
import beman.execution.detail.receiver;
import beman.execution.detail.sched_attrs;
import beman.execution.detail.schedule;
import beman.execution.detail.schedule_result_t;
import beman.execution.detail.scheduler;
import beman.execution.detail.sender;
import beman.execution.detail.sender_in;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.start;
import beman.execution.detail.transform_sender;
#else
#include <beman/execution/detail/as_tuple.hpp>
#include <beman/execution/detail/child_type.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <beman/execution/detail/default_domain.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/error_types_of_t.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/meta_combine.hpp>
#include <beman/execution/detail/meta_prepend.hpp>
#include <beman/execution/detail/meta_to.hpp>
#include <beman/execution/detail/meta_transform.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <beman/execution/detail/query_with_default.hpp>
#include <beman/execution/detail/sched_attrs.hpp>
#include <beman/execution/detail/schedule.hpp>
#include <beman/execution/detail/schedule_result_t.hpp>
#include <beman/execution/detail/scheduler.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/start.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct schedule_from_t {
    template <::beman::execution::scheduler Scheduler, ::beman::execution::sender Sender>
    auto operator()(Scheduler&& scheduler, Sender&& sender) const {
        auto domain{::beman::execution::detail::query_with_default(
            ::beman::execution::get_domain, scheduler, ::beman::execution::default_domain{})};
        return ::beman::execution::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(
                *this, ::std::forward<Scheduler>(scheduler), ::std::forward<Sender>(sender)));
    }

  private:
    template <typename, typename>
    struct get_signatures;
    template <typename Scheduler, typename Sender, typename Env>
    struct get_signatures<
        ::beman::execution::detail::basic_sender<::beman::execution::detail::schedule_from_t, Scheduler, Sender>,
        Env> {
        using scheduler_sender = decltype(::beman::execution::schedule(::std::declval<Scheduler>()));
        template <typename... E>
        using as_set_error = ::beman::execution::completion_signatures<::beman::execution::set_error_t(E)...>;
        using type         = ::beman::execution::detail::meta::combine<
                    decltype(::beman::execution::get_completion_signatures(::std::declval<Sender>(), ::std::declval<Env>())),
                    ::beman::execution::error_types_of_t<scheduler_sender, Env, as_set_error>,
                    ::beman::execution::completion_signatures<::beman::execution::set_error_t(
                ::std::exception_ptr)> //-dk:TODO this one should be deduced
                    >;
    };

  public:
    template <typename Sender, typename... Env>
    static consteval auto get_completion_signatures() {
        return typename get_signatures<std::remove_cvref_t<Sender>, Env...>::type{};
    }

    struct impls_for : ::beman::execution::detail::default_impls {
        template <typename State>
        struct upstream_receiver {
            using receiver_concept = ::beman::execution::receiver_t;
            State* state;

            auto set_value() && noexcept -> void {
                try {
                    ::std::visit(
                        [this]<typename Tuple>(Tuple& result) noexcept -> void {
                            if constexpr (!::std::same_as<::std::monostate, Tuple>) {
                                ::std::apply(
                                    [this](auto&& tag, auto&&... args) {
                                        tag(::std::move(this->state->receiver), ::std::move(args)...);
                                    },
                                    result);
                            }
                        },
                        state->async_result);
                } catch (...) {
                    ::beman::execution::set_error(::std::move(state->receiver), ::std::current_exception());
                }
            }

            template <typename Error>
            auto set_error(Error&& err) && noexcept -> void {
                ::beman::execution::set_error(std::move(state->receiver), std::forward<Error>(err));
            }

            auto set_stopped() && noexcept -> void { ::beman::execution::set_stopped(std::move(state->receiver)); }

            auto get_env() const noexcept -> decltype(auto) {
                return ::beman::execution::detail::fwd_env(::beman::execution::get_env(state->receiver));
            }
        };
        template <typename Receiver, typename Variant>
        struct state_base {
            Receiver receiver;
            Variant  async_result{};
        };
        template <typename Receiver, typename Scheduler, typename Variant>
        struct state_type : state_base<Receiver, Variant> {
            using receiver_t = upstream_receiver<state_base<Receiver, Variant>>;
            using operation_t =
                ::beman::execution::connect_result_t<::beman::execution::schedule_result_t<Scheduler>, receiver_t>;
            operation_t op_state;

            static constexpr bool nothrow() {
                return noexcept(::beman::execution::connect(::beman::execution::schedule(::std::declval<Scheduler>()),
                                                            receiver_t{nullptr}));
            }
            explicit state_type(Scheduler& sch, Receiver& rcvr) noexcept(nothrow())
                : state_base<Receiver, Variant>{rcvr},
                  op_state(::beman::execution::connect(::beman::execution::schedule(sch), receiver_t{this})) {}
        };

        struct get_attrs_impl {

            auto operator()(const auto& data, const auto& child) const noexcept -> decltype(auto) {
                return ::beman::execution::detail::join_env(
                    ::beman::execution::detail::sched_attrs(data),
                    ::beman::execution::detail::fwd_env(::beman::execution::get_env(child)));
            }
        };
        static constexpr auto get_attrs{get_attrs_impl{}};
        struct get_state_impl {
            template <typename Sender, typename Receiver>
                requires ::beman::execution::sender_in<::beman::execution::detail::child_type<Sender>,
                                                       ::beman::execution::env_of_t<Receiver>>
            auto operator()(Sender&& sender, Receiver& receiver) const //-dk:TODO noexcept(see below)
            {
                auto sch{sender.template get<1>()};

                using sched_t   = ::std::remove_cvref_t<decltype(sch)>;
                using variant_t = ::beman::execution::detail::meta::unique<::beman::execution::detail::meta::prepend<
                    ::std::monostate,
                    ::beman::execution::detail::meta::transform<
                        ::beman::execution::detail::as_tuple_t,
                        ::beman::execution::detail::meta::to<
                            ::std::variant,
                            ::beman::execution::detail::meta::combine<
                                ::beman::execution::completion_signatures_of_t<
                                    ::beman::execution::detail::child_type<Sender>,
                                    ::beman::execution::env_of_t<Receiver>>,
                                //-dk:TODO get proper error completion signatures
                                ::beman::execution::completion_signatures<::beman::execution::set_error_t(
                                                                              ::std::exception_ptr),
                                                                          ::beman::execution::set_stopped_t()>>>>>>;

                return state_type<Receiver, sched_t, variant_t>(sch, receiver);
            };
        };
        static constexpr auto get_state{get_state_impl{}};
        struct complete_impl {
            template <typename Tag, typename... Args>
            auto operator()(auto, auto& state, auto& receiver, Tag, Args&&... args) const noexcept -> void {
                using result_t         = ::beman::execution::detail::decayed_tuple<Tag, Args...>;
                constexpr bool nothrow = ::std::is_nothrow_constructible_v<result_t, Tag, Args...>;

                try {
                    [&]() noexcept(nothrow) {
                        state.async_result.template emplace<result_t>(Tag(), std::forward<Args>(args)...);
                    }();
                } catch (...) {
                    if constexpr (not nothrow)
                        ::beman::execution::set_error(::std::move(receiver), ::std::current_exception());
                }

                if (state.async_result.index() == 0)
                    return;

                ::beman::execution::start(state.op_state);
            }
        };
        static constexpr auto complete{complete_impl{}};
    };
};

} // namespace beman::execution::detail

namespace beman::execution {
using schedule_from_t = beman::execution::detail::schedule_from_t;
inline constexpr ::beman::execution::schedule_from_t schedule_from{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULE_FROM
