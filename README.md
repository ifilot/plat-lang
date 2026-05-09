# plat-lang

`plat-lang` is a small experimental programming language with Limburgian
keywords, a tiny runtime model, and an AST-first C++ implementation.

## Build

Requires CMake, Bison, Flex, and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build
```

## Run

Parse a script:

```sh
build/platlang examples/fibonacci.plat
```

Print the AST:

```sh
build/platlang --ast examples/fibonacci.plat
```

Use Limburgish diagnostics:

```sh
build/platlang --lang li examples/fibonacci.plat
```

## Project Notes

Design notes live in `dev/design.md`.
Code style lives in `dev/code-style.md`.
