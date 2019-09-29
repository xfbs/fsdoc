#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

const char *uid_to_name(uid_t uid) {
    struct passwd *passwd = getpwuid(uid);
    assert(passwd && passwd->pw_name);
    return passwd->pw_name;
}

const char *gid_to_name(gid_t gid) {
    struct group *group = getgrgid(gid);
    assert(group && group->gr_name);
    return group->gr_name;
}

const char *time_str(struct timespec ts) {
    static char buf[256];
    struct tm t;
    int len;

    tzset();
    if (localtime_r(&(ts.tv_sec), &t) == NULL) {
        perror("time");
        exit(EXIT_FAILURE);
    }

    len = strftime(buf, sizeof buf, "%FT%TZ", &t);
    if(len == 0) {
        perror("time_str");
        exit(EXIT_FAILURE);
    }

    return buf;
}

const char *file_type_str(mode_t mode) {
    switch(mode & S_IFMT) {
        case S_IFIFO: return "fifo";
        case S_IFCHR: return "character special";
        case S_IFDIR: return "directory";
        case S_IFBLK: return "block special";
        case S_IFREG: return "regular";
        case S_IFLNK: return "symbolic link";
        case S_IFSOCK: return "socket";
#ifdef __APPLE__
        // this only exists on macOS.
        case S_IFWHT: return "whiteout";
#endif
        default: return "unknown";
    }
}

int main(int argc, char *argv[]) {
    /* make sure an argument was given */
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        fprintf(stderr, "Gets file status.\n");
        return EXIT_FAILURE;
    }

    /* use lstat to be able to get info on symbolic links */
    struct stat stat;
    int ret = lstat(argv[1], &stat);

    /* check for errors */
    if(ret != 0) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

#ifdef __linux__
    printf("inode: %li\n", stat.st_ino);
    printf("type:  %s\n", file_type_str(stat.st_mode));
    printf("owner: %u (%s)\n", stat.st_uid, uid_to_name(stat.st_uid));
    printf("group: %u (%s)\n", stat.st_gid, gid_to_name(stat.st_gid));
    printf("links: %lu\n", stat.st_nlink);
    printf("size:  %li\n", stat.st_size);
    printf("atime: %s\n", time_str(stat.st_atim));
    printf("ctime: %s\n", time_str(stat.st_ctim));
    printf("mtime: %s\n", time_str(stat.st_mtim));
    printf("dev:   %ld\n", stat.st_dev);
#elif __APPLE__
    printf("inode: %lli\n", stat.st_ino);
    printf("type:  %s\n", file_type_str(stat.st_mode));
    printf("owner: %u (%s)\n", stat.st_uid, uid_to_name(stat.st_uid));
    printf("group: %u (%s)\n", stat.st_gid, gid_to_name(stat.st_gid));
    printf("links: %hu\n", stat.st_nlink);
    printf("size:  %lli\n", stat.st_size);
    printf("atime: %s\n", time_str(stat.st_atimespec));
    printf("ctime: %s\n", time_str(stat.st_ctimespec));
    printf("mtime: %s\n", time_str(stat.st_mtimespec));
    printf("birth: %s\n", time_str(stat.st_birthtimespec));
    printf("dev:   %d\n", stat.st_dev);
    printf("gen:   %u\n", stat.st_gen);

    const char *flags = fflagstostr(stat.st_flags);
    printf("flags: %08x (%s)\n", stat.st_flags, flags);
    free((void *)flags);
#endif

    return EXIT_SUCCESS;
}
