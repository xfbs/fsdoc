# fsdoc

This a repository containing some notes I took on filesystems on MacOS and on Linux. You can either download the most recent PDF from [here](https://github.com/xfbs/fsdoc/releases/download/latest/fsdoc.pdf) or build it yourself using the provided `Makefile` by doing the following:

    make

Note that you will need `lualatex`, `makeglossaries` and `biber` installed in order for this to work. 

## Tools

This project also comes with a bunch of tools I have written to demonstrate how to use the syscalls provided on your system to work with the filesystem. These tools have been tested on both MacOS and Linux, but might not be entirely bug-free. You can compile them with the `Makefile` by doing

    make -j 4 tools

This will only compile the tools appropriate for your system, as some tools are Linux- or MacOS-only. 

## License

Anything I write, be it code or text, is covered under the MIT license. Small portions of code and text might be taken from other sources and hence covered under other licenses, but I have marked this where appropriate.
