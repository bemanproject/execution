// src/beman/execution/tests/exec-snd-apply.test.cpp                -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/detail/apply_sender.hpp>
#endif
#include <test/execution.hpp>
#include <utility>

// ----------------------------------------------------------------------------

namespace {
template <int>
struct arg {
    int value{};
};
struct empty_domain {
    int value{};
};
struct empty_tag {};
struct non_sender {};

template <bool Noexcept>
struct domain {
    int  value{};
    auto apply_sender(auto tag, auto&& sender, auto&&...) noexcept(Noexcept) {
        return this->value + tag.value + sender.value;
    }
};

template <bool Noexcept>
struct tag {
    int  value{};
    auto apply_sender(auto&& sender, auto&&...) noexcept(Noexcept) { return this->value + sender.value; }
};

struct sender {
    using sender_concept = test_std::sender_t;
    int value{};
};

template <bool Expect, bool Noexcept, typename Sender, typename... Args>
auto has_apply_sender(auto domain, auto tag, Sender&& sender, Args&&... args) {
    static_assert(Expect == requires {
        test_std::apply_sender(domain, tag, std::forward<Sender>(sender), std::forward<Args>(args)...);
    });
    if constexpr (Expect) {
        static_assert(Noexcept == noexcept(test_std::apply_sender(
                                      domain, tag, std::forward<Sender>(sender), std::forward<Args>(args)...)));
        ASSERT(domain.value + 11 ==
               test_std::apply_sender(domain, tag, std::forward<Sender>(sender), std::forward<Args>(args)...));
    }
}
} // namespace

TEST(exec_snd_apply) {
    static_assert(test_std::sender<sender>);
    has_apply_sender<false, false>(empty_domain{}, empty_tag{}, non_sender{});
    has_apply_sender<false, false>(empty_domain{}, empty_tag{}, sender{11});
    has_apply_sender<false, false>(empty_domain{}, empty_tag{}, sender{11}, arg<0>{});
    has_apply_sender<false, false>(empty_domain{}, empty_tag{}, sender{11}, arg<0>{}, arg<1>{});

    has_apply_sender<true, true>(domain<true>{13}, tag<true>{17}, sender{11});
    has_apply_sender<true, true>(domain<true>{13}, tag<true>{17}, sender{11}, arg<0>{});
    has_apply_sender<true, true>(domain<true>{13}, tag<true>{17}, sender{11}, arg<0>{}, arg<1>{});
    has_apply_sender<true, false>(domain<false>{13}, tag<true>{17}, sender{11});
    has_apply_sender<true, false>(domain<false>{13}, tag<true>{17}, sender{11}, arg<0>{});
    has_apply_sender<true, false>(domain<false>{13}, tag<true>{17}, sender{11}, arg<0>{}, arg<1>{});

    has_apply_sender<true, true>(empty_domain{}, tag<true>{17}, sender{11});
    has_apply_sender<true, true>(empty_domain{}, tag<true>{17}, sender{11}, arg<0>{});
    has_apply_sender<true, true>(empty_domain{}, tag<true>{17}, sender{11}, arg<0>{}, arg<1>{});
    has_apply_sender<true, false>(empty_domain{}, tag<false>{17}, sender{11});
    has_apply_sender<true, false>(empty_domain{}, tag<false>{17}, sender{11}, arg<0>{});
    has_apply_sender<true, false>(empty_domain{}, tag<false>{17}, sender{11}, arg<0>{}, arg<1>{});
}
