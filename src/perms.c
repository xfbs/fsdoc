#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

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

int main(int argc, char *argv[]) {
    uid_t uid = getuid();
    uid_t euid = geteuid();
    gid_t gid = getgid();
    gid_t egid = getegid();

    printf("uid %d (%s)\n", uid, uid_to_name(uid));
    printf("euid %d (%s)\n", euid, uid_to_name(euid));
    printf("gid %d (%s)\n", gid, gid_to_name(gid));
    printf("egid %d (%s)\n", egid, gid_to_name(egid));

    return 0;
}
