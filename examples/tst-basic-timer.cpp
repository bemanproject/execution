// examples/tst-basic-timer.cpp                                       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// ----------------------------------------------------------------------------

#include <beman/execution/execution.hpp>
#include <iostream>
#include "tst.hpp"
namespace ex = beman::execution;

// ----------------------------------------------------------------------------

struct receiver {
    using receiver_concept = ex::receiver_t;

    auto set_value() && noexcept {
        std::cout << "timer done\n";
    }
    auto set_error(const std::exception_ptr&) && noexcept {
        std::cout << "timer error\n";
    }
    auto set_stopped() && noexcept {
        std::cout << "timer stopped\n";
    }
};

std::ostream& fmt_now(std::ostream& os) {
    return os << std::chrono::system_clock::now();
}

auto main() -> int {
    std::size_t count{3};
    ex::sync_wait(
        tst::repeat_effect_until(
            ex::just() | ex::then([]() noexcept { std::cout << "effect\n"; }),
            [&count]() noexcept { return --count == 0u; })
    );

    tst::timer timer{};
    std::cout << fmt_now << ": start\n";
    ex::sync_wait(
        ex::when_all(
            tst::resume_after(timer.get_token(), std::chrono::milliseconds(1000))
                | ex::then([]() noexcept { std::cout << fmt_now << ": 1000ms timer fired\n"; }),
            tst::resume_after(timer.get_token(), std::chrono::milliseconds(500))
                | ex::then([]() noexcept { std::cout << fmt_now << ": 500ms timer fired\n"; }),
            tst::resume_after(timer.get_token(), std::chrono::milliseconds(1500))
                | ex::then([]() noexcept { std::cout << fmt_now << ": 1500ms timer fired\n"; }),
            timer.when_done() | ex::then([]() noexcept { std::cout << fmt_now << ": timer done\n"; }),
            ex::just()
        )
    );

}
