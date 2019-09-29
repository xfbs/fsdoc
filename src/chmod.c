#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __linux__
/* linux doesn't have a lchmod. */
#define lchmod chmod
#endif

#define WHO_MASK(x) \
    (x == 'u') ? 04700 : \
    (x == 'g') ? 02070 : \
    (x == 'o') ? 01007 : \
    (x == 'a') ? 07777 : \
    00000

void usage(const char *progname) {
    fprintf(stderr, "Changes mode (permission) of file.\n");
    fprintf(stderr, "Usage: %s [-xh] <mode> <file>\n\n", progname);
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "  -x   Interpret <flags> as hexadecimal string.\n");
    fprintf(stderr, "  -h   Show this help text and quit.\n\n");
    fprintf(stderr, "FLAGS\n");
    fprintf(stderr, "  Use a comma-separated list of flags to set, like ‘ucgh,hidden’.\n");
    fprintf(stderr, "  Supported flags: ucgh scgh opaque hidden uappnd sappnd restricted dump arch.\n");
    fprintf(stderr, "  Prepend ‘no‘ to negate a flag, like ‘noucgh,nohidden’ to unset.\n");
    fprintf(stderr, "  When -x is specified, flags get interpreted as hexadecimal.\n");
}

mode_t fgetmode(const char *filename) {
    struct stat st;
    int ret = stat(filename, &st);
    if(0 != ret) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* only care about file mode bits */
    return st.st_mode & 07777;
}

int updatemode(const char *modestr, mode_t mode, mode_t *out) {
    /* don't allow empty mode strings or 755,,755 (multiple commas) */
    if(*modestr == ',' || *modestr == 0) {
        return -1;
    }

    /* try parsing as octal */
    char *endptr = NULL;
    mode_t parsed = strtoul(modestr, &endptr, 8);

    if(*endptr == 0) {
        *out = parsed;
        return 0;
    } else if(*endptr == ',') {
        return updatemode(endptr + 1, parsed, out);
    }

    /* try parsing as mode string */
    char who = *modestr;

    /* make sure this is a legal subject */
    if(!(who == 'u' || who == 'g' || who == 'o' || who == 'a')) {
        return -2;
    }
    modestr++;

    do {
        char cmd = *modestr;

        switch(cmd) {
            case '-':
            case '+': break;
            case '=':
                /* reset permissions */
                mode &= ~WHO_MASK(who);
                break;
            default: return -3;
        }

        do {
            /* TODO */
        } while(*modestr);
    } while(*modestr && *modestr != ',');

    return 0;
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

    char *modestr = argv[optind];
    const char *filename = argv[optind + 1];

    mode_t mode = fgetmode(filename);

    /* parse modestr and update mode */
    ret = updatemode(modestr, mode, &mode);
    if(0 != ret) {
        usage(progname);
        exit(EXIT_FAILURE);
    }

    /* update file mode */
    ret = lchmod(filename, mode);
    if(ret != 0) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
