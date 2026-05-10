# Veurbeelde

De veurbeelde gebroeke allein de huidige kern van de taol mit
`aafdrokke` / `aafdrökke` en `invuier`.

De veurbeelde zeen verdeeld nao de taol van de commentaren:

```text
examples/en
examples/li
```

Dreij ein veurbeeld mit Engelse commentaren:

```sh
build/platlang examples/en/hello.plat
```

Dreij 't invoerveurbeeld:

```sh
printf 'Mia\n' | build/platlang examples/li/input.plat
```

Dreij ein veurbeeld mit Limburgse commentaren:

```sh
build/platlang examples/li/hello.plat
```

Tests höbbe eur eige invoer ónger `tests/`; veurbeelde zeen veur lezers.

Gooj veurbeelde vermieje features die nog neet bie de huidige taol hure, wie
imports, modules, klasses, methodes, closures, en first-class funksies.
