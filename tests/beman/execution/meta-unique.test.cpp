// src/beman/execution/tests/meta-unique.cpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail;
#else
#include <beman/execution/detail/meta_unique.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <typename...>
struct type_list {};
} // namespace

TEST(meta_unique) {
    static_assert(std::same_as<type_list<>, test_detail::meta::unique<type_list<>>>);
    static_assert(std::same_as<type_list<bool>, test_detail::meta::unique<type_list<bool>>>);
    static_assert(std::same_as<type_list<bool>, test_detail::meta::unique<type_list<bool, bool>>>);
    static_assert(std::same_as<type_list<bool>, test_detail::meta::unique<type_list<bool, bool, bool>>>);
    static_assert(std::same_as<type_list<bool, char>, test_detail::meta::unique<type_list<bool, bool, bool, char>>>);
    static_assert(
        std::same_as<type_list<bool, char, double>, test_detail::meta::unique<type_list<bool, char, double>>>);
    static_assert(std::same_as<type_list<bool, char, double>,
                               test_detail::meta::unique<type_list<bool, bool, char, char, double, double>>>);
    static_assert(std::same_as<type_list<bool, char, double>,
                               test_detail::meta::unique<type_list<bool, char, double, char, bool>>>);
    static_assert(std::same_as<type_list<bool, char, double>,
                               test_detail::meta::unique<type_list<bool, char, double, bool, char>>>);
}
