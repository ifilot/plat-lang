# Taolrónj

Deze pagina besjrieft 't gedrag van de huidige `plat-lang` interpreter. Alles
wat hie neet geneump weurt, huurt nog neet bie de vaste taol.

## Taolgebied

De huidige taol bevat primitieve waerdes, variabelen, lexicale scopes,
expressies, globale funksies, veurwaerde, lusse, veranderbare
`portefeuil`-tabellen, gelokaliseerde meldinge, en twee minimale
standaardbibliotheekfunksies: `aafdrokke` / `aafdrökke` en `invuier`.

Modules, imports, klasses, methodes, exceptions, statische types, closures,
first-class funksies, en ein groetere standaardbibliotheek zeen nog neet
onderdeil van de huidige taol.

## Waerdes

De runtime-waerdes zeen:

| Waerde | Betekenis |
| ------ | --------- |
| `nómmer` / `nommer` | IEEE 754 double-precision nómmer |
| `teks` | tekst |
| `woar` | boolean woar |
| `neetwoar` | boolean neet woar |
| `niks` | gein waerde |
| `portefeuil` | veranderbare tabel |

`plat-lang` haet ein nómmertype. De taol maak gein versjil tösje integers en
floating-point nómmers.

```platlang
loat x = 10
loat y = 3.5
loat naam = "Mia"
loat actief = woar
loat nikske = niks
```

`niks`, `neetwoar`, `0`, eine lege tekst, en ein leeg portefeuil zeen falsy. Alle
angere waerdes zeen truthy.

## Variabelen

Mit `loat` leits te eine naam vast. Zonder beginwaerde kriegt de variabele
`niks`.

```platlang
loat naam = "Jan"
loat later
naam = "Mia"
```

Toewieze aan eine naam die nog neet gedeclareerd is, is ein fout. Eine naam
twee kier declarere in dezelfde scope is ouch ein fout.

Sleutelwäörd, runtime-typenaam, en standaardbibliotheekname zeen besjermd en
kinne neet opnieuw gedefinieerd waere. `nómmer` is de dialect-correcte
runtime-typenaam; `nommer` blief geaccepteerd es ASCII-spelling.

## Expressies

De huidige taol ondersteunt literals, variabelen, haakjes, unaire en binaire
operators, funksie-aanrope, en toegang tot portefeuils.

```platlang
loat resultaat = (2 + 3) * 4
loat ok = resultaat >= 10 en resultaat != 13
```

Belangrieke operators:

| Operators | Betekenis |
| --------- | --------- |
| `-`, `!` | negatie van nómmers en logische negatie |
| `*`, `/`, `%` | vermenigvuldige, deile, remainder |
| `+`, `-` | optelle/aftrekke; `+` plak ouch tekst es eine kant tekst is |
| `<`, `>`, `<=`, `>=` | nómmervergelijking |
| `==`, `!=` | geliek en neet geliek |
| `en`, `of` | logische operators |

Deile of remainder door nul is ein fout.

## Funksies

Funksies zeen globale declaraties op topniveau. Mit `trok` geufs te eine
waerde truuk.

```platlang
funksie dubbel(getal):
    trok getal * 2
enj

aafdrokke(dubbel(21))
```

Veur de oetveuring begint, registreert de interpreter alle topniveau-funksies.
Dao door kin ein funksie ein angere funksie aanrope die later in 't besjtand
staon. Mutueel recursieve globale funksies waerke ouch.

Ein `funksie` binne ein blok is ein syntaxfout. Funksies zeen gein waerdes: se
kinne neet in variabelen, argumente, returnwaerdes, of portefeuils gezat waere.

Ein funksie aanrope mit 't verkeerde aantal argumente is ein fout. Es ein
funksie tot aan 't inj kump zonder `trok`, geuf ze `niks` truuk. `trok` zonder
waerde geuf ouch `niks` truuk. `trok` boete ein funksie is ein fout.

## Veurwaerde

`es` is ein statement. `angesj:` is neet verplicht.

```platlang
es getal > 10:
    aafdrokke("groot")
angesj:
    aafdrokke("klein")
enj
```

Elke tak haet eine eige lexicale scope.

## Portefeuils

`portefeuil` is de eine samengestelde waerde. Portefeuils zeen veranderbaar en kinne
diene es reeks, kaart, record, of naamruimte.

```platlang
loat nommers = { 10, 20, 30 }
loat gebroeker = { naam = "Alice", leefteid = 30 }

aafdrokke(nommers[0])
aafdrokke(gebroeker.naam)
```

Reeks-entries gebroeke zero-based nómmers. Record-entries gebroeke tekst as
sjleutel. Portefeuil-sjleutels kinne allein nómmers of tekst zeen.

Deze vormen zeen geliek veur tekstsjleutels:

```platlang
aafdrokke(gebroeker.naam)
aafdrokke(gebroeker["naam"])
```

Portefeuils kinne ouch berekende sjleutels höbbe:

```platlang
loat sjleutel = "naam"
loat gebroeker = {
    [sjleutel] = "Alice",
}
```

Toegang via ein portefeuil kin ouch toewieze:

```platlang
gebroeker.naam = "Mia"
nommers[1] = 99
```

Ein missende sjleutel geuf `niks`. Portefeuils vergelijke op identiteit: twee
versjillende portefeuils mit dezelfde inhoud zeen neet geliek.

## Lusse

`zolang` herhaolt zolang de veurwaerde truthy is.

```platlang
loat i = 0
zolang i < 3:
    aafdrokke(i)
    i = i + 1
enj
```

`veur` is ein half-open nómmerlus. `veur i = 0, 10:` luip van `0` tot en mit
`9`. De lusvariabele is allein besjikbaar binne de lus.

```platlang
veur i = 0, 10:
    aafdrokke(i)
enj
```

Mit `aafbraeke` sjtops te ein lus. Mit `euversjlaon` geis te nao de volgende
iteratie. Boete ein lus zeen die allebei ein fout.

## Minimale standaardbibliotheek

De standaardbibliotheek haet bewust allein twee funksies.

| Funksie | Gedrag |
| ------- | ------ |
| `aafdrokke(value)`, `aafdrökke(value)` | Drukt `value` aaf mit ein newline en geuf `niks` truuk. |
| `invuier()` | Lees eine regel invoer es `teks`; geuf `niks` bie end-of-input. |

```platlang
aafdrökke("Wie heits te?")
loat naam = invuier()
aafdrökke("hoi " + naam)
```

Deze name zeen besjermd en kinne neet gesjaduwd waere.

Es ein standaardnaam eine dialect-correcte spelling mit diakritische teikes
haet, blief de ASCII-spelling ondersteund es de makkelijke vorm, en de spelling
mit diakritische teikes weurt geaccepteerd es alias. Bie veurbeeld:
`aafdrokke` en `aafdrökke` rope dezelfde printfunksie aan.

## Meldinge

De interpreter meldt syntax- en runtimefoute mit bronlocaties es die besjikbaar
zeen.

```text
2:5: fout: funksiedeclaraties zien allein toegestaon op topniveau; verplaats 'funksie' oet dit blok
```

Engelse meldinge zeen de standaard. Limburgse meldinge kinne mit `--lang li`.

```sh
build/platlang --lang li examples/li/fibonacci.plat
```
