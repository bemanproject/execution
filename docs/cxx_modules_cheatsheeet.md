
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
```cpp
module;
#include <boost/config.hpp>

#ifdef USE_STD_MODULE
import std;
#else
#include <memory>
#include <type_traits>
#endif

export module boost.any;
#include <boost/any.hpp>  // exported
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
