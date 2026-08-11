// tests/beman/execution/exec-write-env.test.cpp                      -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#include <test/sender_env.hpp>
#ifdef BEMAN_HAS_IMPORT_STD
import std;
#else
#include <concepts>
#include <tuple>
#include <utility>
#endif
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <typename>
struct property {
    struct data {
        int  value{};
        auto operator==(const data&) const -> bool = default;
    };
};

struct write_env_env {
    struct base {};
    int  value{};
    auto query(const property<base>&) const -> property<base>::data { return {this->value}; }
};
struct write_env_added {
    int value{};
    struct added {};
    auto query(const property<added>&) const noexcept -> property<added>::data { return {this->value}; }
};

struct write_env_receiver {
    using receiver_concept = test_std::receiver_tag;

    bool* result{nullptr};

    auto get_env() const noexcept -> write_env_env { return {42}; }
    auto set_value(bool value) && noexcept -> void { *this->result = value; }
};

struct write_env_sender {
    using sender_concept        = test_std::sender_tag;
    using completion_signatures = test_std::completion_signatures<test_std::set_value_t(bool)>;
    template <typename, typename...>
    static consteval auto get_completion_signatures() -> completion_signatures {
        return {};
    }
    template <typename Receiver>
    struct state {
        using operation_state_concept = test_std::operation_state_tag;
        std::remove_cvref_t<Receiver> receiver;

        auto start() & noexcept -> void {
            using base_property  = property<write_env_env::base>;
            using added_property = property<write_env_added::added>;
            bool result{false};

            if constexpr (requires {
                              test_std::get_env(receiver).query(base_property{});
                              test_std::get_env(receiver).query(added_property{});
                          }) {
                result = (base_property::data{42} == test_std::get_env(receiver).query(base_property{})) &&
                         (added_property::data{43} == test_std::get_env(receiver).query(added_property{}));
            }

            test_std::set_value(::std::move(receiver), result);
        }
    };

    template <typename Receiver>
    auto connect(Receiver&& receiver) noexcept -> state<Receiver> {
        return {std::forward<Receiver>(receiver)};
    }
};

auto test_write_env() -> void {
    static_assert(test_std::sender<write_env_sender>);
    static_assert(test_std::receiver<write_env_receiver>);
    static_assert(test_detail::queryable<write_env_added>);
    auto plain_op(test_std::connect(write_env_sender{}, write_env_receiver{}));
    static_assert(std::same_as<write_env_env, decltype(test_std::get_env(write_env_receiver{}))>);
    static_assert(std::same_as<write_env_env, decltype(test_std::get_env(plain_op.receiver))>);
    using base_property = property<write_env_env::base>;
    ASSERT(base_property::data{42} == test_std::get_env(plain_op.receiver).query(base_property{}));

    auto we_sender{test_std::write_env(write_env_sender{}, write_env_added{43})};

    static_assert(test_std::sender_in<write_env_sender>);
    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(bool)>,
                               decltype(test_std::get_completion_signatures<write_env_sender, write_env_env>())>);

    using we_type = std::remove_cvref_t<decltype(we_sender)>;
    static_assert(std::same_as<test_detail::completion_signatures_for<we_type, write_env_env>,
                               test_std::completion_signatures<test_std::set_value_t(bool)>>);
    static_assert(std::same_as<test_detail::completion_signatures_for<decltype(we_sender), test_std::env<>>,
                               test_std::completion_signatures<test_std::set_value_t(bool)>>);
    static_assert(std::same_as<test_detail::completion_signatures_for<decltype(we_sender)&, test_std::env<>>,
                               test_std::completion_signatures<test_std::set_value_t(bool)>>);
    static_assert(test_std::sender_in<decltype(we_sender)>);
    static_assert(std::same_as<test_std::completion_signatures<test_std::set_value_t(bool)>,
                               decltype(test_std::get_completion_signatures<decltype(we_sender), write_env_env>())>);

    static_assert(test_std::sender<decltype(we_sender)>);
    static_assert(std::same_as<test_std::write_env_t, test_std::tag_of_t<decltype(we_sender)>>);

    bool has_both_properties{false};
    ASSERT(not has_both_properties);
    auto we_op{test_std::connect(we_sender, write_env_receiver{&has_both_properties})};
    test_std::start(we_op);
    ASSERT(has_both_properties);
    test::use(we_op);
}

auto test_write_env_attributes() {
    test::sender_env s{42};
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, s);
    test::test_sender_env<true>(84, test::test_non_forwardable_attr{}, s);
    test::test_sender_env<true>(42, test::test_forwardable_attr{}, test_std::write_env(s, test_std::env<>{}));
    test::test_sender_env<false>(84, test::test_non_forwardable_attr{}, test_std::write_env(s, test_std::env<>{}));
}
} // namespace

TEST(exec_write_env) {
    test_write_env();
    test_write_env_attributes();
}
