// include/beman/execution/detail/completion_signatures.hpp         -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES
#define INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <type_traits>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signature
#else
#include <beman/execution/detail/completion_signature.hpp>
#endif

    // ----------------------------------------------------------------------------

    namespace beman::execution {
        /*!
 * \brief Class template used to a specify a list of completion signatures
 * \headerfile beman/execution/execution.hpp <beman/execution/execution.hpp>
 *
 * \details
 * This class template is primarily used for type computations and objects of
 * any specializaion are empty. Objects may be created to return them from
 * functions used for type computations like get_completion_signatures(sender, env).
 */
        template <::beman::execution::detail::completion_signature...> struct completion_signatures {};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION_SIGNATURES
