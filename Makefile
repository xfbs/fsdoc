LATEX	= lualatex
GLOSSARIES = makeglossaries
BIBER	= biber
TOOLS	= perms unlink rmdir mkdir mkfile mksym lssym lsdir stat chmod mknod lsxattr getxattr setxattr

ifeq ($(shell uname -s),Darwin)
	TOOLS += chflags lsflags
endif

ifeq ($(shell uname -s),Linux)
	TOOLS += lsattr getcap setcap proccap
	LDFLAGS += -lcap
endif

all: fsdoc.pdf

tools: $(TOOLS:%=src/%)

fsdoc.pdf: fsdoc.tex
	$(LATEX) $<
	$(GLOSSARIES) $(<:%.tex=%)
	$(BIBER) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<

src/%: src/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	$(RM) fsdoc.pdf
	$(RM) *.aux *.glg *.glo *.log *.ist *.out *.toc *.aux *.gls *.bbl *.bcf *.blg *.run.xml
	$(RM) $(TOOLS:%=src/%)
