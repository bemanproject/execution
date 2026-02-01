// examples/inspectc.pp                                               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <tuple>
#include <variant>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#endif

namespace ex = beman::execution;

// ----------------------------------------------------------------------------

namespace meta {
// The code in this namespace is a fairly basic way to print types. It can
// almost certainly be done better, in particular using reflection. However,
// that's beside the point of this example. The important part for the
// example is that meta::type<T>::name() yields a string representing the
// type T in some reasonable way
template <template <typename...> class, typename...>
struct list {
    static auto name() { return "unknown-list"; }
};
template <>
struct list<::beman::execution::completion_signatures> {
    static auto name() { return "ex::completion_signatures"; }
};

template <typename T>
struct type {
    static auto name() { return typeid(T).name(); }
};
template <typename T>
struct type<T&> {
    static auto name() { return typeid(T).name() + std::string("&"); }
};
template <typename T>
struct type<T&&> {
    static auto name() { return typeid(T).name() + std::string("&&"); }
};
template <typename T>
struct type<T*> {
    static auto name() { return typeid(T).name() + std::string("*"); }
};
template <typename T>
struct type<const T> {
    static auto name() { return typeid(T).name() + std::string("const"); }
};
template <typename T>
struct type<volatile T> {
    static auto name() { return typeid(T).name() + std::string("volatile"); }
};
template <typename T>
struct type<const volatile T> {
    static auto name() { return typeid(T).name() + std::string("const volatile"); }
};
// NOLINTBEGIN(hicpp-avoid-c-arrays)
template <typename T, std::size_t N>
struct type<T[N]> {
    static auto name() { return typeid(T).name() + std::string("[") + std::to_string(N) + "]"; }
};
template <typename T, std::size_t N>
struct type<T (&)[N]> {
    static auto name() { return typeid(T).name() + std::string("(&)[") + std::to_string(N) + "]"; }
};
template <typename T, std::size_t N>
struct type<T (*)[N]> {
    static auto name() { return typeid(T).name() + std::string("(*)[") + std::to_string(N) + "]"; }
};
// NOLINTEND(hicpp-avoid-c-arrays)

template <>
struct type<::beman::execution::set_value_t> {
    static auto name() { return "ex::set_value_t"; }
};
template <>
struct type<::beman::execution::set_error_t> {
    static auto name() { return "ex::set_error_t"; }
};
template <>
struct type<::beman::execution::set_stopped_t> {
    static auto name() { return "ex::set_stopped_t"; }
};

template <typename T>
struct type<T()> {
    static auto name() { return type<T>::name() + std::string("()"); }
};
template <typename T, typename A, typename... B>
struct type<T(A, B...)> {
    static auto name() {
        return type<T>::name() + std::string("(") + (type<A>::name() + ... + (std::string(", ") + type<B>::name())) +
               ")";
    }
};
template <typename T>
struct type<T (*)()> {
    static auto name() { return type<T>::name() + std::string("(*)()"); }
};
template <typename T, typename A, typename... B>
struct type<T (*)(A, B...)> {
    static auto name() {
        return type<T>::name() + std::string("(*)(") +
               (type<A>::name() + ... + (std::string(", ") + type<B>::name())) + ")";
    }
};

template <template <typename...> class L>
struct type<L<>> {
    static auto name() { return list<L>::name() + std::string("<>"); }
};

template <template <typename...> class L, typename T, typename... S>
struct type<L<T, S...>> {
    static auto name() {
        return list<L>::name() + std::string("<") + (type<T>::name() + ... + (std::string(", ") + type<S>::name())) +
               ">";
    }
};

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const type<T>& t) {
    return out << t.name();
}
} // namespace meta

// ----------------------------------------------------------------------------

namespace {
struct logger_t {
    template <ex::sender Sndr, ex::receiver Rcvr, typename Log>
    struct state {
        using operation_state_concept = ex::operation_state_t;
        using inner_t                 = decltype(ex::connect(std::declval<Sndr>(), std::declval<Rcvr>()));

        inner_t                  inner;
        std::remove_cvref_t<Log> log;
        state(Sndr&& s, Rcvr&& r, Log&& l)
            : inner(ex::connect(std::forward<Sndr>(s), std::forward<Rcvr>(r))), log(std::forward<Log>(l)) {}
        auto start() & noexcept -> void {
            this->log(meta::type<decltype(ex::get_completion_signatures(std::declval<Sndr>(),
                                                                        ex::get_env(std::declval<Rcvr>())))>::name());
            ex::start(this->inner);
        }
    };

    template <ex::sender Sndr, typename Log>
    struct sender {
        using sender_concept = ex::sender_t;

        Sndr sndr;
        Log  log;

        template <typename Env>
        auto get_completion_signatures(const Env& env) const noexcept {
            return ex::get_completion_signatures(this->sndr, env);
        }

        template <ex::receiver Receiver>
        auto connect(Receiver&& receiver) && noexcept(noexcept(ex::connect(std::move(this->sndr),
                                                                           std::forward<Receiver>(receiver)))) {
            return state<Sndr, Receiver, Log>(
                std::move(this->sndr), std::forward<Receiver>(receiver), std::move(this->log));
        }
    };

    template <ex::sender Sndr, typename Log>
    auto operator()(Sndr&& sndr, Log&& log) const {
        return sender<std::remove_cvref_t<Sndr>, std::remove_cvref_t<Log>>{std::forward<Sndr>(sndr),
                                                                           std::forward<Log>(log)};
    }
};

inline constexpr logger_t logger{};
} // namespace

// ----------------------------------------------------------------------------

int main() {
    auto log = [](std::string_view name) {
        return [name](std::string_view msg) { std::cout << name << " message='" << msg << "'\n"; };
    };

    ex::sync_wait(logger(ex::just(), log("just()")));
    ex::sync_wait(logger(ex::just() | ex::then([]() {}), log("just() | then(...)")));
    ex::sync_wait(logger(ex::just() | ex::then([]() noexcept {}), log("just() | then(...)")));
    ex::sync_wait(logger(ex::just(0, 1), log("just(0, 1)")));
    ex::sync_wait(logger(ex::just(0, 1, 2), log("just(0, 1, 2)")));
    ex::sync_wait(logger(ex::just_error(0), log("just_error(0)")) | ex::upon_error([](auto) {}));
    ex::sync_wait(logger(ex::just_stopped(), log("just_stopped()")) | ex::upon_stopped([]() {}));
}
