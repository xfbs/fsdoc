#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* doesn't make sense to support more than this. */
#define MAX_FLAGS 128

char **split_string(char *str, const char *sep) {
    char **list = malloc(MAX_FLAGS * sizeof(char *));
    size_t pos = 0;

    while((list[pos] = strsep(&str, sep)) != NULL) {
        pos++;
        assert(pos < MAX_FLAGS);
    }

    return list;
}

int main(int argc, char *argv[]) {
    int ret;

    /* make sure an argument was given */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <name> <flags>\n", argv[0]);
        fprintf(stderr, "Updates flags of file\n");
        return EXIT_FAILURE;
    }

    /* split flags from comma-delimited list */
    char **list = split_string(argv[2], ",");

    /* parse mode string */
    unsigned long setp = 0;
    unsigned long clrp = 0;
    if(0 != strtofflags(list, &setp, &clrp)) {
        perror("strtofflags");
        exit(EXIT_FAILURE);
    }
    free(list);

    /* get previous flags */
    struct stat stat;
    if(0 != lstat(argv[1], &stat)) {
        perror("stat");
        return EXIT_FAILURE;
    }

    /* compute new flags */
    unsigned long flags = stat.st_flags;
    flags = flags & (~clrp);
    flags = flags | setp;

    /* set flags */
    if(0 != lchflags(argv[1], flags)) {
        perror("chflags");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
