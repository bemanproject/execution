// tests/beman/execution/issue-252.test.cpp                          -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution.hpp>
#endif

namespace ex = beman::execution;

namespace {
struct non_decomposable_sender {
    using sender_concept = ex::sender_tag;

    template <typename...>
    static consteval auto get_completion_signatures() -> ex::completion_signatures<ex::set_value_t()> {
        return {};
    }

    non_decomposable_sender(int, int) {}
};

struct decomposable_sender {
    using sender_concept      = ex::sender_tag;
    using is_basic_sender_tag = void;

    struct tag {};
    tag tag_value;
    int data;
};
} // namespace

TEST(issue252) {
    static_assert(ex::sender<non_decomposable_sender>);
    static_assert(std::same_as<ex::completion_signatures<ex::set_value_t()>,
                               decltype(ex::get_completion_signatures<non_decomposable_sender, ex::env<>>())>);

    static_assert(std::same_as<void, ex::tag_of_t<non_decomposable_sender>>);
    static_assert(std::same_as<decomposable_sender::tag, ex::tag_of_t<decomposable_sender>>);

    (void)ex::get_completion_signatures<non_decomposable_sender, ex::env<>>();
}
