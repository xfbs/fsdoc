#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret;

    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        fprintf(stderr, "Removes the directory file given by <name>. Requires that "
                "it is empty.\n");
        return EXIT_FAILURE;
    }

    /* try to call unlink */
    if(0 != rmdir(argv[1])) {
        /* print error on failure */
        perror(argv[1]);
    }

    return EXIT_SUCCESS;
}
