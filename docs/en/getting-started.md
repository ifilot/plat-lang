# Getting Started

This page gives you a quick, progressive tour of `plat-lang`. Each step adds one
idea, so you can start with a tiny script and gradually build up to the core
features used by real programs. If you have not built the interpreter yet, start
with [Installation](installation.md). Once `build/platlang` exists, you can run
any script like this:

```sh
build/platlang examples/en/hello.plat
```

!!! note "Diacritics and ASCII spellings"

    Canonical Limburgish uses many diacritics, which are rare in programming
    languages and can be tedious to type on some keyboards. For that reason,
    every keyword or built-in name that carries a diacritic also has a
    non-diacritic spelling in `plat-lang`. For example, both `trök` and `trok`
    are accepted for returning from a function.

## 1. Say hello

A `plat-lang` program can be as small as a call to the built-in print function:

```platlang
aafdrokke("Hallo, plat-lang")
```

`aafdrokke(value)` prints a value followed by a newline. The dialect spelling
`aafdrökke(value)` works too.

## 2. Store values with `loat`

Use `loat` to declare variables. Strings are `teks`, numbers are `nómmer`, and
`+` can concatenate text.

```platlang
loat naam = "Jan"
loat taal = "plat-lang"
loat versie = 1

aafdrokke("Hallo, " + naam)
aafdrokke("Welkom bie " + taal + " " + versie)
```

A declaration without a value starts as `niks`, the language's absence value.

```platlang
loat later
aafdrokke(later)
```

## 3. Make choices with `es`

Conditionals use `es`, optional `angesj:`, and end with `enj`.

```platlang
loat naam = ""
loat fallback = "onbekend"

es naam:
    aafdrokke(naam)
angesj:
    aafdrokke(fallback)
enj
```

Truthiness keeps early programs compact: `niks`, `neetwoar`, `0`, empty text,
and empty tables are falsy; other values are truthy.

## 4. Repeat work with loops

Use `zolang` when you want to loop while a condition stays truthy:

```platlang
loat i = 0

zolang i < 3:
    aafdrokke(i)
    i = i + 1
enj
```

Use `veur` for a half-open numeric range. This prints `0`, `1`, and `2`:

```platlang
veur i = 0, 3:
    aafdrokke(i)
enj
```

Inside loops, `euversjlaon` continues to the next iteration and `aafbraeke`
exits the loop.

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

## 5. Name reusable work with functions

Functions are declared with `funksie` and return with `trok`.

```platlang
funksie dubbel(getal):
    trok getal * 2
enj

aafdrokke(dubbel(21))
```

Top-level functions are registered before execution starts, so a function can
call another function declared later in the same file. Recursion works too:

```platlang
funksie fib(n):
    es n < 2:
        trok n
    angesj:
        trok fib(n - 1) + fib(n - 2)
    enj
enj

aafdrokke(fib(10))
```

## 6. Group data with `portefeuil` tables

`portefeuil` tables are mutable containers. You can use them like records:

```platlang
loat user = {
    name = "Alice",
    age = 30,
}

aafdrokke(user.name)
aafdrokke(user["age"])
```

You can also use them like arrays. Numeric keys start at `0`.

```platlang
loat nums = { 10, 20, 30 }

aafdrokke(nums[0])
nums[1] = 99
aafdrokke(nums[1])
```

Computed keys let a program choose what to read or write:

```platlang
loat field = "name"
aafdrokke(user[field])
```

## 7. Read input

The minimal standard library also includes `invuier()`, which reads one line of
input as `teks`.

```platlang
aafdrokke("What is your name?")
loat naam = invuier()
aafdrokke("hoi " + naam)
```

Run that kind of script with piped input:

```sh
printf 'Mia\n' | build/platlang examples/en/input.plat
```

## Where to go next

Continue with the [Language Tour](language-tour.md) for the full current
language boundary, including supported operators, protected names, diagnostics,
and the features that are intentionally not part of the language yet.
