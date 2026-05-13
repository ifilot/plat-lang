# Language Tour

This page describes the behavior supported by the current `plat-lang`
interpreter.

## Language Scope

The current language includes primitive values, variables, lexical scopes,
expressions, global functions, conditionals, loops, mutable `tabel` tables,
localized diagnostics, and a minimal standard library: `aafdrokke` /
`aafdrökke`, `inveure`, and `waatis`.

## Values

The runtime value types are:

| Value | Meaning |
| ----- | ------- |
| `nómmer` / `nommer` | IEEE 754 double-precision number |
| `teks` | string |
| `woar` | boolean true |
| `kwatsj` | boolean false |
| `niks` | absence of a value |
| `tabel` | mutable table |

`waatis(value)` prints these runtime type names for existing values.

There is one numeric type. The language does not distinguish integers from
floating-point numbers.

```platlang
loat x = 10
loat y = 3.5
loat name = "Mia"
loat active = woar
loat missing = niks
```

Truthiness:

* `niks`, `kwatsj`, `0`, empty strings, and empty tables are falsy
* all other values are truthy

## Variables

Variables are declared with `loat`. A declaration without an initializer starts
as `niks`.

```platlang
loat naam = "Jan"
loat later
naam = "Mia"
```

* Assigning to an undeclared variable is an error. Declaring the same name twice
  in the same scope is also an error.
* Names such as keywords, runtime type names, and standard library built-ins are
  protected. They cannot be redefined. `nómmer` is the dialect-correct runtime
  type name for numbers; `nommer` remains accepted as the ASCII spelling.
* To obtain the type of the variable, use `waatis()`.

## Expressions

The current language supports literal values, variable reads, grouping with
parentheses, unary operators, binary operators, function calls, and table
access.

```platlang
loat result = (2 + 3) * 4
loat ok = result >= 10 en result != 13
```

Supported operators:

| Operators | Meaning |
| --------- | ------- |
| `-`, `!` | numeric negation and logical negation |
| `*`, `/`, `%` | multiplication, division, remainder |
| `+`, `-` | addition/subtraction; `+` also concatenates when either side is text |
| `<`, `>`, `<=`, `>=` | numeric comparison |
| `==`, `!=` | equality and inequality |
| `en`, `of` | logical operators |

Division or remainder by zero is an error. Numeric operators require numbers,
except for `+` when one side is text.

## Functions

Functions are global, top-level declarations. They are declared with `funksie`
and return with `trok`.

```platlang
funksie dubbel(getal):
    trok getal * 2
enj

aafdrokke(dubbel(21))
```

* Before execution starts, all top-level functions are registered. This means a
  function may call another function declared later in the file, and mutual
  recursion works without forward declarations.
* Function declarations are only valid at top level. A nested `funksie` is a
  syntax error. Functions are not values: they cannot be stored in
  variables, passed as arguments, returned, or placed in tables.
* Calling a function with the wrong number of arguments is an error. Falling off
  the end of a function returns `niks`. `trok` without a value also returns
  `niks`. Using `trok` outside a function is an error.

## Conditionals

`es` is a statement. `angesj:` is optional.

```platlang
es getal > 10:
    aafdrokke("groot")
angesj:
    aafdrokke("klein")
enj
```

Every branch body introduces a lexical scope.

## Tables

`tabel` is the only composite value. Tables are mutable and can act as arrays,
maps, records, or namespaces.

```platlang
loat nums = { 10, 20, 30 }
loat user = { name = "Alice", age = 30 }

aafdrokke(nums[0])
aafdrokke(user.name)
```

Array-style entries use zero-based numeric keys. Record-style entries use text
keys. Table keys may only be numbers or text.

The following access forms are equivalent for text keys:

```platlang
aafdrokke(user.name)
aafdrokke(user["name"])
```

Tables also support computed keys:

```platlang
loat key = "name"
loat user = {
    [key] = "Alice",
}
```

Assigning through table access mutates the table:

```platlang
user.name = "Mia"
nums[1] = 99
```

Reading a missing table key returns `niks`. Table equality is identity-based:
two different tables with the same contents are not equal.

## Loops

`zolang` repeats while its condition is truthy.

```platlang
loat i = 0
zolang i < 3:
    aafdrokke(i)
    i = i + 1
enj
```

`veur` is a half-open numeric loop. `veur i = 0, 10:` runs from `0` through
`9`. The loop variable is local to the loop body and is not available after the
loop ends.

```platlang
veur i = 0, 10:
    aafdrokke(i)
enj
```

Loops support `aafbraeke` to exit and `euversjlaon` to continue to the next
iteration. Using either outside a loop is an error.

## Standard library

| Function | Behavior |
| -------- | -------- |
| `aafdrokke(value)`, `aafdrökke(value)` | Prints `value` followed by a newline and returns `niks`. |
| `inveure()` | Reads one line from input and returns it as `teks`; returns `niks` at end of input. |
| `waatis(value)` | Prints the runtime type name of `value` and returns `niks`. |

```platlang
aafdrökke("What is your name?")
loat naam = inveure()
aafdrökke("hoi " + naam)
```

These names are protected and cannot be shadowed.

!!! note "Diacritics and ASCII spellings"

    When a standard name has a dialect-correct spelling with diacritics, the ASCII
    spelling stays supported as the ergonomic form, and the diacritic spelling is
    accepted as an alias. For example, both `aafdrokke` and `aafdrökke` call the
    same print function.

## Diagnostics

The interpreter reports syntax and runtime errors with source locations when
available.

```text
2:5: error: function declarations are only allowed at top level; move 'funksie' out of this block
```

English diagnostics are the default. Limburgish diagnostics can be selected
with `--lang li`.

```sh
build/platlang --lang li examples/li/fibonacci.plat
```
