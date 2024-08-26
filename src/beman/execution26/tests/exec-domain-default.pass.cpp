// src/beman/execution26/tests/exec-domain-default.pass.cpp           -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution26/detail/default_domain.hpp>
#include <beman/execution26/detail/sender.hpp>
#include <beman/execution26/detail/sender_decompose.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace
{
    struct env
    {
        int value{};
        auto operator== (env const&) const -> bool = default;
    };
    struct tagged_env
    {
        int value{};
        auto operator== (tagged_env const&) const -> bool = default;
    };

    struct non_sender {};
    struct simple_sender
    {
        using sender_concept = test_std::sender_t;
        int value{};
        auto operator== (simple_sender const&) const -> bool = default;
    };

    struct simple_tag {};

    template <bool Noexcept>
    struct tag
    {
        template <typename Sender, typename... Env>
        auto transform_sender(Sender&& sender, Env&&...) noexcept(Noexcept)
        {
            return simple_sender{sender.value};
        }

        template <typename Sender, typename Env>
        auto transform_env(Sender&&, Env&& e) noexcept
        {
            return env{e.value};
        }

        template <typename Sender>
        auto apply_sender(Sender&&, int value) noexcept(Noexcept)
        {
            return value;
        }
        template <typename Sender>
        auto apply_sender(Sender&&, int v0, int v1, int v2) noexcept(Noexcept)
        {
            return v0 + v1 + v2;
        }
        template <typename Sender>
        auto apply_sender(Sender&&, bool& flag) noexcept(Noexcept) -> bool&
        {
            return flag;
        }
    };

    template <bool Noexcept>
    struct tagged_sender
    {
        using sender_concept = test_std::sender_t;
        tag<Noexcept> t{};
        int value{17};
        int args{};
    };

    template <typename Domain>
    auto test_transform_sender(Domain)
    {
        static_assert(test_detail::queryable<env>);
        static_assert(test_std::sender<simple_sender>);
        static_assert(not requires{ Domain::transform_sender(non_sender{}); }); 
        static_assert(requires{ Domain::transform_sender(simple_sender{}); }); 
        static_assert(requires{ Domain::transform_sender(simple_sender{}, env{}); }); 
        static_assert(not requires{ Domain::transform_sender(simple_sender{}, env{}, env{}); }); 
        static_assert(noexcept(Domain::transform_sender(simple_sender{}))); 
        static_assert(noexcept(Domain::transform_sender(simple_sender{}, env{}))); 
        using simple_rvalue_type = decltype(Domain::transform_sender(simple_sender{}));
        static_assert(std::same_as<simple_sender&&, simple_rvalue_type>);
        using simple_env_rvalue_type = decltype(Domain::transform_sender(simple_sender{}, env{}));
        static_assert(std::same_as<simple_sender&&, simple_env_rvalue_type>);
        simple_sender sender;
        using simple_lvalue_type = decltype(Domain::transform_sender(sender));
        static_assert(std::same_as<simple_sender&, simple_lvalue_type>);
        using simple_env_lvalue_type = decltype(Domain::transform_sender(sender, env{}));
        static_assert(std::same_as<simple_sender&, simple_env_lvalue_type>);
        simple_sender const csender;
        using simple_clvalue_type = decltype(Domain::transform_sender(csender));
        static_assert(std::same_as<simple_sender const&, simple_clvalue_type>);
        using simple_env_clvalue_type = decltype(Domain::transform_sender(csender, env{}));
        static_assert(std::same_as<simple_sender const&, simple_env_clvalue_type>);

        assert(simple_sender{17} == Domain::transform_sender(simple_sender{17}, env{}));
        assert(&sender == &Domain::transform_sender(sender, env{}));
        assert(&csender == &Domain::transform_sender(csender, env{}));

        static_assert(test_std::sender<tagged_sender<true>>);
        static_assert(test_std::sender<tagged_sender<false>>);
        static_assert(std::same_as<tag<true>, test_std::tag_of_t<tagged_sender<true>>>);
        static_assert(std::same_as<tag<false>, test_std::tag_of_t<tagged_sender<false>>>);
        static_assert(requires{ tag<true>().transform_sender(tagged_sender<true>()); });
        static_assert(requires{ tag<true>().transform_sender(tagged_sender<true>(), env{}); });
        static_assert(requires{ tag<false>().transform_sender(tagged_sender<false>()); });
        static_assert(requires{ tag<false>().transform_sender(tagged_sender<false>(), env{}); });
        static_assert(requires{ Domain::transform_sender(tagged_sender<true>()); });
        static_assert(requires{ Domain::transform_sender(tagged_sender<true>(), env{}); });
        static_assert(not requires{ Domain::transform_sender(tagged_sender<true>(), env{}, env{}); });
        static_assert(requires{ Domain::transform_sender(tagged_sender<false>()); });
        static_assert(requires{ Domain::transform_sender(tagged_sender<false>(), env{}); });
        static_assert(not requires{ Domain::transform_sender(tagged_sender<false>(), env{}, env{}); });
        static_assert(noexcept(Domain::transform_sender(tagged_sender<true>())));
        static_assert(noexcept(Domain::transform_sender(tagged_sender<true>(), env{})));
        static_assert(not noexcept(Domain::transform_sender(tagged_sender<false>())));
        static_assert(not noexcept(Domain::transform_sender(tagged_sender<false>(), env{})));
        {
            using type = decltype(Domain::transform_sender(tagged_sender<true>()));
            static_assert(std::same_as<simple_sender, type>);
            assert(simple_sender{17} == Domain::transform_sender(tagged_sender<true>()));
        }
        {
            using type = decltype(Domain::transform_sender(tagged_sender<true>(), env{}));
            static_assert(std::same_as<simple_sender, type>);
            assert(simple_sender{17} == Domain::transform_sender(tagged_sender<true>(), env{}));
        }
        {
            using type = decltype(Domain::transform_sender(tagged_sender<false>()));
            static_assert(std::same_as<simple_sender, type>);
            assert(simple_sender{17} == Domain::transform_sender(tagged_sender<false>()));
        }
        {
            using type = decltype(Domain::transform_sender(tagged_sender<false>(), env{}));
            static_assert(std::same_as<simple_sender, type>);
            assert(simple_sender{17} == Domain::transform_sender(tagged_sender<false>(), env{}));
        }
        //-dk:TODO test returning a non-sender doesn't work
    }

    template <typename Domain>
    auto test_transform_env(Domain)
    {
        static_assert(test_detail::queryable<env>);
        static_assert(test_std::sender<simple_sender>);
        static_assert(requires{ Domain::transform_env(simple_sender{}, env{}); });
        static_assert(noexcept(Domain::transform_env(simple_sender{}, env{})));
        {
            using type = decltype(Domain::transform_env(simple_sender{}, env{}));
            static_assert(std::same_as<env, type>);
            assert(env{17} == Domain::transform_env(simple_sender{}, env{17}));
        }
        {
            env e{17};
            using type = decltype(Domain::transform_env(simple_sender{}, e));
            static_assert(std::same_as<env&, type>);
            assert(&e == &Domain::transform_env(simple_sender{}, e));
        }
        {
            env const ce{17};
            using type = decltype(Domain::transform_env(simple_sender{}, ce));
            static_assert(std::same_as<env const&, type>);
            assert(&ce == &Domain::transform_env(simple_sender{}, ce));
        }

        static_assert(test_std::sender<tagged_sender<true>>);
        static_assert(requires{ tag<true>{}.transform_env(tagged_sender<true>(), env{}); });
        static_assert(std::same_as<env, decltype(tag<true>{}.transform_env(tagged_sender<true>(), env{}))>);
        static_assert(requires{ Domain::transform_env(tagged_sender<true>{}, env{}); });
        static_assert(noexcept(Domain::transform_env(tagged_sender<true>{}, env{})));
        {
            using type = decltype(Domain::transform_env(tagged_sender<true>{}, env{}));
            static_assert(std::same_as<env, type>);
            assert(env{17} == Domain::transform_env(simple_sender{}, env{17}));
        }
    }

    template <typename Domain>
    auto test_apply_sender(Domain) -> void
    {
        static_assert(test_std::sender<simple_sender>);
        static_assert(not requires{ Domain().apply_sender(simple_tag{}, simple_sender{}); });
        static_assert(not requires{ Domain().apply_sender(simple_tag{}, simple_sender{}, 0); });

        static_assert(not requires{ Domain().apply_sender(tag<true>{}, simple_sender{}); });
        static_assert(requires{ Domain().apply_sender(tag<true>{}, simple_sender{}, 0); });
        static_assert(not requires{ Domain().apply_sender(tag<true>{}, simple_sender{}, 0, 0); });
        static_assert(requires{ Domain().apply_sender(tag<true>{}, simple_sender{}, 0, 0, 0); });
        static_assert(noexcept(Domain().apply_sender(tag<true>{}, simple_sender{}, 0)));
        static_assert(noexcept(Domain().apply_sender(tag<true>{}, simple_sender{}, 0, 0, 0)));
        static_assert(not noexcept(Domain().apply_sender(tag<false>{}, simple_sender{}, 0)));
        static_assert(not noexcept(Domain().apply_sender(tag<false>{}, simple_sender{}, 0, 0, 0)));

        assert(17 == Domain().apply_sender(tag<true>{}, simple_sender{}, 17));
        assert(15 == Domain().apply_sender(tag<true>{}, simple_sender{}, 4, 5, 6));

        bool flag{};
        assert(&flag == &Domain().apply_sender(tag<true>{}, simple_sender{}, flag));
    }
}

// ----------------------------------------------------------------------------

auto main() -> int
{
    test_std::default_domain domain{};
    test_transform_sender(domain);
    test_transform_env(domain);
    test_apply_sender(domain);
}