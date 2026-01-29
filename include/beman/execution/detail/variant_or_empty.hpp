// include/beman/execution/detail/variant_or_empty.hpp              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_VARIANT_OR_EMPTY
#define INCLUDED_BEMAN_EXECUTION_DETAIL_VARIANT_OR_EMPTY

#include <beman/execution/detail/common.hpp>
#include <beman/execution/detail/meta_unique.hpp>
#include <type_traits>
#include <variant>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
struct empty_variant {
    empty_variant() = delete;
};

template <typename... T>
struct variant_or_empty_helper;

template <>
struct variant_or_empty_helper<> {
    using type = ::beman::execution::detail::empty_variant;
};

template <typename... T>
struct variant_or_empty_helper {
    using type = ::beman::execution::detail::meta::unique<::std::variant<::std::decay_t<T>...>>;
};

template <typename... T>
using variant_or_empty = typename ::beman::execution::detail::variant_or_empty_helper<T...>::type;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_VARIANT_OR_EMPTY
