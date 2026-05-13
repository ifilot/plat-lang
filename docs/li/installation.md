# Installacie

`plat-lang` kin oet de broncode gecompileerd waere mit de instructies
hierónger. Es alternatief kins te ein veurgebouwde binary downloade.

## Afhankelikhede Installere

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

## Ein Script Dreije

```sh
build/platlang examples/li/fibonacci.plat
```

Lees eine regel van standaardinvoer:

```platlang
aafdrökke("Wie heits te?")
loat naam = inveure()
aafdrökke("hoi " + naam)
```

## Limburgse Meldinge

```sh
build/platlang --lang li examples/li/fibonacci.plat
```

## Tests Dreije

```sh
ctest --test-dir build
```
