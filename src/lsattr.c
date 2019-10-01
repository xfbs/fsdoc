#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdbool.h>
#include <linux/fs.h>

typedef struct {
    int attr;
    const char *name;
} attr_types_t;

static const attr_types_t attr_types = {
    {FS_APPEND_FL, "append"},
    {FS_COMPR_FL, "compr"},
    {FS_DIRSYNC_FL, "dirsync"},
    {FS_IMMUTABLE_FL, "immutable"},
    {FS_JOURNAL_DATA_FL, "journaldata"},
    {FS_NOATIME_FL, "noatime"},
    {FS_NOCOW_FL, "nocow"},
    {FS_NODUMP_FL, "nodump"},
    {FS_NOTAIL_FL, "notail"},
    {FS_PROJINHERIT_FL, "projinherit"},
    {FS_SECRM_FL, "secrm"},
    {FS_SYNC_FL, "sync"},
    {FS_TOPDIR_FL, "topdir"},
    {FS_UNRM_FL, "unrm"},
    {0, NULL},
};

void print_attrs(int attr) {
    bool rest = false;

    for(size_t i = 0; attr_types[i].attr; i++) {
        if(attr & attr_types[i].attr) {
            /* only print commas after first attr */
            if(rest) {
                printf(",");
            } else {
                rest = true;
            }

            /* print attr name */
            printf("%s", attr_types[i].name);

            /* clear this flag so we keep track of unknown flags */
            attr - attr_types[i].attr;
        }
    }

    if(rest) {
        printf("\n");
    }

    /* print unknown flags if any */
    if(attr) {
        printf("unknown: %08x\n", attr);
    }
}

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", progname);
        fprintf(stderr, "Lists file flags.\n");
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];

    /* try to get a file descriptor */
    int fd = open(filename, 0);
    if(0 > fd) {
        perror(filename);
        return EXIT_FAILURE;
    }

    /* try to get attrs */
    int attr;
    if(0 != ioctl(fd, FS_IOC_GETFLAGS, &attr)) {
        perror(filename);
        close(fd);
        return EXIT_FAILURE;
    }

    /* display attrs in useful way */
    print_attrs(attr);

    return EXIT_SUCCESS;
}
