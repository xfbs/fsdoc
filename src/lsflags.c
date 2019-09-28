#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

int main(int argc, char *argv[]) {
    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        fprintf(stderr, "Lists file flags.\n");
        return EXIT_FAILURE;
    }

    /* use lstat to be able to get info on symbolic links */
    struct stat stat;
    int ret = lstat(argv[1], &stat);

    /* check for errors */
    if(ret != 0) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    const char *flags = fflagstostr(stat.st_flags);
    puts(flags);
    free((void *)flags);

    return EXIT_SUCCESS;
}
