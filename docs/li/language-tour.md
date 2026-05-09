# Taolrónj

## Variabelen

Mit `loat` leits te eine naam vast.

```platlang
loat naam = "Jan"
naam = "Mia"
```

## Funksies

Funksies zeen globale declaraties. Mit `trok` geufs te eine waerde truuk.

```platlang
funksie dubbel(getal):
    trok getal * 2
enj
```

## Veurwaerde

`es` is ein statement. `angesj:` is neet verplicht.

```platlang
es getal > 10:
    aafdrokke("groot")
angesj:
    aafdrokke("klein")
enj
```

## Mepkes

`mepke`-tabellen kinne diene es reekse, kaarte, records, en naamruimtes.

```platlang
loat gebroeker = { naam = "Alice", leefteid = 30 }
aafdrokke(gebroeker.naam)
```

## Lusse

`veur`-lusse zeen half-open. `veur i = 0, 10:` luip van `0` tot en mit `9`.

```platlang
veur i = 0, 10:
    aafdrokke(i)
enj
```
