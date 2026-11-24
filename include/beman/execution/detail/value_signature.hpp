// include/beman/execution/detail/value_signature.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_VALUE_SIGNATURE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_VALUE_SIGNATURE

#include <beman/execution/detail/common.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename R>
struct value_signature {
    using type = ::beman::execution::set_value_t(R);
};
template <>
struct value_signature<void> {
    using type = ::beman::execution::set_value_t();
};

template <typename R>
using value_signature_t = ::beman::execution::detail::value_signature<R>::type;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_VALUE_SIGNATURE
