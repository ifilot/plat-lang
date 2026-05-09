# Aan De Geng

## Installeer Benodigdhede

Op Debian of Ubuntu:

```sh
sudo apt-get update
sudo apt-get install -y cmake g++ flex bison
```

## Bouwe

```sh
cmake -S . -B build
cmake --build build
```

## Schriefsel Oetveure

```sh
build/platlang examples/li/fibonacci.plat
```

## AST Aafdrokke

```sh
build/platlang --ast examples/li/fibonacci.plat
```

## Limburgse Meldinge

```sh
build/platlang --lang li examples/li/fibonacci.plat
```

## Tests Dreije

```sh
ctest --test-dir build
```
