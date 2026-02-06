// src/beman/execution/tests/exec-snd-concepts.test.cpp             -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <tuple>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/detail/product_type.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/detail/sender_decompose.hpp>
#include <beman/execution/detail/sender_for.hpp>
#include <beman/execution/detail/sender_in.hpp>
#include <beman/execution/detail/tag_of_t.hpp>

#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
struct non_sender {};

struct own_sender_t : test_std::sender_t {};

struct std_sender {
    using sender_concept = test_std::sender_t;
};
struct own_sender {
    using sender_concept = own_sender_t;
};

struct tag_t {};
struct tagged_sender {
    using sender_concept = test_std::sender_t;
    tag_t tag;
    int   data;
};
struct tagged_sender1 {
    using sender_concept = test_std::sender_t;
    tag_t tag;
    int   data;
    int   child1;
};
struct tagged_sender2 {
    using sender_concept = test_std::sender_t;
    tag_t tag;
    int   data;
    int   child1;
    int   child2;
};
struct tagged_sender3 {
    using sender_concept = test_std::sender_t;
    tag_t tag;
    int   data;
    int   child1;
    int   child2;
    int   child3;
};
struct tagged_sender4 {
    using sender_concept = test_std::sender_t;
    tag_t tag;
    int   data;
    int   child1;
    int   child2;
    int   child3;
    int   child4;
};
struct product_sender0 : test_detail::product_type<tag_t, int> {
    using sender_concept = test_std::sender_t;
};
struct product_sender1 : test_detail::product_type<tag_t, int, int> {
    using sender_concept = test_std::sender_t;
};
struct product_sender2 : test_detail::product_type<tag_t, int, int, int> {
    using sender_concept = test_std::sender_t;
};
struct product_sender3 : test_detail::product_type<tag_t, int, int, int, int> {
    using sender_concept = test_std::sender_t;
};
struct product_sender4 : test_detail::product_type<tag_t, int, int, int, int, int> {
    using sender_concept = test_std::sender_t;
};

// -------------------------------------------------------------------------

auto test_valid_completion_signatures() {
    static_assert(not test_detail::valid_completion_signatures<int>);
    static_assert(test_detail::valid_completion_signatures<test_std::completion_signatures<>>);
    static_assert(
        test_detail::valid_completion_signatures<test_std::completion_signatures<test_std::set_error_t(int)>>);
}

auto test_is_sender() -> void {
    static_assert(not test_detail::is_sender<non_sender>);
    static_assert(not test_detail::is_sender<const std_sender&&>);
    static_assert(not test_detail::is_sender<const std_sender&>);
    static_assert(not test_detail::is_sender<std_sender&>);
    static_assert(test_detail::is_sender<const std_sender>);
    static_assert(test_detail::is_sender<std_sender>);
    static_assert(test_detail::is_sender<own_sender>);
}

auto test_enable_sender() -> void {
    static_assert(not test_detail::enable_sender<non_sender>);
    static_assert(not test_detail::enable_sender<const std_sender&&>);
    static_assert(not test_detail::enable_sender<const std_sender&>);
    static_assert(not test_detail::enable_sender<std_sender&>);
    static_assert(test_detail::enable_sender<const std_sender>);
    static_assert(test_detail::enable_sender<std_sender>);
    static_assert(test_detail::enable_sender<own_sender>);
}

auto test_sender() -> void {
    static_assert(not test_std::sender<non_sender>);
    static_assert(test_std::sender<const std_sender&&>);
    static_assert(test_std::sender<const std_sender&>);
    static_assert(test_std::sender<std_sender&>);
    static_assert(test_std::sender<const std_sender>);
    static_assert(test_std::sender<std_sender>);
    static_assert(test_std::sender<own_sender>);
}

auto test_sender_in() -> void {
    struct non_queryable {
        non_queryable()                                        = default;
        non_queryable(non_queryable&&)                         = default;
        non_queryable(const non_queryable&)                    = default;
        ~non_queryable()                                       = delete;
        auto operator=(non_queryable&&) -> non_queryable&      = default;
        auto operator=(const non_queryable&) -> non_queryable& = default;
    };
    struct queryable {};
    struct non_sender_in {};
    struct sender_in {
        using sender_concept        = test_std::sender_t;
        using completion_signatures = test_std::completion_signatures<>;
    };

    static_assert(test_std::sender<sender_in>);
    static_assert(not test_std::sender_in<non_sender_in>);
    static_assert(not test_std::sender_in<sender_in, non_queryable>);
    static_assert(test_std::sender_in<sender_in>);
    static_assert(test_std::sender_in<sender_in, queryable>);
    //-dk:TODO add missing test cases for sender_in
}

auto test_tag_of_t() -> void {
    // auto&& [tag, data, ...children] = tagged_sender();
    static_assert(std::same_as<tag_t, test_std::tag_of_t<tagged_sender>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<tagged_sender1>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<tagged_sender2>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<tagged_sender3>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<tagged_sender4>>);

    static_assert(std::same_as<tag_t, test_std::tag_of_t<product_sender0>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<product_sender1>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<product_sender2>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<product_sender3>>);
    static_assert(std::same_as<tag_t, test_std::tag_of_t<product_sender4>>);
}

auto test_sender_for() -> void {
    static_assert(test_std::sender<tagged_sender>);
    static_assert(test_detail::sender_for<tagged_sender, tag_t>);
    static_assert(test_std::sender<tagged_sender1>);
    static_assert(test_detail::sender_for<tagged_sender1, tag_t>);

    static_assert(test_std::sender<std_sender>);
    static_assert(not test_detail::sender_for<std_sender, tag_t>);
}
} // namespace

TEST(exec_snd_concepts) {
    test_valid_completion_signatures();
    test_is_sender();
    test_enable_sender();
    test_sender();
    test_sender_in();
    test_tag_of_t();
    test_sender_for();
}
