// docs/code/tst-sync_wait.hpp                                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#ifndef INCLUDED_DOCS_CODE_TST_SYNC_WAIT
#define INCLUDED_DOCS_CODE_TST_SYNC_WAIT

#include <utility>
#include <type_traits>
#include "tst-config.hpp"

// ----------------------------------------------------------------------------

namespace tst {
template <tst::ex::sender Sender>
struct add_set_value {
    template <typename S>
    struct is_set_value : std::false_type {};
    template <typename... A>
    struct is_set_value<tst::ex::set_value_t(A...)> : std::true_type {};
    template <typename>
    struct contains_set_value;
    template <typename... S>
    struct contains_set_value<tst::ex::completion_signatures<S...>>
        : std::bool_constant<(... || is_set_value<S>::value)> {};

    template <typename T, bool = contains_set_value<T>::value>
    struct add_signature {
        using type = T;
    };
    template <typename... S>
    struct add_signature<tst::ex::completion_signatures<S...>, false> {
        using type = tst::ex::completion_signatures<tst::ex::set_value_t(), S...>;
    };
    using sender_concept = tst::ex::sender_t;
    template <typename Env>
    constexpr auto get_completion_signatures(const Env& e) noexcept {
        using orig = decltype(tst::ex::get_completion_signatures(std::declval<Sender>(), e));
        return typename add_signature<orig>::type{};
    }
    std::remove_cvref_t<Sender> inner;
    template <tst::ex::receiver Rcvr>
    auto connect(Rcvr&& rcvr) && {
        return tst::ex::connect(std::move(this->inner), std::forward<Rcvr>(rcvr));
    }
};
template <tst::ex::sender Sender>
add_set_value(Sender&&) -> add_set_value<std::remove_cvref_t<Sender>>;

inline constexpr struct just_error_t {
    template <typename E>
    auto operator()(E&& e) const {
        return add_set_value(ex::just_error(std::forward<E>(e)));
    }
} just_error{};
inline constexpr struct when_all_t {
    template <tst::ex::sender... Senders>
    auto operator()(Senders&&... sndrs) const {
        return ex::when_all(tst::add_set_value(std::forward<Senders>(sndrs))...);
    }
} when_all{};

template <tst::ex::sender Sender>
auto sync_wait(Sender&& sndr) {
    return tst::ex::sync_wait(add_set_value<Sender>{std::forward<Sender>(sndr)});
}
} // namespace tst

// ----------------------------------------------------------------------------

#endif
