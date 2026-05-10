# Veurbeelde

## Juffen

Juffen is 't Nederlandse telspel achter dit soort uitdaging. Tel omhoog, mer
zeg `Juf` in plaats van 't getal es 't getal ein veelvoud van 7 is of 't
cijfer 7 bevat.

```platlang
# Druk de Juffen-reeks van 1 tot en mit 50.
funksie bevat_zeve(getal):
    es getal % 10 == 7:
        trok woar
    enj

    es getal >= 70 en getal < 80:
        trok woar
    enj

    trok neetwoar
enj

funksie is_juf(getal):
    trok getal % 7 == 0 of bevat_zeve(getal)
enj

veur getal = 1, 51:
    es is_juf(getal):
        aafdrokke("Juf")
    angesj:
        aafdrokke(getal)
    enj
enj
```

Oetvoer:

```text
1
2
3
4
5
6
Juf
8
9
10
11
12
13
Juf
15
16
Juf
18
19
20
Juf
22
23
24
25
26
Juf
Juf
29
30
31
32
33
34
Juf
36
Juf
38
39
40
41
Juf
43
44
45
46
Juf
48
Juf
50
```

## Groetsjte gemene deler

't Algoritme van Euclides löit herbruikbare globale funksies, veranderinge
binne lusse, en returnwaerdes zeen.

```platlang
funksie absoluut(getal):
    es getal < 0:
        trok -getal
    enj

    trok getal
enj

funksie groetsjte_gemene_deler(eins, twie):
    eins = absoluut(eins)
    twie = absoluut(twie)

    zolang twie != 0:
        loat rest = eins % twie
        eins = twie
        twie = rest
    enj

    trok eins
enj

aafdrokke(groetsjte_gemene_deler(84, 30))
aafdrokke(groetsjte_gemene_deler(1071, 462))
```

Oetvoer:

```text
6
21
```

## Priemgetalle

Dit veurbeeld gebroek ein hulpfunksie mit vroege `trok`-stappe en ein lus die
onnuedige kandidaote euversjleit.

```platlang
funksie is_priem(getal):
    es getal < 2:
        trok neetwoar
    enj

    es getal == 2:
        trok woar
    enj

    es getal % 2 == 0:
        trok neetwoar
    enj

    loat deler = 3
    zolang deler * deler <= getal:
        es getal % deler == 0:
            trok neetwoar
        enj

        deler = deler + 2
    enj

    trok woar
enj

aafdrokke("Priemgetalle onder 50:")

veur getal = 1, 50:
    es is_priem(getal):
        aafdrokke(getal)
    enj
enj
```

Oetvoer:

```text
Priemgetalle onder 50:
2
3
5
7
11
13
17
19
23
29
31
37
41
43
47
```

## Zeef van Eratosthenes

Portefeuils kinne diene es veranderbare reekse. Deze zeef markeert
samengestelde getalle en drukt daonao alle priemgetalle onder eine limiet aaf.

```platlang
loat limiet = 50
loat is_samengesteld = {}

veur getal = 0, limiet + 1:
    is_samengesteld[getal] = neetwoar
enj

loat priem = 2
zolang priem * priem <= limiet:
    es !is_samengesteld[priem]:
        loat veelvoud = priem * priem

        zolang veelvoud <= limiet:
            is_samengesteld[veelvoud] = woar
            veelvoud = veelvoud + priem
        enj
    enj

    priem = priem + 1
enj

aafdrokke("Priemgetalle oet de zeef:")

veur getal = 2, limiet + 1:
    es !is_samengesteld[getal]:
        aafdrokke(getal)
    enj
enj
```

Oetvoer:

```text
Priemgetalle oet de zeef:
2
3
5
7
11
13
17
19
23
29
31
37
41
43
47
```

## Punte-samevatting

Dit is ein klein data-verwerkend programma. 't Bewaart punte in ein
portefeuil, rekent ein totaal oet, haudt de laegste en hoegste waerde bie, en
deilt 't gemiddelde in.

```platlang
loat punte = { 92, 76, 88, 61, 95, 84 }
loat aantal = 6
loat totaal = 0
loat laegste = punte[0]
loat hoegste = punte[0]

veur teller = 0, aantal:
    loat punt = punte[teller]
    totaal = totaal + punt

    es punt < laegste:
        laegste = punt
    enj

    es punt > hoegste:
        hoegste = punt
    enj
enj

loat gemiddelde = totaal / aantal

aafdrokke("gemiddelde: " + gemiddelde)
aafdrokke("laegste: " + laegste)
aafdrokke("hoegste: " + hoegste)

es gemiddelde >= 90:
    aafdrokke("cijfer: A")
angesj:
    es gemiddelde >= 80:
        aafdrokke("cijfer: B")
    angesj:
        es gemiddelde >= 70:
            aafdrokke("cijfer: C")
        angesj:
            aafdrokke("cijfer: mieer oefene")
        enj
    enj
enj
```

Oetvoer:

```text
gemiddelde: 82.6667
laegste: 61
hoegste: 95
cijfer: B
```

## Records en totalen

Portefeuils kinne ouch diene es records. Dit veurbeeld modelleert ein klein
winkelkerfke mit geneste recordwaerdes en nómmer-sjleutels veur de reeks.

```platlang
funksie regel_totaal(artikel):
    trok artikel.pries * artikel.aantal
enj

loat kerf = {
    { naam = "sjriefbook", pries = 4.5, aantal = 3 },
    { naam = "potlood", pries = 1.25, aantal = 6 },
    { naam = "tas", pries = 18, aantal = 1 },
}

loat aantal = 3
loat subtotaal = 0

veur teller = 0, aantal:
    loat artikel = kerf[teller]
    loat regelbedrag = regel_totaal(artikel)

    aafdrokke(artikel.naam + ": " + regelbedrag)
    subtotaal = subtotaal + regelbedrag
enj

loat btw = subtotaal * 0.21
loat eindtotaal = subtotaal + btw

aafdrokke("subtotaal: " + subtotaal)
aafdrokke("btw: " + btw)
aafdrokke("totaal: " + eindtotaal)
```

Oetvoer:

```text
sjriefbook: 13.5
potlood: 7.5
tas: 18
subtotaal: 39
btw: 8.19
totaal: 47.19
```

## Viefkes mit geheuge

Recursieve funksies zeen handig, mer gewone recursieve Fibonacci herhaolt väöl
waerk. Ein portefeuil kin al berekende antwoorde vasthaaje.

```platlang
loat geheuge = {
    [0] = 0,
    [1] = 1,
}

funksie viefkes(getal):
    loat oet_geheuge = geheuge[getal]

    es oet_geheuge != niks:
        trok oet_geheuge
    enj

    loat resultaat = viefkes(getal - 1) + viefkes(getal - 2)
    geheuge[getal] = resultaat

    trok resultaat
enj

veur getal = 0, 16:
    aafdrokke("viefkes(" + getal + ") = " + viefkes(getal))
enj
```

Oetvoer:

```text
viefkes(0) = 0
viefkes(1) = 1
viefkes(2) = 1
viefkes(3) = 2
viefkes(4) = 3
viefkes(5) = 5
viefkes(6) = 8
viefkes(7) = 13
viefkes(8) = 21
viefkes(9) = 34
viefkes(10) = 55
viefkes(11) = 89
viefkes(12) = 144
viefkes(13) = 233
viefkes(14) = 377
viefkes(15) = 610
```

## Runtime-Type bekieke

`waatis(waerde)` is handig es data van versjillende plekke kump, of es eine
missende portefeuil-sjleutel `niks` kin oplevere.

```platlang
loat persoon = {
    naam = "Mia",
    leefteid = 31,
    actief = woar,
}

waatis(persoon.naam)
waatis(persoon.leefteid)
waatis(persoon.actief)
waatis(persoon.missend)
waatis(persoon)
```

Oetvoer:

```text
teks
nómmer
woar
niks
portefeuil
```

Tests höbbe eur eige fixtures ónger `tests/`; veurbeelde zeen veur lezers.
