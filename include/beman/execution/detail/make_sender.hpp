// include/beman/execution/detail/make_sender.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_MAKE_SENDER
#define INCLUDED_BEMAN_EXECUTION_DETAIL_MAKE_SENDER

#include <beman/execution/detail/basic_sender.hpp>
#include <beman/execution/detail/movable_value.hpp>
#include <beman/execution/detail/sender.hpp>
#include <concepts>
#include <type_traits>

#include <beman/execution/detail/suppress_push.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct make_sender_empty {};

template <typename Tag, typename Data = ::beman::execution::detail::make_sender_empty, typename... Child>
    requires ::std::semiregular<Tag> && ::beman::execution::detail::movable_value<Data> &&
             (::beman::execution::sender<Child> && ...)
constexpr auto make_sender(Tag tag, Data&& data, Child&&... child) {
    return ::beman::execution::detail::basic_sender<Tag, ::std::decay_t<Data>, ::std::decay_t<Child>...>{
        tag, ::std::forward<Data>(data), ::std::forward<Child>(child)...};
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#include <beman/execution/detail/suppress_pop.hpp>

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_MAKE_SENDER
