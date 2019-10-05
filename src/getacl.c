/*
 * For functions uuid_to_name(), printacl(), and the ACL permission and
 * flag structs, which are taken from ‘print.c’ of the ls tool from Apple's
 * file_cmds open source project, the following applies:
 *
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Michael Fischbein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/acl.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <stdbool.h>
#include <membership.h>
#include "membershipPriv.h"

enum displaytype {
    DISPLAY_RAW,
    DISPLAY_PRETTY,
};

/* acl permissions and what they can be set on */
#define ACL_PERM_DIR	(1<<0)
#define ACL_PERM_FILE	(1<<1)
static struct {
	acl_perm_t	perm;
	char		*name;
	int		flags;
} acl_perms[] = {
	{ACL_READ_DATA,		"read",		ACL_PERM_FILE},
	{ACL_LIST_DIRECTORY,	"list",		ACL_PERM_DIR},
	{ACL_WRITE_DATA,	"write",	ACL_PERM_FILE},
	{ACL_ADD_FILE,		"add_file",	ACL_PERM_DIR},
	{ACL_EXECUTE,		"execute",	ACL_PERM_FILE},
	{ACL_SEARCH,		"search",	ACL_PERM_DIR},
	{ACL_DELETE,		"delete",	ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_APPEND_DATA,	"append",	ACL_PERM_FILE},
	{ACL_ADD_SUBDIRECTORY,	"add_subdirectory", ACL_PERM_DIR},
	{ACL_DELETE_CHILD,	"delete_child",	ACL_PERM_DIR},
	{ACL_READ_ATTRIBUTES,	"readattr",	ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_WRITE_ATTRIBUTES,	"writeattr",	ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_READ_EXTATTRIBUTES, "readextattr",	ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_WRITE_EXTATTRIBUTES, "writeextattr", ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_READ_SECURITY,	"readsecurity",	ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_WRITE_SECURITY,	"writesecurity", ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_CHANGE_OWNER,	"chown",	ACL_PERM_FILE | ACL_PERM_DIR},
	{0, NULL, 0}
};

/* acl flags describing how permissions should be interpreted */
static struct {
	acl_flag_t	flag;
	char		*name;
	int		flags;
} acl_flags[] = {
	{ACL_ENTRY_FILE_INHERIT, 	"file_inherit",		ACL_PERM_DIR},
	{ACL_ENTRY_DIRECTORY_INHERIT,	"directory_inherit",	ACL_PERM_DIR},
	{ACL_ENTRY_LIMIT_INHERIT,	"limit_inherit",	ACL_PERM_FILE | ACL_PERM_DIR},
	{ACL_ENTRY_ONLY_INHERIT,	"only_inherit",		ACL_PERM_DIR},
	{0, NULL, 0}
};

void usage(const char *progname) {
    fprintf(stderr, "Usage: %s [-r] <name>\n\n", progname);
    fprintf(stderr, "Lists the Access Control Lists of the specified file.\n\n");
    fprintf(stderr, "FLAGS:\n");
    fprintf(stderr, "  -r    List raw ACL, as returned by acl_to_text().\n");
    fprintf(stderr, "  -h    Show this help text.\n");
    exit(EXIT_FAILURE);
}

#define fail(name) \
    perror(name); \
    exit(EXIT_FAILURE)

void display_raw(acl_t acl) {
    /* convert to textual representation */
    char *acltext = acl_to_text(acl, NULL);

    if(acltext == NULL) {
        fail("acl_to_text");
    }

    /* print in text form */
    printf("%s\n", acltext);

    /* cleanup */
    acl_free(acltext);
}

char *uuid_to_name(uuid_t *uuid) {
    /* big enough to hold "group:groupname" and "user:username" */
    static char name[MAXLOGNAME + 10];
	int rec_type;
	char *rec_name = NULL;

    /* try to translate this UUID into a name */
    int ret = mbr_identifier_translate(
            ID_TYPE_UUID,
            *uuid,
            sizeof(*uuid),
            ID_TYPE_NAME,
            (void **) &rec_name,
            &rec_type);

    /* if we can't translate this UUID, give up and return it raw. */
	if(0 != ret) {
        uuid_unparse_upper(*uuid, name);
        return name;
	}

    /* build string */
    const char *typestr = (rec_type == MBR_REC_TYPE_USER) ? "user" : "group";
	snprintf(name, sizeof name, "%s:%s", typestr, rec_name);

    /* cleanup */
	free(rec_name);

	return name;
}

void printacl(acl_t acl, bool isdir) {
	acl_entry_t	entry = NULL;
    int entry_id = ACL_FIRST_ENTRY;

    while(0 == acl_get_entry(acl, entry_id, &entry)) {
        entry_id = ACL_NEXT_ENTRY;

        acl_tag_t tag;
		if(0 != acl_get_tag_type(entry, &tag)) {
            fail("acl_get_tag_type");
        }

        acl_flagset_t flags;
		if(0 != acl_get_flagset_np(entry, &flags)) {
            fail("acl_get_flagset_np");
        }

        acl_permset_t perms;
		if(0 != acl_get_permset(entry, &perms)) {
            fail("acl_get_permset");
        }

        uuid_t *applicable = (uuid_t *) acl_get_qualifier(entry);
		if (applicable == NULL) {
            fail("acl_get_qualifier");
        }

        /* translate uuid to name */
		char *name = uuid_to_name(applicable);
		acl_free(applicable);

        /* figure out what type this is based off the tag */
        char *type = "unknown";
		switch(tag) {
            case ACL_EXTENDED_ALLOW:
                type = "allow";
                break;
            case ACL_EXTENDED_DENY:
                type = "deny";
                break;
            default: break;
		}

        /* construct flag string by checking if this entry is to be inherited */
        const char *flagstr = "";
        if(acl_get_flag_np(flags, ACL_ENTRY_INHERITED)) {
            flagstr = " inherited";
        }

        /* print target, flag string and type */
		printf("%s%s %s ", name, flagstr, type);

        /* print permissions */
        bool first = true;
		for (size_t i = 0; acl_perms[i].name != NULL; i++) {
            /* skip this permission if it isn't set */
			if(0 == acl_get_perm_np(perms, acl_perms[i].perm)) {
				continue;
            }

            /* skip this permission if it doesn't apply */
			if(!(acl_perms[i].flags & (isdir ? ACL_PERM_DIR : ACL_PERM_FILE))) {
				continue;
            }

            if(!first) {
                putchar(',');
            }

			printf("%s", acl_perms[i].name);
            first = false;
		}

		for (size_t i = 0; acl_flags[i].name != NULL; i++) {
            /* skip this permission if it isn't set */
			if(0 == acl_get_flag_np(flags, acl_flags[i].flag)) {
				continue;
            }

            /* skip this permission if it doesn't apply */
			if(!(acl_flags[i].flags & (isdir ? ACL_PERM_DIR : ACL_PERM_FILE))) {
				continue;
            }

            if(!first) {
                putchar(',');
            }

			printf("%s", acl_flags[i].name);
            first = false;
		}

		putchar('\n');
	}
}

int is_dir(const char *path) {
    struct stat st;
    if(0 != stat(path, &st)) {
        fail("stat");
    }
    return S_ISDIR(st.st_mode);
}

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    enum displaytype displaytype = DISPLAY_PRETTY;

    int ch;
    while((ch = getopt(argc, argv, "rh")) != -1) {
        switch(ch) {
            case 'h':
                usage(progname);
                exit(EXIT_SUCCESS);
            case 'r':
                displaytype = DISPLAY_RAW;
                break;
            case '?':
            default:
                usage(progname);
                exit(EXIT_FAILURE);
        }
    }

    /* make sure an argument was given */
    if((argc - optind) != 1) {
        usage(progname);
        return EXIT_FAILURE;
    }

    const char *filename = argv[optind];

    /* load acl */
    acl_t acl = acl_get_file(filename, ACL_TYPE_EXTENDED);

    /* error handling */
    if(acl == NULL) {
        fail("acl_get_file");
    }

    switch(displaytype) {
        case DISPLAY_RAW:
            display_raw(acl);
            break;
        case DISPLAY_PRETTY:
            printacl(acl, is_dir(filename));
            break;
    }

    acl_free(acl);

    return EXIT_SUCCESS;
}
