// examples/stop_token.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <condition_variable>
#include <iostream>
#include <exception>
#include <latch>
#include <mutex>
#include <thread>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
#else
#include <beman/execution/execution.hpp>
#include <beman/execution/stop_token.hpp>
#endif

namespace exec = beman::execution;

// The stop token classes are used to cancel work. Each
// stop token may be connected to a stop source on which
// cancellation can be requested using source.request_stop().
// The stop token reflects whether stop was requested on any
// source.
//
// There are two different needs on how the cancallation is
// indicated:
// - Active work, e.g., doing a lengthy computation,
//   would occasionally check the results are still needed
//   and abort the computation if it is not. To do so, the
//   function would use token.stop_requested() which yields
//   true once stopping was requested.
// - Inactive work, e.g., something waiting for input from
//   a user or a network connection is asleep and can't
//   reasonably check whether stopping was requested. Instead,
//   a callback gets registered which gets triggered when
//   stopping is requested. This callback can then cancel
//   the work. The callback is registered by creating an
//   object using the token's callback_type and deregistered
//   when this object is destroyed.
//
// The two use-cases are shown in the functions active() and
// inactive() respectively: they are run as threads from main()
// using a token associated with a stop source. Once both
// threads are started stopping is requested and the threads
// are joined.

// TODOs:
// The example can be simplified once standard library implementation
// catch-up. The example as is still properly shows things.
// - This example should really use std::latch but, sadly, that
//   doesn't seem to be readily enabled on MacOS.
// - std::print isn't available everywhere, yet. Let's try a simple
//   placeholder.
namespace {
::std::mutex io_lock;
void         print(std::string_view text, auto&&...) {
    const std::lock_guard guard(io_lock);
    ::std::cout << text;
}

template <typename Token>
auto active(const Token& token) -> void {
    auto i{0ull};
    while (not token.stop_requested()) {
        // do work
        ++i;
    }
    print("active thread done: {}\n", i);
}

template <typename Token, typename Callback>
struct stop_callback_for_t {
    exec::stop_callback_for_t<Token, Callback> cb;
    stop_callback_for_t(const Token& token, Callback callback) : cb(token, callback) {}
};

#ifdef __cpp_lib_latch
template <typename Token>
auto inactive(const Token& token) -> void {
    ::std::latch              latch(1);
    const stop_callback_for_t cb(token, [&latch] { latch.count_down(); });

    latch.wait();
    print("inactive thread done (latch)\n");
}
#else
template <typename Token>
auto inactive(Token token) -> void {
    ::std::condition_variable cond;
    stop_callback_for_t       cb(token, [&cond] { cond.notify_one(); });

    ::std::mutex       lock;
    ::std::unique_lock guard(lock);
    cond.wait(guard, [token] { return token.stop_requested(); });
    print("inactive thread done (condition_variable)\n");
}
#endif
} // namespace

auto main() -> int {
    try {

        exec::stop_source source;
        ::std::thread     act([token = source.get_token()] { active(token); });
        ::std::thread     inact([token = source.get_token()] { inactive(token); });

        print("threads started\n");
        source.request_stop();
        print("threads cancelled\n");

        act.join();
        inact.join();
        print("done\n");
    } catch (const std::exception& ex) {
        std::cout << "ERROR: " << ex.what() << "\n";
    }
}
