// src/beman/execution/tests/notify.test.cpp                        -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail.notify;
#else
#include <beman/execution/detail/notify.hpp>
#include <beman/execution/execution.hpp>
#endif

// ----------------------------------------------------------------------------

TEST(notify) {
    static_assert(std::same_as<const test_detail::notify_t, decltype(test_detail::notify)>);

    {
        test_detail::notifier n;
        auto                  s{test_detail::notify(n)};
        static_assert(test_std::sender<decltype(s)>);
        static_assert(test_std::sender_in<decltype(s)>);
        n.complete();
        test_std::sync_wait(s);
    }
    {
        struct receiver {
            using receiver_concept = test_std::receiver_t;
            bool& result;
            auto  set_value() && noexcept -> void { this->result = true; }
        };
        bool                  finish1{};
        bool                  finish2{};
        test_detail::notifier n;
        auto                  op1{test_std::connect(test_detail::notify(n), receiver{finish1})};
        test_std::start(op1);
        auto op2{test_std::connect(test_detail::notify(n), receiver{finish2})};
        test_std::start(op2);
        ASSERT(finish1 == false);
        ASSERT(finish2 == false);
        n.complete();
        ASSERT(finish1 == true);
        ASSERT(finish2 == true);
    }
}
