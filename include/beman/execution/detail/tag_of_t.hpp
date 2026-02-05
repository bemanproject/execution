// include/beman/execution/detail/tag_of_t.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_TAG_OF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_TAG_OF

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
template <typename Sender>
using tag_of_t = typename decltype(::beman::execution::detail::get_sender_meta(::std::declval<Sender&&>()))::tag_type;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_TAG_OF
