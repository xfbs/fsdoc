/* rmdir
 *
 * removes a directory file. The directory must be empty (it must now have any
 * entries other than ‘.’ and ‘..’.
 */

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int ret;

    /* make sure an argument was given */
    assert(argc > 1);

    /* try to call unlink */
    if(0 != rmdir(argv[1])) {
        /* print error on failure */
        perror(argv[1]);
    }

    return EXIT_SUCCESS;
}
