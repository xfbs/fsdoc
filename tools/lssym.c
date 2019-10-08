#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret;
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", progname);
        fprintf(stderr, "Reads symlink with name <name>\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    char buffer[256];
    ssize_t length;

    /* try to create dir */
    if((length = readlink(filename, buffer, sizeof(buffer) - 1)) == -1) {
        /* signal error */
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* null-terminate the buffer */
    buffer[length] = 0;

    /* print result */
    printf("%s\n", buffer);

    return EXIT_SUCCESS;
}
