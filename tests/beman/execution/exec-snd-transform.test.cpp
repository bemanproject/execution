// src/beman/execution/tests/exec-snd-transform.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <type_traits>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
enum class kind : unsigned char { dom, tag };
struct env {};

template <kind>
struct final_sender {
    using sender_concept = test_std::sender_tag;
    using index_type     = std::integral_constant<int, 0>;
    int  value{};
    auto operator==(const final_sender&) const -> bool = default;
};

template <int>
struct sender;

template <int I>
struct tag {
    template <typename TagParam, typename Sender, typename... Env>
    auto transform_sender(TagParam, Sender sndr, Env&&...) const noexcept {
        if constexpr (1 < I)
            return sender<I - 1>{{}, sndr.value};
        else
            return final_sender<kind::tag>{sndr.value};
    }
};

template <int I>
struct sender {
    using sender_concept      = test_std::sender_tag;
    using is_basic_sender_tag = void;
    using index_type          = std::integral_constant<int, I>;
    tag<I> t;
    int    value{};
};

struct special_domain {
    template <typename TagParam, typename Sender>
        requires std::same_as<std::remove_cvref_t<Sender>, final_sender<kind::dom>>
    auto transform_sender(TagParam, Sender&& sndr, auto&&...) const -> decltype(auto) {
        return std::forward<Sender>(sndr);
    }
    template <typename TagParam, typename Sender>
        requires std::same_as<std::remove_cvref_t<Sender>, final_sender<kind::tag>>
    auto transform_sender(TagParam, Sender&& sndr, auto&&...) const {
        return final_sender<kind::dom>{sndr.value};
    }
    template <typename TagParam, typename Sndr>
        requires(!std::same_as<std::remove_cvref_t<Sndr>, final_sender<kind::dom>> &&
                 !std::same_as<std::remove_cvref_t<Sndr>, final_sender<kind::tag>>)
    auto transform_sender(TagParam, Sndr&& sndr, auto&&...) const {
        using index_type = std::remove_cvref_t<Sndr>::index_type;
        if constexpr (1 < index_type::value)
            return sender<index_type::value - 1>{{}, sndr.value};
        else
            return final_sender<kind::dom>{sndr.value};
    }
};

template <typename Domain>
struct compl_domain_attrs {
    auto query(test_std::get_completion_domain_t<test_std::set_value_t>) const noexcept { return Domain{}; }
};

template <int I, typename Domain>
struct domain_sender {
    using sender_concept      = test_std::sender_tag;
    using is_basic_sender_tag = void;
    using index_type          = std::integral_constant<int, I>;
    tag<I> t;
    int    value{};
    auto   get_env() const noexcept { return compl_domain_attrs<Domain>{}; }
};

struct start_domain {
    template <typename Sndr>
        requires std::same_as<std::remove_cvref_t<Sndr>, final_sender<kind::tag>>
    auto transform_sender(test_std::start_t, Sndr&& sndr, const auto&) const {
        return final_sender<kind::dom>{sndr.value};
    }
};

template <typename Domain>
struct starting_domain_env {
    auto query(test_std::get_domain_t) const noexcept { return Domain{}; }
};

auto test_indeterminate_domain_transform() {
    test_std::indeterminate_domain<> idom;
    static_assert(requires { idom.transform_sender(test_std::set_value, final_sender<kind::tag>{42}, env{}); });
    auto result = idom.transform_sender(test_std::set_value, final_sender<kind::tag>{42}, env{});
    static_assert(test_std::sender<decltype(result)>);
    ASSERT(result.value == 42);

    test_std::indeterminate_domain<test_std::default_domain> idom2;
    auto result2 = idom2.transform_sender(test_std::set_value, final_sender<kind::tag>{17}, env{});
    ASSERT(result2.value == 17);
}
} // namespace

TEST(exec_snd_transform) {
    static_assert(std::same_as<tag<1>, test_std::tag_of_t<sender<1>>>);
    static_assert(std::same_as<tag<2>, test_std::tag_of_t<sender<2>>>);

    using Tag1 = std::integral_constant<int, 1>;
    using Tag2 = std::integral_constant<int, 2>;
    static_assert(
        std::same_as<final_sender<kind::tag>, decltype(tag<1>{}.transform_sender(Tag1{}, sender<1>{{}, 0}))>);
    static_assert(
        std::same_as<final_sender<kind::tag>, decltype(tag<1>{}.transform_sender(Tag1{}, sender<1>{{}, 0}, env{}))>);
    static_assert(std::same_as<sender<1>, decltype(tag<2>{}.transform_sender(Tag2{}, sender<2>{{}, 0}))>);
    static_assert(std::same_as<final_sender<kind::tag>,
                               decltype(test_std::default_domain{}.transform_sender(Tag1{}, sender<1>{{}, 0}))>);
    static_assert(
        std::same_as<final_sender<kind::tag>,
                     decltype(test_std::default_domain{}.transform_sender(Tag1{}, sender<1>{{}, 0}, env{}))>);

    // Identity: final senders (no tag customization) pass through unchanged
    {
        static_assert(requires { test_std::transform_sender(final_sender<kind::tag>{42}, env{}); });
        final_sender<kind::tag> r = test_std::transform_sender(final_sender<kind::tag>{42}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 42);
    }
    {
        final_sender<kind::dom> r = test_std::transform_sender(final_sender<kind::dom>{42}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 42);
    }

    // Single-step recursive reduction through default completion domain
    {
        final_sender<kind::tag> r = test_std::transform_sender(sender<1>{{}, 42}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 42);
    }

    // Multi-step recursive reduction
    {
        final_sender<kind::tag> r = test_std::transform_sender(sender<2>{{}, 17}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 17);
    }
    {
        final_sender<kind::tag> r = test_std::transform_sender(sender<3>{{}, 42}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 42);
    }
    {
        final_sender<kind::tag> r = test_std::transform_sender(sender<5>{{}, 99}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 99);
    }

    {
        final_sender<kind::dom> r = test_std::transform_sender(domain_sender<1, special_domain>{{}, 42}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 42);
    }
    {
        final_sender<kind::tag> r = test_std::transform_sender(domain_sender<2, special_domain>{{}, 17}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 17);
    }
    {
        final_sender<kind::tag> r = test_std::transform_sender(domain_sender<5, special_domain>{{}, 99}, env{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 99);
    }

    {
        final_sender<kind::dom> r = test_std::transform_sender(sender<1>{{}, 42}, starting_domain_env<start_domain>{});
        static_assert(test_std::sender<decltype(r)>);
        static_assert(std::same_as<decltype(r), final_sender<kind::dom>>);
        ASSERT(r.value == 42);
    }
    {
        // Multi-step completion, then starting domain
        final_sender<kind::dom> r = test_std::transform_sender(sender<3>{{}, 77}, starting_domain_env<start_domain>{});
        static_assert(std::same_as<decltype(r), final_sender<kind::dom>>);
        ASSERT(r.value == 77);
    }

    // Both custom completion and starting domains
    {
        final_sender<kind::dom> r =
            test_std::transform_sender(domain_sender<1, special_domain>{{}, 42}, starting_domain_env<start_domain>{});
        static_assert(test_std::sender<decltype(r)>);
        ASSERT(r.value == 42);
    }

    // indeterminate_domain
    test_indeterminate_domain_transform();
}
