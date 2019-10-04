#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/acl.h>

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <name>\n", progname);
        fprintf(stderr, "Lists the ACL of the specified file.\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    /* load acl */
    acl_t acl = acl_get_file(filename, ACL_TYPE_EXTENDED);

    /* error handling */
    if(acl == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* convert to textual representation */
    char *acltext = acl_to_text(acl, NULL);

    if(acltext == NULL) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    /* print in text form */
    printf("%s\n", acltext);

    /* cleanup */
    acl_free(acltext);
    acl_free(acl);

    return EXIT_SUCCESS;
}
