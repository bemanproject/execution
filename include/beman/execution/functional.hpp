// include/beman/execution/functional.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_FUNCTIONAL
#define INCLUDED_BEMAN_EXECUTION_FUNCTIONAL

// ----------------------------------------------------------------------------

#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.call_result_t import beman.execution.detail.callable import beman.execution.detail.decayed_typeof import beman.execution.detail.nothrow_callable
#else
#include <beman/execution/detail/call_result_t.hpp>
#include <beman/execution/detail/callable.hpp>
#include <beman/execution/detail/decayed_typeof.hpp>
#include <beman/execution/detail/nothrow_callable.hpp>
#endif

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_FUNCTIONAL
