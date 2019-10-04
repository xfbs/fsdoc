#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/acl.h>

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <acl> <name>\n", progname);
        fprintf(stderr, "Sets the ACL of the specified file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[2];
    const char *aclstr = argv[1];

    /* parse acl */
    acl_t acl = acl_from_text(aclstr);

    /* error handling */
    if(acl == NULL) {
        perror("error parsing acl");
        exit(EXIT_FAILURE);
    }

    /* try to set file acls */
    int ret = acl_set_file(filename, ACL_TYPE_EXTENDED, acl);

    if(0 != ret) {
        perror("error setting acl");
        exit(EXIT_FAILURE);
    }

    /* cleanup */
    acl_free(acl);

    return EXIT_SUCCESS;
}
