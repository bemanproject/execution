// include/beman/execution/detail/with_error.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_WITH_ERROR
#define INCLUDED_BEMAN_EXECUTION_DETAIL_WITH_ERROR

#include <coroutine>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------
/*
 * \brief Tag type used to indicate an error is produced.
 * \headerfile beman/execution/task.hpp <beman/execution/task.hpp>
 * \internal
 */
namespace beman::execution::detail {
template <typename E>
struct with_error {
    using type = ::std::remove_cvref_t<E>;
    type error;
};
template <typename E>
with_error(E&&) -> with_error<E>;

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
