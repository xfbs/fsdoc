#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret;
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <name> <target>\n", progname);
        fprintf(stderr, "Creates symlink with name <name> to <target>\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *target = argv[2];

    /* try to create dir */
    if(0 != symlink(target, filename)) {
        /* signal error */
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
