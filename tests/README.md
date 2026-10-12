# Unit Test Rules for Agents (`tests/`)

This document defines how to write unit tests for decompiled `src/` code.
Read it before adding or modifying any test. The core decompilation workflow
lives in the repo root; this file only covers the test suite.

## Framework

- **Catch2 v2** (single header, vendored as `catch.hpp`; do not modify it).
- `catch_main.cpp` defines `CATCH_CONFIG_MAIN` — it provides the `main()`, so
  **no test file may define its own `main`**.
- Tests use `TEST_CASE`, `SECTION` and `REQUIRE`.

## Directory structure — mirror `src/`

`src/` is organized by module (`client/Fresh`, `client/ProjectG`,
`client/Wangreal`, `shared`, ...). Tests mirror that, one folder per module,
one file per unit:

```
tests/<module>/test_<symbol>.cpp
```

Example: `client/Fresh/source/FrArea.cpp` → `tests/Fresh/test_FrArea.cpp`.

Use Catch2 tags for filtering, always starting with the module:
`TEST_CASE("...", "[Fresh][FrArea]")`. Run a subset with
`YapangHostUnitTest "[Fresh][FrArea]"` (or `YapangHostUnitTest "[FrArea]"`).

## Host-buildability gate (mandatory)

Only add tests for units that **compile and link standalone on the host**.
This project targets PE32/Windows: many headers include `<windows.h>` and the
Wangreal view stack (D3D), which a host has no SDK for. A unit is directly
testable when it compiles against the real headers; when it pulls in heavy
platform dependencies, supply stand-ins instead (next section).

Verify a unit compiles on the host with:

```bash
clang++ -std=c++17 -fms-extensions \
    -Itests/shim -Isrc -Isrc/client/Fresh/include \
    -Isrc/client/ProjectG -Isrc/client/Wangreal/include -Isrc/shared \
    -fsyntax-only src/client/Fresh/source/FrArea.cpp
```

(`-fms-extensions` enables the `__declspec(novtable/thread)` the sources use.)

## Shims and stand-ins

Platform-bound dependencies are replaced, never patched in `src/`:

- **Header shims** live in `tests/shim/` and are put on the include path
  *before* `src/`, so a heavy `#include` resolves to a minimal host version:
  - `shim/windows.h` — the handful of Win32 types the Fresh headers need
    (`tagRECT`, `tagBITMAPINFO`, ...). Add a type only when a host test needs it.
  - `shim/frgraphicinterface.h` — replaces the Fresh GDI header, which pulls the
    whole Wangreal view stack; keeps only `DrawTexture` + `FrALPHA`.
- **Link stand-ins** live in `<module>/<unit>_standins.cpp` and define the
  symbols the unit under test calls but does not own (e.g. `FrWnd`, the window
  manager, the document, `Bitmap`). They are linked into the test executable;
  the real units are deliberately **not** compiled.

The code under test is always the real `src/**/*.cpp` unit. Only its
dependencies are inert.

Rules:
- Never modify `src/` to make code testable (no test-only accessors, no
  removing `#ifdef`s).
- Keep shims minimal and commented with why the real header cannot be used.
- Protected members are reached through a small `Test<Unit>` subclass in the
  fixture (`using Unit::method;`), not by editing `src/`.

## Build wiring

- Standalone unit tests get their own executable (`YapangHostUnitTest`) that
  compiles the needed `src/**/*.cpp` files into a static library
  (`YapangHostUnit`) — never link the full client for these.
- Add every `src` `.cpp` the unit depends on to `YapangHostUnit`; add the
  matching stand-in file to `YapangHostUnitTest`.
- Edit `tests/CMakeLists.txt` **directly**.

## Quick verification (no full build needed)

```bash
clang++ -std=c++17 -fms-extensions -Dstrcmpi=strcasecmp \
    -Wno-ignored-attributes -Wno-format \
    -Itests/shim -Itests -Isrc -Isrc/client/Fresh/include \
    -Isrc/client/ProjectG -Isrc/client/Wangreal/include -Isrc/shared \
    src/client/Fresh/source/FrArea.cpp \
    tests/Fresh/FrArea_standins.cpp tests/Fresh/test_FrArea.cpp \
    tests/catch_main.cpp -o /tmp/yapang_unit_test && /tmp/yapang_unit_test
```

Or via CMake:

```bash
cmake -S . -B build && cmake --build build
./build/bin/YapangHostUnitTest
```

## Semantics and style

- Assert the **behavior of the decompiled code** as written. Prefer hard-coded
  expected values over re-computing the expression under test.
- Write code the way the original developer would: structs/indexing over
  pointer arithmetic, `REQUIRE(a == b)` over boolean flips.
- No `volatile`, inline assembly, or compiler tricks in tests either.
- Reset recorded/global state at the start of every `TEST_CASE`/`SECTION`.
  Catch2 re-runs the case body per `SECTION`; do not rely on execution order.

## Do / Don't

| Do | Don't |
|---|---|
| Mirror `src/` module folders | Add tests for units with no host path |
| Shim heavy headers under `tests/shim/` | Modify `src/` for testability |
| Link inert stand-ins | Compile the real heavy units too |
| Reset recorded state per test | Rely on test execution order |
| Edit `tests/CMakeLists.txt` directly | Define a `main()` in a test file |
