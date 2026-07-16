// include/beman/execution/detail/completion.hpp                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION
#define INCLUDED_BEMAN_EXECUTION_DETAIL_COMPLETION

#include <beman/execution.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename R>
struct completion {
    using type = ::beman::execution::set_value_t(R);
};
template <>
struct completion<void> {
    using type = ::beman::execution::set_value_t();
};

template <typename R>
using completion_t = typename beman::execution::detail::completion<R>::type;

} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
