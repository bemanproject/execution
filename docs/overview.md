<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# std::execution Overview
This page provides an overview of the components in `std::execution`. The documentation on this page doesn’t represent all details of the specification. However, it should capture enough details to be a suitable resource to determine how the various components are used.

For each of the components a summary view is provided. To get more details expand the respective section.
## Terms
This section defines a few terms used throughout the description on this page. The terms aren’t taken from the specification and are, thus, somewhat informal.

<details>
<summary>completion signal</summary>
When an asynchronous operation completes it _signals_ its completion by calling a completion function on a <code><a href=‘#receiver’>receiver</a></code>:

- <code><a href=‘#set-value’>std::execution::set_value</a>(_receiver_, _args_...)</code> is called when an operation completes successfully. A call to this completion function is referred to as _value completion signal_.
- <code><a href=‘#set-error’>std::execution::set_error</a>(_receiver_, _error_)</code> is called when an operation fails to deliver its success results. A call to this completion function is referred to as _error completion signal_.
- <code><a href=‘#set-stopped’>std::execution::set_stopped</a>()</code> is called when an operation was cancelled. A call to this completion function is referred to as _cancellation completion signal_.
- Collectively the value, error, and cancellation completion signals are referred to as _completion signal_. Note that any <code><a href=‘#start’>start</a></code>ed asynchronous operation triggers exactly one completion signal.
</details>
<details>
<summary>environment</summary>
The term _enviroment_ refers to the bag of properties associated with an <code>_object_</code> by the call <code><a href=‘#get-env’>std::execution::get_env</a>(_object_)</code>. By default the environment for objects is empty (<code><a href=‘#empty-env’>std::execution::empty_env</a></code>). In particular, environments associated with <code><a href=‘#receiver’>receiver</a></code>s are used to provide access  to properties like the <a href=‘#get-stop-token’>stop token</a>, <a href=‘#get-scheduler’>scheduler</a>, or <a href=‘#get-allocator’>allocator</a> associated with the <code><a href=‘#receiver’>receiver</a></code>. The various properties associated with an object are accessed via <a href=‘#queries’>queries</a>.
</details>

## Concepts
This section lists the concepts from `std::execution`.

<details>
<summary><code>operation_state&lt;<i>State</i>&gt;</code></summary>

Operation states represent asynchronous operations ready to be <code><a href=‘#start’>start</a></code>ed or executing. Operation state objects are normally neither movable nor copyable. Once <code><a href=‘#start’>start</a></code>ed the object needs to be kept alive until a <a href=‘#completion-signal’>completion signal</a> is received. Users don’t interact with operation states explicitly except when implementing new sender algorithms.

Required members for <code>_State_</code>:

- The type `operation_state_concept` is an alias for `operation_state_t` or a type derived thereof.
- <code><i>state</i>.<a href=‘#start’>start</a>() & noexcept</code>

<details>
<summary>Example</summary>

This example shows a simple operation state object which immediately completes successfully without any values (as <code><a href=‘#just’></a>()</code> would do). Normally <code><a href=‘#start’>start</a>()</code> initiates an asynchronous operation completing at some point later.

```c++
template <std::execution::receiver Receiver>
struct example_state
{
    using operation_state_concept = std::execution::operation_state_t;
    std::remove_cvref_t<Receiver> receiver;

    auto start() & noexcept {
        std::execution::set_value(std::move(this->receiver));
    }
};

static_assert(std::execution::operation_state<example_state<SomeReceiver>>);
```
</details>
</details>
<details>
<summary><code>receiver&lt;<i>Receiver</i>&gt;</code></summary>

Receivers are used to receive <a href=‘#completion-signal’>completion signals</a>:
when an asynchronous operation completes the corresponding <a href=‘#completion-signal’>completion signal</a>
is called with the appropriate arguments. In addition receivers provide access to the
<a href=‘#environment’>environment</a> for the operation via the <a href=‘#get-env’><code>get_env</code></a> method.
Users don’t interact with receivers explicitly except when implementing new sender algorithms.

Required members for <code>_Receiver_</code>:

- The type `receiver_concept` is an alias for `receiver_t` or a type derived thereof`.
- Rvalues of type <code>_Receiver_</code> are movable.
- Lvalues of type <code>_Receiver_</code> are copyable.
- <code><a href=‘#get-env’>std::execution::get_env</a>(_receiver_)</code> returns an object. By default this operation returns <code><a href=‘empty-env’>std::execution::empty_env</a></code>.

Typical members for <code>_Receiver_</code>:

- <code><a href=‘get_env’>get_env</a>() const noexcept</code>
- <code><a href=‘set_value’>set_value</a>(args…) && noexcept -> void</code>
- <code><a href=‘set_error’>set_error</a>(error) && noexcept -> void</code>
- <code><a href=‘set_stopped’>set_stopped</a>() && noexcept -> void</code>

<details>
<summary>Example</summary>

The example receiver prints the name of each the received <a href=‘#completion-signal’>completion signal</a> before forwarding it to a receiver. It forwards the request for an environment (<code><a href=‘#get_env’>get_env</a></code>) to the nested receiver. This example is resembling a receiver as it would be used by a sender injecting logging of received signals.

```c++
template <std::execution::receiver NestedReceiver>
struct example_receiver
{
    using receiver_concept = std::execution::receiver_t;
    std::remove_cvref_t<NestedReceiver> nested;

    auto get_env() const noexcept {
        return std::execution::get_env(this->nested);
    }
    template <typename… A>
    auto set_value(A&&… a) && noexcept -> void {
        std::cout << “set_value\n”;
        std::execution::set_value(std::move(this->nested), std::forward<A>(a)…);
    }
    template <typename E>
    auto set_error(E&& e) && noexcept -> void {
        std::cout << “set_error\n”;
        std::execution::set_error(std::move(this->nested), std::forward<E>(e));
    }
    auto set_stopped() && noexcept -> void {
        std::cout << “set_stopped\n”;
        std::execution::set_stopped(std::move(this->nested));
    }
};

static_assert(std::execution::receiver<example_receiver<SomeReceiver>>);
```
</details>
</details>
<details>
<summary><code>receiver_of&lt;<i>Receiver, Completions</i>&gt;</code></summary>

The concept <code>receiver_of&lt;<i>Receiver, Completions</i>&gt;</code> tests whether <code><a href=‘#receiver’>std::execution::receiver</a>&lt;_Receiver_&gt;</code> is true and if an object of type <code>_Receiver_</code> can be invoked with each of the <a href=‘#completion-signal’>completion signals</a> in <code>_Completions_</code>.

<details>
<summary>Example</summary>

The example defines a simple <code><a href=‘#receiver’>receiver</a></code> and tests whether it models `receiver_of` with different <a href=‘#completion-signal’>completion signals</a> in <code>_Completions_</code>
(note that not all cases are true).

```c++
struct example_receiver
{
    using receiver_concept = std::execution::receiver_t;

    auto set_value(int) && noexcept ->void {}
    auto set_stopped() && noexcept ->void {}
};


// matching the exact signals models receiver_of:
static_assert(std::execution::receiver_of<example_receiver,
    std::execution::completion_signals<
        std::execution::set_value_t(int),
        std::execution::set_stopped_t()
    >);
// providing a superset of signal models models receiver_of:
static_assert(std::execution::receiver_of<example_receiver,
    std::execution::completion_signals<
        std::execution::set_value_t(int)
    >);
// providing only a subset of signals doesn’t model receiver_of:
static_assert(not std::execution::receiver_of<example_receiver,
    std::execution::completion_signals<
        std::execution::set_value_t(),
        std::execution::set_value_t(int)
    >);

```
</details>
</details>
<details>
<summary><code>scheduler&lt;<i>Scheduler</i>&gt;</code></summary>
Schedulers are used to specify the execution context where the asynchronous work is to be executed. A scheduler is a lightweight handle providing a <code><a href=‘#schedule’>schedule</a></code> operation yielding a <code><a href=‘sender’>sender</a></code> with a value <a href=‘#completion-signal’>completion signal</a> without parameters. The completion is on the respective execution context.

Requirements for <code>_Scheduler_</code>:
- The type <code>_Scheduler_::scheduler_concept</code> is an alias for `scheduler_t` or a type derived thereof`.
- <code><a href=‘#schedule’>schedule</a>(_scheduler_) -> <a href=‘sender’>sender</a></code>
- The <a href=‘#get-completion-scheduler’>value completion scheduler</a> of the <code><a href=‘sender’>sender</a></code>’s <a href=‘#environment’>environment</a> is the <code>_scheduler_</code>:
    _scheduler_ == std::execution::get_completion_scheduler&lt;std::execution::set_value_t&gt;(
       std::execution::get_env(std::execution::schedule(_scheduler_))
    )
- <code>std::equality_comparable&lt;_Scheduler_&gt;</code>
- <code>std::copy_constructible&lt;_Scheduler_&gt;</code>
</details>
<details>
<summary><code>sender&lt;<i>Sender</i>&gt;</code></summary>

Senders represent asynchronous work. They may get composed from multiple senders to model a workflow. Senders can’t be run directly. Instead, they are passed to a <a href=‘#sender-consumer’</a> which <code><a href=‘#connect’>connect</a></code>s the sender to a <code><a href=‘#receiver’>receiver</a></code> to produce an <code><a href=‘#operation-state’>operation_state</a></code> which may get started. When using senders to represent work the inner workings shouldn’t matter. They do become relevant when creating sender algorithms.

Requirements for <code>_Sender_</code>:
- The type <code>_Sender_::sender_concept</code> is an alias for `sender_t` or a type derived thereof or <code>_Sender_</code> is a suitable _awaitable_.
- <code><a href='get_env'>std::execution::get_env</a>(_sender_)</code> is valid. By default this operation returns <code><a href=‘empty-env’>std::execution::empty_env</a></code>.
- Rvalues of type <code>_Sender_</code> can be moved.
- Lvalues of type <code>_Sender_</code> can be copied.

Typical members for <code>_Sender_</code>:
- <code><a href=‘get_env’>get_env</a>() const noexcept</code>
- <code><a href=‘get_completion_signatures’>get_completion_signatures</a>(_env_) const noexcept -&gt; <a href=‘completion-signatures’>std::execution::completion_signatures</a>&lt;...&gt;</code>
- <code>_Sender_::completion_signatures</code> is a type alias for <code><a href=‘completion-signatures’>std::execution::completion_signatures</a>&lt;...&gt;</code> (if there is no <code><a href=‘get_completion_signatures’>get_completion_signatures</a></code> member).
- <code><a href=‘#connect’>connect</a>(_sender_, <a href=‘#receiver’>receiver</a>) -&gt; <a href=‘#operation-state’>operation_state</a></code>

<details>
<summary>Example</summary>
The example shows a sender implementing an operation similar to <code><a href=‘#just’>just</a>(_value)</code>.

```c++
struct example_sender
{
    template <std::execution::receiver Receiver>
    struct state
    {
        using operation_state_concept = std::execution::operation_state_t;
        std::remove_cvref_t<Receiver> receiver;
        int                           value;
        auto start() & noexcept {
            std::execution::set_value(
                std::move(this->receiver),
                this->value
            );
        }
    };
    using sender_concept = std::execution::sender_t;
    using completion_signatures = std::execution::completion_signatures<
        std::execution::set_value_t(int)
    >;

    int value{};
    template <std::execution::receiver Receiver>
    auto connect(Receiver&& receiver) const -> state<Receiver> {
        return { std::forward<Receiver>(receiver), this->value };
    }
};

static_assert(std::execution::sender<example_sender>);
```
</details>
</details>
<details>
<summary><code>sender_in&lt;<i>Sender, Env</i> = std::execution::empty_env&gt;</code></summary>

The concept <code>sender_in&lt;<i>Sender, Env</i>&gt;</code> tests whether <code>_Sender_</code> is a <code><a href=‘#sender’>sender</a></code>, <code>_Env_</code> is a destructible type, and <code><a href=‘#get_completion_signatures’>std::execution::get_completion_signatures</a>(_sender_, _env_)</code> yields a specialization of <code><a href=‘#completion_signatures’>std::execution::completion_signatures</a></code>.
</details>
<details>
<summary><code>sender_to&lt;<i>Sender, Receiver</i>&gt;</code></summary>

The concept <code>sender_to&lt;<i>Sender, Receiver</i>&gt;</code> tests if <code><a href=‘#sender_in’>std::execution::sender_in</a>&lt;_Sender_, <a href='#env_of_t'>std::execution::env_of_t</a>&lt;_Receiver_&gt;&gt;</code> is true, and if <code>_Receiver_</code> can receive all <a href=‘#completion-signals’>completion signals</a> which can be sent by <code>_Sender_</code>, and if <code>_Sender_</code> can be <code><a href=‘#connect’>connect</a></code>ed to <code>_Receiver_</code>.

To determine if <code>_Receiver_</code> can receive all <a href=‘#completion-signals’>completion signals</a> from <code>_Sender_</code> it checks that for each <code>_Signature_</code> in <code><a href=‘#get_completion_signals’>std::execution::get_completion_signals</a>(_sender_, std::declval&lt;<a href='#env_of_t'>std::execution::env_of_t</a>&lt;_Receiver_&gt;&gt;())</code> the test <code><a href=‘#receiver_of’>std::execution::receiver_of</a>&lt;_Receiver_, _Signature_&gt;</code> yields true. To determine if <code>_Sender_</code> can be <code><a href=‘#connect’>connect</a></code>ed to <code>_Receiver_</code> the concept checks if <code><a href=‘#connect’>connect</a>(std::declval&lt;_Sender_&gt;(), std::declval&lt;_Receiver_&gt;)</code> is a valid expression.
</details>
<details>
<summary><code>sends_stopped&lt;<i>Sender, Env</i> = std::execution::empty_env&gt;</code></summary>

The concept <code>sends_stopped&lt;<i>Sender, Env</i>&gt;</code> determines if <code>_Sender_</code> may send a <code><a href=‘#set_stopped’>stopped</a></code> <a href=‘#completion-signals’>completion signal</a>. To do so, the concepts determines if <code><a href=‘#get_completion_signals’>std::execution::get_completion_signals</a>(_sender_, _env_)</code> contains the signatures <code><a href=‘#set_stopped’>std::execution::set_stopped_t</a>()</code>.
</details>
<details>
<summary><code>stoppable_token&lt;_Token_&gt;</code></summary>
A <code>stoppable_token&lt;_Token_&gt;</code>, e.g., obtained via <code><a href=‘#get-stop-token’>std::execution::get_stop_token</a>(_env_)</code> is used to support cancellation of asynchronous operations. Using <code>_token_.stop_requested()</code> an active operation can poll whether it was requested to cancel. An inactive operation waiting for a notification can use an object of a specialization of the template <code>_Token_::callback_type</code> to get notified when cancellation is requested.

Required members for <code>_Token_</code>:

- <code>_Token_::callback_type&lt;_Callback_&gt;</code> can be specialized with a <code>std::callable&lt;_Callback_&gt;</code> type.
- <code>_token_.stop_requested() const noexcept -&gt; bool</code>
- <code>_token_.stop_possible() const noexcept -&gt; bool</code>
- <code>std::copyable&lt;_Token_&gt;</code>
- <code>std::equality_comparable&lt;_Token_&gt;</code>
- <code>std::swapable&lt;_Token_&gt;</code>
<blockquote>
<details>
<summary>Example: concept use</summary>
<div>

```c++
static_assert(std::execution::unstoppable_token<std::execution::never_stop_token>);
static_assert(std::execution::unstoppable_token<std::execution::stop_token>);
static_assert(std::execution::unstoppable_token<std::execution::inline_stop_token>);
```
</div>
</details>
<details>
<summary>Example: polling</summary>
<blockquote>
This example shows a sketch of using a <code>stoppable_token&lt;_Token_&gt;</code> to cancel an active operation. The computation in this example is represented as `sleep_for`.

```c++
void compute(std::stoppable_token auto token)
{
    using namespace std::chrono::literals;
    while (not token.stop_requested()) {
         std::this_thread::sleep_for(1s);
    }
}
```
</blockquote>
</details>
<details>
<summary>Example: inactive</summary>
<blockquote>
This example shows how an <code><a href=‘#operation-state’>operation_state</a></code> can use the <code>callback_type</code> together with a <code>_token_</code> to get notified when cancellation is requested.

```c++
template <std::execution::receiver Receiver>
struct example_state
{
    struct on_cancel
    {
        example_state& state;
        auto operator()() const noexcept {
            this->state.stop();
        }
    };
    using operation_state_concept = std::execution::operation_state_t;
    using env = std::execution::env_of_t<Receiver>;
    using token = std::execution::stop_callback_of_t<env>;
    using callback = std::execution::stop_callback_of_t<token, on_cancel>;
    std::remove_cvref_t<Receiver> receiver;
    std::optional<callback>       cancel{};
    std::atomic<std::size_t>      outstanding{};
    auto start() & noexcept {
        this->outstanding += 2u;
        this->cancel.emplace(
            std::execution::get_stop_token(this->receiver),
            on_cancel{*this}
        );
        if (this->outstanding != 2u)
           std::execution::set_stopped(std::move(this->receiver));
        else {
           register_work(this);
           if (this->outstanding == 0u)
               std::execution::set_value(std::move(this->receiver));
        }
    }
    auto stop() {
        unregister_work(this);
        if (--this->outstanding == 0u)
            std::execution::set_stopped(std::move(this->receiver));
    }
    auto complete() {
        if (this->outstanding == 2u) {
            this->cancel.reset();
            std::execution::set_value(std::move(this->receiver));
        }
    }
};
```
</blockquote>
</details>
</blockquote>
</details>
<details>
<summary><code>unstoppable_token&lt;_Token_&gt;</code></summary>
The concept <code>unstoppable_token&lt;Token&gt;</code> is modeled by a <code>_Token_</code> if <code>stoppable_token&lt;_Token_&gt;</code> is true and it can statically be determined that both <code>_token_.stop_requested()</code> and <code>_token_.stop_possible()</code> are `constexpr` epxressions yielding `false`. This concept is used to avoid extra work when using stop tokens which will never indicate that cancellations are requested.
<blockquote>
<details>
<summary>Example</summary>
The concept yields `true` for the <code><a href=‘#never-stop-token’>std::execution::never_stop_token</a></code>:

```c++
static_assert(std::execution::unstoppable_token<std::execution::never_stop_token>);
static_assert(not std::execution::unstoppable_token<std::execution::stop_token>);
static_assert(not std::execution::unstoppable_token<std::execution::inline_stop_token>);
```
</details>
</blockquote>
</details>

## Queries
The queries are used to obtain properties associated with an object.

<details>
<summary>Example defining a query on an environment</summary>
This example shows how to define an environment class which provides a <a href=‘#get-allocator’><code>get_allocator</code></a> query. The objects stores a `std::pmr::memory_resource*` and returns a correspondingly initialized `std::pmr::polymorphic_allocator<>`.

```
struct alloc_env {
   std::pmr::memory_resource res{std::pmr::new_delete_resource()};

   auto query(get_allocator_t const&) const noexcept {
       return std::pmr::polymorphic_allocator<>(this->res);
   }
};
```
</details>
<details>
<summary><code>forwarding_query(<i>query</i>) -> bool</code></summary>
**Default**: `false`
<br/>
The expression <code>forwarding_query(<i>query</i>)</code> is a `constexpr` query used to determine if the query <code><i>query</i></code> should be forwarded when wrapping an environment. The expression is required to be a core constant expression if <code><i>query</i></code> is a core constant expression.

The result of the expression is determined as follows:
<ol>
    <li>The result is the value of the expression <code><i>query</i>.query(forwarding_query)</code> if this expression is valid and `noexcept`.</li>
    <li>The result is <code>true</code> if the type of <code><i>query</i></code> is <code>public</code>ly derived from <code>forwarding_query</code>.</li>
    <li>Otherwise the result is <code>false</code>.
</ol>
<blockquote>
<details>
<summary>Example</summary>
When defining a custom query <code><i>custom</i></code> it is desirable to allow the query getting forwarded. It is necessary to explicit define the result of <code>forwarding_query(<i>custom</i>)</code>. The result can be defined by providing a corresponding `query` member function. When using this approach the function isn’t allowed to throw, needs to return `bool`, and needs to be a core constant expression:

```
struct custom_t {
    // ...
    constexpr bool query(forwarding_query_t const&) const noexcept {
        return true;
    }
};
inline constexpr custom_t custom{};
```

Alternatively, the query can be defined as forwarding by deriving publicly from `forwarding_query_t`:

```
struct custom_t: forwarding_query_t {
    // ...
};
```
</details>
<blockquote>
</details>
<details>
<summary><code>get_env(<i>queryable</i>) -> <i>env</i></code></summary>
**Default**: <a href='#empty_env'>`empty_env`</a>
<br/>
The expresion <code>get_env(<i>queryable</i>)</code> is used to get the environment <code><i>env</i></code> associated with <code><i>queryable</i></code>. To provide a non-default environment for a <code><i>queryable</i></code> a `get_env` member needs to be defined. If <code><i>queryable</i></code> doesn’t provide the <code>get_env</code> query an object of type <code><a href=‘#empty_env’>empty_env</a></code> is returned.
The value of the expression is <ol>
   <li>the result of <code>as_const(<i>queryable</i>).get_env()</code> if this expression is valid and <code>noexcept</code>.</li>
   <li><code>empty_env</code> otherwise.
</ol>
<div>
<details>
<summary>Example</summary>
The example defines an <a href=‘#environment’>environment</a> class <code>env</code> which stores a pointer to the relevant data and is returned as the <a href=‘#environment’>environment</a> for the type `queryable`:

```c++
struct data { /*...*/ };

struct env { data* d; /* ... */ };

struct queryable {
    data* d;\
    // ...
    env get_env() const noexcept { return { this->d }; }
};
```

Note that the `get_env` member is both `const` and `noexcept`.
</details>
</div>
</details>
<details>
<summary><code>get_allocator(<i>env</i>) -> <i>allocator</i></code></summary>
**Default**: <i>none</i>
<br/>
The expression <code>get_allocator(<i>env</i>)</code> returns an <code><i>allocator</i></code> for any memory allocations in the respective context. If <code><i>env</i></code> doesn’t support this query any attempt to access it will result in a compilation error.  The value of the expression <code>get_allocator(<i>env</i>)</code> is the result of <code>as_const(<i>env</i>).query(get_allocator)</code> if
<ul>
   <li>the expression is valid</code>;</li>
   <li>the expression is <code>noexcept</code>;</li>
   <li>the result of the expression satisfies <code><i>simple-allocator</i></code>.</li>
</ul>
Otherwise the expression is ill-formed.
</details>
<div>
<details>
<summary>Example</summary>
This example shows how to define an environment class which provides a <a href=‘#get-allocator’><code>get_allocator</code></a> query. The objects stores a `std::pmr::memory_resource*` and returns a correspondingly initialized `std::pmr::polymorphic_allocator<>`.

```
struct alloc_env {
   std::pmr::memory_resource res{std::pmr::new_delete_resource()};

   auto query(get_allocator_t const&) const noexcept {
       return std::pmr::polymorphic_allocator<>(this->res);
   }
};
```
</details>
</div>
</details>
<details>
<summary><code>get_completion_scheduler&lt;<iTtag</i>&gt;(<i>env</i>) -> <i>scheduler</i></code></summary>
**Default**: <i>none</i>
<br/>
The expression <code>get_complet_scheduler&lt;Tag&gt;(<i>env</i>)</code> yields the completion scheduler for the completion signal <code>Tag</code> associated with <code><i>env</i></code>. This query can be used to determine the scheduler a sender <code><i>sender</i></code> completes on for a given completion signal <code>Tag</code> by using <code>get_completion_scheduler&lt;Tag&gt;(get_env(<i>sender</i>))</code>. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_completion_scheduler&lt;Tag&gt;)</code> if
<ol>
   <li><code>Tag</code> is one of the types <code>set_value_t</code>, <code>set_error_t</code>, or <code>set_stopped_t</code>;
   <li>this expression is valid;</li>
   <li>this expression is <code>noexcept</code>;</li>
   <li>the expression’s type satisfies <code>scheduler</code>.
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_completion_signatures(<i>sender</i>, <i>env</i>)</code></summary>
The expression <code>get_completion_signatures(<i>sender</i>, <i>env</i>)</code> returns an object whose type is a specialization of <a href=‘#completion-signatures’><code>completion_signatures</code></a> defining the possible completion signatures of <code><i>sender</i></code> when connected to a <a href=‘#receiver’><code><i>receiver</i></code></a> whose <a href=‘#environment'>environment</a> <code>get_env(<i>receiver</i>)</code> is <code><i>env</i></code>. A <a href=‘#sender’><code>sender</code></a> can define the result of this query either by defining a member function <code>get_completion_signatures</code> or using a type alias <code>completion_signatures</code>.

To determine the result the <code><i>sender</i></code> is first transformed using <code>transform_sender(<i>domain</i>, <i>sender</i>, <i>env</i>)</code> to get <code><i>new-sender</i></code> with type <code><i>New-Sender-Type</i></code>. With that the result type is
<ol>
    <li>the type of <code><i>new-sender</i>.get_completion_signatures(<i>env</i>)</code> if this expression is valid;</li>
    <li>the type <code>remove_cvref_t&lt;<i>New-Sender-Type</i>&gt;::completion_signatures</code> if this type exists;</li>
    <li><code>completion_signatures&lt;set_value_t(<i>T</i>), set_error_t(exception_ptr), set_stopped_t()&gt;</code> if <code><i>New-Sender-Type</i></code> is an awaitable type which would yield an object of type <code><i>T</i></code> when it is <code>co_await</code>ed;</li>
    <li>invalid otherwise.</code>
</ol>
<div>
<details>
<summary>Example</summary>
When a <a href=‘#sender’><code>sender</code></a> doesn’t need to compute the completion signatures based on an <a href=‘#environment’>environment</a> it is easiest to use a the type alias, e.g.:
```c++
struct sender {
    using sender_concept = std::execution::sender_t;
    using completion_signatures = std::completion_signatures<
        std::execution::set_value_t(int),
        std::execution::set_error_t(std::error_code),
        std::execution::set_stopped()
    >;
    // ...
};
```
</details>
</div>
</details>
<details>
<summary><code>get_delegation_scheduler(<i>env</i>) -> <i>scheduler</i></code></summary>
The expression <code>get_delegation_scheduler(<i>env</i>)</code> yields the scheduler associated with <code><i>env</i></code> which is used for forward progress delegation. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_delegation_scheduler) -> <i>scheduler</i></code> if
<ol>
   <li>this expression is valid;</li>
   <li>this expression is <code>noexcept</code>;</li>
   <li>the expression’s type satisfies <code>scheduler</code>.
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_domain(<i>env</i>) -> <i>domain</i></code>
</summary>
The expression <code>get_domain(<i>env</i>)</code> yields the domain associated with <code><i>env</i></code>. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_domain)</code> if
<ol>
   <li>this expression is valid;</li>
   <li>this expression is <code>noexcept</code>.</li>
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_forward_progress_guarantee(<i>scheduler</i>) -> forward_progress_guarantee</code></summary>
The expression <code>get_forward_progress_guarantee(<i>scheduler</i>)</code> yields the forward progress guarantee of the <i>scheduler</i>’s execution agent. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_scheduler)</code> if
<ol>
   <li>this expression is valid;</li>
   <li>this expression is <code>noexcept</code>;</li>
   <li>the expression’s type is <code>forward_progress_guarantee</code>.
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_scheduler(<i>env</i>) -> <i>scheduler</i></code></summary>
The expression <code>get_scheduler(<i>env</i>)</code> yields the scheduler associated with <code><i>env</i></code>. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_scheduler)</code> if
<ol>
   <li>this expression is valid;</li>
   <li>this expression is <code>noexcept</code>;</li>
   <li>the expression’s type satisfies <code>scheduler</code>.
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_stop_token(<i>env</i>) -> <i>stoppable_token</i></code></summary>
The expression <code>get_stop_token(<i>env</i>)</code> yields the stop token associated with <code><i>env</i></code>. The value is the result of the expression <code>as_const(<i>env</i>).query(get_stop_token)</code> if
<ul>
   <li>the expression is valid;</li>
   <li>the expression is <code>noexcept</code>;</li>
   <li>the expression satisfies <code>stoppable_token</code>.</li>
</ul>
Otherwise the value is <code>never_stop_token{}</code>.
</details>

## Customization Point Objects

<details>
<summary><code>connect(<i>sender, receiver</i>) -> <i>operation_state</i></code></summary>
</details>
<details>
<summary><code>set_error(<i>receiver</i>, <i>error</i>) noexcept -> void</code></summary>
The expression <code>set_error(<i>receiver</i>, <i>error</i>)</code> invokes the <code>set_error</code> completion signal on <code><i>receiver</i></code> with the argument <code><i>error</i></code>, i.e., it invokes <code><i>receiver</i>.set_error(<i>error</i>)</code>.
</details>
<details>
<summary><code>set_stopped(<i>receiver</i>) noexcept -> void</code></summary>
The expression <code>set_stopped(<i>receiver</i>)</code> invokes the <code>set_stopped</code> completion signal on <code><i>receiver</i></code>, i.e., it invokes <code><i>receiver</i>.set_stopped()</code>.
</details>
<details>
<summary><code>set_value(<i>receiver</i>, <i>value</i>...) noexcept -> void</code></summary>
The expression <code>set_value(<i>receiver</i>, <i>value</i>...)</code> invokes the <code>set_value</code> completion signal on <code><i>receiver</i></code> with the argument(s) <code><i>value</i>...</code>, i.e., it invokes <code><i>receiver</i>.set_value(<i>value</i>...)</code>.
</details>
<details>
<summary><code>start(<i>state</i>) noexcept -> void</code></summary>
The expression <code>start(<i>state</i>)</code> starts the execution of the <code>operation_state</code> object <code><i>state</i></code>. Once this expression started executing the object <code><i>state</i></code> is required to stay valid at least until one of the completion signals of <code><i>state</i></code>’s <code>receiver</code> is invoked. Once started exactly one of the completion signals is eventually called.
</details>

## Senders

### Sender Factories

- <code>just(<i>value...</i>) -> <i>sender-of</i>&lt;set_value_t(<i>Value...</i>)&gt;</code>
- <code>just_error(<i>error</i>) -> <i>sender-of</i>&lt;set_error_t(<i>Error</i>)&gt;</code>
- <code>just_stopped() -> <i>sender-of</i>&lt;set_stopped_t()&gt;</code>
- <code>read_env(<i>query</i>) -> <i>sender-of</i>&lt;set_value_t(<i>query-result</i>)&gt;</code>
- <code>schedule(<i>scheduler</i>) -> <i>sender-of</i>&lt;set_value_t()&gt;</code>

### Sender Adaptors

- `bulk`
- <code>continues_on(<i>sender</i>, <i>scheduler</i>) -> <i>sender</i></code>
- <code>into_variant(<i>sender</i>) -> <i>sender-of</i>&lt;set_value_t(std::variant&lt;T...&gt;)&gt;</code>`
- <code>let_error(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code>
- <code>let_stopped(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code>
- <code>let_value(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code>
- `on`
- <code>schedule_from(<i>scheduler</i>, <i>sender</i>) -> <i>sender</i></code>
- `split`
- <code>starts_on(<i>scheduler</i>, <i>sender</i>) -> <i>sender</i></code>
- `stopped_as_error`
- `stopped_as_optional`
- <code>then(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code>
- <code>upon_error(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code>
- <code>upon_stopped(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code>
- <code>when_all(<i>sender</i>...) -> <i>sender</i></code>
- <code>when_all_with_variant(<i>sender</i>...) -> <i>sender</i></code>

### Sender Consumers

- <code>sync_wait(<i>sender</i>) -> std::optional&lt;std::tuple&lt;T...&gt;&gt;</code>

## Helpers

- `as_awaitable`
- `with_awaitable_sender`
- `apply_sender`
- `completion_signatures`
- `completion_signatures_t`
- `connect_result_t`
- `default_domain`
- `empty_env`
- `env_of_t`
- `error_types_of_t`
- `fwd_env`
- `operation_state_t`
- `receiver_t`
- `run_loop`
- `scheduler_t`
- `schedule_result_t`
- `sender_adaptor_closure`
- `sender_t`
- `stop_token_of_t`
- `tag_of_t`
- `transform_sender`
- `transform_completion_signatures`
- `transform_completion_signatures_of`
- `value_types_of_t`

## Stop Token
- `never_stop_token`
- `stop_token`
- `inplace_stop_token`
