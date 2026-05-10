MAIN := main
PDF := $(MAIN).pdf

.PHONY: all clean watch

all: $(PDF)

$(PDF): $(MAIN).tex references.bib sections/*.tex
	@if command -v latexmk >/dev/null 2>&1; then \
		latexmk -pdf -interaction=nonstopmode -halt-on-error $(MAIN).tex; \
	else \
		pdflatex -interaction=nonstopmode -halt-on-error $(MAIN).tex; \
		bibtex $(MAIN); \
		pdflatex -interaction=nonstopmode -halt-on-error $(MAIN).tex; \
		pdflatex -interaction=nonstopmode -halt-on-error $(MAIN).tex; \
	fi

watch:
	@if command -v latexmk >/dev/null 2>&1; then \
		latexmk -pdf -pvc -interaction=nonstopmode $(MAIN).tex; \
	else \
		echo "latexmk is not installed; watch mode is unavailable."; \
		exit 1; \
	fi

clean:
	@if command -v latexmk >/dev/null 2>&1; then \
		latexmk -C $(MAIN).tex; \
	fi
	@rm -f $(MAIN).aux $(MAIN).bbl $(MAIN).blg $(MAIN).fdb_latexmk \
		$(MAIN).fls $(MAIN).log $(MAIN).out $(MAIN).toc $(MAIN).pdf
