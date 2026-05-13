# Keyword Translation Research

This note records the current `plat-lang` keyword and runtime-name choices
against D'n Dictionair at `limburgs.net`.

Last checked: 2026-05-10.

## Source and Method

The site is an Angular application, but its frontend uses JSON endpoints that
can be queried directly:

```text
GET https://www.limburgs.net/api/search/{term}?languageDirection=NL-LI&deep=false
GET https://www.limburgs.net/api/search/{term}?languageDirection=LI-NL&deep=false
GET https://www.limburgs.net/api/lemma/{id}?languageDirection=NL-LI
```

These endpoints are useful for research, but they are not a documented public
API. Treat them as a manual review aid, not as a build, test, or release
dependency.

The dictionary data is copyrighted by the Limburgse Academie. Keep this file as
a compact project glossary with our decisions and short evidence summaries; do
not mirror or scrape the dictionary into the repository.

This pass checked every current lexer keyword plus every protected runtime type
or built-in name:

```text
loat funksie es angesj zolang veur enj trok aafbraeke euversjlaon
niks woar kwatsj en of
nómmer nommer teks tabel
aafdrokke aafdrökke inveure
```

## Reading the Evidence

Evidence grades are project-local:

| Grade | Meaning |
| ----- | ------- |
| Strong | The current spelling or its diacritic form appears with the intended meaning in relevant dialects. |
| Medium | The word is recognizable and usable, but the exact programming sense is indirect, noisy, or dialect-specific. |
| Weak | The dictionary does not directly support the spelling or intended sense; this is a deliberate language-design choice. |

The current vocabulary leans toward Roermond/Sittard-compatible spellings when
there is a clear choice. ASCII spellings are kept for ease of typing when the
dictionary-backed form uses diacritics.

## Current Language Surface

| Concept | Current spelling | Evidence | Recommendation |
| ------- | ---------------- | -------- | -------------- |
| variable declaration / let | `loat` | Weak. `LI-NL loat` returned no direct hit. Dutch `laten` gives forms such as `doen`, `doon`, and Venlo `laote`, but not `loat`. | Keep only as an established language keyword. Mark it as project-approved, not dictionary-confirmed. |
| function | `funksie` | Weak. `LI-NL funksie` returned no direct hit. Dutch `functie` returned `betrèkking` / `betrekking`, which means job/relation/function and is unsuitable as a programming keyword. | Keep as a technical borrowing. It is clearer for programmers than the dictionary result. |
| if | `es` | Strong. Dutch `als` returns `es` for Roermond, Sittard, and Valkenburg; Maastricht and Venlo return `as`. `LI-NL es` also confirms the conjunction sense in Sittard and Valkenburg, though the search has unrelated noun results too. | Keep `es`. Consider `as` only if keyword aliases are introduced. |
| else | `angesj` | Strong but Sittard-specific. Dutch `anders` returns `angesj` for Sittard, alongside `aanders`, `angers`, `andersj`, and `anders` in other dialects. `LI-NL angesj` confirms an "anders/different" sense. | Keep `angesj` for the current dialect center. |
| while | `zolang` | Strong. Dutch `zolang` returns `zolang` for Roermond and Sittard, with `zoelang` in Maastricht and `zoalang` in Valkenburg. `LI-NL zolang` confirms Roermond and Sittard. | Keep `zolang`. |
| for | `veur` | Medium. `LI-NL veur` gives "voor" senses across several dialects, though Dutch `voor` is noisy because it includes noun, time, place, and idiom senses. | Keep `veur`; it is short, recognizable, and already fits the language. |
| end | `enj` | Strong as an ASCII spelling. Dutch `eind` returns Sittard `ènj`; `LI-NL enj` normalizes to `ènj` and confirms "einde/eind". | Keep `enj`. Add `ènj` only if keyword aliases are introduced. |
| return | `trok` | Medium. `LI-NL trok` normalizes to `trök` in Maastricht, Roermond, and Sittard for "back/ago". Dutch `teruggeven` returns `trökgeve`, `trökgaeve`, and `truukgaeve`. | Keep `trok` as the compact ASCII programming form. `trök` is the natural future alias. |
| break | `aafbraeke` | Strong. Dutch `afbreken` returns `aafbraeke` for Roermond, Sittard, and Venlo; `LI-NL aafbraeke` confirms the same. | Keep `aafbraeke`. |
| continue | `euversjlaon` | Strong for the loop-control sense. Dutch `overslaan` gives `euversjlaon` for Roermond and Sittard, matching the programming behavior: skip the rest of this iteration. Dutch `doorgaan` gives `doorgaon`, but that better means continuing onward rather than skipping. | Use `euversjlaon`. It is long, but semantically sharp and dictionary-backed. |
| nil / null | `niks` | Medium. Dutch `niets` and `niks` mainly return `geinein` / `genein`, but `LI-NL niks` exists and related compounds carry "nothing/useless/trivial" senses. | Keep `niks`; it is compact and recognizable. |
| true | `woar` | Medium. `LI-NL woar` confirms the true/correct sense in Valkenburg. Dutch `waarheid` and `LI-NL waor` show broader support for `waor` in Roermond, Sittard, and Venlo. | Keep `woar` for compatibility. Consider `waor` as a future alias or replacement if we want stronger Roermond/Sittard backing. |
| false | `kwatsj` | Weak as a dictionary lookup. `LI-NL kwatsj` returned no direct hit. Dutch `nietwaar` returns an interjection sense such as Roermond `neewaor`, not boolean false. | Keep as transparent composition of `neet` + `woar`. If `waor` becomes an alias, consider `neetwaor` too. |
| logical AND | `en` | Strong. Dutch `en` returns `en` as a coordinating conjunction across Maastricht, Roermond, Sittard, Valkenburg, and Venlo. | Keep `en`. |
| logical OR | `of` | Medium. `LI-NL of` returns conjunction entries, but several meanings are "as if/although"; Dutch `of` search returns `es-of` / `as-of` and many unrelated `offer...` hits. The plain spelling is still the expected Dutch/Limburgish operator word. | Keep `of`. |
| number type | `nómmer` / `nommer` | Strong. Dutch `nummer` returns `nómmer` for Maastricht, Roermond, Sittard, and Valkenburg. `LI-NL nommer` and `LI-NL nómmer` normalize to the same entries. | Keep `nómmer` as the dialect-correct spelling and `nommer` as the ASCII alias. |
| text type | `teks` | Strong. Dutch `tekst` returns `teks` for Maastricht, Roermond, Sittard, and Valkenburg. `LI-NL teks` confirms text-related meanings across the same dialects. | Keep `teks`. |
| composite table type | `tabel` | Strong for the intended map/folder metaphor. `LI-NL tabel` confirms "map voor papieren" for Maastricht, Roermond, Sittard, and Valkenburg. Dutch `map` also returns `tabel` for the same dialects. | Use `tabel` as the composite runtime type name. |
| print built-in | `aafdrokke` / `aafdrökke` | Strong. Dutch `afdrukken` returns Maastricht `aofdrökke` and `aafdrökke` for Roermond, Sittard, Valkenburg, and Venlo. `LI-NL aafdrokke` normalizes to `aafdrökke`. | Keep both spellings. The diacritic form is dictionary-backed; the ASCII form is ergonomic. |
| input built-in | `inveure` | Medium. `LI-NL inveure` confirms a Sittard noun meaning "invoer/import". Dutch `invoeren` returns Sittard `inveuree`; Roermond and Venlo use `inveure`. For "invoeren in een computer", the dictionary gives `inleze` / `inlaeze`. | Keep `inveure` for now as a Sittard-centered input name. Consider `inlaeze` / `inleze` if the built-in is reframed as reading from input rather than input/import. |

## Future or Related Names

| Concept | Candidate | Evidence | Note |
| ------- | --------- | -------- | ---- |
| map / folder value | `map` | Medium. Dutch `map` returns `map` for Maastricht, Roermond, Valkenburg, and Venlo, but not Sittard in the sampled result. | Short and obvious, but less distinctive. |
| table value | `taofel` | Strong as "tabel/table", returned across all listed dialects. | Dictionary-backed, but less accurate than `tabel` for the map/folder metaphor. |
| continue/go on | `doorgaon` | Strong for `doorgaan` in Roermond, Sittard, and Venlo. | Useful if the language ever needs a plain "continue onward" word, but not as precise for loop `continue`. |
| true | `waor` | Stronger dialect coverage than `woar` for true/correct. | Candidate alias if boolean names get dialect aliases. |

## Suggested Follow-Up

The current spelling set is usable, but two names deserve an explicit design
decision before the language is called stable:

1. `woar` / `kwatsj`: decide whether to keep Valkenburg-centered `woar` or add
   Roermond/Sittard-backed `waor` and `neetwaor` aliases.
2. `loat` and `funksie`: keep them as programming-language borrowings, but avoid
   presenting them as dictionary-confirmed Limburgish words.
