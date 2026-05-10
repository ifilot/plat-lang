# plat-lang Paper Scaffold

This directory contains an arXiv-oriented LaTeX scaffold for a paper tentatively
titled:

> Designing a Programming Language Around Regional-Language Identity: The Case
> of `plat-lang`

The current draft is a scaffold, not a finished publication. It is intended to
give the project a credible paper structure, a clear thesis, and marked places
where evidence, citations, diagrams, and project-specific detail still need to
be added.

## Build

From this directory:

```sh
make
```

The Makefile uses `latexmk` when available. If `latexmk` is not installed, it
falls back to:

```sh
pdflatex main.tex
bibtex main
pdflatex main.tex
pdflatex main.tex
```

To remove generated files:

```sh
make clean
```

If `latexmk` is installed, continuous rebuilds are available with:

```sh
make watch
```

## What to Edit

- `main.tex`: document setup, title, packages, listing style, and section order.
- `sections/*.tex`: the paper body.
- `references.bib`: bibliography entries. Current entries are placeholders.
- `figures/`: architecture diagrams, screenshots, or other figures.
- `tables/`: optional table source files if tables become large enough to split.

## Evidence Still Needed

Before submitting to arXiv, the draft needs:

- a stable `plat-lang` release tag and commit hash;
- an archived release with a DOI;
- accurate citations replacing all TODO bibliography entries;
- clearer evidence for claims about accessibility and cultural impact;
- feedback from Limburgian speakers, learners, teachers, or community groups;
- a language reference or appendix tied to the release version;
- an implementation diagram and testing summary;
- a positionality statement from the authors.

## Recommended Next Steps

1. Add a stable `plat-lang` release.
2. Archive the release with a DOI.
3. Add more examples.
4. Add a language reference.
5. Collect feedback from Limburgian speakers or learners.
6. Replace TODO citations with real literature.
