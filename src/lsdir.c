#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/errno.h>

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
    struct dirent *entry;

    /* reset errno to zero, because we don't know if readdir returns NULL
     * because of an error or not, and we use it to check for that. */
    errno = 0;

    /* print all dir entry names. */
    while((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    /* detect and output error. */
    if(0 != errno) {
        perror(dirname);
        exit(EXIT_FAILURE);
    }

    /* clean up */
    ret = closedir(dir);
    if(0 != ret) {
        perror(dirname);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
