// include/beman/execution/detail/await_suspend_result.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_AWAIT_SUSPEND_RESULT
#define INCLUDED_BEMAN_EXECUTION_DETAIL_AWAIT_SUSPEND_RESULT

#include <beman/execution/detail/common.hpp>
#include <concepts>
#include <coroutine>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Auxiliary type trait used to detect specializations of `std::coroutine_handle`.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename>
struct is_coroutine_handle : ::std::false_type {};
/*!
 * \brief The actual partial specialization detecting `std::coroutine_handle`.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename T>
struct is_coroutine_handle<::std::coroutine_handle<T>> : ::std::true_type {};

/*!
 * \brief A concept used to identify valid results for `await_suspend`.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 * \internal
 */
template <typename T>
concept await_suspend_result =
    ::std::same_as<T, void> || ::std::same_as<T, bool> || ::beman::execution::detail::is_coroutine_handle<T>::value;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_AWAIT_SUSPEND_RESULT
