<section class="plat-intro">
  <img src="assets/images/plat-lang-logo.svg" alt="plat-lang logo">
  <h1>plat-lang</h1>
  <p>
    An experimental interpreter language where code stays small, rules stay
    regular, and the keywords come from Limburg.
  </p>
  <div class="plat-actions">
    <a class="plat-button" href="getting-started.html">Get started</a>
    <a class="plat-button secondary" href="https://github.com/ifilot/plat-lang">GitHub</a>
  </div>
</section>

## A Small Script

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

<div class="plat-grid">
  <div class="plat-card">
    <h3>Limburgian Voice</h3>
    <p>Keywords such as <code>funksie</code>, <code>loat</code>, and <code>trok</code> give the language its own character.</p>
  </div>
  <div class="plat-card">
    <h3>Small Runtime</h3>
    <p>Numbers, strings, booleans, <code>niks</code>, and mutable <code>mepke</code> tables form the core.</p>
  </div>
  <div class="plat-card">
    <h3>Interpreted</h3>
    <p>Run <code>.plat</code> scripts directly with <code>platlang</code>; the interpreter parses, checks, and executes the program for you.</p>
  </div>
</div>
