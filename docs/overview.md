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
The term _enviroment_ refers to the bag of properties associated with an <code>_object_</code> by the call <code><a href=‘#get-env’>std::execution::get_env</a>(_object_)</code>. By default the environment for objects is empty (<code><a href=‘#env’>std::execution::env&lt;&gt;</a></code>). In particular, environments associated with <code><a href=‘#receiver’>receiver</a></code>s are used to provide access  to properties like the <a href=‘#get-stop-token’>stop token</a>, <a href=‘#get-scheduler’>scheduler</a>, or <a href=‘#get-allocator’>allocator</a> associated with the <code><a href=‘#receiver’>receiver</a></code>. The various properties associated with an object are accessed via <a href=‘#queries’>queries</a>.
</details>

## Concepts
This section lists the concepts from `std::execution`.

<details>
<summary><code>operation_state&lt;<i>State</i>&gt;</code></summary>

Operation states represent asynchronous operations ready to be <code><a href=‘#start’>start</a></code>ed or executing. Operation state objects are normally neither movable nor copyable. Once <code><a href=‘#start’>start</a></code>ed the object needs to be kept alive until a <a href=‘#completion-signal’>completion signal</a> is received. Users don’t interact with operation states explicitly except when implementing new sender algorithms.

Required members for <code>_State_</code>:

- The type `operation_state_concept` is an alias for `operation_state_tag` or a type derived thereof.
- <code><i>state</i>.<a href=‘#start’>start</a>() & noexcept</code>

<details>
<summary>Example</summary>

This example shows a simple operation state object which immediately completes successfully without any values (as <code><a href=‘#just’></a>()</code> would do). Normally <code><a href=‘#start’>start</a>()</code> initiates an asynchronous operation completing at some point later.

```c++
template <std::execution::receiver Receiver>
struct example_state
{
    using operation_state_concept = std::execution::operation_state_tag;
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

- The type `receiver_concept` is an alias for `receiver_tag` or a type derived thereof`.
- Rvalues of type <code>_Receiver_</code> are movable.
- Lvalues of type <code>_Receiver_</code> are copyable.
- <code><a href=‘#get-env’>std::execution::get_env</a>(_receiver_)</code> returns an object. By default this operation returns <code><a href=‘env’>std::execution::env&lt;&gt;</a></code>.

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
    using receiver_concept = std::execution::receiver_tag;
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
    using receiver_concept = std::execution::receiver_tag;

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
- The type <code>_Scheduler_::scheduler_concept</code> is an alias for `scheduler_tag` or a type derived thereof.
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

Senders represent asynchronous work. They may get composed from multiple senders to model a workflow. Senders can’t be run directly. Instead, they are passed to a <a href=‘#sender-consumer’>sender consumer</a> which <code><a href=‘#connect’>connect</a></code>s the sender to a <code><a href=‘#receiver’>receiver</a></code> to produce an <code><a href=‘#operation-state’>operation_state</a></code> which may get started. When using senders to represent work the inner workings shouldn’t matter. They do become relevant when creating sender algorithms.

Requirements for <code>_Sender_</code>:
- The type <code>_Sender_::sender_concept</code> is an alias for `sender_tag` or a type derived thereof or <code>_Sender_</code> is a suitable _awaitable_.
- <code><a href='get_env'>std::execution::get_env</a>(_sender_)</code> is valid. By default this operation returns <code><a href=‘env’>std::execution::env&lt;&gt;</a></code>.
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
        using operation_state_concept = std::execution::operation_state_tag;
        std::remove_cvref_t<Receiver> receiver;
        int                           value;
        auto start() & noexcept {
            std::execution::set_value(
                std::move(this->receiver),
                this->value
            );
        }
    };
    using sender_concept = std::execution::sender_tag;
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
<summary><code>sender_in&lt;<i>Sender, Env</i> = std::execution::env&lt;&gt;&gt;</code></summary>

The concept <code>sender_in&lt;<i>Sender, Env</i>&gt;</code> tests whether <code>_Sender_</code> is a <code><a href=‘#sender’>sender</a></code>, <code>_Env_</code> is a destructible type, and <code><a href=‘#get_completion_signatures’>std::execution::get_completion_signatures</a>(_sender_, _env_)</code> yields a specialization of <code><a href=‘#completion_signatures’>std::execution::completion_signatures</a></code>.
</details>
<details>
<summary><code><i>sender-to</i>&lt;<i>Sender, Receiver</i>&gt;</code></summary>

The concept <code><i>sender-to</i>&lt;<i>Sender, Receiver</i>&gt;</code> tests if <code><a href=‘#sender_in’>std::execution::sender_in</a>&lt;_Sender_, <a href='#env_of_t'>std::execution::env_of_t</a>&lt;_Receiver_&gt;&gt;</code> is true, and if <code>_Receiver_</code> can receive all <a href=‘#completion-signals’>completion signals</a> which can be sent by <code>_Sender_</code>, and if <code>_Sender_</code> can be <code><a href=‘#connect’>connect</a></code>ed to <code>_Receiver_</code>.

To determine if <code>_Receiver_</code> can receive all <a href=‘#completion-signals’>completion signals</a> from <code>_Sender_</code> it checks that for each <code>_Signature_</code> in <code><a href=‘#get_completion_signals’>std::execution::get_completion_signals</a>(_sender_, std::declval&lt;<a href='#env_of_t'>std::execution::env_of_t</a>&lt;_Receiver_&gt;&gt;())</code> the test <code><a href=‘#receiver_of’>std::execution::receiver_of</a>&lt;_Receiver_, _Signature_&gt;</code> yields true. To determine if <code>_Sender_</code> can be <code><a href=‘#connect’>connect</a></code>ed to <code>_Receiver_</code> the concept checks if <code><a href=‘#connect’>connect</a>(std::declval&lt;_Sender_&gt;(), std::declval&lt;_Receiver_&gt;)</code> is a valid expression.
</details>
<details>
<summary><code>sends_stopped&lt;<i>Sender, Env</i> = std::execution::env&lt;&gt;&gt;</code></summary>

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
- <code>std::swappable&lt;_Token_&gt;</code>
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
    using operation_state_concept = std::execution::operation_state_tag;
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
<b>Default</b>: `false`
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
</blockquote>
</details>
<details>
<summary><code>get_env(<i>queryable</i>) -> <i>env</i></code></summary>
<b>Default</b>: <a href='#env'>`env&lt;&gt;`</a>
<br/>
The expression <code>get_env(<i>queryable</i>)</code> is used to get the environment <code><i>env</i></code> associated with <code><i>queryable</i></code>. To provide a non-default environment for a <code><i>queryable</i></code> a `get_env` member needs to be defined. If <code><i>queryable</i></code> doesn’t provide the <code>get_env</code> query an object of type <code><a href=‘#env’>env&lt;&gt;</a></code> is returned.
The value of the expression is <ol>
   <li>the result of <code>as_const(<i>queryable</i>).get_env()</code> if this expression is valid and <code>noexcept</code>.</li>
   <li><code>env&lt;&gt;</code> otherwise.
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
<b>Default</b>: <i>none</i>
<br/>
The expression <code>get_allocator(<i>env</i>)</code> returns an <code><i>allocator</i></code> for any memory allocations in the respective context. If <code><i>env</i></code> doesn’t support this query any attempt to access it will result in a compilation error.  The value of the expression <code>get_allocator(<i>env</i>)</code> is the result of <code>as_const(<i>env</i>).query(get_allocator)</code> if
<ul>
   <li>the expression is valid;</li>
   <li>the expression is <code>noexcept</code>;</li>
   <li>the result of the expression satisfies <code><i>simple-allocator</i></code>.</li>
</ul>
Otherwise the expression is ill-formed.
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
<summary><code>get_completion_domain&lt;<i>Tag</i>&gt;(<i>attrs</i>) -> <i>domain</i></code></summary>
<b>Default</b>: <i>none</i>
<br/>
The expression <code>get_completion_domain&lt;Tag&gt;(<i>attrs</i>)</code> yields the completion domain for the completion signal <code>Tag</code> associated with the sender <code><i>attrs</i></code>. This query can be used to determine the domain a sender <code><i>sender</i></code> completes on for a given completion signal <code>Tag</code> by using <code>get_completion_domain&lt;Tag&gt;(get_env(<i>sender</i>), ev...)</code>. The value of the expression is
<ol>
   <li>equivalent to <code>as_const(<i>env</i>).query(get_completion_domain&lt;Tag&gt;, ev...)</code> if
   <code>Tag</code> is one of the types <code>set_value_t</code>, <code>set_error_t</code>, or <code>set_stopped_t</code> and the expression is valid;</li>
   <li>equivalent to <code>as_const(<i>env</i>).query(get_completion_domain&lt;Tag&gt;)</code> if
   <code>Tag</code> is one of the types <code>set_value_t</code>, <code>set_error_t</code>, or <code>set_stopped_t</code> and the expression is valid;</li>
   <li>equivalent to <code>get_completion_domain&lt;set_value_t&gt;(get_env(<i>sender</i>), ev...)</code> if <code>Tag</code> is <code>void</code> and the expression is valid;</li>
   <li>equivalent to <code><i>TRY-QUERY</i>(get_completion_scheduler&lt;Tag&gt;(attrs, env...), get_completion_domain&lt;set_value_t&gt;, ev...)</i></code> if this expression is well-formed;</li>
   <li>equivalent to <code>default_domain</code> if <code>scheduler&lt;decltype(attrs)&gt;</code> is <code>true</code> and <code>0u &lt; sizeof...(ev)</code>;</li>
   <li>ill-formed otherwise.</li>
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_completion_scheduler&lt;<i>Tag</i>&gt;(<i>env</i>) -> <i>scheduler</i></code></summary>
<b>Default</b>: <i>none</i>
<br/>
The expression <code>get_completion_scheduler&lt;Tag&gt;(<i>env</i>)</code> yields the completion scheduler for the completion signal <code>Tag</code> associated with <code><i>env</i></code>. This query can be used to determine the scheduler a sender <code><i>sender</i></code> completes on for a given completion signal <code>Tag</code> by using <code>get_completion_scheduler&lt;Tag&gt;(get_env(<i>sender</i>))</code>. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_completion_scheduler&lt;Tag&gt;)</code> if
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
    <li>invalid otherwise.</li>
</ol>
<div>
<details>
<summary>Example</summary>
When a <a href=‘#sender’><code>sender</code></a> doesn’t need to compute the completion signatures based on an <a href=‘#environment’>environment</a> it is easiest to use a the type alias, e.g.:
```c++
struct sender {
    using sender_concept = std::execution::sender_tag;
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
<summary><code>get_domain(<i>env</i>) -> <i>domain</i></code></summary>
The expression <code>get_domain(<i>env</i>)</code> yields the domain associated with <code><i>env</i></code>. The value of the expression is equivalent to
<ol>
   <li><code>auto(as_const(<i>env</i>).query(get_domain))</code> if this expression is valid;</li>
   <li>otherwise, <code>get_completion_domain&lt;set_value_t&gt;(get_scheduler(env), <i>HIDE-SCHED</i>(env))</code> if this expression is valid;</li>
   <li><code>default_domain()</code> (except <code><i>env</i></code> is evaluted).</li>
</ol>
Otherwise the expression is invalid.
</details>
<details>
<summary><code>get_forward_progress_guarantee(<i>scheduler</i>) -> forward_progress_guarantee</code></summary>
The expression <code>get_forward_progress_guarantee(<i>scheduler</i>)</code> yields the forward progress guarantee of the <i>scheduler</i>’s execution agent. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_forward_progress_guarantee)</code> if
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
<summary><code>get_start_scheduler(<i>env</i>) -> <i>scheduler</i></code></summary>
The expression <code>get_start_scheduler(<i>env</i>)</code> yields the scheduler associated with <code><i>env</i></code>. The value of the expression is equivalent to <code>as_const(<i>env</i>).query(get_scheduler)</code> if
<ol>
   <li>this expression is valid;</li>
   <li>this expression is <code>noexcept</code>;</li>
   <li>the expression’s type satisfies <code>scheduler</code>.
</ol>
Otherwise the expression is invalid.

If the expression <code>get_start_scheduler(get_env(<i>rcvr</i>))</code>
is well-formed it should yield the scheduler the operation state resulting
from <code>connect(<i>sndr></i>, <i>rcvr</i>)</code> gets <code>start</code>ed on.
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
<summary><code>connect(<i>sender</i>, <i>receiver</i>) -> <i>operation_state</i></code></summary>
The expression <code>connect(<i>sender</i>, <i>receiver</i>)</code> combines <code><i>sender</i></code> and <code><i>receiver</i></code> into an operation state <code><i>state</i></code>. When this <code><i>state</i></code> gets started using <code>start(<i>state</i>)</code> the operation represented by <code><i>sender</i></code> gets started and reports its completion to <code><i>receiver</i></code> or an object copied or moved from <code><i>receiver</i></code>. While the operation state <code><i>state</i></code> isn’t started it can be destroyed but once it got started it needs to stay valid until one of the completion signals is called on <code><i>receiver</i></code>.
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

Sender factories create a sender which forms the start of a graph of lazy work items.

<details>
<summary><code>just(<i>value...</i>) -> <i>sender-of</i>&lt;set_value_t(<i>Value...</i>)&gt;</code></summary>
The expression <code>just(<i>value...</i>)</code> creates a sender which sends <code><i>value...</i></code> on the `set_value` (success) channel when started (note that <code><i>value...</i></code> can be empty).

<b>Completions</b>
<ul>
<li><code>set_value_t(decltype(<i>value</i>)...)</code></li>
</ul>
</details>
<details>
<summary><code>just_error(<i>error</i>) -> <i>sender-of</i>&lt;set_error_t(<i>Error</i>)&gt;</code></summary>
The expression <code>just_error(<i>error</i>)</code> creates a sender which sends <code><i>error</i></code> on the `set_error` (failure) channel when started.

<b>Completions</b>
<ul>
<li><code>set_error_t(decltype(<i>error</i>))</code></li>
</ul>
</details>
<details>
<summary><code>just_stopped() -> <i>sender-of</i>&lt;set_stopped_t()&gt;</code></summary>
The expression <code>just_stopped()</code> creates a sender which sends a completion on the `set_stopped` (cancellation) channel when started.

<b>Completions</b>
<ul>
<li><code>set_stopped_t()</code></li>
</ul>
</details>
<details>
<summary><code>read_env(<i>query</i>) -> <i>sender-of</i>&lt;set_value_t(<i>query-result</i>)&gt;</code></summary>
The expression <code>read_env(<i>query</i>)</code> creates a sender which sends the result of querying <code><i>query</i></code> the environment of the <code><i>receiver</i></code> it gets connected to on the `set_value` channel when started. Put differently, it calls <code>set_value(move(<i>receiver</i>), <i>query</i>(get_env(<i>receiver</i>)))</code>. For example, in a coroutine it may be useful to extra the stop token associated with the coroutine which can be done using <code>read_env</code>:

```c++
auto token = co_await read_env(get_stop_token);
```

<b>Completions</b>
<ul>
<li><code>set_value_t(decltype(<i>query</i>(get_env(<i>receiver</i>))))</code>
</ul>
</details>
<details>
<summary><code>schedule(<i>scheduler</i>) -> <i>sender-of</i>&lt;set_value_t()&gt;</code></summary>
The expression <code>schedule(<i>scheduler</i>)</code> creates a sender which upon success completes on the <code>set_value</code> channel without any arguments running on the execution context associated with <code><i>scheduler</i></code>. Depending on the scheduler it is possible that the sender can complete with an error if the scheduling fails or using `set_stopped()` if the operation gets cancelled before it is successful.

<b>Completions</b>
<ul>
<li><code>set_value_t()</code> upon success</li>
<li><code>set_error_t(<i>Error</i>)</code> upon failure if <code><i>scheduler</i></code> may fail</li>
<li><code>set_stopped_t()</code> upon cancellation if <code><i>scheduler</i></code> supports cancellation
</ul>
</details>

### Sender Adaptors
The sender adaptors take one or more senders and adapt their respective behavior to complete with a corresponding result. The description uses the informal function <code><i>completions-of</i>(<i>sender</i>)</code> to represent the completion signatures which <code><i>sender</i></code> produces. Also, completion signatures are combined using <code>+</code>: the result is the deduplicated set of the combined completion signatures.

<details>
<summary><code>affine(<i>sender</i>) -> <i>sender-of</i><<i>completions-of</i>(<i>sender</i>)></code></summary>
The expression <code>affine(<i>sender</i>)</code> creates
a sender which completes on the same scheduler it was started on, even if <code><i>sender</i></code> changes the scheduler. The scheduler to resume on is determined using <code>get_scheduler(get_env(<i>rcvr</i>))</code> where <code><i>rcvr</i></code> is the receiver the sender is <code>connect</code>ed to.

The primary use of <code>affine</code> is implementing scheduler affinity for <code>task</code>.
</details>

<details>
<summary><code>associate(<i>sndr</i>, <i>token</i>) -> <i>sender</i></summary>
</details>

<details>
<summary><code>bulk(<i>sndr</i>, <i>policy</i>, <i>shape</i>, <i>fun</i>) -> <i>sender</i></summary>
</details>
<details>
<summary><code>bulk_chunked(<i>sndr</i>, <i>policy</i>, <i>shape</i>, <i>fun</i>) -> <i>sender</i></summary>
</details>
<details>
<summary><code>bulk_unchunked(<i>sndr</i>, <i>policy</i>, <i>shape</i>, <i>fun</i>) -> <i>sender</i></summary>
</details>
<details>
<summary><code>continues_on(<i>sender</i>, <i>scheduler</i>) -> <i>sender-of</i><<i>completions-of</i>(<i>sender</i>) + <i>completions-of</i>(schedule(<i>scheduler</i>))></code></summary>
The expression <code>continues_on(<i>sender</i>, <i>scheduler</i>)</code> creates a sender <code><i>cs</i></code> which starts <code><i>sender</i></code> when started. The results from <code><i>sender</i></code> are stored. Once that is <code><i>cs</i></code> creates a sender using <code>schedule(<i>scheduler</i>)</code> and completes itself on the execution once that sender completes.

<b>Completions</b>
<ul>
<li><code><i>completions-of</i>(<i>sender</i>)</code></li>
<li><code><i>completions-of</i>(schedule(<i>scheduler</i>))</code></li>
</ul>
</details>
<details>
<summary><code>into_variant(<i>sender</i>) -> <i>sender-of</i>&lt;set_value_t(std::variant&lt;<i>Tuple</i>...&gt;)&gt;</code></summary>
The expression <code>into_variant(<i>sender</i>)</code> creates a sender which transforms the results of possibly multiple <code>set_value</code> completions of <code><i>sender</i></code> into one <code>set_value</code> completion representing the different upstream results as different options of a <code>variant&lt;<i>Tuple</i>...&gt;</code> where each <code><i>Tuple</i></code> is a <code>tuple</code> of values initialized with the respective arguments passed to <code>set_value</code>. The order of options in the <code>variant</code> isn’t specified.
</details>
<details>
<summary><code>let_error(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code></summary>
The expression <code>let_error(<i>upstream</i>, <i>fun</i>)</code> yields a sender <code><i>sndr</i></code> which uses an error completion (<code>set_error</code>) of <code><i>upstream</i></code> as an argument to invoke <code><i>fun</i></code> which has to return another sender <code><i>inner-sndr</i></code> and the completion of <code><i>inner-sndr</i></code> becomes the completion of <code><i>sndr</i></code>. If this invocation results in an exception <code><i>sndr</i></code> completes with an error completion the result of <code>std::current_exception</code>. If <code><i>upstream</i></code> completes successfully (<code>set_value</code>) or with a cancellation (<code>set_stopped</code>) this completion becomes the completion of <code><i>sndr</i></code>.
</details>
<details>
<summary><code>let_stopped(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code></summary>
The expression <code>let_stopped(<i>upstream</i>, <i>fun</i>)</code> yields a sender <code><i>sndr</i></code> which uses a cancellation completion (<code>set_stopped</code>) of <code><i>upstream</i></code> to invoke <code><i>fun</i></code> which has to return another sender <code><i>inner-sndr</i></code> and the completion of <code><i>inner-sndr</i></code> becomes the completion of <code><i>sndr</i></code>. If this invocation results in an exception <code><i>sndr</i></code> completes with an error completion the result of <code>std::current_exception</code>. If <code><i>upstream</i></code> completes successfully (<code>set_value</code>) or with an error (<code>set_error</code>) this completion becomes the completion of <code><i>sndr</i></code>.
</details>
<details>
<summary><code>let_value(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code></summary>
The expression <code>let_value(<i>upstream</i>, <i>fun</i>)</code> yields a sender <code><i>sndr</i></code> which uses a succesful completion (<code>set_value</code>) of <code><i>upstream</i></code> as arguments to invoke <code><i>fun</i></code> which has to return another sender <code><i>inner-sndr</i></code> and the completion of <code><i>inner-sndr</i></code> becomes the completion of <code><i>sndr</i></code>. If this invocation results in an exception <code><i>sndr</i></code> completes with an error completion the result of <code>std::current_exception</code>. If <code><i>upstream</i></code> completes with a cancellation (<code>set_stopped</code>) or with an error (<code>set_error</code>) this completion becomes the completion of <code><i>sndr</i></code>.
</details>
<details>
<summary><code>on(_sched_, _sndr_)</code>, <code>on(_sndr, _sched_, _closure_)</code></summary>
The <code>on</code> algorithm is a pipeable sender adaptor. Let <code><i>on-sender</i></code> be
<ul>
  <li><code>_sndr_</code> when the form <code>on(_sched_, _sndr_)</code> is used;</li>
  <li><code>_closure_(_sndr_)</code> when the form <code>on(_sndr_, _sched_, _closure_)</code> is used.</li>
</ul>
The sender <code><i>on-sndr</i></code> is started on <code>_sched_</code>'s execution context.
The <code>on</code> algorithm completes on the original scheduler (obtained using <code>get_start_scheduler</code>) with the result of the <code><i>on-sndr</i></code>.
</details>
<details>
<summary><code>schedule_from(<i>sender</i>) -> <i>sender</i></code></summary>
The expression <code>schedule_from(<i>sender</i>)</code> yields a
sender which behaves likes <code><i>sender</i></code>. The purpose of the <code>schedule_from</code>
is to allow schedulers to customize the way how to transition off the execution context.
</details>
<details>
<summary><code>spawn_future(<i>sndr</i>, <i>token</i>) -> <i>sender</i></summary>
</details>
<details>
<summary>`split`</summary>
</details>
<details>
<summary><code>starts_on(<i>scheduler</i>, <i>sender</i>) -> <i>sender</i></code></summary>
The expression <code>starts_on(<i>scheduler</i>, <i>sender</i>)</code> yields a sender
which starts <code><i>sender</i></code> on the <code><i>scheduler</i></code>'s context, i.e.,
it starts <code>schedule(<i>scheduler</i>)</code> and then starts <code><i>sender</i></code>
where the scheduler's sender completes.
</details>
<details>
<summary><code>stopped_as_error(<i>sndr</i>)</code></summary>
</details>
<details>
<summary><code>stopped_as_optional(<i>sndr</i>)</code></summary>
</details>
<details>
<summary><code>then(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code></summary>
The expression <code>then(<i>upstream</i>, <i>fun</i>)</code> yields a sender <code><i>sndr</i></code> which on successful competion of <code><i>upstream</i></code> (<code>set_value</code>) calls <code><i>fun</i></code> with the arguments passed to <code>set_value</code> and yields the function return as its own result. If the function throws or <code><i>upstream</i></code> completes with an error (<code>set_error</code>) the exception or the error becomes <code><i>sndr</i></code>'s result. If <code><i>upstream</i></code> completes with a cancellation (<code>set_stopped</code>).
</details>
<details>
<summary><code>unstoppable(<i>sender</i>) -> <i>sender</i></code></summary>
The expression <code>unstoppable(<i>sender</i>)</code> yields a sender which
passes its receiver's environment to <code><i>sender</i></code> except that
the <code>get_stop_token</code> query return <code>never_stop_token</code>:
the resulting sender behaves like <code><i>sender</i></code> except that it
is unstoppable.
</details>
<details>
<summary><code>upon_error(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code></summary>
The expression <code>upon_error(<i>upstream</i>, <i>fun</i>)</code> yields a sender <code><i>sndr</i></code> which passes an error completion (<code>set_error</code>) of <code><i>upstream</i></code> to <code><i>fun</i></code> and uses this result of this function invocation for its own successful (<code>set_value</code>) completion. If the function invocation throws <code>std::current_exception()</code> becomes <code><i>sndr</i></code>'s error (<code>set_error</code>) completiom. The success (<code>set_value</code>) and cancellation (<code>set_stopped</code>) completions of <code><i>upstream</i></code> are forwarded.
</details>
<details>
<summary><code>upon_stopped(<i>upstream</i>, <i>fun</i>) -> <i>sender</i></code></summary>
The expression <code>upon_stopped(<i>upstream</i>, <i>fun</i>)</code> yields a sender <code><i>sndr</i></code> which turns a cancellation completion (<code>set_stopped</code>) result of <code><i>upstream</i></code> to <code><i>fun</i></code> and uses this result of this function invocation for its own successful (<code>set_value</code>) completion. If the function invocation throws <code>std::current_exception()</code> becomes <code><i>sndr</i></code>'s error (<code>set_error</code>) completiom. The success (<code>set_value</code>) and error (<code>set_error</code>) completions of <code><i>upstream</i></code> are forwarded.
</details>
<details>
<summary><code>when_all(<i>sender</i>...) -> <i>sender</i></code></summary>
The expression <code>when_all(<i>sender</i>...)</code> yields a sender <code>sndr</code> which completes successfully (<code>set_value</code>) when all nested senders <code><i>sender<i>...</code> completed successfully using the results of the nested senders in order. If any of the senders completes with an error (<code>set_error</code>) or a cancellation (<code>set_stopped</code>) the first such completion becomes the completion of <code><i>sndr</i></code> once all nested senders completed. A stop is requested for the stop source(s) whose token was passed to the nested senders.
</details>
<details>
<summary><code>when_all_with_variant(<i>sender</i>...) -> <i>sender</i></code></summary>
</details>
<details>
<summary><code>write_env(<i>sender</i>, <i>env</i>) -> <i>sender</i></code></summary>
The expression <code>write_env(<i>sender</i>, <i>env</i>)</code> creates a sender
which passes a receiver to <code><i>sender</i></code> which combines the environment
<code><i>env</i></code> with the environment from the receiver's environment. The
queries from <code><i>env</i></code> take precedence over those from the receiver's environment.
</details>


### Sender Consumers

<details>
<summary><code>sync_wait(<i>sender</i>) -> std::optional&lt;std::tuple&lt;T...&gt;&gt;</code></summary>
</detail>
<details>
<summary><code>sync_wait_with_variant(<i>sender</i>) -> std::optional&lt;std::variant&lt;std::tuple&lt;T...&gt;...&gt;&gt;</code></summary>
</detail>
<details>
<summary><code>spawn(<i>sndr</i>, <i>token</i>) -> void</code></summary>
</details>

## Helpers

- `as_awaitable`
- `with_awaitable_sender`
- `apply_sender`
<details>
<summary><code>completion_signatures&lt;<i>Sig</i>...&gt;</code></summary>
The template specialization <code>completion_signatures&lt;<i>Sig</i>...&gt;</code> is a list
of completion signatures used to declare and compute the result types of senders.
</p>
It has two exposition-only members template:
<ol>
<li><code><i>count-of</i>(<i>tag</i>)</code> providing a constant expression with the count of <code><i>tag</i></code> completions.</li>
</li><code><i>for-each</i>(<i>fun</i>)</code> invoking <code><i>fun</i></code> with a pointer to each of the completion signatures <code><i>Sig</i>...</code>. The function is used to verify the completion signature types.
</ol>
</details>
- `completion_signatures_t`
- `connect_result_t`
- `default_domain`
<details>
<summary><code>env&lt;Ev...&gt;</code></summary>
The expression <code>env(<i>ev</i>...)</code> creates an environment by combining the environments
<code><i>ev</i>...</code>. If multiple of the environments support an identical query, the first
one from the first environment is used.
</details>
- `env_of_t`
<details>
<summary><code>error_types_of_t&lt;<i>Sndr</i>, <i>Env</i> = env&lt;&gt;, <i>Variant</i> = <i>variant-or-empty</i>&gt;</code></summary>
The template specialization <code>error_types_of_t&lt;<<i>sndr</i>, <i>Env</i>, <i>Variant</i>&gt;</code> gets <code><i>Sndr</i></code> error completion signatures when using the environment <code><i>Env</i></code>. The results is represented as a <code><i>Variant</i>&lt;<i>E</i>...</code> where <code><i>E</i>...</code> is the list of argument types to the completion signatures.
</details>
- `fwd_env`
- `operation_state_tag`
<details>
<summary><code>prop&lt;Query, Value&gt;</code></summary>
The expression <code>prop(<i>query</i>, <i>value</i>)</code> create an object which can
be queried for query <code><i>query</i></code> which results in <code><i>value</i></code>.

</details>
- `receiver_tag`
<details>
<summary><code>run_loop</code></summary>
The class <code>run_loop</code> provides a scheduler to execute work on. It is used
to implement <code>sync_wait(<i>sndr</i>)</code>. The <code>public</code> methods on
an object <code><i>loop</i></code> of type <code>run_loop</code> are:
<ul>
<li><code><i>loop</i>.get_scheduler()</code> to get a scheduler scheduling work on <code><i>loop</i></code>.</li>
<li><code><i>loop</i>.finish()</code> to request <code><i>loop</i></code> to exit processing work items. Note that <code>run_loop</code> doesn't maintain a stop source, i.e., when this operation is invoked the work doesn't get cancelled.</li>
<li><code><i>loop</i>.run()</code> to have the current thread executed work items scheduled on <code><i>loop</i></code>. The thread will continue processing work items as long as <code><i>loop</i>.finish()</code> wasn't called or there is work scheduled on <code><i>loop</i></code>.
</ul>
</details>
- `scheduler_tag`
- `schedule_result_t`
- `sender_adaptor_closure`
- `sender_tag`
<details>
<summary><code>bool sends_stopped&lt;<i>Sndr</i>, <i>Env</i> = env&lt;&gt;</code></summary>
The Boolean variable <code>sends_stopped&lt;<i>Sndr</i>, <i>Env</i>&gt; is <code>true</code> if the completion signatures of <code><i>Sndr</i></code> when using the environment <code><i>Env</i></code> contain a cancellation signature (<code>set_stopped_t()</code>).
</details>
- `stop_token_of_t`

<details>
<summary><code>tag_of_t&lt;Sndr&gt;</code></summary>
<ul>
   <li>if <code>auto&&[tag, data, children...] = sndr;</code> is well-formed the type <code>decltype(auto(tag))</code>;</li>
   <li>otherwise ill-formed.</li>
</ul>
</details>

- `transform_sender`
- `transform_completion_signatures`
- `transform_completion_signatures_of`
<details>
<summary><code>value_types_of_t&lt;<i>Sndr</i>, <i>Env</i> = env&lt;&gt;, <i>Tuple</i> = <i>decayed-tuple</i>, <i>Variant</i> = <i>variant-or-empty</i>&gt;</code></summary>
The template specialization <code>value_types_of_t&lt;<i>Sndr</i>, <i>Env</i>, <i>Tuple</i>, <i>Variant</i>&gt;</code> gets <code><i>Sndr</i></code> success completion signatures when using the environment <code><i>Env</i></code>. The resulting type is a <code><i>Variant</i></code> of <code><i>Tuple</i></code> elements where each <code><i>Tuple</i></code> represents the argument types of one of the value completion signatures.
</details>

## Stop Token
- `never_stop_token`
- `stop_token`
- `inplace_stop_token`

## Exposition Only

<details>
<summary><code><i>as-except-ptr</i>(err)</code></summary>
Turns <code>err</code> smartly into an <code>exception_ptr</code>:
<ol>
<li>if <code>same_as&lt;decay_t&lt;decltype(err)&gt;, exception_ptr&gt;</code> &#x21d2; <code>err</code></li>
<li>else if <code>same_as&lt;decay_t&lt;decltype(err)&gt;, error_code&gt;</code> &#x21d2; <code>make_exception_ptr(system_error(err))</code></li>
<li>else <code>make_exception_ptr(err)</code></li>
</ol>
</details>

<details>
<summary><code>concept <i>class-type</i>&lt;T&gt;</code></summary>
Determines if the type <code>T</code> is a decayed class type:
<code><i>decays-to</i>&lt;T, T&gt; &amp;&amp; is_class_v&lt;T&gt;</code>
</details>

<details>
<summary><code><i>COMMON-DOMAIN</i>(domains...)</code></summary>
The expression <code><i>COMMON-DOMAIN</i>(domains...)</code> is

<ul>
  <li><code>common_type_t&lt;decltype(auto(domains))...&gt;()</code> if this expression is valid</li>
  <li><code>indeterminate_domain&lt;decltype(auto(domains))...&gt;()</code> with duplicates removed from the template arguments</li>
</ul>
</details>

<details>
<summary><code><i>COMPL-DOMAIN</i>&lt;Tag&gt;(sndr, ev...)</code></summary>
The expression <code><i>COMPL-DOMAIN</i>&lt;Tag&gt;(sndr, ev...)</code> gets <code>sndr</code>s completion
domain given the optional environment <code>ev...</code>:

<ul>
  <li><code>get_completion_domain<Tag>(get_env(sndr), ev..)</code> if this expression is well-formed,
  <li><code>indeterminate_domain()</code> otherwise.</li>
</ul>

<b>Note</b>: it seems this exposition-only name is actually unused!
</details>

<details>
<summary><code>concept <i>completion-signature</i>&lt;Signature&gt;</code></summary>
This concept determines if the type <code>Signature</code> is a completion signature, i.e.,
if it has one of these three forms:
<ol>
<li><code>set_value_t(T...)</code></li>
<li><code>set_error_t(T)</code></li>
<li><code>set_stopped_t()</code></li>
</ol>
</details>

<details>
<summary><code>concept <i>decays-to</i>&lt;From, To&gt;</code></summary>
Determines if <code>To</code> is the result of decaying the type <code>From</code>:
<code>same_as&lt;decay_t&lt;From&gt;, To&gt;</code>
</details>

<details>
<summary><code><i>FWD-ENV</i>(env)</code></summary>
The expression <code><i>FWD-ENV</i>(env)</code> yields a queryable object <code>q</code> supporting only forwardable queries. Let <code>qry</code> be a query object and <code>a...</code> be a possibly emnpty pack of arguments. Then <code>q.query(qry, a...)</code> is
<ul>
   <li>equivalent to <code>env.query(qry, a...)</code> if <code>forwarding_query(qry)</code> is <code>true</code></li>
   <li>ill-formed if <code>forwarding_query(qry)</code> is <code>false</code>
</ul>
</details>

<details>
<summary><code><i>FWD-ENV-T</i>(Env)</code></summary>
The type <code><i>FWD-ENV-T</i>(Env)</code> is <code>decltype(FWD-ENV(decl_val&lt;Env&gt;()))</code>.
</details>

<details>
<summary><code><i>gather-signatures</i>&lt;<i>Tag</i>, <i>Signatures</i>, <i>Tuple</i>, <i>Variant</i>&gt;</code></summary>
The template specializaton <code><i>gather-signatures</i>&lt;<i>Tag</i>, <i>Signatures</i>, <i>Tuple</i>, <i>Variant</i>&gt;</code> represents the completion signatures in the type list <i>Signatures</i> using <code><i>Tag</i></code> (one of <code>set_value_t</code>, <code>set_error_t</code>, or <code>set_stopped_t</code>) as a <code><i>Variant</i></code> of <code><i>Tuple</i></code>s. Each <code><i>Tuple</i></code> has the element types of of one of the matching completion signatures.
</details>

<details>
<summary><code>concept <i>has-completions</i>&lt;Rcvr, Completions&gt;</code></summary>
This concept determines if an object of type <code>Rcvr</code> supports ech of the completion signatures in <code>Completions</code>.
</details>
<details>

<summary><code><i>HIDE-SCHED</i>(q)</code></summary>
For a query object <code>tag</code> and arguments <code>a...</code> the
expressions <code><i>HIDE-SCHED</i>(q).query(tag, a...)</code> is
<ul>
  <li>undefined if <code>decay_t&lt;decltype(tag)&gt;</code> is <code>get_scheduler_t</code> or <code>get_domain_t</code></li>
  <li>equivalent to <code>q.query(tag, a...)</code> otherwise</li>
</details>

<details>
<summary><code><i>infallible-scheduler</i>&lt;Sched&gt;</code></summary>

Determines if <code>Sched</code> is a scheduler (i.e., <code>scheduler&lt;Sched&gt;</code> is <code>true</code>)
and if <code>Sched</code>'s sender has only a <code>set_value_t()</code> completion signature when used with
an environment with an <code>unstoppable_token&lt;Tok&gt;</code> stop token <code>Tok</code>. If the
stop token <code>Tok</code> is not <code>unstoppable_token&lt;Tok&gt;</code> the completion signatures
can include a <code>set_stopped_t()</code> completion signature in addition to the <code>set_value_t()</code> completion signature.
</details>

<details>
<summary><code><i>JOIN-ENV</i>(ev1, ev2)</code></summary>
The expression <code><i>JOIN-ENV</i>(ev1, ev2)</code> yields a queryable object <code>env</code> such that for a query <code>qry</code> and a pack of arguments <code>a...</code> the result of the expression <code>env.query(qry, a...)</code> is

<ol>
   <li><code>equivalent to ev1.query(qry, a...)</code></li> if this expression is well-format,otherwise</li>
   <li><code>equivalent to ev2.query(qry, a...)</code></li> if this expression is well-format,otherwise</li>
   <li>ill-formed</code>
</ol>

</details>

<details>
<summary><code><i>make-sender</i>(tag, data = <i>empty</i>{}, child...)</code></summary>
Creates and object of type <code><i>basic-sender</i>&lt;decltype(tag), decay_t&lt;decltype(data)&lt;, decay_t&lt;decltype(child)&gt;...&lt</code>
that's direct initialized with the forwarded arguments.
</details>

<details>
<summary><code><i>MAKE-ENV</i>(qry, value)</code></summary>
The expression <code><i>MAKE-ENV</i>(qry, value)</code> creates a queryable object <code>env</code> such that for a pack of argments <code>a ...</code> the expression <code>env.query(qry, a...)</code> yields <code>value</code>.
</details>

<details>
<summary><code>constexpr bool <i>MATCHING-SIG</i>&lt;F1, F2&gt;</code></summary>
Determines if the two function signatures <code>F1</code> and <code>F2</code> match:

If <code>same_as&lt;F1, R1(A1...)&gt;</code> and <code>same_as&lt;F2, R2(A2...)&gt;</code> then
<code><i>MATCHING-SIG</i>&lt;F1, F2&gt; == same_as&lt;R1(A1&amp;&amp;...), R2(A2&amp;&amp;...)&gt;</code>.
</details>

<details>
<summary><code>concept <i>movable-value</i>&lt;T&gt;</code></summary>
Determines if objects of type <code>T</code> are movable, non-array values:
<code>move_constructible&lt;decay_t&lt;T&gt;&gt; &amp;&amp; constructible_from&lt;decay_t&lt;T&gt;&gt; && (!is_array_v&lt;remove_reference_t&lt;T&gt;&gt;)</code>
</details>

<details>
<summary><code>struct <i>product-type</i>&lt;T...&gt;</code></summary>
The type <code><i>product-type</i>&lt;T...&gt;</code> is a <code>tuple</code>-like type.
Instead of constructors it supports only direct initialization, allowing it to hold
inplace constructed elements. It supports <code>p.get&lt;I&gt;()</code> and <code>p.apply(fun)</code>
member functions.
</details>

<details>
<summary><code>concept <i>queryable</i>&lt;T&gt;</code></summary>
Determines if objects of type <code>T</code> are queryable:
<code>destructible&lt;T&gt;</code>. The semantic implication is
that using the queryable object with query objects behave as required.
</details>

<details>
<summary><code><i>query-with-default</i>(tag, env, value)</code></summary>
Returns
<ul>
    <li><code>tag(env)</code> if this expression is well-formed</li>
    <li><code>value</code> otherwise</li>
</ul>
</details>

<details>
<summary><code>concept <i>receiver-of</i>&lt;Rcvr, Signatures&gt;</code></summary>
This concept determines if an object of type <code>Rcvr</code> is a receiver (i.e., <code>receiver&lt;Rcvr&gt;</code> is <code>true</code>) and supports each of the completion signatures in <code>Signatures</code>.
</details>

<details>
<summary><code><i>SCHED-ENV</i>(sch)</code></summary>
The expression <code><i>SCHED-ENV</i>(sch)</code> yields a queryable <code>o</code> such that
<ul>
  <li><code>get_start_scheduler(o)</i> is equivalent to <code>get_start_scheduler(get_env(o))</code></li>
  <li><code>get_domain(o)</i> is equivalent to <code>get_start_scheduler(get_env(o))</code></li>
</ul>
</details>

<details>
<summary><code><i>stop-when</i>(<i>sndr</i>, <i>token</i>)</code></summary>
</details>

<details>
<summary><code><i>TRY-QUERY</i>(q, tag, a...)</code></summary>
Tries to apply the query <code>tag</code> to the queryable object <code>q</code>:
if passing the arguments <code>a...</code> is valid passes these arguments (<code><i>AS-CONST</i>(q).query(tag, a...)</code>), otherwise
queries <code>q</code> for <code>tag</code> without the arguments (<code><i>AS-CONST</i>(q).query(tag)</code>), although the
arguments are evaluated.
</details>

<details>
<summary><code>concept <i>valid-completion-for</i>&lt;Signature, Rcvr&gt;</code></summary>
This concept determines if an object of type <code>Rcvr</code> supports the completion signature <code>Signature</code>.
</details>

<details>
<summary><code>concept <i>valid-specialization</i>&lt;T, Args...&gt;</code></summary>
This concept determines the class template <code>T</code> can be specialized with arguments <code>Args...</code>, i.e., if <code>T&lt;Args...&gt;</code> is valid.
</details>
