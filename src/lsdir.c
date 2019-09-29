#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int ret;

    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <dir>\n", argv[0]);
        fprintf(stderr, "Lists directory contents.\n");
        return EXIT_FAILURE;
    }

    const char *dirname = argv[1];

    /* open dir */
    DIR *dir = opendir(dirname);
    if(!dir) {
        perror(dirname);
        exit(EXIT_FAILURE);
    }

    /* iterate through dir */
    struct dirent entry;
    struct dirent *result;

    while(1) {
        ret = readdir_r(dir, &entry, &result);
        if(0 != ret) {
            perror(dirname);
            exit(EXIT_FAILURE);
        }

        if(!result) {
            break;
        }

        printf("%s\n", entry.d_name);
    }

    /* clean up */
    ret = closedir(dir);
    if(0 != ret) {
        perror(dirname);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
