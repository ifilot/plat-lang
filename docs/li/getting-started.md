# Oan De Geng

Deze pagina geuf dich eine korte, stapsgewieze rónj door `plat-lang`. Elke stap
veug ein idee toe, zodat te kins beginne mit ein klein schriefsel en langzaam
oetkums bie de kernfeatures die echte programma's gebroeke. Es te de
interpreter nog neet gebouwd höbs, begin dan bie [Installacie](installation.md).
Zodra `build/platlang` bestaot, kins te elk schriefsel zo dreije:

```sh
build/platlang examples/li/hello.plat
```

!!! note "Diakritische teikes en ASCII-spellinge"

    Canoniek Limburgs gebroek väöl diakritische teikes. Die zeen zeldzaam in
    programmeertaole en soms lastig te tiepe. Daorum haet elke keyword of
    ingebouwde naam mit ein diakritisch teike ouch eine spelling zonder
    diakritisch teike in `plat-lang`. Bie veurbeeld: `trök` en `trok` zeen
    allebei geaccepteerd veur truukgaeve oet ein funksie.

## 1. Zeg Hallo

Ein `plat-lang` programma kin zo klein zeen es ein aanroep nao de ingebouwde
printfunksie:

```platlang
aafdrokke("Hallo, plat-lang")
```

`aafdrokke(value)` drukt ein waerde aaf mit ein newline. De dialectspelling
`aafdrökke(value)` waerkt ouch.

## 2. Waerdes Bewaore Mit `loat`

Mit `loat` declareers te variabelen. Tekste zeen `teks`, getalle zeen
`nómmer`, en `+` kin tekst aan-ein plakke.

```platlang
loat naam = "Jan"
loat taol = "plat-lang"
loat versie = 1

aafdrokke("Hallo, " + naam)
aafdrokke("Welkom bie " + taol + " " + versie)
```

Ein declaratie zonder waerde begint es `niks`, de afwezigheidswaerde van de
taol.

```platlang
loat later
aafdrokke(later)
```

## 3. Keuzes Make Mit `es`

Veurwaerde gebroeke `es`, optioneel `angesj:`, en eendige mit `enj`.

```platlang
loat naam = ""
loat fallback = "onbekend"

es naam:
    aafdrokke(naam)
angesj:
    aafdrokke(fallback)
enj
```

Truthiness haudt vroege programma's compact: `niks`, `kwatsj`, `0`, lege
tekst, en lege tabels zeen falsy; alle angere waerdes zeen truthy.

## 4. Waerk Herhaole Mit Lusse

Gebroek `zolang` es te wils herhaole zolang ein veurwaerde truthy blief:

```platlang
loat i = 0

zolang i < 3:
    aafdrokke(i)
    i = i + 1
enj
```

Gebroek `veur` veur ein half-open nómmerreeks. Dit drukt `0`, `1`, en `2`:

```platlang
veur i = 0, 3:
    aafdrokke(i)
enj
```

Binnen lusse geit `euversjlaon` door nao de volgende iteratie en `aafbraeke`
sjtopt de lus.

```platlang
veur i = 0, 10:
    es i == 3:
        euversjlaon
    enj

    es i == 8:
        aafbraeke
    enj

    aafdrokke(i)
enj
```

## 5. Herbruikbaar Waerk Naom Gaeve Mit Funksies

Funksies declareers te mit `funksie` en gaeve truuk mit `trok`.

```platlang
funksie dubbel(getal):
    trok getal * 2
enj

aafdrokke(dubbel(21))
```

Topniveau-funksies waere geregistreerd veurdat de oetveuring begint. Dao door
kin ein funksie ein angere funksie aanrope die later in hetzelfde besjtand
steit. Recursie waerkt ouch:

```platlang
funksie viefkes(getal):
    es getal < 2:
        trok getal
    angesj:
        trok viefkes(getal - 1) + viefkes(getal - 2)
    enj
enj

aafdrokke(viefkes(10))
```

## 6. Data Groepere Mit `tabel`-Tabellen

`tabel`-tabellen zeen veranderbare containers. Te kins ze gebroeke es
records:

```platlang
loat gebroeker = {
    naam = "Alice",
    leefteid = 30,
}

aafdrokke(gebroeker.naam)
aafdrokke(gebroeker["leefteid"])
```

Te kins ze ouch gebroeke es reekse. Nómmer-sjleutels beginne bie `0`.

```platlang
loat nommers = { 10, 20, 30 }

aafdrokke(nommers[0])
nommers[1] = 99
aafdrokke(nommers[1])
```

Berekende sjleutels laote ein programma zelf kieke wat 't moot laeze of
sjrieve:

```platlang
loat veld = "naam"
aafdrokke(gebroeker[veld])
```

## 7. Invoer Laeze

De minimale standaardbibliotheek bevat ouch `inveure()`, dat ein regel invoer
laes es `teks`.

```platlang
aafdrokke("Wie heits te?")
loat naam = inveure()
aafdrokke("hoi " + naam)
```

Dreij zo'n schriefsel mit gepijpde invoer:

```sh
printf 'Mia\n' | build/platlang examples/li/input.plat
```

## Woe Nao Toe

Ga door mit de [Taolrónj](language-tour.md) veur de volle huidige taolgrens,
inclusief ondersteunde operators, besjermde name, meldinge, en features die
bewust nog neet bie de taol hure.
