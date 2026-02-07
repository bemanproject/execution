// include/beman/execution/detail/scheduler_t.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER_T
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER_T

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#endif

// ----------------------------------------------------------------------------

namespace beman::execution {
/*!
 * \brief Tag type to indicate a class is a scheduler.
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 */
struct scheduler_t {};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SCHEDULER_T
