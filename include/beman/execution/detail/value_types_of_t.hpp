// include/beman/execution/detail/value_types_of_t.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_VALUE_TYPE_OF
#define INCLUDED_BEMAN_EXECUTION_DETAIL_VALUE_TYPE_OF

#include <beman/execution/detail/config.hpp>
#include <beman/execution/detail/completion_signatures_of_t.hpp>
#include <beman/execution/detail/decayed_tuple.hpp>
#include <beman/execution/detail/env.hpp>
#include <beman/execution/detail/gather_signatures.hpp>
#include <beman/execution/detail/set_value.hpp>
#include <beman/execution/detail/variant_or_empty.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution {
BEMAN_EXECUTION_EXPORT template <typename Sender,
                                 typename Env                         = ::beman::execution::env<>,
                                 template <typename...> class Tuple   = ::beman::execution::detail::decayed_tuple,
                                 template <typename...> class Variant = ::beman::execution::detail::variant_or_empty>
    requires ::beman::execution::sender_in<Sender, Env>
using value_types_of_t =
    ::beman::execution::detail::gather_signatures< ::beman::execution::set_value_t,
                                                   ::beman::execution::completion_signatures_of_t<Sender, Env>,
                                                   Tuple,
                                                   Variant>;
}
// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_VALUE_TYPE_OF
