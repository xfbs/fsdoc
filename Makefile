LATEX	= lualatex
GLOSSARIES = makeglossaries
TOOLS	= src/perms src/unlink

all: fsdoc.pdf

tools: $(TOOLS)

fsdoc.pdf: fsdoc.tex
	$(LATEX) $<
	$(GLOSSARIES) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<

src/%: src/%.c
	$(CC) -o $@ $<

clean:
	$(RM) fsdoc.pdf
	$(RM) *.aux *.glg *.glo *.log *.ist *.out *.toc *.aux *.gls
	$(RM) $(TOOLS)
