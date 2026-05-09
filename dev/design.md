# plat-lang Design Overview

`plat-lang` is a small, dynamically typed programming language designed around
simplicity, readability, and composition.

Its most distinctive feature is linguistic: core keywords use Limburgian words
instead of English ones. Where possible, the language prefers the Sittard
dialect as its foundation. This gives `plat-lang` a recognizable cultural
identity while keeping the underlying language model calm, regular, and easy to
learn.

The design is inspired by Lua, Lisp, and Smalltalk, but aims to remain smaller
and simpler than all three.

One guiding motto is:

> What would Dijkstra do?

The Limburgian phrasing of this motto is still open.

## Philosophy

`plat-lang` values clarity over maximal performance and learnability over
maximal static safety. The language should feel:

* small but expressive
* regular and unsurprising
* easy to read
* pleasant to write
* powerful through composition rather than feature volume

The language is built around a few guiding principles:

* few primitive concepts
* strong composition
* uniform semantics
* low cognitive load
* expressive power through simple rules

## Core Concepts

The language is built from a small set of concepts:

* values
* variables
* expressions
* functions
* tables
* conditionals
* loops

More advanced behavior should emerge from combining these concepts rather than
from adding many specialized language features.

## Primitive Types

`plat-lang` has a small set of primitive runtime types:

| English  | plat-lang runtime name |
| -------- | ---------------------- |
| number   | `nommer`               |
| string   | `teks`                 |
| boolean  | `woar`                 |
| nil      | `niks`                 |
| table    | `mepke`                |

These localized names are intended for runtime reflection and type inspection.
Functions are global declarations rather than ordinary runtime values, so there
is no user-visible `function` primitive type.

The language deliberately does not define separate primitive concepts for:

* integers
* floats
* classes
* structs
* arrays
* dictionaries
* objects

Those concepts are either represented by existing primitive values or emerge
from tables and global functions.

## Numbers

`plat-lang` has one numeric type: `number`.

Internally, numbers are represented as IEEE 754 double-precision floating point
values.

```platlang
loat x = 10
loat y = 3.14
loat z = x + y
```

This keeps the numeric model small and avoids:

* separate integer and floating-point types
* implicit numeric coercion systems
* numeric type hierarchies
* overflow rules for multiple integer widths

Division always produces a floating-point result.

```platlang
5 / 2  # 2.5
```

## Tables

`plat-lang` has one universal composite type: `mepke`.

Tables can act as:

* arrays
* maps
* records
* objects
* namespaces

```platlang
loat nums = {1, 2, 3}
loat person = { name = "Alice", age = 30 }
loat with_trailing_comma = { "a", "b", "c", }
```

Internally, these are the same structure. Tables are mutable.
Array-style table access is zero-based.
Table literals may use trailing commas.
Table keys may only be strings or numbers.

Field access supports both dot and index syntax:

```platlang
person.name
person["name"]
```

These forms are equivalent when the field name is a string-compatible key.
Table literals also support expression keys, as long as the resulting key is a
string or number.

```platlang
loat key = "name"
loat user = {
  [key] = "Alice",
}
```

## Variables

Variables are declared with `loat`.

```platlang
loat x = 10
loat naam = "Jan"
loat later
```

Declarations without an initializer default to `niks`.

Variables can later be reassigned:

```platlang
x = x + 1
```

Variables are dynamically typed:

```platlang
loat value = 10
value = "hello"
```

Variables use lexical scoping. Inner scopes can access bindings from outer
scopes.

Reserved keywords, literal names, and protected built-in names cannot be used as
variable names. Assigning to an undeclared variable is an error. Global
variables are allowed.

```platlang
loat version = "0.1"

funksie show_version():
  aafdrokke(version)
enj
```

Names share one namespace. Function names, variable names, and other user
bindings cannot collide in the same scope. Redeclaring a variable in the same
scope is an error.

Assignment searches existing scopes. `loat` creates a new variable in the
current scope. A local variable may shadow a global variable or global function,
but shadowing a function name means that name no longer resolves as callable in
that local scope.

Built-in functions are protected everywhere and cannot be shadowed in any scope.
Keywords and literal type names are protected as well.

## Expressions

`plat-lang` is expression-oriented. Most constructs return values and can be
composed naturally. Assignment is not an expression.

```platlang
loat x = 1 + 2 * 3
loat max = es a > b: a angesj: b enj
```

## Functions

Functions are declared with `funksie`. All functions are global. Functions
cannot be assigned to variables, passed as arguments, returned from other
functions, or stored in tables.

Function declarations are only valid at global scope. They cannot appear inside
any block. A function may be defined once and then used forever; redefining a
function is an error.

```platlang
funksie square(x):
  trok x * x
enj

aafdrokke(square(5))
```

Function parameters are mutable local variables.

```platlang
funksie increment(x):
  x = x + 1
  trok x
enj
```

Functions return values explicitly with `trok`. Falling off the end of a
function returns `niks`. `trok` may also be used without a value, in which case
it returns `niks`.
Using `trok` outside a function is an error.

```platlang
funksie stop():
  trok
enj
```

Calling a function with the wrong number of arguments is an error. Variadic
functions are not part of the initial language design. Function declarations are
handled by the interpreter, not by the parser. The parser recognizes function
declaration syntax but does not register functions.

Top-level function calls can only call functions that have already been declared
earlier during execution. Inside function bodies, calls may refer to functions
declared later in the file, because the body is evaluated only after top-level
declaration execution has continued. Calling an unknown function is a runtime
error.

```platlang
funksie add(a, b):
  trok a + b
enj

aafdrokke(add(1, 2))
```

This allows recursion and mutually referential global functions without adding
separate forward declaration syntax.

```platlang
funksie even(n):
  es n == 0:
    trok woar
  angesj:
    trok odd(n - 1)
  enj
enj

funksie odd(n):
  es n == 0:
    trok neetwoar
  angesj:
    trok even(n - 1)
  enj
enj
```

## Control Flow

The language supports minimal but sufficient control flow.

Conditionals use `es`, `:`, optional `angesj:`, and `enj`. The colon marks the
boundary between the condition and each branch body.

```platlang
es x > 10:
  aafdrokke("big")
angesj:
  aafdrokke("small")
enj
```

```platlang
es klaar:
  aafdrokke("done")
enj
```

When used as an expression, an `es` without `angesj:` evaluates to `niks` if
the condition is falsy.

Loops use `zolang` and `:`.

```platlang
zolang x < 10:
  es x == 5:
    aafbraeke
  enj

  x = x + 1
enj
```

A numeric loop form may also be supported:

```platlang
veur i = 0, 10:
  es i == 3:
    weier
  enj

  aafdrokke(i)
enj
```

Numeric loop variables are numbers and are local to the loop body. The loop
variable does not exist after the loop ends. Numeric loops are half-open:
`veur i = 0, 10:` runs with `i` values from `0` through `9`.
Loops support `aafbraeke` to exit the loop and `weier` to continue to the next
iteration. Using either word outside a loop is an error.

## Objects Through Tables

`plat-lang` has no classes. Object-like data can be represented with tables,
but functions remain global rather than being stored directly on table values.

```platlang
loat player = { name = "mage", hp = 100 }

funksie attack_player(target):
  aafdrokke("attack")
enj
```

This favors plain data and global behavior over inheritance while keeping the
object model small.

## Limburgian Keywords

All core language keywords should use Limburgian words. The exact spelling and
dialect choices may evolve, but the language intentionally embraces a
Limburgian identity.

| English | plat-lang |
| ------- | --------- |
| let     | `loat`    |
| fn      | `funksie` |
| if      | `es`      |
| else    | `angesj`  |
| while   | `zolang`  |
| for     | `veur`    |
| end     | `enj`     |
| return  | `trok`    |
| break   | `aafbraeke` |
| continue | `weier`  |
| nil     | `niks`    |
| true    | `woar`    |
| false   | `neetwoar` |

Keywords are reserved and cannot be used as variable names.

The function definition keyword and runtime type name are both `funksie`.
`aafdrokke` is the standard print built-in, not a keyword.

## Limburgian Spelling and ASCII Identifiers

Standard Limburgish spelling often uses accents and diacritics. These are
important for correct written Limburgish, but they are awkward to type on many
keyboard layouts. German historically faced a similar practical issue with
umlauts in computing environments.

`plat-lang` source files should support extended character input, including
accented letters and umlauts, especially inside strings and comments. However,
official language keywords and standard built-in function names must stay within
plain ASCII. This keeps programs easy to type on common keyboards while still
allowing users to write Limburgian text where the source format naturally
permits it.

Example:

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

Reference source for Limburgian vocabulary and spelling:

[https://www.limburgs.net](https://www.limburgs.net)

## Syntax Style

The syntax is lightweight and readable.

Characteristics:

* no semicolons
* minimal punctuation
* simple keywords
* visually clean structure
* comments start with `#` and continue to the end of the line
* otherwise, whitespace has no semantic meaning beyond separating tokens
* equality uses `==`; inequality uses `!=`
* table equality is identity-based

```platlang
loat x = 10
loat y = x * 2

aafdrokke(y)
```

Because whitespace is otherwise insignificant, the following program is
equivalent:

```platlang
loat x = 10 loat y = x * 2 aafdrokke(y)
```

Strings support at least these escape sequences:

| Escape | Meaning |
| ------ | ------- |
| `\n`   | newline |
| `\t`   | tab |
| `\"`   | double quote |
| `\\`   | backslash |

Strings are mutable.

## Runtime Model

The runtime model is intentionally small.

Core runtime structures:

* AST
* value
* environment
* function declaration
* table

The parser builds an AST. The interpreter executes that AST. The parser
recognizes syntax, but it does not register functions or execute semantic name
resolution.

The parser processes the whole source file before interpretation begins. Syntax
errors stop execution before any code runs.

AST nodes carry source locations, at minimum line and column, so diagnostics can
point to the relevant source position.

Semantic errors are reported during interpretation when encountered. This
includes undeclared variables, redeclarations, assignment to protected names,
invalid table keys, arity mismatches, and unknown function calls.

Runtime errors stop the entire program. The initial language does not include
exceptions or recovery semantics.

User-facing diagnostics support two languages from the start. Diagnostics should
be identified by stable error codes and rendered through localized message
templates rather than hard-coded directly at call sites. English and Limburgian
diagnostics are the initial target.

Diagnostic language is selected with a command-line flag:

```text
plat-lang --lang en program.plat
plat-lang --lang li program.plat
```

The default diagnostic language is English. The language code `li` follows the
standard Limburgish language subtag. Internally, `li` maps to the Sittard-based
Limburgian diagnostic set. If a more specific tag is ever needed, `li-x-sittard`
may be used as a private-use BCP 47 tag. Source syntax is unaffected by the
diagnostic language; all keywords remain Limburgian.

`plat-lang` source files use the `.plat` extension.

Most language behavior should be expressible in terms of values flowing through
expressions inside lexical environments, with tables and global function
declarations providing the main tools for abstraction.

Programs execute top-level statements directly, making `plat-lang` suitable for
scripts. A special `main` function is not required. Top-level statements may
appear before or after function declarations. Function declarations register
global functions when interpreted.

Every `:` introduces a block scope. This includes `es`, `angesj:`, `zolang`,
`veur`, and `funksie` bodies. Function declarations are only valid at top level,
never inside a block.

Truthiness follows familiar dynamic-language conventions. `niks` is falsy.
Other values use sensible falsy rules similar to Python and C: false booleans,
zero numbers, and empty strings/tables are falsy; non-empty values are truthy.

## Deliberately Omitted Features

The language intentionally avoids:

* classes
* inheritance
* static typing
* generics
* macros
* exceptions
* operator overloading
* first-class functions
* closures
* nested functions
* multiple composite data structures
* complicated module systems

These features are omitted to preserve conceptual clarity, uniformity,
simplicity, and locality of reasoning.

## Example Programs

### Fibonacci

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

### Tables

```platlang
loat user = { name = "Alice", age = 30 }
aafdrokke(user.name)
```

## Overall Mental Model

The whole language can be summarized as:

> Values flow through expressions inside environments, using tables and global
> function declarations as the primary tools for composition.

This creates a useful and expressive language from a deliberately small
foundation.
