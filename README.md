<p align="center">
  <img src="logo/plat-lang-logo.svg" alt="plat-lang logo" width="220">
</p>

<h1 align="center">plat-lang</h1>

<p align="center">
  An experimental interpreter language where code stays small, rules stay
  regular, and the keywords come from Limburg.
</p>

<p align="center">
  <a href="https://github.com/ifilot/plat-lang/actions/workflows/ci.yml"><img src="https://github.com/ifilot/plat-lang/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT"></a>
</p>

## Example

```platlang
funksie fib(n):
    es n < 2:
        trok n
    angesj:
        trok fib(n - 1) + fib(n - 2)
    enj
enj

aafdrokke(fib(10))
```

## Language Boundary

The current language includes primitive values, variables, expressions,
top-level global functions, conditionals, loops, mutable `tabel` tables,
localized diagnostics, and a tiny standard library.

The standard library currently contains only:

| Function | Purpose |
| -------- | ------- |
| `aafdrokke(value)`, `aafdrökke(value)` | Print one value followed by a newline. |
| `inveure()` | Read one input line as text, or return `niks` at end of input. |
| `waatis(value)` | Print the runtime type name of one value. |

Modules, imports, classes, methods, exceptions, closures, first-class
functions, static types, and the broader standard library are intentionally
outside the current language.

More scripts live in `examples/en` and `examples/li`. The `li` examples use
Limburgish comments.

## Build

Requires CMake, Bison, Flex, and a C++20 compiler.

On Debian/Ubuntu:

```sh
sudo apt-get update
sudo apt-get install -y cmake g++ flex bison
```

Then build:

```sh
cmake -S . -B build
cmake --build build
```

## Development Builds

Every successful push to the `develop` branch publishes a continuously updated
unstable build at the
[`develop-latest` prerelease](https://github.com/ifilot/plat-lang/releases/tag/develop-latest).
Use tagged releases for stable versions.

## Run

Execute a script:

```sh
build/platlang examples/en/fibonacci.plat
```

Print the version:

```sh
build/platlang --version
```

Print the AST:

```sh
build/platlang --ast examples/en/fibonacci.plat
```

Use Limburgish diagnostics:

```sh
build/platlang --lang li examples/li/fibonacci.plat
```

## Test

```sh
ctest --test-dir build
```

## Development Tools

List lexer keywords, type words, and built-ins from the implementation source:

```sh
dev/list_keywords.py --format json
```

Release packages include the same data as `platlang_keywords.json`.

## Documentation

```sh
python -m venv ~/.venv
source ~/.venv/bin/activate
pip install -r docs/requirements.txt
mkdocs serve -f docs/mkdocs.yml
```

Build static docs:

```sh
mkdocs build -f docs/mkdocs.yml --strict
```

Published documentation tracks the current executable. Older executables remain
available through Git tags/releases, but the website documents the current
language.

## Project Notes

Design notes live in `dev/design.md`.
Code style lives in `dev/code-style.md`.

## License

This implementation is licensed under the MIT License. Programs written in
`plat-lang` are not affected by the implementation license.
