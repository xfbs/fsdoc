LATEX	= lualatex

fsdoc.pdf: fsdoc.tex
	$(LATEX) $<
