// include/beman/execution/detail/make_env.hpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_BEMAN_EXECUTION_DETAIL_MAKE_ENV
#define INCLUDED_BEMAN_EXECUTION_DETAIL_MAKE_ENV

#include <beman/execution/detail/common.hpp>
#include <type_traits>
#include <utility>

// ----------------------------------------------------------------------------

namespace beman::execution::detail {
template <typename Query, typename Value>
class make_env {
  private:
    Value value;

  public:
    template <typename V>
    make_env(const Query&, V&& v) : value(::std::forward<V>(v)) {}
    constexpr auto query(const Query&) const noexcept -> const Value& { return this->value; }
    constexpr auto query(const Query&) noexcept -> Value& { return this->value; }
};
template <typename Query, typename Value>
make_env(Query&&, Value&& value) -> make_env<::std::remove_cvref_t<Query>, ::std::remove_cvref_t<Value>>;
} // namespace beman::execution::detail

// ----------------------------------------------------------------------------

#endif // INCLUDED_BEMAN_EXECUTION_DETAIL_MAKE_ENV
