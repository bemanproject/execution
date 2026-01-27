// include/beman/execution/detail/env_of_t.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_ENV_OF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_ENV_OF

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/get_env.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief Determine the type of the environment associated with a type
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
BEMAN_EXECUTION_EXPORT template <typename T>
using env_of_t = decltype(::beman::execution::get_env(::std::declval<T>()));
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_ENV_OF
