# Language Tour

## Variables

Variables are declared with `loat`.

```platlang
loat naam = "Jan"
naam = "Mia"
```

## Functions

Functions are global declarations. Return values use `trok`.

```platlang
funksie dubbel(getal):
    trok getal * 2
enj
```

## Conditionals

`es` is a statement. `angesj:` is optional.

```platlang
es getal > 10:
    aafdrokke("groot")
angesj:
    aafdrokke("klein")
enj
```

## Tables

`mepke` tables act as arrays, maps, records, and namespaces.

```platlang
loat gebroeker = { naam = "Alice", leefteid = 30 }
aafdrokke(gebroeker.naam)
```

## Loops

`veur` loops are half-open. `veur i = 0, 10:` runs from `0` through `9`.

```platlang
veur i = 0, 10:
    aafdrokke(i)
enj
```
