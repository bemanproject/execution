// include/beman/execution/detail/allocator_aware_move.hpp          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_ALLOCATOR_AWARE_MOVE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_ALLOCATOR_AWARE_MOVE

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <memory>
#include <type_traits>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.env_of_t;
import beman.execution.detail.get_allocator;
import beman.execution.detail.get_env;
import beman.execution.detail.product_type;
#else
#include <beman/execution/detail/env_of_t.hpp>
#include <beman/execution/detail/get_allocator.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <beman/execution/detail/product_type.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
/*!
 * \brief Create a `remove_cvref_t<T>` from `obj`, passing `alloc` where the type (or its elements) use allocators.
 * \internal
 */
template <typename Alloc, typename T>
auto make_with_allocator(const Alloc& alloc,
                         T&&          obj) noexcept(!::std::uses_allocator_v<::std::remove_cvref_t<T>, Alloc> &&
                                           ::std::is_nothrow_constructible_v<::std::remove_cvref_t<T>, T>)
    -> ::std::remove_cvref_t<T> {
    if constexpr (::std::uses_allocator_v<::std::remove_cvref_t<T>, Alloc>)
        return ::std::make_obj_using_allocator<::std::remove_cvref_t<T>>(alloc, ::std::forward<T>(obj));
    else
        return ::std::remove_cvref_t<T>(::std::forward<T>(obj));
}

template <typename Alloc, typename T>
    requires(decltype(::beman::execution::detail::is_product_type(::std::declval<const T&>()))::value)
auto make_with_allocator(const Alloc& alloc, T&& obj) noexcept(
    noexcept(::std::remove_cvref_t<T>::make_from(alloc, ::std::forward<T>(obj)))) -> ::std::remove_cvref_t<T> {
    return ::std::remove_cvref_t<T>::make_from(alloc, ::std::forward<T>(obj));
}

/*!
 * \brief Utility function use to move a possibly allocator aware object with an allocator from an environment.
 * \headerfile beman/execution.hpp <beman/execution.hpp>
 * \internal
 *
 * Without an allocator in the environment `obj` is just forwarded: the cost of copying/moving is incurred by whoever
 * initializes an object from the result. Otherwise a new object is created using the allocator.
 */
template <typename T, typename Context>
auto allocator_aware_move(T&& obj, Context&&) noexcept -> T&& {
    return ::std::forward<T>(obj);
}

template <typename T, typename Context>
    requires ::std::invocable<::beman::execution::get_allocator_t, ::beman::execution::env_of_t<Context>>
auto allocator_aware_move(T&& obj, Context&& context) noexcept(
    noexcept(::beman::execution::detail::make_with_allocator(
        ::beman::execution::get_allocator(::beman::execution::get_env(context)), ::std::forward<T>(obj))))
    -> ::std::remove_cvref_t<T> {
    return ::beman::execution::detail::make_with_allocator(
        ::beman::execution::get_allocator(::beman::execution::get_env(context)), ::std::forward<T>(obj));
}
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_ALLOCATOR_AWARE_MOVE
