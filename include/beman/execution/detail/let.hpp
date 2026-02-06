// include/beman/execution/detail/let.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_LET
#define INCLUDED_BEMAN_EXECUTION_DETAIL_LET

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/connect.hpp>
#include <beman/execution/detail/allocator_aware_move.hpp>
#include <beman/execution/detail/completion_signatures_for.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_adaptor.hpp>
#include <beman/execution/detail/make_sender.hpp>
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/get_domain_early.hpp>
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/impls_for.hpp>
#include <beman/execution/detail/default_impls.hpp>
#include <beman/execution/detail/forward_like.hpp>
#include <beman/execution/detail/sched_env.hpp>
#include <beman/execution/detail/make_env.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/meta_filter.hpp>
#include <beman/execution/detail/meta_transform.hpp>
#include <beman/execution/detail/meta_prepend.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <beman/execution/detail/meta_combine.hpp>
#include <beman/execution/detail/meta_to.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <beman/execution/detail/type_list.hpp>
#include <beman/execution/detail/join_env.hpp>
#include <beman/execution/detail/fwd_env.hpp>
#include <beman/execution/detail/emplace_from.hpp>
#include <concepts>
#include <exception>
#include <utility>
#include <variant>
#include <type_traits>

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_push.hpp>

namespace beman::execution::detail {
template <typename Completion>
struct let_t {
    template <::beman::execution::detail::movable_value Fun>
    auto operator()(Fun&& fun) const {
        return ::beman::execution::detail::sender_adaptor{*this, ::std::forward<Fun>(fun)};
    }
    template <::beman::execution::sender Sender, ::beman::execution::detail::movable_value Fun>
    auto operator()(Sender&& sender, Fun&& fun) const {
        auto domain(::beman::execution::detail::get_domain_early(sender));
        return ::beman::execution::detail::transform_sender(
            domain,
            ::beman::execution::detail::make_sender(*this, ::std::forward<Fun>(fun), std::forward<Sender>(sender)));
    }

    template <typename Sender>
    static auto env(Sender&& sender) {
        if constexpr (requires {
                          ::beman::execution::detail::sched_env(
                              ::beman::execution::get_completion_scheduler<Completion>(
                                  ::beman::execution::get_env(sender)));
                      })
            return ::beman::execution::detail::sched_env(
                ::beman::execution::get_completion_scheduler<Completion>(::beman::execution::get_env(sender)));
        else if constexpr (requires {
                               ::beman::execution::detail::make_env(
                                   ::beman::execution::get_domain,
                                   ::beman::execution::get_domain(::beman::execution::get_env(sender)));
                           })
            return ::beman::execution::detail::make_env(
                ::beman::execution::get_domain, ::beman::execution::get_domain(::beman::execution::get_env(sender)));
        else
            return ::beman::execution::env<>{};
    }
    template <typename Sender, typename Env>
    static auto join_env(Sender&& sender, Env&& e) -> decltype(auto) {
        return ::beman::execution::detail::join_env(env(sender), ::beman::execution::detail::fwd_env(e));
    }
};

template <typename Completion>
struct impls_for<::beman::execution::detail::let_t<Completion>> : ::beman::execution::detail::default_impls {

    template <typename Receiver, typename Env>
    struct let_receiver {
        using receiver_concept = ::beman::execution::receiver_t;

        Receiver& receiver;
        Env       env;

        auto get_env() const noexcept -> decltype(auto) {
            return ::beman::execution::detail::join_env(
                this->env, ::beman::execution::detail::fwd_env(::beman::execution::get_env(this->receiver)));
        }
        template <typename Error>
        auto set_error(Error&& error) && noexcept -> void {
            ::beman::execution::set_error(::std::move(this->receiver), ::std::forward<Error>(error));
        }
        auto set_stopped() && noexcept -> void { ::beman::execution::set_stopped(::std::move(this->receiver)); }
        template <typename... Args>
        auto set_value(Args&&... args) && noexcept -> void {
            ::beman::execution::set_value(::std::move(this->receiver), ::std::forward<Args>(args)...);
        }
    };

    template <typename>
    struct filter_pred : ::std::false_type {};
    template <typename... A>
    struct filter_pred<Completion(A...)> : ::std::true_type {};
    template <typename>
    struct to_tuple;
    template <typename C, typename... A>
    struct to_tuple<C(A...)> {
        using type = ::beman::execution::detail::decayed_tuple<A...>;
    };
    template <typename T>
    using to_tuple_t = typename to_tuple<T>::type;
    template <typename Fun, typename Receiver, typename Env>
    struct to_state {
        template <typename Tuple>
        using trans =
            decltype(::beman::execution::connect(::std::apply(::std::declval<Fun>(), ::std::declval<Tuple>()),
                                                 ::std::declval<let_receiver<Receiver, Env>>()));
    };

    static constexpr auto get_state{[]<typename Sender, typename Receiver>(Sender&& sender, Receiver&& receiver) {
        auto& fun{sender.template get<1>()};
        auto& child{sender.template get<2>()};

        using fun_t   = ::std::remove_cvref_t<decltype(fun)>;
        using child_t = ::std::remove_cvref_t<decltype(child)>;
        using env_t   = decltype(::beman::execution::detail::let_t<Completion>::env(child));
        using sigs_t = ::beman::execution::completion_signatures_of_t<child_t, ::beman::execution::env_of_t<Receiver>>;
        using comp_sigs_t = ::beman::execution::detail::meta::filter<filter_pred, sigs_t>;
        using type_list_t = ::beman::execution::detail::meta::to<::std::variant, comp_sigs_t>;
        using tuples_t    = ::beman::execution::detail::meta::transform<to_tuple_t, type_list_t>;
        using unique_t    = ::beman::execution::detail::meta::unique<tuples_t>;
        using args_t      = ::beman::execution::detail::meta::prepend<std::monostate, unique_t>;
        using ops_t       = ::beman::execution::detail::meta::prepend<
                  ::std::monostate,
                  ::beman::execution::detail::meta::unique<::beman::execution::detail::meta::transform<
                      to_state<fun_t, ::std::remove_cvref_t<Receiver>, env_t>::template trans,
                      tuples_t>>>;

        struct state_t {
            fun_t  fun;
            env_t  env;
            args_t args;
            ops_t  ops2;
        };
        return state_t{beman::execution::detail::allocator_aware_move(
                           ::beman::execution::detail::forward_like<Sender>(fun), receiver),
                       ::beman::execution::detail::let_t<Completion>::env(child),
                       {},
                       {}};
    }};
    template <typename Receiver, typename... Args>
    static auto
    let_bind(auto& state, Receiver& receiver, Args&&... args) noexcept(noexcept(::beman::execution::connect(
        ::std::apply(::std::move(state.fun),
                     ::std::move(state.args.template emplace<::beman::execution::detail::decayed_tuple<Args...>>(
                         ::std::forward<Args>(args)...))),
        let_receiver<Receiver, decltype(state.env)>{receiver, state.env}))) {
        using args_t = ::beman::execution::detail::decayed_tuple<Args...>;
        auto mkop{[&] {
            return ::beman::execution::connect(
                ::std::apply(::std::move(state.fun),
                             ::std::move(state.args.template emplace<args_t>(::std::forward<Args>(args)...))),
                let_receiver<Receiver, decltype(state.env)>{receiver, state.env});
        }};
        ::beman::execution::start(
            state.ops2.template emplace<decltype(mkop())>(beman::execution::detail::emplace_from{mkop}));
    }
    struct complete_impl {
        template <class Tag, class... Args>
        auto operator()(auto, auto& state, auto& receiver, Tag, Args&&... args) const {
            if constexpr (::std::same_as<Tag, Completion>) {
                try {
                    let_bind(state, receiver, ::std::forward<Args>(args)...);
                } catch (...) {
                    if constexpr (not noexcept(let_bind(state, receiver, ::std::forward<Args>(args)...)))
                        ::beman::execution::set_error(::std::move(receiver), ::std::current_exception());
                }
            } else {
                Tag()(::std::move(receiver), ::std::forward<Args>(args)...);
            }
        }
    };
    static constexpr auto complete{complete_impl{}};
};

template <typename Completion, typename Fun, typename Sender, typename Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::basic_sender<::beman::execution::detail::let_t<Completion>, Fun, Sender>,
    Env> {
    template <typename>
    struct other_completion : ::std::true_type {};
    template <typename... A>
    struct other_completion<Completion(A...)> : ::std::false_type {};
    template <typename>
    struct matching_completion : ::std::false_type {};
    template <typename... A>
    struct matching_completion<Completion(A...)> : ::std::true_type {};

    template <typename>
    struct apply_decayed;
    template <typename C, typename... A>
    struct apply_decayed<C(A...)> {
        using sender_type = ::beman::execution::detail::call_result_t<Fun, ::std::decay_t<A>...>;
    };
    template <typename>
    struct get_completions;
    template <template <typename...> class L, typename... C>
    struct get_completions<L<C...>> {
        using type = ::beman::execution::detail::meta::unique<
            ::beman::execution::detail::meta::combine<decltype(::beman::execution::get_completion_signatures(
                std::declval<typename apply_decayed<C>::sender_type>(), std::declval<Env>()))...>>;
    };

    using upstream_env = decltype(::beman::execution::detail::let_t<Completion>::join_env(::std::declval<Sender>(),
                                                                                          ::std::declval<Env>()));
    using upstream_completions = decltype(::beman::execution::get_completion_signatures(
        ::std::declval<Sender>(), ::std::declval<upstream_env>()));
    using other_completions    = ::beman::execution::detail::meta::filter<other_completion, upstream_completions>;
    using matching_completions = ::beman::execution::detail::meta::filter<matching_completion, upstream_completions>;
    using type = ::beman::execution::detail::meta::combine<typename get_completions<matching_completions>::type,
                                                           other_completions>;
};
} // namespace beman::execution::detail

#include <beman/execution/detail/suppress_pop.hpp>

namespace beman::execution {
using let_error_t   = ::beman::execution::detail::let_t<::beman::execution::set_error_t>;
using let_stopped_t = ::beman::execution::detail::let_t<::beman::execution::set_stopped_t>;
using let_value_t   = ::beman::execution::detail::let_t<::beman::execution::set_value_t>;

inline constexpr ::beman::execution::let_error_t   let_error{};
inline constexpr ::beman::execution::let_stopped_t let_stopped{};
inline constexpr ::beman::execution::let_value_t   let_value{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_LET
