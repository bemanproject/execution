// include/beman/execution/detail/is_constant.hpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_IS_CONSTANT
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_IS_CONSTANT

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <auto>
concept is_constant = true;
}

// ----------------------------------------------------------------------------

#endif
