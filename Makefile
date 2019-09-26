LATEX	= lualatex

fsdoc.pdf: fsdoc.tex
	$(LATEX) $<

src/perms: src/perms.c
	$(CC) -o $@ $<

clean:
	$(RM) fsdoc.pdf
