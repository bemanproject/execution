#include <beman/execution/execution.hpp>
#include <iostream>
#include <format>
#include <thread>

namespace ex = beman::execution;

namespace {
    std::thread::id main_thread_id = std::this_thread::get_id();

    template<typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args) {
        auto thread_id = std::this_thread::get_id();
        auto thread_name = thread_id == main_thread_id ? "main" : (std::stringstream{} << thread_id).str();
        std::clog << std::format("[thread-{}] {}\n", thread_name, std::format(fmt, std::forward<Args>(args)...));
    }
}

int main() {
    ex::run_loop ex_context1;
    std::thread thread1{
        [&ex_context1] {
            debug("ex_context1 run...");
            ex_context1.run();
        }
    };

    ex::run_loop ex_context2;
    std::thread thread2{
        [&ex_context2] {
            debug("ex_context2 run...");
            ex_context2.run();
        }
    };

    ex::sender auto snd = ex::just() | ex::then([] {
        debug("#1");
    }) | ex::continues_on(ex_context1.get_scheduler()) | ex::then([] {
        debug("#2");
    }) | ex::continues_on(ex_context2.get_scheduler()) | ex::then([] {
        debug("#3");
    });

    ex::sync_wait(snd);

    ex_context1.finish();
    ex_context2.finish();

    thread2.join();
    thread1.join();
}
