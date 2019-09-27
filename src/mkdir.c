/* mkdir
 *
 * Creates a directory file with access permissions specified.
 */

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int ret;

    /* make sure an argument was given */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <name> <mode>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* parse mode string */
    int mode = strtol(argv[2], NULL, 8);

    /* try to call unlink */
    if(0 != mkdir(argv[1], mode)) {
        /* print error on failure */
        perror(argv[1]);
    }

    return EXIT_SUCCESS;
}
