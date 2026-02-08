module;
// src/beman/execution/bulk.cppm                                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/bulk.hpp>

export module beman.execution.detail.bulk;

namespace beman::execution {
export using beman::execution::bulk_t;
export using beman::execution::bulk;
} // namespace beman::execution

namespace beman::execution::detail {
export BEMAN_SPECIALIZE_EXPORT
struct impls_for<bulk_t>;

export template <class Shape, class F, class Sender, class Env>
struct completion_signatures_for_impl<
    ::beman::execution::detail::
        basic_sender<::beman::execution::detail::bulk_t, ::beman::execution::detail::product_type<Shape, F>, Sender>,
    Env>;
} // namespace beman::execution::detail
