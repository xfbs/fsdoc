LATEX	= lualatex
GLOSSARIES = makeglossaries
BIBER	= biber
TOOLS  += mkdir mkfile mksym mknod mkfifo
TOOLS  += lssym lsdir
TOOLS  += unlink rmdir
TOOLS  += lsxattr getxattr setxattr
TOOLS  += stat perms
TOOLS  += chmod

ifeq ($(shell uname -s),Darwin)
TOOLS += chflags lsflags getacl setacl
endif

ifeq ($(shell uname -s),Linux)
TOOLS += lsattr chattr getcap setcap proccap
LDFLAGS += -lcap
endif

all: fsdoc.pdf

tools: $(TOOLS:%=tools/%)

fsdoc.pdf: fsdoc.tex
	$(LATEX) $<
	$(GLOSSARIES) $(<:%.tex=%)
	$(BIBER) $(<:%.tex=%)
	$(LATEX) $<
	$(LATEX) $<

tools/%: tools/%.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	$(RM) fsdoc.pdf
	$(RM) *.aux *.glg *.glo *.log *.ist *.out *.toc *.aux *.gls *.bbl *.bcf *.blg *.run.xml
	$(RM) $(TOOLS:%=tools/%)
