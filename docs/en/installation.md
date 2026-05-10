# Installation

`plat-lang` can be compiled from source via the instructions provided below.
Alternatively, one can download a precompiled binary.

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

## Limburgish Diagnostics

```sh
build/platlang --lang li examples/li/fibonacci.plat
```

## Run Tests

```sh
ctest --test-dir build
```