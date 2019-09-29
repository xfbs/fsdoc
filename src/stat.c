#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

/* platform-specific settings */
#ifdef __APPLE__
#include <sys/types.h>
#define HAS_GEN
#define HAS_FLAGS
#define HAS_BIRTH
#define st_atim st_atimespec
#define st_ctim st_ctimespec
#define st_mtim st_mtimespec
#define st_btim st_birthtimespec
#define INODE_FMT "%lli"
#define LINKS_FMT "%hu"
#define SIZE_FMT "%lli"
#define DEV_FMT "%d,%d"
#endif

#ifdef __linux__
#include <sys/sysmacros.h>
#define INODE_FMT "%li"
#define LINKS_FMT "%lu"
#define SIZE_FMT "%li"
#define DEV_FMT "%d,%d"
#endif

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

/* create a modestring, like "-rwxr-xr-x". there is the strmode() function to
 * do this on BSD systems, but afaik it doesn't exist on linux.
 */
const char *mode_str(mode_t mode, char str[11]) {
    switch(mode & S_IFMT) {
        case S_IFIFO: str[0] = 'p'; break;
        case S_IFCHR: str[0] = 'c'; break;
        case S_IFDIR: str[0] = 'd'; break;
        case S_IFBLK: str[0] = 'b'; break;
        case S_IFREG: str[0] = '-'; break;
        case S_IFLNK: str[0] = 'l'; break;
        case S_IFSOCK: str[0] = 's'; break;
#ifdef __APPLE__
        /* this only exists on macOS. */
        case S_IFWHT: str[0] = 'w'; break;
#endif
        default: str[0] = '?'; break;
    }

    str[1] = (mode & S_IRUSR) ? 'r' : '-';
    str[2] = (mode & S_IWUSR) ? 'w' : '-';
    str[3] = (mode & S_IXUSR) ?
        ((mode & S_ISUID) ? 's' : 'x') :
        ((mode & S_ISUID) ? 'S' : '-') ;

    str[4] = (mode & S_IRGRP) ? 'r' : '-';
    str[5] = (mode & S_IWGRP) ? 'w' : '-';
    str[6] = (mode & S_IXGRP) ? 'x' : '-';

    str[7] = (mode & S_IROTH) ? 'r' : '-';
    str[8] = (mode & S_IWOTH) ? 'w' : '-';
    str[9] = (mode & S_IXOTH) ? 'x' : '-';

    /* null-terminate string */
    str[10] = 0;

    return str;
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

    char modestr[11];

    printf("inode: "INODE_FMT"\n", stat.st_ino);
    printf("type:  %s\n", file_type_str(stat.st_mode));
    printf("mode:  %04o (%s)\n", stat.st_mode & 07777, mode_str(stat.st_mode, modestr));
    printf("owner: %u (%s)\n", stat.st_uid, uid_to_name(stat.st_uid));
    printf("group: %u (%s)\n", stat.st_gid, gid_to_name(stat.st_gid));
    printf("links: "LINKS_FMT"\n", stat.st_nlink);
    printf("size:  "SIZE_FMT"\n", stat.st_size);
    printf("atime: %s\n", time_str(stat.st_atim));
    printf("mtime: %s\n", time_str(stat.st_mtim));
    printf("ctime: %s\n", time_str(stat.st_ctim));

#ifdef HAS_BIRTH
    printf("birth: %s\n", time_str(stat.st_btim));
#endif

    printf("dev:   "DEV_FMT"\n", major(stat.st_dev), minor(stat.st_dev));
    printf("rdev:  "DEV_FMT"\n", major(stat.st_rdev), minor(stat.st_rdev));

#ifdef HAS_GEN
    printf("gen:   %u\n", stat.st_gen);
#endif

#ifdef HAS_FLAGS
    const char *flags = fflagstostr(stat.st_flags);
    printf("flags: %08x (%s)\n", stat.st_flags, flags);
    free((void *)flags);
#endif

    return EXIT_SUCCESS;
}
