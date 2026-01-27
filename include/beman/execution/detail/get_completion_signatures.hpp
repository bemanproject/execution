// include/beman/execution/detail/get_completion_signatures.hpp     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SIGNATURES

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/await_result_type.hpp>
#include <beman/execution/detail/completion_signatures.hpp>
#include <beman/execution/detail/env_promise.hpp>
#include <beman/execution/detail/get_domain_late.hpp> //-dk:TODO remove
#include <beman/execution/detail/is_awaitable.hpp>
#include <beman/execution/detail/transform_sender.hpp> //-dk:TODO remove

#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT struct get_completion_signatures_t {
  private:
    template <typename Sender, typename Env>
    static auto get(Sender&& sender, Env&& env) noexcept {
        auto new_sender{[](auto&& sndr, auto&& e) -> decltype(auto) {
            auto domain{::beman::execution::detail::get_domain_late(sndr, e)};
            return ::beman::execution::transform_sender(domain, ::std::forward<Sender>(sndr), ::std::forward<Env>(e));
        }};

        using sender_type = ::std::remove_cvref_t<decltype(new_sender(sender, env))>;
        using decayed_env = ::std::remove_cvref_t<Env>;
        if constexpr (requires { new_sender(sender, env).get_completion_signatures(env); })
            return decltype(new_sender(sender, env).get_completion_signatures(env)){};
        else if constexpr (requires { typename sender_type::completion_signatures; })
            return typename sender_type::completion_signatures{};
        else if constexpr (::beman::execution::detail::
                               is_awaitable<sender_type, ::beman::execution::detail::env_promise<decayed_env>>) {
            using result_type =
                ::beman::execution::detail::await_result_type<sender_type,
                                                              ::beman::execution::detail::env_promise<decayed_env>>;
            if constexpr (::std::same_as<void, result_type>) {
                return ::beman::execution::completion_signatures<::beman::execution::set_value_t(),
                                                                 ::beman::execution::set_error_t(::std::exception_ptr),
                                                                 ::beman::execution::set_stopped_t()>{};
            } else {
                return ::beman::execution::completion_signatures<::beman::execution::set_value_t(result_type),
                                                                 ::beman::execution::set_error_t(::std::exception_ptr),
                                                                 ::beman::execution::set_stopped_t()>{};
            }
        }
    }

  public:
    template <typename Sender, typename Env>
        requires(!::std::same_as<void,
                                 decltype(get_completion_signatures_t::get(::std::declval<Sender>(),
                                                                           ::std::declval<Env>()))>)
    auto operator()(Sender&& sender, Env&& env) const noexcept {
        return this->get(::std::forward<Sender>(sender), ::std::forward<Env>(env));
    }
};
BEMAN_EXECUTION_EXPORT inline constexpr get_completion_signatures_t get_completion_signatures{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_GET_COMPLETION_SIGNATURES
