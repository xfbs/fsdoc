#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int ret;
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <name> <mode>\n", progname);
        fprintf(stderr, "Creates a FIFO file with name <name> and mode specified "
                "by <mode>.\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    const char *modestr = argv[2];

    /* parse mode string */
    int mode = strtol(modestr, NULL, 8);

    if(0 != mkfifo(filename, mode)) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
