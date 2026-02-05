# Supporting Modules

Author: Dietmar Kühl dietmar.kuehl@me.com
Date: 2026-02-05

Originally [`beman.execution`](https://github.com/bemanproject/execution)
was implemented with out `module` support. Eventually, `module`
support in all compilers and in [`cmake`](https://cmake.org/) made
it reasonable to add `module` support. This document describes some
of the experiences of the journey adding `module` support for
[`beman.execution`](https://github.com/bemanproject/execution). It is
likely that my attempts on how to support `module`s were misguided and
I'm happy to learn how things can be done properly or better.

## History of Adding Module Support

First, let's start describing what I did. The starting point for
that was a working
[`beman.execution`](https://github.com/bemanproject/execution)
implementation which had no `module` support. It had tests and some
examples and the `CMakeLists.txt` built these. The default build
procedure run from the `Makefile` was already set up to use `ninja`.

Claus Klein had started to land `cmake` support of `module`s. In
particular there were `cmake` rules to detect whether there is a
dependency scanner and whether `import std;` is supported. The
statement from Claus was that using `import std;` would be the
second step - the first step is to actually create a `module` for
[`beman.execution`](https://github.com/bemanproject/execution)
including the standard library headers.

### Use `export using`

I had tried to create a `module` right after the Hagenberg meeting
at the end of 2025. Since I was clueless how to go about that I had
asked peolple who worked on the `module` specification and implemented
the corresponding support in compilers. The recommendation was to
create a `module` file, include all the headers, start the `module`,
and have `export using <name>;` declarations. That is, something
like this:

```c++
module;

#include <beman/execution/execution.hpp>

export module beman.execution;

namespace beman::execution {
    export using ::beman::execution::forwarding_query_t;
    // more of these - one for each name to be exported
}
```

Creating a file like that was fairly mechanical work and I just did
it.  I tried it. There were a bunch of errors which I could work
out easily but eventually I got stuck with unscruitible errors.
Since I never planned to write about the experience I don't recall
what the errors were.  When I asked about it the message was that
neither `cmake` nor the compilers are quite there. So I abandoned
this first attempt.

Time passed and in the beginning of 2026 some `cmake` support for
`module`s was added via a PR to the repository. The code wasn't
ready to use `module` but it seems reasonable to retry. A first
feeble try to get things built using the `export using` approach
again resulted in errors and there were voice stating that this
won't work but `module` support should actually work. The error
messages seemed to imply that I should `export` the declarations
immediately. So I abandoned that particular approach, again.

### Generate Module Friendly Code: `mk-module.py`

The attempts up to this point had indicated a few things which
were somewhat misaligned with the way the code in
[`beman.execution`](https://github.com/bemanproject/execution)
is laid out. The code is structured into lots of small _components_
(loosely based on John Lakos's idea of components in his 1996 version
of "Large-Scale C++"). Each component consists of

<ul>
  <li>
  a header declaring an entity like a class/class template, a
  function/function template, or a concept as well as everything
  needed to do so;
  </li>
  <li>
  a test file verifying that everything promised is, indeed, defined
  and confirm using tests that it works;
  </li>
  <li>
  an optional source file with the definitions of what is declared
  in the header; in case of templates the header will actually
  contain these definitions
  </li>
</ul>

Each component's file includes all necessary headers and a component
`A` is said to _directly depend on a component_ `D` if any of `A`'s
files includes `D`'s header. Creating a graph with the components
as nodes and directed edges from each component to all components
it directly depends on results in dependency graph which does not
contain any cycles. Building the code without `module`s just works
fine.

When trying directly `export` the declarations of names when they
are first declared, it quickly transpired that this doesn't work
due to the structure of `module` files required by contemporary
compilers:

<ol>
  <li>
  The first 7 charactesr of a `module` file shall be `module;`
  without anything preceding them.  [`g++`](https://gcc.gnu.org/)
  and [`clang++`](https://llvm.org/) are somewhat relaxed about
  this requirement but some compiler is rather strict. This, however,
  is at least easily achieved.
  </li>
  <li>
  Any `export` of a name has to follow the `module`'s name declaration,
  in this case after `export module beman.execution;`. This makes
  sense: the compiler needs to know what `module` the names belong to.
  </li>
  <li>
  The problem comes with any standard library header included after
  this name declaration: that entirely confuses the compilers. That
  is, all headers really need to be included before the name
  declaration. That is pretty much _not_ how the components in
  [`beman.execution`](https://github.com/bemanproject/execution) are
  organized.
  </li>
</ol>

To still achieve the objective of `export`ing a name when it first
gets declared, the structure needs to be changed. However, the components
are organized in a consistent structure. So the idea is to use this structure
to reorganize the files for `module` builds:

<ol>
  <li>
  Add an `export` keyword in front of any declaration which should be
  `export`ed (well, really a name which can be defined so the headers can
  function both when building a `module` and when just using headers; the
  implementation uses `BEMAN_EXECUTION_EXPORT`).
  </li>
  <li>
  Use a script (named
  [`bin/mk-module.py`](https://github.com/bemanproject/execution/blob/main/bin/mk-module.py))
  to create a `module` definition:
    <ol>
      <li>
      Start the file with `module;` (and some header stating the
      file is generated).
      </li>
      <li>
      Create a list of all used standard library headers and put
      these right below the files head.
      </li>
      <li>
      Add the `module` name declaration.
      </li>
      <li>
      Copy the declaration from all the components in correct
      dependency order, i.e., each component's declarations is
      preceded by the declarations it directly depends on.
      </li>
    </ol>
  </li>
  <li>Profit!</li>
</ol>

Creating the script to write the file was reasonably straight forward
although I spend way too much time making it fancy and include
suitable `#line` directives to find the actual source. Compiling
the resulting still didn't quite work, of course. There was a bunch
of silly errors in the component headers which could be quickly
resolved, though. That wasn't quite as true for the test files,
though (more [ruminations on tests](#modules-vs-testing) below):

- Many tests didn't include all standard library headers they
  dependent on. Since the corresponding header were actually included
  by a component header things still worked. So, the corresponding
  headers needed to be added.
- Instead of `#include <beman/execution/execution.hpp>` the tests
  now use `import beman.execution;` (well, the test really use
  conditional compilation to either use a header or an `import`
  statement). Including any standard library header _after_ the
  `import` statement again confuses the compiler, i.e., some
  reordering in the files was needed: the test files deliberately
  included the component's header first (to make sure all needed
  headers are included by the component header) but this include
  statement is now replaced by the `import` statement.
- The tests actually use some of the implementation-defined entities
  which were not meant to be `export`ed. To still have these tests
  I ended up `export`ing the necessary implementation-defined names.
  That needs to be corrected eventually (assuming that is actually
  possible which isn't quite as clear).
- Of course, the tests actually used the various names and it turned
  out that quite a few names, e.g., the `operator|`, were missing.

That worked OK with one compiler. Then I tried a different compiler
and lots of issues emerged:

- More names needed to be `export`ed for the tests.
- Some things just didn't compile at all and needed to be changed
  (I managed to avoid the problems but I haven't quite understood why).
- Symbols were undefined.

I ended up spending quite a bit of time reshuffling where headers
go, fixing some actual bugs, and working around what looks like
compiler problems. Most of that was, however, fairly mechanical
and eventually I got a `module` declaration working with all major
C++ compilers (using recent versions of each).

### Retry `export using`

Using a script to generate a `module` file restructured to better
match the `module` needs did get me a working `module` definition.
However, that shouldn't really be necessary. While fixing various
minor bugs I did fix a few things which looked as if they may have
had an impact when trying to use `export using <name>`. So tried
that approach again and this time it worked, at least, for some of
the compilers.  That looked promising!

One compiler put up a fight, though! I'm using the an exposition-only
`product_type` class template as is described in
[[exec]](https://eel.is/c++draft/exec#snd.expos-17) and I got a
compiler error about using <code>std::get&lt;N&gt;(<i>sender</i>)</code>.
After some experimentation I found that `export`ing the `product_type`
template and the relevant `tuple_size` and `tuple_element`
specializations I could resolve this problem, too.

Once I got past that I encountered a problem which is probably quite
common: following the specification of exposition-only `impls_for`
class template I used lambda functions for the various "overrides".
One compiler complained about undefined symbols about these! Of
course, using lambda functions in a header is problematic because
each instance of a lambda function has a different type, even if
they are spelled identical! So I replaced all of these lambda functions
by `struct`s which only have one member which is an `operator()`.

With that I also got a working `module` definition. While I'm quite
fond of my generator I prefer this approach! There shouldn't be a
need to rewrite an implementation just to make it a `module`. I
should also get away not needing any macros to insert/remove the
`export` keywords from declarations. Instead, the `export`ed names
are just listed in the module definition file. What is currently
missing is a bit of a clean-up to remove some of the artifacts.
Also, there may be more implementation details exported than is
actually necessary.

### `import std;`

Currently, `import std;` is _not_, yet, used. It should be straight
forward to conditionally choose between `import std;` and including
the headers.

## Changes Needed to Support Modules

When enabling modules I needed to apply quite a few, mostly
rather simple changes. Some of the necessary changes did take
me a bit to actually discover. Here is broadly what I needed
to change:

- I had slotted and `import beman.execution;` in where the
    the project header(s) were included. Some headers came
    before, others came after. It seems that doesn't work:
    any header should preceded the `import` statements or
    the `module` name declaration.

- Especially in the tests I hadn't always included all headers
    for standard library components which may be potentially
    used. However, these are necessary, even if the standard
    library component isn't even named and just used. For
    example:

    ```c++
    #include <tuple>
    import beman.execution;
    namespace ex = beman::execution;

    int main() {
        auto[rc] = *ex::sync_wait(ex::just(0));
        return rc;
    }
    ```

    Removing `#include <tuple>` causes a compilation failure:
    `sync_wait` return an `std::optional<std::tuple<T...>>` and the
    structured binding needs to know about the `std::tuple`. Oddly,
    the `std::optional` can be deferenced.

- My biggest blocker was the definition of `join_env`: the original
    definition used a `requires` clause which checked whether at
    least one of two expressions were values. Implementation used
    an `if constexpr` to decide whether the first of the two
    expression is value and used that and otherwise the other
    expression would be used. That is, something like this:

    ```c++
    template <typename E1, typename E2>
    struct join_env {
        E1 e1;
        E2 e2;
        template <typename Q>
            requires(
                requires(Q q, const E1& e){ q(e); } ||
                requires(Q q, const E2& e){ q(e); }
            )
        auto query(Q q) const noexcept {
            if constexpr (requires(Q q, const E1& e){ q(e); })
                return q(this->e1);
            else
                return q(this->e2);
        }
    };
    ```

    However, the compiler insisted in the definition of the function
    that neither of the two expressions from the `requires` clause
    was valid. Eventually I just turned the `query` into two
    overloads, the first requiring that `q(e1)` is valid and the
    second requiring that `q(e1)` is not valid but `q(e2)` is valid.
    I think it was only one compiler causing this issue.

## Scanning and Building

One of the things which seems odd is that each time `beman.execution`
is built, the files are scanned for dependencies. That scanning
often takes longer than the actual build of the respective object
file.  Also, since the `module` gets rebuild, all the tests `import`ing
the `module` get built again. When developing with including headers
only the tests which included modified headers (possibly indirectly)
needed to be rebuilt. Since the components and tests were created
in dependency order, that normally meant that only one test needed
to be rebuilt. Only if an already tested component needed to be
changed multiple tests needed to be built.

The promise of `module`s was that builds get faster. I don't have
objective measurements but it seems the development actually got
slower. While concentrating on fixing a particular component I
often removed all other tests and the examples from the build.

## Modules vs. Testing

Testing the `module` components is, yet, another issue! There are
a few issues and I haven't worked out all of them, yet:

1. There are quite a few classes and functions which are implementation
    details. I like to test these. In fact, I normally don't use `private`
    member functions because I can't test them. Instead, this functionality
    would become `public` members of an implementation class which then
    becomes a `private` member of the actual component. The implementation
    class can be tested separately. However, anything which isn't `export`ed
    isn't accessible from outside the `module`.

    I still want to test that the `module` works, including all the
    implementation details. Currently, the implementation details
    are tested by just using the headers to get the declarations.
    That doesn't seem quite right, though. Maybe the way to is to
    have a second `module` for the implementation details, say,
    `beman.execution.detail`, and use that to test the implementation
    details.

2. Some tests would benefit from common tools. For example, there could be
    a `test::scheduler` which is used to verify that the various scheduling
    operations do the right thing. A `test::scheduler` would be defined in
    a header and included into each test. Of course, the `test::scheduler`
    would need the declarations of some `module` components, e.g., of
    `set_value_t` and, thus, use `import beman.execution`.

    That didn't quite occur to me but it _seems_ that may actually work!
    An initial test seems to show that the compilers do not get upset about
    multiple `import beman.execution` statements.

## Conclusion

So far it isn't clear to me whether `module`s do provide a benefit. The
main sticking points are:

1. I don't known, yet, how to test implementation details without `export`ing
    them.
2. The "scan deps" step seems to take quite long.
3. So far I haven't managed to avoid `export`ing some of the implementation
    details. However, that _may_ be due to some uses actually requiring them.
4. There is different behavior between different compilers.

Some of the issues I encountered are likely due to ignorance: probably
all issues can be resolved with a bit of adjusted practices.
