// src/beman/execution/tests/forward-like.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <concepts>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/forward_like.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct object {
    int value{};
};

template <typename Expect, typename T>
auto test_forward_like(T&& outer) {
    static_assert(std::same_as<Expect, decltype(test_detail::own_forward_like<T>(outer.value))>);
    static_assert(std::same_as<decltype(test_detail::forward_like<T>(outer.value)),
                               decltype(test_detail::own_forward_like<T>(outer.value))>);
}
} // namespace

TEST(forward_like) {
    object       o{};
    const object co{};
    test_forward_like<int&>(o);
    test_forward_like<int&&>(std::move(o)); // NOLINT(hicpp-move-const-arg,performance-move-const-arg)
    test_forward_like<const int&>(co);
    test_forward_like<const int&&>(std::move(co)); // NOLINT(hicpp-move-const-arg,performance-move-const-arg)
}
