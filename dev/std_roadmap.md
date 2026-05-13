# plat-lang Standard Library Roadmap

This file is a roadmap for work after the current language core. It is not the
current standard library contract.

The current standard library contains only:

| Function | Purpose |
| -------- | ------- |
| `aafdrokke(value)`, `aafdrökke(value)` | Prints a value followed by a newline. |
| `inveure()` | Reads one input line as text, or returns `niks` at end of input. |

Everything else in this document is proposed future behavior.

`plat-lang` will eventually need a standard library, but it should grow in the
same spirit as the language itself: small, regular, table-centered, and easy to
remember.

The standard library should provide practical tools for scripts without turning
the language into a framework. Its first job is to make the existing primitive
values pleasant to use:

* `nómmer`
* `teks`
* `woar`
* `niks`
* `tabel`

Because functions are global declarations and are not runtime values, early
standard library features should be plain global functions. The library should
avoid object hierarchies, method tables, callback-heavy APIs, and abstractions
that require first-class functions.

## Design Principles

The standard library should follow these rules:

* Prefer a small set of composable functions over many specialized helpers.
* Treat `tabel` as the universal composite value.
* Keep ASCII spellings available, while allowing dialect-correct aliases with
  diacritics where they improve the Limburgish spelling.
* Prefer predictable behavior over clever shortcuts.
* Make common scripting tasks easy.
* Avoid features that imply missing language concepts.
* Add low-level runtime built-ins only when they cannot reasonably be written in
  `plat-lang` itself.

A useful test for proposed additions is:

> Can this be explained as a global function over simple values and tables?

If not, it probably does not belong in the first standard library.

## Stage 1: Value Helpers

These functions are the next useful additions after the current minimal library.
They are likely to be native built-ins because they expose runtime behavior or
host I/O.

| Function | Purpose |
| -------- | ------- |
| `teks(value)` | Converts any value to text. |
| `soort(value)` | Returns the runtime type name of a value. |
| `lengde(value)` | Returns the length of a string or table. |
| `leeg(value)` | Returns whether a string or table is empty. |
| `kopie(value)` | Returns a shallow copy of a string or table. |

These functions make reflection, conversion, and basic value handling explicit
without adding new language syntax.

`soort` should return the localized runtime type names:

* `nómmer`
* `teks`
* `woar`
* `niks`
* `tabel`

`lengde` and `leeg` should initially apply only to `teks` and `tabel`. Passing
other values should be a runtime error unless a later design deliberately
extends their behavior.

`kopie` should be shallow. For tables, it should copy the table storage but keep
nested values unchanged.

## Stage 2: Tables

Tables are the only composite type in `plat-lang`, so table utilities are the
most important part of the early library.

| Function | Purpose |
| -------- | ------- |
| `duw(t, value)` | Appends a value to an array-like table. |
| `pak(t)` | Removes and returns the last array-like value. |
| `haol(t, key, fallback)` | Gets a value, returning `fallback` when missing. |
| `haet(t, key)` | Returns whether a key exists. |
| `sjrap(t, key)` | Removes a key from a table. |
| `sleutels(t)` | Returns a table containing the table keys. |
| `waerdes(t)` | Returns a table containing the table values. |
| `omdriej(t)` | Reverses an array-like table in place. |
| `sorteer(t)` | Sorts an array-like table in place. |

The first table functions should support both record-like and array-like use.
Array-like operations should follow the existing zero-based indexing rule.

`sorteer` should start conservatively. It can support tables containing only
numbers or only strings. Mixed-type sorting can remain undefined or produce a
runtime error until a clear ordering rule is designed.

Higher-order functions such as `map`, `filter`, and `reduce`
should wait. They do not fit the current language model because functions are
not first-class values.

## Stage 3: Strings

String helpers should cover ordinary text processing without introducing a
large pattern language.

| Function | Purpose |
| -------- | ------- |
| `bevat(text, part)` | Returns whether `text` contains `part`. |
| `begint_met(text, part)` | Returns whether `text` starts with `part`. |
| `endigt_met(text, part)` | Returns whether `text` ends with `part`. |
| `splits(text, sep)` | Splits text into a table of strings. |
| `plak(parts, sep)` | Joins a table of strings. |
| `klein(text)` | Converts text to lowercase. |
| `groet(text)` | Converts text to uppercase. |
| `sjnie(text, begin, end)` | Returns a substring. |
| `trim(text)` | Removes leading and trailing whitespace. |
| `vervang(text, old, new)` | Replaces occurrences of `old` with `new`. |

Regular expressions should not be part of the first string library. Plain
substring operations are easier to specify and easier to keep portable.

## Stage 4: Numbers and Math

The numeric library should reflect the single-number model. All numbers are
IEEE 754 double-precision values, so math functions should operate on `nómmer`
without introducing integer-specific behavior.

| Function | Purpose |
| -------- | ------- |
| `abs(x)` | Absolute value. |
| `min(a, b)` | Smaller of two numbers. |
| `max(a, b)` | Larger of two numbers. |
| `vloer(x)` | Floor. |
| `plafond(x)` | Ceiling. |
| `afronden(x)` | Round to nearest number. |
| `wurtel(x)` | Square root. |
| `macht(x, y)` | Power. |
| `sin(x)` | Sine. |
| `cos(x)` | Cosine. |
| `tan(x)` | Tangent. |
| `toeval()` | Random number from `0` up to `1`. |
| `toeval_tusje(a, b)` | Random number in the range `[a, b)`. |

The first random functions should be simple. Explicit seeds and random generator
objects can wait until there is a concrete need for reproducible random
sequences.

## Stage 5: Files and Process

Whole-file I/O is the best first fit for `plat-lang`. It is useful for scripts
and avoids designing file handles, stream lifetimes, modes, buffering, and
cleanup rules too early.

| Function | Purpose |
| -------- | ------- |
| `lees_pad(path)` | Reads a whole file as text. |
| `sjrief_pad(path, text)` | Writes text to a file. |
| `bestuit(path)` | Returns whether a path exists. |
| `regels(path)` | Reads a file as a table of lines. |
| `argumente()` | Returns command-line arguments as a table. |
| `oetgang(code)` | Exits the process with a numeric status code. |

These APIs should use strings for paths and text. Binary I/O should wait until
the language has a clearer story for byte data.

## Stage 6: Debugging and Runtime Checks

Small debugging helpers make the language easier to develop and teach.

| Function | Purpose |
| -------- | ------- |
| `toon(value)` | Returns or prints a debug representation of a value. |
| `fout(text)` | Stops execution with a runtime error. |
| `bewear(condition, text)` | Stops execution when a condition is falsy. |

`toon` should expose table contents more usefully than `Value::to_string`,
which currently prints table identity. It can either print directly or return a
debug string. Returning a string is more composable.

`fout` and `bewear` should integrate with the diagnostic system rather than
printing ad hoc messages.

## Deliberately Deferred

The first standard library should not include:

* classes or method-like APIs
* async or concurrency
* networking
* dates and time beyond a possible simple timestamp helper
* regular expressions
* JSON
* binary I/O
* package management
* a large module system
* callback-based collection functions

These features may become useful later, but each one implies additional design
surface. They should wait until the core language and first standard library are
stable enough to show what shape they actually need.

## Implementation Notes

The implementation should distinguish between:

* native built-ins, implemented in C++ and protected like `aafdrokke`
* library functions, implemented in `plat-lang` once loading standard source
  files is supported

Native built-ins are appropriate when a function needs host access, runtime
introspection, diagnostics, or efficient primitive operations. Everything else
should prefer ordinary `plat-lang` code once the language can load standard
library files.

The initial implementation can register all native standard functions in the
interpreter. If a module or import system is added later, this roadmap should be
revisited before splitting the library into namespaces.
