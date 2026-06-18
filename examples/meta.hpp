// examples/meta.hpp                                                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_EXAMPLES_META
#define INCLUDED_EXAMPLES_META

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

#endif
