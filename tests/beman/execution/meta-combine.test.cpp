// src/beman/execution/tests/meta-combine.cpp                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail;
#else
#include <beman/execution/detail/meta_combine.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <typename...>
struct type_list {};
} // namespace

TEST(meta_combine) {
    static_assert(std::same_as<type_list<>, test_detail::meta::combine<type_list<>, type_list<>>>);
    static_assert(std::same_as<type_list<bool, char>, test_detail::meta::combine<type_list<bool, char>, type_list<>>>);
    static_assert(std::same_as<type_list<bool, char, double, int>,
                               test_detail::meta::combine<type_list<bool, char>, type_list<double, int>>>);
}
