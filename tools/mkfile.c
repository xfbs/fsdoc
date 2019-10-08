#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int ret;
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <name> <mode>\n", progname);
        fprintf(stderr, "Creates regular file with name <name> and mode specified "
                "by <mode>.\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *modestr = argv[2];

    /* parse mode string */
    int mode = strtol(modestr, NULL, 8);

    /* try to call create file */
    int fd = open(filename, O_CREAT | O_EXCL, mode);

    /* check for error */
    if(0 > fd) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    close(fd);

    return EXIT_SUCCESS;
}
