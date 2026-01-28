// src/beman/execution/tests/exec-snd-transform.test.cpp            -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_USE_MODULE
import beman.execution;
#else
#include <beman/execution/detail/transform_sender.hpp>
#include <beman/execution/detail/sender.hpp>
#include <beman/execution/execution.hpp>
#include <beman/execution/detail/tag_of_t.hpp>
#endif
#include <test/execution.hpp>
#include <concepts>
#include <type_traits>

// ----------------------------------------------------------------------------

namespace {
enum class kind : unsigned char { dom, tag };
struct env {};
struct empty_domain {};

template <kind>
struct final_sender {
    using sender_concept = test_std::sender_t;
    using index_type     = std::integral_constant<int, 0>;
    int  value{};
    auto operator==(const final_sender&) const -> bool = default;
};

template <int>
struct sender;

template <int I>
struct tag {
    template <typename Sender, typename... Env>
    auto transform_sender(Sender sndr, Env&&...) const noexcept {
        if constexpr (1 < I)
            return sender<I - 1>{{}, sndr.value};
        else
            return final_sender<kind::tag>{sndr.value};
    }
};

template <int I>
struct sender {
    using sender_concept = test_std::sender_t;
    using index_type     = std::integral_constant<int, I>;
    tag<I> t;
    int    value{};
};

struct special_domain {
    template <typename Sender>
        requires std::same_as<std::remove_cvref_t<Sender>, final_sender<kind::dom>>
    auto transform_sender(Sender&& sndr, auto&&...) const -> decltype(auto) {
        return std::forward<Sender>(sndr);
    }
    template <typename Sender>
        requires std::same_as<std::remove_cvref_t<Sender>, final_sender<kind::tag>>
    auto transform_sender(Sender&& sndr, auto&&...) const {
        return final_sender<kind::dom>{sndr.value};
    }
    auto transform_sender(auto&& sndr, auto&&...) const {
        using index_type = std::remove_cvref_t<decltype(sndr)>::index_type;
        if constexpr (1 < index_type::value)
            return sender<index_type::value - 1>{{}, sndr.value};
        else
            return final_sender<kind::dom>{sndr.value};
    }
};

template <bool Noexcept, typename Expect, typename Dom, typename Sender>
auto test_transform_sender(Dom&& dom, Sender&& sndr) {
    auto value{sndr.value};
    static_assert(test_std::sender<std::remove_cvref_t<Sender>>);
    static_assert(requires { test_std::transform_sender(dom, std::forward<Sender>(sndr)); });
    if constexpr (requires { test_std::transform_sender(dom, std::forward<Sender>(sndr)); }) {
        static_assert(std::same_as<Expect, decltype(test_std::transform_sender(dom, std::forward<Sender>(sndr)))>);
        static_assert(Noexcept == noexcept(test_std::transform_sender(dom, std::forward<Sender>(sndr))));
        ASSERT(value == test_std::transform_sender(dom, std::forward<Sender>(sndr)).value);
    }
}

template <bool Noexcept, typename Expect, typename Dom, typename Sender>
auto test_transform_sender_env(Dom&& dom, Sender&& sndr) {
    auto value{sndr.value};
    static_assert(requires { test_std::transform_sender(dom, std::forward<Sender>(sndr), env{}); });
    if constexpr (requires { test_std::transform_sender(dom, std::forward<Sender>(sndr), env{}); }) {
        static_assert(
            std::same_as<Expect, decltype(test_std::transform_sender(dom, std::forward<Sender>(sndr), env{}))>);
        static_assert(Noexcept == noexcept(test_std::transform_sender(dom, std::forward<Sender>(sndr), env{})));
        ASSERT(value == test_std::transform_sender(dom, std::forward<Sender>(sndr), env{}).value);
    }

    static_assert(not requires { test_std::transform_sender(dom, std::forward<Sender>(sndr), env{}, env{}); });
}
} // namespace

TEST(exec_snd_transform) {
    test_transform_sender<true, final_sender<kind::tag>&&>(empty_domain{}, final_sender<kind::tag>{42});
    test_transform_sender_env<true, final_sender<kind::tag>&&>(empty_domain{}, final_sender<kind::tag>{42});
    final_sender<kind::tag> fs{42};
    test_transform_sender<true, final_sender<kind::tag>&>(empty_domain{}, fs);
    test_transform_sender_env<true, final_sender<kind::tag>&>(empty_domain{}, fs);
    const final_sender<kind::tag> cfs{42};
    test_transform_sender<true, const final_sender<kind::tag>&>(empty_domain{}, cfs);
    test_transform_sender_env<true, const final_sender<kind::tag>&>(empty_domain{}, cfs);

    static_assert(std::same_as<tag<1>, test_std::tag_of_t<sender<1>>>);
    static_assert(std::same_as<tag<2>, test_std::tag_of_t<sender<2>>>);

    static_assert(std::same_as<final_sender<kind::tag>, decltype(tag<1>{}.transform_sender(sender<1>{{}, 0}))>);
    static_assert(std::same_as<final_sender<kind::tag>, decltype(tag<1>{}.transform_sender(sender<1>{{}, 0}, env{}))>);
    static_assert(std::same_as<final_sender<kind::tag>,
                               decltype(test_std::default_domain{}.transform_sender(sender<1>{{}, 0}))>);
    static_assert(std::same_as<final_sender<kind::tag>,
                               decltype(test_std::default_domain{}.transform_sender(sender<1>{{}, 0}, env{}))>);

    static_assert(std::same_as<sender<1>, decltype(tag<2>{}.transform_sender(sender<2>{{}, 0}))>);
    static_assert(std::same_as<sender<1>, decltype(tag<2>{}.transform_sender(sender<2>{{}, 0}, env{}))>);

    test_transform_sender<true, final_sender<kind::tag>>(empty_domain{}, sender<1>{{}, 42});
    test_transform_sender_env<true, final_sender<kind::tag>>(empty_domain{}, sender<1>{{}, 42});
    test_transform_sender<true, final_sender<kind::tag>>(empty_domain{}, sender<2>{{}, 42});
    test_transform_sender_env<true, final_sender<kind::tag>>(empty_domain{}, sender<2>{{}, 42});
    test_transform_sender<true, final_sender<kind::tag>>(empty_domain{}, sender<5>{{}, 42});
    test_transform_sender_env<true, final_sender<kind::tag>>(empty_domain{}, sender<5>{{}, 42});

    static_assert(std::same_as<final_sender<kind::dom>&&,
                               decltype(special_domain{}.transform_sender(final_sender<kind::dom>{}))>);
    static_assert(
        std::same_as<final_sender<kind::dom>, decltype(special_domain{}.transform_sender(final_sender<kind::tag>{}))>);
    static_assert(
        std::same_as<final_sender<kind::dom>, decltype(special_domain{}.transform_sender(sender<1>{{}, 42}))>);
    static_assert(std::same_as<sender<1>, decltype(special_domain{}.transform_sender(sender<2>{{}, 42}))>);
    test_transform_sender<true, final_sender<kind::dom>&&>(special_domain{}, final_sender<kind::dom>{42});
    test_transform_sender_env<true, final_sender<kind::dom>&&>(special_domain{}, final_sender<kind::dom>{42});
    final_sender<kind::dom> fd{42};
    test_transform_sender<true, final_sender<kind::dom>&>(special_domain{}, fd);
    test_transform_sender_env<true, final_sender<kind::dom>&>(special_domain{}, fd);
    const final_sender<kind::dom> cfd{42};
    test_transform_sender<true, const final_sender<kind::dom>&>(special_domain{}, cfd);
    test_transform_sender_env<true, const final_sender<kind::dom>&>(special_domain{}, cfd);
    test_transform_sender<true, final_sender<kind::dom>>(special_domain{}, final_sender<kind::tag>{42});
    test_transform_sender_env<true, final_sender<kind::dom>>(special_domain{}, final_sender<kind::tag>{42});
    test_transform_sender<true, final_sender<kind::dom>>(special_domain{}, sender<1>{{}, 42});
    test_transform_sender_env<true, final_sender<kind::dom>>(special_domain{}, sender<1>{{}, 42});
    test_transform_sender<true, final_sender<kind::dom>>(special_domain{}, sender<2>{{}, 42});
    test_transform_sender_env<true, final_sender<kind::dom>>(special_domain{}, sender<2>{{}, 42});
    test_transform_sender<true, final_sender<kind::dom>>(special_domain{}, sender<5>{{}, 42});
    test_transform_sender_env<true, final_sender<kind::dom>>(special_domain{}, sender<5>{{}, 42});
}
