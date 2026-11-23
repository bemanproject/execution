// include/beman/execution/detail/meta_bind                           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_META_BIND
#define INCLUDED_BEMAN_EXECUTION_DETAIL_META_BIND

#include <beman/execution/detail/common.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail::meta {
template <template <typename...> typename Trait, typename... Args>
struct bind {
    template <typename... Rest>
    using front = Trait<Args..., Rest...>;

    template <typename... Rest>
    using back = Trait<Rest..., Args...>;
};
} // namespace beman::execution::detail::meta

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_META_BIND
