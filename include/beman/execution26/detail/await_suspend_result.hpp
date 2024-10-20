// include/beman/execution26/detail/await_suspend_result.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION26_DETAIL_AWAIT_SUSPEND_RESULT
#define INCLUDED_BEMAN_EXECUTION26_DETAIL_AWAIT_SUSPEND_RESULT

#include <concepts>
#include <coroutine>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution26::detail
{
    template <typename> struct is_coroutine_handle: ::std::false_type {};
    template <typename T>
    struct is_coroutine_handle<::std::coroutine_handle<T>>
        : ::std::true_type
    {
    };

    template <typename T>
    concept await_suspend_result
        =  ::std::same_as<T, void>
        || ::std::same_as<T, bool>
        || ::beman::execution26::detail::is_coroutine_handle<T>::value
        ;
}

// ----------------------------------------------------------------------------

#endif