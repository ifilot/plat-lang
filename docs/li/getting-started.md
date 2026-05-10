# Oan De Geng

Deze commando's bouwe en dreije de huidige interpreter. De taol die hie
besjreve weurt is de kern mit de minimale standaardbibliotheekfunksies
`aafdrokke` / `aafdrökke` en `invuier`.

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

Ein regel van standaardinvoer laeze:

```platlang
aafdrökke("Wie heits te?")
loat naam = invuier()
aafdrökke("hoi " + naam)
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

## Documentatie

Gepubliceerde documentatie volg de letste release van de oetveurder. Auwere
oetveurderversies blieve besjikbaar via Git-tags/releases, mer de website
besjrieft de huidige taol.

Bouw de huidige working tree mit:

```sh
mkdocs build -f docs/mkdocs.yml --strict
```
