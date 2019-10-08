#include <stdio.h>
#include <stdlib.h>
#include <sys/xattr.h>
#include <string.h>

#ifdef __linux__
#define setxattr(path, name, value, size, position, flags) \
    setxattr(path, name, value, size, flags)
#endif

const size_t bufsize = 4 * 1024;

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <attr> <file>\n", progname);
        fprintf(stderr, "Lists extended attributes set for file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[2];
    const char *attrname = argv[1];

    /* allocate buffer to hold data */
    size_t buffer_size = bufsize;
    size_t buffer_pos = 0;
    char *buffer = malloc(buffer_size);

    if(!buffer) {
        perror("buffer");
        exit(EXIT_FAILURE);
    }

    /* read all of stdin until eof */
    size_t read;
    while(0 != (read = fread(buffer + buffer_pos, 1, buffer_size - buffer_pos, stdin))) {
        buffer_pos += read;

        /* extend buffer if needed */
        if(buffer_pos == buffer_size) {
            buffer_size += bufsize;
            char *new = realloc(buffer, buffer_size);

            if(!new) {
                perror("realloc");
                free(buffer);
                exit(EXIT_FAILURE);
            } else {
                buffer = new;
            }
        }
    }

    /* handle stdin error */
    if(ferror(stdin)) {
        perror("stdin");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    /* set xattr */
    int ret = setxattr(filename, attrname, buffer, buffer_pos, 0, 0);

    /* release buffer */
    free(buffer);

    if(0 != ret) {
        perror("setxattr");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
