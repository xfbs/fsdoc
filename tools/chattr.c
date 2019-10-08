#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdbool.h>
#include <linux/fs.h>
#include <string.h>

typedef struct {
    int attr;
    const char *name;
} attr_types_t;

static const attr_types_t attr_types[] = {
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
    {FS_EXTENT_FL, "extent"},
    {FS_DIRTY_FL, "dirty"},
    {FS_COMPRBLK_FL, "comprblk"},
    {FS_NOCOMP_FL, "nocomp"},
    {FS_ENCRYPT_FL, "encrypt"},
    {FS_BTREE_FL, "btree"},
    {FS_INDEX_FL, "hashindex"},
    {FS_IMAGIC_FL, "imagic"},
    {FS_HUGE_FILE_FL, "hugefile"},
    {FS_EA_INODE_FL, "eainode"},
    {FS_EOFBLOCKS_FL, "eofblocks"},
    {FS_INLINE_DATA_FL, "inlinedata"},
    {0, NULL},
};

bool is_negated(const char *str) {
    return *str == 'n' && *(str + 1) == 'o';
}

int update_attr(char *attrname, int *attr) {
    /* normalise attribute name to non-negated */
    bool attr_neg = is_negated(attrname);

    /* skip over 'no' */
    if(attr_neg) {
        attrname += 2;
    }

    for(size_t i = 0; attr_types[i].name; i++) {
        const char *cur_attr = attr_types[i].name;
        bool cur_attr_neg = is_negated(cur_attr);

        /* skip over 'no' */
        if(cur_attr_neg) {
            cur_attr += 2;
        }

        /* check if names match */
        if(0 != strcmp(attrname, cur_attr)) {
            continue;
        }

        /* if they have the same negation status, set attr, otherwise unset. */
        if(attr_neg == cur_attr_neg) {
            *attr |= attr_types[i].attr;
        } else {
            *attr &= ~attr_types[i].attr;
        }

        return 0;
    }

    /* no match */
    printf("Error: unrecognised attr (%s)\n", attrname);

    return -1;
}

int update_attrs(char *attrstr, int *attr) {
    char *stringp = attrstr;
    char *token;
    const char *delim = ",";

    while((token = strsep(&stringp, delim)) != NULL) {
        if(0 != update_attr(token, attr)) {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    /* make sure an argument was given */
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <attr> <name>\n", progname);
        fprintf(stderr, "Change file flags.\n");
        return EXIT_FAILURE;
    }

    char *attrstr = argv[1];
    const char *filename = argv[2];

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

    /* change flags */
    int ret = update_attrs(attrstr, &attr);
    if(0 != ret) {
        close(fd);
        return EXIT_FAILURE;
    }

    /* set updated flags */
    if(0 != ioctl(fd, FS_IOC_SETFLAGS, &attr)) {
        perror(filename);
        close(fd);
        return EXIT_FAILURE;
    }

    /* clean up */
    close(fd);

    return EXIT_SUCCESS;
}
