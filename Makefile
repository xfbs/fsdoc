LATEX	= lualatex
GLOSSARIES = makeglossaries
TOOLS	= perms unlink rmdir mkdir stat

all: fsdoc.pdf

tools: $(TOOLS:%=src/%)

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
	$(RM) $(TOOLS:%=src/%)
