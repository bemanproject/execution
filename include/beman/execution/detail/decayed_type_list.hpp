// include/beman/execution/detail/decayed_type_list.hpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_DECAYED_TYPE_LIST
#define INCLUDED_BEMAN_EXECUTION_DETAIL_DECAYED_TYPE_LIST

#include <beman/execution/detail/common.hpp>
#include "beman/execution/detail/type_list.hpp"
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <class... Args>
using decayed_type_list = ::beman::execution::detail::type_list<::std::decay_t<Args>...>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_DECAYED_TYPE_LIST
