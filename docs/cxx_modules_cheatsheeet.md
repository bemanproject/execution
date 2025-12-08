
# C++20/23 Modules – Quick Reference Cheat Sheet

This cheat sheet summarizes **the correct order of `#include`, `import`, and `export module`** in C++20/23 modules.

---

## 1️⃣ Module File Zones
```
[Zone A] Global Module Fragment  → before `export module`
[Zone B] Module Interface        → after `export module`
[Zone C] Module Implementation   → optional/private
```

---

## 2️⃣ Global Module Fragment (`module;`)
- Must appear **before** `export module`
- Allowed here:
  - `#include` legacy headers
  - macros
  - conditional compilation
  - rare `import`
- Example:
```cpp
module;
#include <boost/config.hpp>
#define INTERNAL_MACRO 1
```

---

## 3️⃣ Module Declaration
```cpp
export module my.module;
```
- Marks start of module interface
- Everything after is part of the module
- Prefer `import` instead of `#include` inside

---

## 4️⃣ Import Rules
- ✅ After module: `import std;`, `import bar;`
- ✅ Rarely allowed in global fragment
- ❌ Forbidden before module line:
```cpp
import std;
export module foo; // ❌
```

---

## 5️⃣ Include Rules
- ✅ Before `export module`: allowed (legacy headers)
- ✅ After `export module`: content becomes part of module
- ❌ System headers inside module body: may cause errors
Use `import std;` instead

---

## 6️⃣ Recommended Layout
```cpp
module;                 // Global fragment
#include <legacy.hpp>    // OK
#define HELPER 1          // OK

export module my.module;  // Module interface begins
import std;                // Prefer imports

export int foo();
#include "public_api.hpp"  // Exported
```

---

## 7️⃣ Header-Only Library Example

### the boost/any/modules/boost_any.cppm

```cpp
// Copyright (c) 2016-2025 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

module;

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/throw_exception.hpp>

// FIXME(CK): #include <boost/type_index.hpp>
import boost.type_index;

#ifdef BOOST_ANY_USE_STD_MODULE
import std;
#else
#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <utility>
#endif

#define BOOST_ANY_INTERFACE_UNIT

export module boost.any;

#ifdef __clang__
#   pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#endif

#include <boost/any.hpp>
#include <boost/any/basic_any.hpp>
#include <boost/any/unique_any.hpp>

```

### the boost/any.hpp

```cpp
// See http://www.boost.org/libs/any for Documentation.

#ifndef BOOST_ANY_INCLUDED
#define BOOST_ANY_INCLUDED

#include <boost/any/detail/config.hpp>

#if !defined(BOOST_USE_MODULES) || defined(BOOST_ANY_INTERFACE_UNIT)

#ifndef BOOST_ANY_INTERFACE_UNIT
#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <memory>  // for std::addressof
#include <type_traits>

#include <boost/throw_exception.hpp>
#include <boost/type_index.hpp>

#endif  // #ifndef BOOST_ANY_INTERFACE_UNIT

#include <boost/any/bad_any_cast.hpp>
#include <boost/any/fwd.hpp>
#include <boost/any/detail/placeholder.hpp>

namespace boost {

BOOST_ANY_BEGIN_MODULE_EXPORT

    /// \brief A class whose instances can hold instances of any
    /// type that satisfies \forcedlink{ValueType} requirements.
    class any
    {
    public:

        /// \post this->empty() is true.
        constexpr any() noexcept
          : content(0)
        {
        }

        /// Makes a copy of `value`, so

        /// ... #### more code ###

    private: // representation
        template<typename ValueType>
        friend ValueType * unsafe_any_cast(any *) noexcept;

        friend class boost::anys::unique_any;

        placeholder * content;
        /// @endcond
    };

    /// Exchange of the contents of `lhs` and `rhs`.
    /// \throws Nothing.
    inline void swap(any & lhs, any & rhs) noexcept
    {
        lhs.swap(rhs);
    }

    /// ... ### more code ###

    /// \returns ValueType stored in `operand`
    /// \throws boost::bad_any_cast if `operand` does not contain
    /// specified ValueType.
    template<typename ValueType>
    ValueType any_cast(any & operand)
    {
        using nonref = typename std::remove_reference<ValueType>::type;

        nonref * result = boost::any_cast<nonref>(std::addressof(operand));
        if(!result)
            boost::throw_exception(bad_any_cast());

        // Attempt to avoid construction of a temporary object in cases when
        // `ValueType` is not a reference. Example:
        // `static_cast<std::string>(*result);`
        // which is equal to `std::string(*result);`
        typedef typename std::conditional<
            std::is_reference<ValueType>::value,
            ValueType,
            typename std::add_lvalue_reference<ValueType>::type
        >::type ref_type;

#ifdef BOOST_MSVC
#   pragma warning(push)
#   pragma warning(disable: 4172) // "returning address of local variable or temporary" but *result is not local!
#endif
        return static_cast<ref_type>(*result);
#ifdef BOOST_MSVC
#   pragma warning(pop)
#endif
    }

    /// \returns `ValueType` stored in `operand`
    /// \throws boost::bad_any_cast if `operand` does not contain
    /// specified `ValueType`.
    template<typename ValueType>
    inline ValueType any_cast(const any & operand)
    {
        using nonref = typename std::remove_reference<ValueType>::type;
        return boost::any_cast<const nonref &>(const_cast<any &>(operand));
    }

    /// \returns `ValueType` stored in `operand`, leaving the `operand` empty.
    /// \throws boost::bad_any_cast if `operand` does not contain
    /// specified `ValueType`.
    template<typename ValueType>
    inline ValueType any_cast(any&& operand)
    {
        static_assert(
            std::is_rvalue_reference<ValueType&&>::value /*true if ValueType is rvalue or just a value*/
            || std::is_const< typename std::remove_reference<ValueType>::type >::value,
            "boost::any_cast shall not be used for getting nonconst references to temporary objects"
        );
        return boost::any_cast<ValueType>(operand);
    }

BOOST_ANY_END_MODULE_EXPORT

}

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
// Copyright Antony Polukhin, 2013-2025.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif  // #if !defined(BOOST_USE_MODULES) || defined(BOOST_ANY_INTERFACE_UNIT)

#endif

```
---

## 8️⃣ Quick Rules Summary

| Phase                     | Allowed                                |
|----------------------------|---------------------------------------|
| Before `export module`     | `#include`, macros, conditional, rare `import` |
| After `export module`      | `import`, includes become part of module interface |
| Never                      | `import` before module line, system headers inside module body, macros leaking into exported API |

**Rule of Thumb:**
1. `module;` → legacy code
2. `export module X;` → module interface
3. `import` → after module
4. Include only when you want it exported

---

**Tip:** Follow this layout for Beman, Boost, fmt, JSON, or other header-only libraries to wrap them safely as modules.
