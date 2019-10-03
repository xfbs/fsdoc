#include <stdio.h>
#include <stdlib.h>
#include <sys/xattr.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", progname);
        fprintf(stderr, "Lists extended attributes set for file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    /* determine size of extended attribute list */
    ssize_t size = listxattr(filename, NULL, 0, 0);

    /* handle error */
    if(0 > size) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* handle no xattrs existing */
    if(0 == size) {
        exit(EXIT_SUCCESS);
    }

    char namebuf[size + 1];
    size = listxattr(filename, namebuf, size + 1, 0);

    /* handle error */
    if(0 > size) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* print returned attributes */
    size_t offset = 0;
    while(offset < size) {
        printf("%s\n", &namebuf[offset]);
        offset += strlen(&namebuf[offset]) + 1;
    }

    return EXIT_SUCCESS;
}
