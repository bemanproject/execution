// include/beman/execution/detail/indices_for.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_INDICES_FOR
#define INCLUDED_BEMAN_EXECUTION_DETAIL_INDICES_FOR

#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender>
using indices_for = typename ::std::remove_reference_t<Sender>::indices_for;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_INDICES_FOR
