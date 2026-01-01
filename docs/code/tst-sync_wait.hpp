// docs/code/tst-sync_wait.hpp                                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_DOCS_CODE_TST_SYNC_WAIT
#define INCLUDED_DOCS_CODE_TST_SYNC_WAIT

#include <beman/execution/execution.hpp>
#include <utility>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace tst {
    template <beman::execution::sender Sender>
    struct sync_wait_sender {
        template <typename S> struct is_set_value: std::false_type {};
        template <typename... A> struct is_set_value<beman::execution::set_value_t(A...)>: std::true_type {};
        template <typename> struct contains_set_value;
        template <typename... S> struct contains_set_value<beman::execution::completion_signatures<S...>>: std::bool_constant<(... || is_set_value<S>::value)> {
        };

        template <typename T, bool = contains_set_value<T>::value> struct add_set_value { using type = T; };
        template <typename...S> struct add_set_value<beman::execution::completion_signatures<S...>, false> {
            using type = beman::execution::completion_signatures<
                beman::execution::set_value_t(), S...
            >;
        };
        using sender_concept = beman::execution::sender_t;
        template <typename Env>
        constexpr auto get_completion_signatures(Env const& e) noexcept {
            using orig = decltype(beman::execution::get_completion_signatures(std::declval<Sender>(), e));
            return typename add_set_value<orig>::type{};
        }
        std::remove_cvref_t<Sender> inner;
        template <beman::execution::receiver Rcvr>
        auto connect(Rcvr&& rcvr) && {
            return beman::execution::connect(
                std::move(this->inner),
                std::forward<Rcvr>(rcvr));
        }
    };
    template <beman::execution::sender Sender>
    auto sync_wait(Sender&& sndr) {
        return beman::execution::sync_wait(sync_wait_sender<Sender>{std::forward<Sender>(sndr)});
    }
}

// ----------------------------------------------------------------------------

#endif
