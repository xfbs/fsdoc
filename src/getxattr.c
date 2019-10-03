#include <stdio.h>
#include <stdlib.h>
#include <sys/xattr.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <attr> <file>\n", progname);
        fprintf(stderr, "Lists extended attributes set for file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[2];
    const char *attrname = argv[1];

    /* determine size of extended attribute list */
    ssize_t size = getxattr(filename, attrname, NULL, 0, 0, 0);

    /* handle error */
    if(0 > size) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* handle no xattrs existing */
    if(0 == size) {
        exit(EXIT_SUCCESS);
    }

    char buffer[size + 1];
    size = getxattr(filename, attrname, buffer, size + 1, 0, 0);

    /* handle error */
    if(0 > size) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* print returned attributes */
    fwrite(buffer, size, 1, stdout);

    return EXIT_SUCCESS;
}
