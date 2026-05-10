# Getting Started

These commands build and run the current interpreter. The language documented
here is the core language plus the minimal standard library functions
`aafdrokke` / `aafdrökke` and `invuier`.

## Install Dependencies

On Debian or Ubuntu:

```sh
sudo apt-get update
sudo apt-get install -y cmake g++ flex bison
```

## Build

```sh
cmake -S . -B build
cmake --build build
```

## Run a Script

```sh
build/platlang examples/en/fibonacci.plat
```

Read one line from standard input:

```platlang
aafdrökke("What is your name?")
loat name = invuier()
aafdrökke("hoi " + name)
```

## Print the AST

```sh
build/platlang --ast examples/en/fibonacci.plat
```

## Limburgish Diagnostics

```sh
build/platlang --lang li examples/li/fibonacci.plat
```

## Run Tests

```sh
ctest --test-dir build
```

## Documentation

Published documentation tracks the current executable. Older executables remain
available through Git tags/releases, but the website documents the current
language.

Build the current working tree with:

```sh
mkdocs build -f docs/mkdocs.yml --strict
```
