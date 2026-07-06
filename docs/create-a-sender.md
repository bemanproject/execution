# Create A Sender

This page describes how to create senders. The examples used aren't
necessarily super useful but are intended to describe the different
aspects of creating a sender.

## Asynchronous Stack

The first example is an asynchronous stack: popping from the stack
is an asynchronous operation which either completes immediately if
there is work or completes when new work becomes available. Pushing
to the asynchronous stack succeeds immediately. It could be an
extension to impose a limit on the stack size and make pushing also
a asynchronous operation making the two operation somewhat symmetric.

The interface to the asynchronous stack could look like this:

```c++
template <typename T>
class asynchronous_stack {
    std::stack<T> stack;
    // TODO
public:
    void       push(T value);
    pop_sender pop();
};
```

This immediately raises the question what is `pop_sender`? Well,
it is a sender, i.e., `ex::sender<asynchronous_stack<T>::pop_sender>`
is `true` which completes successfully with a `T` value:

```c++
template <typename T>
class asynchronous_stack {
public:
    struct pop_sender {
        using sender_concept = ex::sender_tag;
        template <typename...>
        static consteval auto get_completion_signatures() {
            return ex::completion_signatures<ex::set_value_t(T)>{};
        }
        // TODO
    };

    void       push(T);
    pop_sender pop() { return pop_sender{/* TODO */}; }
};

static_assert(ex::sender<asynchronous_stack<int>::pop_sender>);
static_assert(ex::sender_in<asynchronous_stack<int>::pop_sender>);
```

The declaration of `sender_concept` indicates to the `ex::execution`
library that `pop_sender` actually implements (models) the `concept`
`ex::sender`. That is required for various interfaces taking senders
as arguments.

The function `get_completion_signatures()` is a compile-time function
which provides access to the possible completions of the sender
`pop_sender`. The current declaration states that `pop_sender`
always completes successfully with a value of type `T`. For example,
this sender could be used in a work graph where the result is sent to
a function:

```c++
asynchronous_stack<int> st;
auto sender = st.pop() | ex::then([](int v) noexcept { std::cout << "got value=" << v << "\n"; });
```

Of course, `sender` doesn't actually do anything, yet. The above
code is just showing how to create a work graph using this sender.
To actually make it do some work we'll need to implement its logic.
The logic for `pop_sender` will somehow need to look at an
`asynchronous_stack<T>` to either extract a value if one is present
or to register interest in values becoming available. In both cases
it needs a reference to the `asynchronous_stack<T>`:

```c++
template <typename T>
class asynchronous_stack {
public:
    struct pop_sender {
        using sender_concept = ex::sender_tag;
        template <typename...>
        static consteval auto get_completion_signatures() {
            return ex::completion_signatures<ex::set_value_t(T)>{};
        }

        asynchronous_stack& self;
        // TODO
    };

    void       push(T);
    pop_sender pop() { return pop_sender{*this}; }
};
```

The `pop_sender` itself actually doesn't really do any work: it
wouldn't even know where to send any results to. Instead, it is the
interface used to `ex::connect` to a receiver which is provides a
way to notify the completion. When `ex::connect(sndr, rcvr)` is
invoked (and there are no customizations) it behaves as if
`sndr.connect(rcvr)` was invoked. Thus, `pop_sender` should have a
`connect` member function taking a receiver. The result of invoking
this function is an operation state:

```c++
template <typename T>
class asynchronous_stack {
    template <ex::receiver Rcvr>
    struct state {
        using operation_state_concept = ex::operation_state_tag;
        /* TODO */
        void start() & noexcept { /* TODO */ }
    };
public:
    struct pop_sender {
        // ...

        asynchronous_stack& self;
        template <ex::receiver Rcvr>
        auto connect(Rcvr&& rcvr) const {
            static_assert(ex::operation_state<state<Rcvr>>);
            return state<Rcvr>{/* TODO */};
        }
    };

    // ...
};
```

The `state` is identified as an `ex::operation_state` similarly to
a sender: by declaring `operation_state_concept` as an alias for
`ex::operation_state_tag` (or a class derived thereof) it is
identified as an `ex::operation_state`. To implement the `concept`
`ex::operation_state` the `state` class also needs a `start()`
function which never throws (any errors would be reported via
suitable completion signature on the erceiver).

The actual work happens when `state`s `start()` function is
invoked: at that point it is checked whether there is any value
already available in the `asynchronous_stack<T>` and, if that is
the case, it is passed to the receiver's `set_value` function.
Thus, the `state` will need a reference to the `asynchronous_stack<T>`
and the receiver:

```c++
template <typename T>
class asynchronous_stack {
    template <ex::receiver Rcvr>
    struct state {
        using operation_state_concept = ex::operation_state_tag;
        std::remove_cvref_t<Rcvr> rcvr;
        asynchronous_stack&       self;
        void start() & noexcept { /* TODO */ }
    };
public:
    struct pop_sender {
        // ...

        asynchronous_stack& self;
        template <ex::receiver Rcvr>
        auto connect(Rcvr&& rcvr) const {
            return state<Rcvr>{std::forward<Rcvr>(rcvr), self};
        }
    };

    // ...
};
```

With that we have created enough of the scaffolding to actually give
sender an initial try: for a test we can just use `asynchronous_stack<int>`
and upon `start()`ing the operation state we could just complete with a
known value, e.g.:

```c++
template <typename T>
class asynchronous_stack {
    template <ex::receiver Rcvr>
    struct state {
        using operation_state_concept = ex::operation_state_tag;
        std::remove_cvref_t<Rcvr> rcvr;
        asynchronous_stack&       self;
        void start() & noexcept { ex::set_value(std::move(rcvr), 17); }
    };
    // ...
};

int main() {
    asynchronous_stack<int> st;
    auto sender = st.pop() | ex::then([](int v) noexcept { std::cout << "got value=" << v << "\n"; });
    ex::sync_wait(std::move(sender));
}
```

Running the resulting executable (on my Mac that is
`./build/simple-Darwin/stagedir/bin/beman_execution.example.tutorial-create-a-sender`)
results in printing that it got value 17:

```
got value=17
```

So, we got something running but it isn't quite the correct logic.
It is easy to implement the necessary logic when there are already
values in the stack: for that the `push(T)` function just adds
values to a `std::stack<T>` and `state::start` checks whether there
is a value and produces a corresponding result if that is the case:

```c++
template <typename T>
class asynchronous_stack {
    std::stack<T> stack;

    template <ex::receiver Rcvr>
    struct state {
        // ...
        void start() & noexcept {
            if (not this->self.stack.empty()) {
                T value(std::move(this->self.stack.top()));
                this->self.stack.pop();
                ex::set_value(std::move(rcvr), std::move(value));
            }
            // TODO
        }
    };
    void       push(T value) { this->stack.push(std::move(value)); }
    // ...
};

int main() {
    asynchronous_stack<int> st;
    auto sender = st.pop() | ex::then([](int v) noexcept { std::cout << "got value=" << v << "\n"; });

    for (int value{1}; value < 4; ++ value) {
        st.push(value);
    }

    for (int value{1}; value < 4; ++ value) {
        ex::sync_wait(sender);
    }
}
```

The `push` function unconditionally pushes elements into the `stack`
and the `start` function doesn't do anything if there are no values.
If the `stack` is `empty`, the `start` function should register the
operation state with the `asynchronous_stack` and `push` should
verify if there is a registered operation state. Since the operation
state objects live at least until they complete they can be used
for an intrusive list: there is no need to do a separate allocation.
The list is built by deriving from a `struct node` which simply has
a `next` pointer to another `node` and a `virtual` function `complete`
taking a `T` object as argument. The `asynchronous_stack` keeps a
list of `node`s named `awaiting` in the example. This example doesn't
to be fair: the most recently started `pop_sender` is the first one
to complete. Since the `node` becomes a base class of `state`, it
is easiest to give `state` a constructor:

```c++
template <typename T>
class asynchronous_stack {
    struct node {
        node* next{};
        virtual void complete(T) = 0;
    };
    std::stack<T> stack;
    node*         awaiting{};

    template <ex::receiver Rcvr>
    struct state: node {
        // ...
        state(Rcvr&& r, asynchronous_stack& s): rcvr(std::forward<Rcvr>(r)), self(s) {}
        void start() & noexcept {
            if (not this->self.stack.empty()) {
                T value(std::move(this->self.stack.top()));
                this->self.stack.pop();
                ex::set_value(std::move(rcvr), std::move(value));
            }
            else {
                this->next = std::exchange(this->self.awaiting, this);
            }
        }
        void complete(T value) override {
            ex::set_value(std::move(rcvr), std::move(value));
        }
    };
    void       push(T value) { this->stack.push(std::move(value)); }
    // ...
};
```

The new logic added is in `start` which now adds itself to the list
of `awaiting` operation state by replacing `awaiting` with itself
an setting its `next` pointer to the previous head of the list. To
take advantage of already `awaiting` operation states the `push`
function needs to check if there is any `node` and, if so, extract
this node `n` and invoke `n->complete(std::move(value))`:

```c++
template <typename T>
class asynchronous_stack {
    // ...
    void       push(T value) {
        if (this->awaiting) {
            std::exchange(this->awaiting, this->awaiting->next)->complete(std::move(value));
        }
        else {
            this->stack.push(std::move(value));
        }
    }
};
```

To actually demonstrate the functionality it becomes necessary to
`start` some `pop_sender`s before adding more work. To do so, the
`main` function uses a `ex::counting_scope`: invoking `ex::spawn(work,
scope.get_token())` results in `connect`ing `work` to a receiver
and `start`ing the result operation state. So, work is `push`ed in
two groups of three items. After the first three items are `push`ed
six `pop()` requests are `spawn`ed. The first three of them will
complete immediately. The second three will complete once more
work gets `push`ed:

```c++
int main() {
    ex::counting_scope      scope;
    asynchronous_stack<int> st;
    auto sender = st.pop() | ex::then([](int v) noexcept { std::cout << "got value=" << v << "\n"; });

    for (int value{1}; value < 4; ++ value) {
        st.push(value);
    }
    std::cout << "pushed 1,2,3\n";

    for (int value{1}; value < 7; ++ value) {
        ex::spawn(st.pop() | ex::then([value](int v) noexcept {
            std::cout << "got value=" << v << " for request " << value << "\n";
        }), scope.get_token());
    }

    std::cout << "requested 6 values\n";

    for (int value{4}; value < 7; ++ value) {
        st.push(value);
    }
    std::cout << "pushed 4,5,6\n";

    ex::sync_wait(scope.join());
}
```

The `counting_scope` needs to be `join`ed before it can be destroyed.
`scope.join()` returns a sender which completes once all held work
completes. The example is carefully setup to contain no outstanding
work when synchronously awaiting `scope.join()`. However, if more
`pop()` requests get `spawn`ed into the `scope` than there are
elements which are `push`ed, the operation will never complete! To
deal with that situation it is possibly to `scope.request_stop()`
before awaiting completion of `scope.join()`. The `scope.request_stop()`
will stop all outstanding work - assuming it can be stopped. But
`pop_sender` has no support, yet, for getting stopped.

The way to support stop requests is to set up a stop callback using
the stop token associated with a receiver `r`. To get the stop token
`ex::get_stop_token(ex::get_env(r))` can be used. Such a stop token
support a callback which can then be used to complete with
`set_stopped`:

```c++
template <typename T>
class asynchronous_stack {
    // ...
    template <ex::receiver Rcvr>
    struct state: node {
        // ...
        struct stop_fun {
            state& self;
            void operator()() noexcept {
                state& self = this->self;
                self.callback.reset();
                ex::set_stopped(std::move(self.rcvr));
            }
        };
        using stop_token_t = ex::stop_token_of_t<decltype(ex::get_env(std::declval<Rcvr&>()))>;
        using callback_t = ex::stop_callback_for_t<stop_token_t, stop_fun>;
        std::optional<callback_t> callback; 
        state(Rcvr&& r, asynchronous_stack& s): rcvr(std::forward<Rcvr>(r)), self(s) {}
        void start() & noexcept {
            if (not this->self.stack.empty()) {
                T value(std::move(this->self.stack.top()));
                this->self.stack.pop();
                ex::set_value(std::move(rcvr), std::move(value));
            }
            else {
                this->callback.emplace(ex::get_stop_token(ex::get_env(this->rcvr)), *this);
                this->next = std::exchange(this->self.awaiting, this);
            }
        }
        void complete(T value) override {
            this->callback.reset();
            ex::set_value(std::move(rcvr), std::move(value));
        }
    };
    struct pop_sender {
        using sender_concept = ex::sender_tag;
        template <typename...>
        static consteval auto get_completion_signatures() {
            return ex::completion_signatures<ex::set_value_t(T), ex::set_stopped_t()>{};
        }
        // ...
    };
    void       push(T value) { this->stack.push(std::move(value)); }
    // ...
};
```
