#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret;
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", progname);
        fprintf(stderr, "Removes the directory entry (file) given by <name>.\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    /* try to call unlink */
    if(0 != unlink(filename)) {
        /* print error on failure */
        perror(filename);
    }

    return EXIT_SUCCESS;
}
