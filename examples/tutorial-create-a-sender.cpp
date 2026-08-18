// examples/tutorial/create-a-sender.cpp                              -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <iostream>
#include <optional>
#include <stack>
#ifdef BEMAN_HAS_MODULES
import beman.execution;
import beman.execution.detail;
#else
#include <beman/execution/execution.hpp>
#endif

namespace ex = beman::execution;

namespace {
template <typename T>
class asynchronous_stack {
    struct node {
        node*        next{};
        virtual void complete(T) = 0;
    };
    std::stack<T> stack;
    node*         awaiting{};

    template <ex::receiver Rcvr>
    struct state : node {
        using operation_state_concept = ex::operation_state_tag;
        struct stop_fun {
            state& st;
            void   operator()() noexcept {
                std::cout << "stop callback start\n";
                state& s = this->st;
                this->st.callback.reset();
                for (auto it{&this->st.self.awaiting}; it; it = &(*it)->next) {
                    if (*it == &this->st) {
                        *it = this->st.next;
                        break;
                    }
                }
                std::cout << "stop callback completing\n";
                ex::set_stopped(std::move(s.rcvr));
                std::cout << "stop callback done\n";
            }
        };
        using stop_token_t = ex::stop_token_of_t<decltype(ex::get_env(std::declval<Rcvr&>()))>;
        using callback_t   = ex::stop_callback_for_t<stop_token_t, stop_fun>;
        std::remove_cvref_t<Rcvr> rcvr;
        asynchronous_stack&       self;
        std::optional<callback_t> callback;
        state(Rcvr&& r, asynchronous_stack& s) : rcvr(std::forward<Rcvr>(r)), self(s) {}
        void start() & noexcept {
            if (not this->self.stack.empty()) {
                T value(std::move(this->self.stack.top()));
                this->self.stack.pop();
                ex::set_value(std::move(rcvr), std::move(value));
            } else {
                this->next = std::exchange(this->self.awaiting, this);
                this->callback.emplace(ex::get_stop_token(ex::get_env(this->rcvr)), stop_fun{*this});
            }
        }
        void complete(T value) override {
            this->callback.reset();
            ex::set_value(std::move(rcvr), std::move(value));
        }
    };

  public:
    struct pop_sender {
        using sender_concept = ex::sender_tag;
        template <typename...>
        static consteval auto get_completion_signatures() {
            return ex::completion_signatures<ex::set_value_t(T), ex::set_stopped_t()>{};
        }

        asynchronous_stack& self;
        template <ex::receiver Rcvr>
        auto connect(Rcvr&& rcvr) const {
            static_assert(ex::operation_state<state<Rcvr>>);
            return state<Rcvr>{std::forward<Rcvr>(rcvr), self};
        }
    };

    void push(T value) {
        if (this->awaiting) {
            std::exchange(this->awaiting, this->awaiting->next)->complete(std::move(value));
        } else {
            this->stack.push(std::move(value));
        }
    }
    pop_sender pop() { return pop_sender{*this}; }
};

static_assert(ex::sender<asynchronous_stack<int>::pop_sender>);
static_assert(ex::sender_in<asynchronous_stack<int>::pop_sender>);

struct stop_test {
    using sender_concept = ex::sender_tag;
    template <typename...>
    static consteval auto get_completion_signatures() {
        return ex::completion_signatures<ex::set_value_t()>();
    }
    template <typename Rcvr>
    struct state {
        using operation_state_concept = ex::operation_state_tag;

        struct cb {
            state* self;
            auto   operator()() const noexcept -> void {
                std::cout << "cb\n";
                ex::set_value(std::move(self->rcvr));
            }
        };
        using callback = ex::stop_callback_for_t<ex::stop_token_of_t<ex::env_of_t<Rcvr>>, cb>;

        std::remove_cvref_t<Rcvr> rcvr;
        std::optional<callback>   callb;
        auto                      start() & noexcept -> void {
            std::cout << "stop_test start\n";
            this->callb.emplace(ex::get_stop_token(ex::get_env(this->rcvr)), cb{this});
        }
    };
    template <typename Rcvr>
    state<Rcvr> connect(Rcvr&& rcvr) const {
        return {std::forward<Rcvr>(rcvr)};
    }
};
} // namespace
// ----------------------------------------------------------------------------

int main() {
    std::cout << std::unitbuf;
#if 1
    asynchronous_stack<int> st;
    ex::counting_scope      scope;
    [[maybe_unused]] auto   sender = st.pop() | ex::then([](int v) { std::cout << "got value=" << v << "\n"; });

#if 1
    for (int value{1}; value < 4; ++value) {
        st.push(value);
    }
    std::cout << "pushed 1,2,3\n";

    int count{8};
    for (int value{1}; value < count; ++value) {
        ex::spawn(st.pop() | ex::then([value](int v) noexcept {
                      std::cout << "got value=" << v << " for request " << value << "\n";
                  }) | ex::upon_stopped([value] noexcept { std::cout << "request " << value << " was stopped\n"; }),
                  scope.get_token());
    }

    std::cout << "requested " << (count - 1) << " values\n";

    for (int value{4}; value < 7; ++value) {
        st.push(value);
    }
    std::cout << "pushed 4,5,6\n";
#endif

    std::cout << "requesting stop\n";
    scope.request_stop();
    std::cout << "requested stop\n";
    ex::sync_wait(scope.join() | ex::then([] { std::cout << "joined\n"; }));
    std::cout << "joined\n";
#else

    std::optional<ex::inplace_stop_source> source1{};
    source1.emplace();
    std::optional<ex::inplace_stop_source> source2{};
    source2.emplace();
    struct receiver {
        using receiver_concept = ex::receiver_tag;
        std::optional<ex::inplace_stop_source>& source1;
        std::optional<ex::inplace_stop_source>& source2;
        auto query(ex::get_stop_token_t) const noexcept { return this->source2->get_token(); }
        auto get_env() const noexcept {
            std::cout << "get_env\n";
            return *this;
        }
        auto set_value() noexcept {
            std::cout << "receiver::set_value\n";
            source1.reset();
            std::cout << "receiver::set_value done\n";
        }
        auto set_stopped() noexcept { std::cout << "receiver::set_stopped\n"; }
    };
#if 1
    auto sws(ex::connect(ex::detail::stop_when(stop_test(), source1->get_token()), receiver{source1, source2}));
#else
    auto sws(ex::connect(stop_test(), receiver{source1, source2}));
#endif
    std::cout << "start\n";
    ex::start(sws);
    std::cout << "request stop\n";
    source1->request_stop();
    std::cout << "requested stop\n";
#endif
}
