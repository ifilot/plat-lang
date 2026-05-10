<section class="plat-intro">
  <img src="../assets/images/plat-lang-logo.svg" alt="plat-lang logo">
  <h1>plat-lang</h1>
  <p>
    Ein experimentele oetveurdertaol wo de code klein blief, de regels
    geregeld blieve, en de sleutelwäörd oet Limburg komme. De documentatie
    volg de letste oetveurder: eine kleine kern mit ein minimale
    standaardbibliotheek.
  </p>
  <div class="plat-actions">
    <a class="plat-button" href="getting-started.html">
      <span class="plat-button-icon" aria-hidden="true">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="m13.13 22.19-1.63-3.83c1.57-.58 3.04-1.36 4.4-2.27zM5.64 12.5l-3.83-1.63 6.1-2.77C7 9.46 6.22 10.93 5.64 12.5M21.61 2.39S16.66.269 11 5.93c-2.19 2.19-3.5 4.6-4.35 6.71-.28.75-.09 1.57.46 2.13l2.13 2.12c.55.56 1.37.74 2.12.46A19.1 19.1 0 0 0 18.07 13c5.66-5.66 3.54-10.61 3.54-10.61m-7.07 7.07c-.78-.78-.78-2.05 0-2.83s2.05-.78 2.83 0c.77.78.78 2.05 0 2.83s-2.05.78-2.83 0m-5.66 7.07-1.41-1.41zM6.24 22l3.64-3.64c-.34-.09-.67-.24-.97-.45L4.83 22zM2 22h1.41l4.77-4.76-1.42-1.41L2 20.59zm0-2.83 4.09-4.08c-.21-.3-.36-.62-.45-.97L2 17.76z"/></svg>
      </span>
      <span>Oan de geng</span>
    </a>
    <a class="plat-button secondary" href="https://github.com/ifilot/plat-lang">
      <span class="plat-button-icon" aria-hidden="true">
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512"><path d="M173.9 397.4c0 2-2.3 3.6-5.2 3.6-3.3.3-5.6-1.3-5.6-3.6 0-2 2.3-3.6 5.2-3.6 3-.3 5.6 1.3 5.6 3.6m-31.1-4.5c-.7 2 1.3 4.3 4.3 4.9 2.6 1 5.6 0 6.2-2s-1.3-4.3-4.3-5.2c-2.6-.7-5.5.3-6.2 2.3m44.2-1.7c-2.9.7-4.9 2.6-4.6 4.9.3 2 2.9 3.3 5.9 2.6 2.9-.7 4.9-2.6 4.6-4.6-.3-1.9-3-3.2-5.9-2.9M252.8 8C114.1 8 8 113.3 8 252c0 110.9 69.8 205.8 169.5 239.2 12.8 2.3 17.3-5.6 17.3-12.1 0-6.2-.3-40.4-.3-61.4 0 0-70 15-84.7-29.8 0 0-11.4-29.1-27.8-36.6 0 0-22.9-15.7 1.6-15.4 0 0 24.9 2 38.6 25.8 21.9 38.6 58.6 27.5 72.9 20.9 2.3-16 8.8-27.1 16-33.7-55.9-6.2-112.3-14.3-112.3-110.5 0-27.5 7.6-41.3 23.6-58.9-2.6-6.5-11.1-33.3 2.6-67.9 20.9-6.5 69 27 69 27 20-5.6 41.5-8.5 62.8-8.5s42.8 2.9 62.8 8.5c0 0 48.1-33.6 69-27 13.7 34.7 5.2 61.4 2.6 67.9 16 17.7 25.8 31.5 25.8 58.9 0 96.5-58.9 104.2-114.8 110.5 9.2 7.9 17 22.9 17 46.4 0 33.7-.3 75.4-.3 83.6 0 6.5 4.6 14.4 17.3 12.1C436.2 457.8 504 362.9 504 252 504 113.3 391.5 8 252.8 8M105.2 352.9c-1.3 1-1 3.3.7 5.2 1.6 1.6 3.9 2.3 5.2 1 1.3-1 1-3.3-.7-5.2-1.6-1.6-3.9-2.3-5.2-1m-10.8-8.1c-.7 1.3.3 2.9 2.3 3.9 1.6 1 3.6.7 4.3-.7.7-1.3-.3-2.9-2.3-3.9-2-.6-3.6-.3-4.3.7m32.4 35.6c-1.6 1.3-1 4.3 1.3 6.2 2.3 2.3 5.2 2.6 6.5 1 1.3-1.3.7-4.3-1.3-6.2-2.2-2.3-5.2-2.6-6.5-1m-11.4-14.7c-1.6 1-1.6 3.6 0 5.9s4.3 3.3 5.6 2.3c1.6-1.3 1.6-3.9 0-6.2-1.4-2.3-4-3.3-5.6-2"/></svg>
      </span>
      <span>GitHub</span>
    </a>
  </div>
</section>

## Ein Klein Schriefsel

```platlang
funksie viefkes(getal):
    es getal < 2:
        trok getal
    angesj:
        trok viefkes(getal - 1) + viefkes(getal - 2)
    enj
enj

aafdrokke(viefkes(10))
```

<div class="plat-grid">
  <div class="plat-card">
    <h3>Limburgse sjtem</h3>
    <p>Sleutelwäörd wie <code>funksie</code>, <code>loat</code>, en <code>trok</code> geve de taol eine eige klank.</p>
  </div>
  <div class="plat-card">
    <h3>Kleine runtime</h3>
    <p>Nommers, tekste, booleans, <code>niks</code>, en veranderbare <code>portefeuil</code>-tabellen make de kern.</p>
  </div>
  <div class="plat-card">
    <h3>Geinterpreteerd</h3>
    <p>Veur <code>.plat</code>-schriefsels rechstreeks oet mit <code>platlang</code>; de oetveurder leest, controleert, en voert 't programma veur dich oet.</p>
  </div>
</div>

## In 't Kort

De huidige taol bevat allein de kern: primitieve waerdes, variabelen,
expressies, globale funksies, veurwaerde, lusse, veranderbare `portefeuil`-tabellen,
en gelokaliseerde meldinge.

De standaardbibliotheek begint bewust klein mit allein `aafdrokke(value)` /
`aafdrökke(value)` veur oetvoer en `invuier()` veur invoer. Modules, imports, klasses, methodes,
exceptions, closures, first-class funksies, statische types, en de groetere
standaardbibliotheek zeen bewust oetgesteld.
