# Getting Started

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
