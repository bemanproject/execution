// src/beman/execution/tests/queryable.test.cpp                     -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/execution.hpp>
#include "test/execution.hpp"

// ----------------------------------------------------------------------------

namespace {
template <typename T>
concept has_foo = test_detail::queryable<T> && requires(T t) { t.foo; };

struct non_destructible {
    non_destructible()                                           = default;
    non_destructible(non_destructible&&)                         = default;
    non_destructible(const non_destructible&)                    = default;
    ~non_destructible()                                          = delete;
    auto operator=(non_destructible&&) -> non_destructible&      = default;
    auto operator=(const non_destructible&) -> non_destructible& = default;
};

template <typename T>
    requires std::destructible<T>
auto f(const T&) -> int {
    return 0;
}

template <typename T>
    requires has_foo<T>
auto f(const T&) -> int {
    return 1;
}

struct bar {
    int foo;
};
} // namespace

TEST(queryable) {
    static_assert(test_detail::queryable<int>);
    static_assert(not test_detail::queryable<non_destructible>);

    ASSERT(f(0) == 0);
    ASSERT(f(bar{}) == 1);
}
