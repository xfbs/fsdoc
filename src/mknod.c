#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    const char *name;
    mode_t mode;
} special_type;

special_type special_types[] = {
    {"character", S_IFCHR},
    {"block", S_IFBLK},
    {"none", 0},
    {NULL, 0},
};

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 6) {
        fprintf(stderr, "Usage: %s <name> <perm> <type> <major> <minor>\n", progname);
        return EXIT_FAILURE;
    }

    char *errpos = NULL;
    const char *filename = argv[1];
    const char *permstr = argv[2];
    const char *typestr = argv[3];
    const char *majorstr = argv[4];
    const char *minorstr = argv[5];

    /* parse mode string */
    mode_t mode = strtoul(permstr, &errpos, 8);
    if(*errpos) {
        perror(permstr);
        exit(EXIT_FAILURE);
    }

    for(size_t i = 0; true; i++) {
        if(!special_types[i].name) {
            fprintf(stderr, "Error: unrecognised type (%s)\n", typestr);
            exit(EXIT_FAILURE);
        }

        if(0 == strcmp(special_types[i].name, typestr)) {
            mode |= special_types[i].mode;
            break;
        }
    }

    int major = strtoul(majorstr, &errpos, 10);
    if(*errpos) {
        perror(permstr);
        exit(EXIT_FAILURE);
    }

    int minor = strtoul(minorstr, &errpos, 10);
    if(*errpos) {
        perror(permstr);
        exit(EXIT_FAILURE);
    }

    int ret = mknod(filename, mode, makedev(major, minor));
    if(0 != ret) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
