#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

void usage(const char *progname) {
    fprintf(stderr, "Updates flags of file.\n");
    fprintf(stderr, "Usage: %s [-xh] <flags> <file>\n\n", progname);
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "  -x   Interpret <flags> as hexadecimal string.\n");
    fprintf(stderr, "  -h   Show this help text and quit.\n\n");
    fprintf(stderr, "FLAGS\n");
    fprintf(stderr, "  Use a comma-separated list of flags to set, like ‘ucgh,hidden’.\n");
    fprintf(stderr, "  Supported flags: ucgh scgh opaque hidden uappnd sappnd restricted dump arch.\n");
    fprintf(stderr, "  Prepend ‘no‘ to negate a flag, like ‘noucgh,nohidden’ to unset.\n");
    fprintf(stderr, "  When -x is specified, flags get interpreted as hexadecimal.\n");
}

unsigned long getflags(char *flagsstr, const char *filename, bool hex) {
    if(hex) {
        /* parse hex flagsstr and return */
        char *endptr = NULL;
        unsigned long flags = strtoul(flagsstr, &endptr, 16);

        if(*endptr != 0) {
            perror("strtoul");
        }

        return flags;
    }

    /* split flags from comma-delimited list */
    char **list = split_string(flagsstr, ",");

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
    if(0 != lstat(filename, &stat)) {
        perror("stat");
        return EXIT_FAILURE;
    }

    /* compute new flags */
    unsigned long flags = stat.st_flags;
    flags = flags & (~clrp);
    flags = flags | setp;

    return flags;
}

int main(int argc, char *argv[]) {
    int ret, ch;
    bool hex = false;
    const char *progname = argv[0];

    while((ch = getopt(argc, argv, "hx")) != -1) {
        switch(ch) {
            case 'h':
                usage(progname);
                exit(EXIT_SUCCESS);
            case 'x':
                hex = true;
                break;
            case '?':
            default:
                usage(progname);
                exit(EXIT_FAILURE);
        }
    }

    /* make sure an argument was given */
    if((argc - optind) != 2) {
        usage(progname);
        return EXIT_FAILURE;
    }

    char *flagsstr = argv[optind];
    const char *filename = argv[optind + 1];

    /* compute flags from argument */
    unsigned long flags = getflags(flagsstr, filename, hex);

    /* set flags */
    if(0 != lchflags(filename, flags)) {
        perror("chflags");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
