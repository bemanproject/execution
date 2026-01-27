// include/beman/execution/detail/sender_adaptor_closure.hpp        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_ADAPTOR_CLOSURE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_ADAPTOR_CLOSURE

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/sender.hpp>
#include <concepts>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail::pipeable {
struct sender_adaptor_closure_base {};
} // namespace beman::execution::detail::pipeable

namespace beman::execution {
// NOLINTBEGIN(bugprone-crtp-constructor-accessibility)
BEMAN_EXECUTION_EXPORT template <typename>
struct sender_adaptor_closure : ::beman::execution::detail::pipeable::sender_adaptor_closure_base {};
// NOLINTEND(bugprone-crtp-constructor-accessibility)

} // namespace beman::execution

namespace beman::execution::detail {
template <typename Closure>
concept is_sender_adaptor_closure =
    ::std::derived_from<::std::decay_t<Closure>, ::beman::execution::sender_adaptor_closure<::std::decay_t<Closure>>>;
}

namespace beman::execution::detail::pipeable {
template <::beman::execution::sender Sender, typename Adaptor>
    requires(!::beman::execution::sender<Adaptor>) &&
            ::std::derived_from<::std::decay_t<Adaptor>,
                                ::beman::execution::sender_adaptor_closure<::std::decay_t<Adaptor>>> &&
            requires(Sender&& sender, Adaptor&& adaptor) {
                { adaptor(::std::forward<Sender>(sender)) } -> ::beman::execution::sender;
            }
auto operator|(Sender&& sender, Adaptor&& adaptor) {
    return adaptor(::std::forward<Sender>(sender));
}
} // namespace beman::execution::detail::pipeable

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_ADAPTOR_CLOSURE
