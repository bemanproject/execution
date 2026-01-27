// include/beman/execution/detail/impls_for.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_IMPLS_FOR
#define INCLUDED_BEMAN_EXECUTION_DETAIL_IMPLS_FOR

#include <beman/execution/detail/default_impls.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Tag>
struct impls_for : ::beman::execution::detail::default_impls {};
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_IMPLS_FOR
