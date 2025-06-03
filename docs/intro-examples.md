<!-- SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception -->

# Introduction by Example

This page provides a series of examples showing how to use the
`std::execution` components.

<details>
<summary><code>"Hello, world"</code> - synchronous using asynchronous components</summary>

Code: [`examples/intro-1-hello-world.cpp`]()
@include examples/intro-1-hello-world.cpp

The first example is a very complicated way to a version of `"Hello,
world"`: it uses components for dealing with asynchronous work to
synchronously produce the result. The intention is to show a basic
use of some involved components to build up a feeling of how things
work.

The components for `std::execution` are declared in the header
`<execution>`.  This particular implementation implements the
cmponents in namespace `beman::execution` declared in the header
`<beman/execution/execution.hpp>`:

```cpp
#include <beman/execution/execution.hpp>
#include <iostream>
#include <string>
#include <tuple>

namespace ex = ::beman::execution;
using namespace std::string_literals;
```

Most of these declarations should be familiar. The namespace alias
`ex` is used to support easy migration to a different implementation,
in particular the standard name `std::execution` once it becomes
available with standard library implementations. The other examples
will have a similar start which is only mentioned in the explanation
to point out unusual parts like the use of custom components.

All interesting work happens in the `main` function:

```cpp
int main()
{
    auto[result] = ex::sync_wait(
        ex::when_all(
            ex::just("hello, "s),
            ex::just("world"s)
        )
        | ex::then([](auto s1, auto s2){ return s1 + s2; })
        ).value_or(std::tuple(""s));

    std::cout << result << '\n';
}
```

This code code be simpler even when using components from
`std::execution`.  Showing a few more components is intended to
better reflect how an asynchronous program might look like. This
examples uses a _sender factory_ (`ex::just`), two _sender adaptors_
(`ex::when_all` and `ex::then`), and finally a _sender consumer_
(`ex::sync_wait`) to build up work and to execute it. The idea of
a _sender_ is that it represents work which can be composed with
algorithms into a unit of work which is eventually executed.

Each work item can complete asynchronously at some later time, i.e.,
calling it like a function and using a returned value isn't really
an option. Instead, when the work is started it does whatever is
needed to get the work completed and get a _completion signal_
delivered. Delivering a completion signal consists of calling a
function on a suitable objects. The important part is that once
work is started it always delivers exactly one completion signal
which can indicate success, failure, or cancellation. Later examples
for creating senders will go into more details about the cancellation
signals.

The components used in this example do all of that synchronously:

- `ex::just("string"s)` completes immediately when started with
    successful completion which includes the string passed as
    argument.
- <code>ex::when_all(_sender1_, _sender2_)</code> starts the senders
    passed as arguments. When all of the senders complete, it
    produces its own completion. In the case of success all the
    received values are passed to the completion signal. In case
    of an error all outstanding work is cancelled and the first
    error becomes `when_all`'s completion signal once all children
    have completed.  Similarly, in case of cancellation all children
    get cancelled and once all complete `when_all` produces a
    cancellation signal. In the example the two children each produces
    one string as completion signal and `when_all` produces these two
    strings as its completion signal.
- <code>_sender_ | ex::then(_fun_)</code> is equivalent to using
    <code>ex::then(_sender_, _fun_)</code>. The `ex::then` calls
    the function <code>_fun_</code> with its child sender completes
    successful.  The arguments to <code>_fun_</code> are the values
    received from the child completion signal.  In the example, the
    child is `when_all(...)` and it produces two strings which are
    passed to <code>_fun_</code>. The completion signal of `ex::then`
    is successful with the value returned from the call to
    <code>_fun_</code> (which may `void`) if the call returns
    normally. If an exception is thrown `ex::then` completes with
    an `std::exception_ptr` to the exception thrown. In the example
    the completion is just a concatenation of the two strings.
- <code>sync_wait(_sender_)</code> starts its argument and then
    blocks until the work completes although the thread calling
    `sync_wait` may contribute to the completion of the work. The
    function returns a an
    <code>std::optional&lt;std::tuple&lt;_results_...&gt;&gt;</code>>.
    If the child sender completes successfully the values from the
    child's completion signal become the elements of the tuple. If
    the child completes with an error, the error is thrown as an
    exception. Otherwise, if the work gets cancelled, an empty
    `std::optional<...>` is returned. In the example, the child
    sends a string which gets wrapped into a `std::tuple` which in
    turn gets wrapped into an `std::optional`. Thus, the somewhat
    round-about way to get the result: first using
    `value_or(std::tuple(""s))` to get the value from the `std::optional`
    which is then decomposed from the `std::tuple` using structured
    bindings.

</details>

<details>
<summary><code>"Hello, async"</code> - a simple asynchronous example</summary>

Code: [`examples/intro-2-hello-async.cpp`]()
@include examples/intro-2-hello-async.cpp

</details>
