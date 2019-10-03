#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <cap> <name>\n", progname);
        fprintf(stderr, "Sets the capabilities of the specified file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[2];
    const char *capstr = argv[1];

    /* parse capabilities */
    cap_t cap = cap_from_text(capstr);

    /* error handling */
    if(cap == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* try to set file caps */
    int ret = cap_set_file(filename, cap);

    if(0 != ret) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* cleanup */
    cap_free(cap);

    return EXIT_SUCCESS;
}
