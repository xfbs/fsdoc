LATEX	= lualatex
GLOSSARIES = makeglossaries

all: fsdoc.pdf

fsdoc.pdf: fsdoc.tex
	$(LATEX) $<
	$(GLOSSARIES) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<

src/perms: src/perms.c
	$(CC) -o $@ $<

clean:
	$(RM) fsdoc.pdf
	$(RM) *.aux *.glg *.glo *.log *.ist *.out *.toc *.aux *.gls
