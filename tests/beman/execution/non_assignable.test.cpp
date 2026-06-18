// tests/beman/execution/non_assignable.test.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <type_traits>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail;
#else
#include <beman/execution/detail/non_assignable.hpp>
#endif

// ----------------------------------------------------------------------------

TEST(non_assignable) {
    test_detail::non_assignable na0;
    test_detail::non_assignable na1{};
    test_detail::non_assignable na2{na1};
    test_detail::non_assignable na3{test_detail::non_assignable{}};
    static_assert(not std::is_assignable_v<decltype(na0), decltype(na1)>);
    static_assert(not std::is_assignable_v<decltype(na0), test_detail::non_assignable>);
    static_assert(not std::is_assignable_v<decltype(na0), test_detail::non_assignable&&>);
}
