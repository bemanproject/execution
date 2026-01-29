// include/beman/execution/detail/sender_for.hpp                    -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_FOR
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_FOR

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Sender, typename Tag>
concept sender_for = ::beman::execution::sender<Sender> && ::std::same_as<::beman::execution::tag_of_t<Sender>, Tag>;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SENDER_FOR
