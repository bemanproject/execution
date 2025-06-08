// include/beman/execution/detail/meta_contain_same.hpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_META_CONTAIN_SAME
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_META_CONTAIN_SAME

#include <beman/execution/detail/meta_contains.hpp>

// ----------------------------------------------------------------------------

namespace beman::execution::detail::meta {
template <typename, typename>
struct contain_same_t;
template <template <typename...> class L0, typename... M0, template <typename...> class L1, typename... M1>
struct contain_same_t<L0<M0...>, L1<M1...>> {
    static constexpr bool value =
        ((sizeof...(M0) == sizeof...(M1)) && ... && ::beman::execution::detail::meta::contains<M0, M1...>);
};

template <typename S0, typename S1>
inline constexpr bool contain_same = contain_same_t<S0, S1>::value;

} // namespace beman::execution::detail::meta

// ----------------------------------------------------------------------------

#endif
