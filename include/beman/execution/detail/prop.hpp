// include/beman/execution/detail/prop.hpp                            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_PROP
#define INCLUDED_INCLUDE_BEMAN_EXECUTION_DETAIL_PROP

#include <type_traits>

// ----------------------------------------------------------------------------

namespace beman::execution {
template <typename Query, typename Value>
struct prop {
    [[no_unique_address]] Query query_{};
    Value                       value_;

    template <typename Q, typename V>
    prop(Q q, V&& v) : query_(q), value_(v) {}
    prop(prop&&)                = default;
    prop(const prop&)           = default;
    auto operator=(prop&&)      = delete;
    auto operator=(const prop&) = delete;

    constexpr auto query(Query) const noexcept -> Value { return this->value_; }
};

template <typename Query, typename Value>
prop(Query, Value) -> prop<Query, ::std::unwrap_reference_t<Value>>;
} // namespace beman::execution

// ----------------------------------------------------------------------------

#endif
