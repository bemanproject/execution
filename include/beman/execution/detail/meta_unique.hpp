// include/beman/execution/detail/meta_unique.hpp                   -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_META_UNIQUE
#define INCLUDED_BEMAN_EXECUTION_DETAIL_META_UNIQUE

#include <beman/execution/detail/meta_contains.hpp>
#include <beman/execution/detail/meta_prepend.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution::detail::meta::detail {
template <typename, typename>
struct make_unique;
template <typename>
struct unique;

template <template <typename...> class List, typename... R>
struct make_unique<List<R...>, List<>> {
    using type = List<R...>;
};

template <template <typename...> class List, typename... R, typename H, typename... T>
struct make_unique<List<R...>, List<H, T...>> {
    using type = typename ::beman::execution::detail::meta::detail::make_unique<
        ::std::conditional_t<::beman::execution::detail::meta::contains<H, R...>, List<R...>, List<R..., H>>,
        List<T...>>::type;
};

template <template <typename...> class List, typename... T>
struct unique<List<T...>> {
    using type = typename ::beman::execution::detail::meta::detail::make_unique<List<>, List<T...>>::type;
};
} // namespace beman::execution::detail::meta::detail

namespace beman::execution::detail::meta {
template <typename T>
using unique = typename ::beman::execution::detail::meta::detail::unique<T>::type;
}

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_META_UNIQUE
