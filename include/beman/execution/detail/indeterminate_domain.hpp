// include/beman/execution/detail/indeterminate_domain.hpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_INDETERMINATE_DOMAIN
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_INDETERMINATE_DOMAIN

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename...>
struct indeterminate_domain;
}

namespace beman::execution {
template <typename... T>
using indeterminate_domain = detail::indeterminate_domain<T...>;
}

// ----------------------------------------------------------------------------

#endif
