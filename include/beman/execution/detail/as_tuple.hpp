// include/beman/execution/detail/as_tuple.hpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_AS_TUPLE
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_AS_TUPLE

#include <beman/execution/detail/decayed_tuple.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename T>
struct as_tuple;
template <typename Rc, typename... A>
struct as_tuple<Rc(A...)> {
    using type = ::beman::execution::detail::decayed_tuple<Rc, A...>;
};

template <typename T>
using as_tuple_t = typename ::beman::execution::detail::as_tuple<T>::type;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif
