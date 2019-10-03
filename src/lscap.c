#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", progname);
        fprintf(stderr, "Lists the capabilities of the specified file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    /* load capabilities */
    cap_t cap = cap_get_file(filename);

    /* error handling */
    if(cap == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* convert to textual representation */
    char *captext = cap_to_text(cap, NULL);

    if(captext == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* print in text form */
    printf("%s\n", captext);

    /* cleanup */
    cap_free(captext);
    cap_free(cap);

    return EXIT_SUCCESS;
}
