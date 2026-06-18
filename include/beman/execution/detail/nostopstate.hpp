// include/beman/execution/detail/nostopstate.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_NOSTOPSTATE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_NOSTOPSTATE

#include <beman/execution/detail/common.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
struct nostopstate_t {
    explicit nostopstate_t() = default;
};

inline constexpr nostopstate_t nostopstate{};
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_NOSTOPSTATE
