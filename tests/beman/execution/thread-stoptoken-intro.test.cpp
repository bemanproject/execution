// src/beman/execution/tests/thread-stoptoken-intro.test.cpp
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/execution/detail/immovable.hpp>
#include <test/execution.hpp>
#include <test/stop_token.hpp>
#include <utility>
#if false && defined(BEMAN_HAS_MODULES) //-dk:TODO module disabled
import beman.execution;
#else
#include <beman/execution/stop_token.hpp>
#endif

namespace {
struct test_source : test_detail::immovable {
    struct callback_base {
        callback_base()                                                = default;
        callback_base(callback_base&&)                                 = default;
        callback_base(const callback_base&)                            = default;
        virtual ~callback_base()                                       = default;
        auto         operator=(callback_base&&) -> callback_base&      = default;
        auto         operator=(const callback_base&) -> callback_base& = default;
        virtual auto call() -> void                                    = 0;
    };
    struct token {
        template <typename Fun>
        struct callback_type : callback_base {
            Fun          d_fun;
            test_source* d_source{};
            callback_type(token t, Fun fun) : d_fun(fun), d_source(t.d_source) { this->d_source->add(this); }
            callback_type(callback_type&&)      = delete;
            callback_type(const callback_type&) = delete;
            ~callback_type() override { this->d_source->remove(this); }
            auto operator=(callback_type&&) -> callback_type&      = delete;
            auto operator=(const callback_type&) -> callback_type& = delete;
            auto call() -> void override { this->d_fun(); }
        };

        test_source* d_source;
        auto         stop_possible() const noexcept -> bool { return true; }
        auto         stop_requested() const noexcept -> bool { return this->d_source->d_value; }
        auto         operator==(const token&) const -> bool;
    };
    struct no_call : callback_base {
        auto call() -> void override {}
    };

    ::std::mutex              d_lock;
    ::std::condition_variable d_cond;
    ::std::thread::id         d_id{};
    bool                      d_in_progress{};
    bool                      d_value{};
    no_call                   d_no_call;
    callback_base*            d_callback{&this->d_no_call};

    auto get_token() const -> token { return {const_cast<test_source*>(this)}; }
    auto stop_possible() const noexcept -> bool { return true; }
    auto stop_requested() const noexcept -> bool { return this->d_value; }
    auto request_stop() -> bool {
        bool           rc{};
        callback_base* cb{};
        {
            ::std::lock_guard guard(this->d_lock);
            rc                  = ::std::exchange(this->d_value, true);
            this->d_in_progress = true;
            this->d_id          = ::std::this_thread::get_id();
            cb                  = ::std::exchange(this->d_callback, &this->d_no_call);
        }
        cb->call();
        {
            ::std::lock_guard guard(this->d_lock);
            this->d_in_progress = false;
            this->d_id          = ::std::thread::id();
        }
        this->d_cond.notify_one();
        return rc;
    }
    auto add(callback_base* cb) -> void {
        {
            ::std::lock_guard guard(this->d_lock);
            if (not this->d_value) {
                this->d_callback = cb;
                return;
            }
        }
        cb->call();
    }
    auto remove(callback_base*) -> void {
        ::std::unique_lock guard(this->d_lock);
        this->d_cond.wait(
            guard, [this] { return this->d_in_progress == false || this->d_id == ::std::this_thread::get_id(); });
        ::std::exchange(this->d_callback, &this->d_no_call);
    }
};
} // namespace

TEST(thread_stoptoken_intro) {
    // Plan:
    // - Given a test stoppable source.
    // - When running the stop source tests.
    // - Then the tests pass.
    // As the tests [static_]ASSERT upon failure, incorrect stop sources are not tests.
    // Reference: [thread.stoptoken.intro]

    test::stop_source([] { return ::test_source(); });
}
