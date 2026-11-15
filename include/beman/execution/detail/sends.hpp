// include/beman/execution/detail/sends.hpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_SENDS
#define INCLUDED_BEMAN_EXECUTION_DETAIL_SENDS

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.completion_signatures_of_t;
import beman.execution.detail.completion_tag;
import beman.execution.detail.gather_signatures;
import beman.execution.detail.sender_in;
import beman.execution.detail.type_list;
#else
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/completion_tag.hpp>
#include <beman/execution/detail/gather_signatures.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/type_list.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Tag, typename Sender, typename Env>
    requires ::beman::execution::sender_in<Sender, Env> && ::beman::execution::detail::completion_tag<Tag>
inline constexpr bool sends =
    (!::std::same_as<
        ::beman::execution::detail::type_list<>,
        ::beman::execution::detail::gather_signatures<Tag,
                                                      ::beman::execution::completion_signatures_of_t<Sender, Env>,
                                                      ::beman::execution::detail::type_list,
                                                      ::beman::execution::detail::type_list>>);
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_SENDS
