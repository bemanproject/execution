// include/beman/execution/detail/error_types_of.hpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_CONTEXT_ERROR_TYPES_OF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_CONTEXT_ERROR_TYPES_OF

#include <beman/execution/execution.hpp>
#include <exception>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename>
struct error_types_of {
    using type = ::beman::execution::completion_signatures<::beman::execution::set_error_t(::std::exception_ptr)>;
};
template <typename Context>
    requires requires { typename Context::error_types; }
struct error_types_of<Context> {
    using type = typename Context::error_types;
};
template <typename Context>
using error_types_of_t = typename error_types_of<Context>::type;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_CONTEXT_ERROR_TYPES_OF
