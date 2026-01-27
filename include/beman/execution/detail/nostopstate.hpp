// include/beman/execution/detail/nostopstate.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_NOSTOPSTATE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_NOSTOPSTATE

#include <beman/execution/detail/config.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT struct nostopstate_t {
    explicit nostopstate_t() = default;
};

BEMAN_EXECUTION_EXPORT inline constexpr nostopstate_t nostopstate{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_NOSTOPSTATE
