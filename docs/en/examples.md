# Examples

The examples only use the current core language plus `aafdrokke` /
`aafdrökke` and `invuier`.

Example scripts are split by comment language:

```text
examples/en
examples/li
```

Run an English-commented example:

```sh
build/platlang examples/en/hello.plat
```

Run the input example:

```sh
printf 'Mia\n' | build/platlang examples/en/input.plat
```

Run a Limburgish-commented example:

```sh
build/platlang examples/li/hello.plat
```

Tests use their own fixtures under `tests/`; examples are for readers.

Good examples should avoid features that are not part of the current language
boundary, including imports, modules, classes, methods, closures, and
first-class functions.
