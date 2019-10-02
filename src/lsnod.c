#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

#define BUFLEN 256

int lsblock(dev_t dev, char type[BUFLEN], char desc[BUFLEN], char name[BUFLEN]);
int lschar(dev_t dev, char type[BUFLEN], char desc[BUFLEN], char name[BUFLEN]);

int main(int argc, char *argv[]) {
    const char *progname = argv[0];

    if(argc != 4) {
        fprintf(stderr, "Usage: %s <type> <minor> <major>\n", progname);
        fprintf(stderr, "Returns information on the given special file.\n");
        exit(EXIT_FAILURE);
    }

    char *errpos = NULL;
    const char *typename = argv[1];
    const char *majorstr = argv[2];
    const char *minorstr = argv[3];

    /* parse major */
    int major = strtoul(majorstr, &errpos, 10);
    if(*errpos) {
        perror(majorstr);
        exit(EXIT_FAILURE);
    }

    /* parse minor */
    int minor = strtoul(minorstr, &errpos, 10);
    if(*errpos) {
        perror(minorstr);
        exit(EXIT_FAILURE);
    }

    /* compute mode */
    dev_t dev = makedev(major, minor);

    /* buffers for output */
    char type[BUFLEN];
    char desc[BUFLEN];
    char name[BUFLEN];

    if(0 == strcmp(typename, "block")) {
        lsblock(dev, type, desc, name);
    } else if(0 == strcmp(typename, "character")) {
        lschar(dev, type, desc, name);
    } else {
        fprintf(stderr, "Error: unrecognised type (%s)\n", typename);
        exit(EXIT_FAILURE);
    }

    printf("type: %s\n", type);
    printf("desc: %s\n", desc);
    printf("name: %s\n", name);

    return EXIT_SUCCESS;
}

#define type(...) snprintf(type, BUFLEN, __VA_ARGS__)
#define desc(...) snprintf(desc, BUFLEN, __VA_ARGS__)
#define name(...) snprintf(name, BUFLEN, __VA_ARGS__)

const char *Nth[] = {
    "First",
    "Second",
    "Third",
    "Fourth",
    "Fifth",
    "Sixth",
    "Seventh",
    "Eighth",
    "Nineth",
    "Tenth",
    "Eleventh",
    "Twelveth",
    "13th",
    "14th",
    "15th",
    "16th",
    "17th",
    "18th",
    "19th",
    "20th",
    "21st",
    "22nd",
    "23rd",
    "24th",
    "25th",
    "26th",
    "27th",
    "28th",
    "29th",
    "30th",
    "31st",
    "32nd",
    "33rd",
    "34th",
    "35th",
    "36th",
    "37th",
    "38th",
    "39th",
    "40th",
    "41st",
    "42nd",
    "43rd",
    "44th",
    "45th",
    "46th",
    "47th",
    "48th",
    "49th",
    "50th",
    "51st",
    "52nd",
    "53rd",
    "54th",
    "55th",
    "56th",
    "57th",
    "58th",
    "59th",
    "60th",
    "61st",
    "62nd",
    "63rd",
    "64th",
    "65th",
    "66th",
    "67th",
    "68th",
    "69th",
    "70th",
    "71st",
    "72nd",
    "73rd",
    "74th",
    "75th",
    "76th",
    "77th",
    "78th",
    "79th",
    "80th",
    "81st",
    "82nd",
    "83rd",
    "84th",
    "85th",
    "86th",
    "87th",
    "88th",
    "89th",
    "90th",
    "91st",
    "92nd",
    "93rd",
    "94th",
    "95th",
    "96th",
    "97th",
    "98th",
    "99th",
    "100th",
};

int lsblock(dev_t dev, char type[BUFLEN], char desc[BUFLEN], char name[BUFLEN]) {
    int maj = major(dev);
    int min = minor(dev);

    /* null-terminate strings */
    type[0] = 0;
    name[0] = 0;
    desc[0] = 0;

    switch(maj) {
        case 0:
            type("Unnamed devices (e.g. non-device mounts)");
            name("");
            if(min == 0) desc("reserved as null device number");
            break;
        case 1:
            type("RAM disk");
            if(min < 250) {
                name("ram%u", min);
                desc("RAM disk");
            } else if(min == 250) {
                name("initrd");
                desc("Initial RAM disk");
            }
            break;
        case 2:
            type("Floppy disks");
            name("");
            desc("");
            break;
        case 3:
            type("First MFM, RLL and IDE hard disk/CD-ROM interface");
            name("");
            desc("");
            break;
        case 4:
            type("Aliases for dynamically allocated major devices");
            name("");
            desc("");
            break;
        case 5:
            type("");
            name("");
            desc("");
            break;
        case 6:
            type("");
            name("");
            desc("");
            break;
        case 7:
            type("Loopback devices");
            name("");
            desc("");
            break;
        case 8:
            type("SCSI disk devices (0-15)");
            name("");
            desc("");
            break;
        case 9:
            type("Metadisk (RAID) devices");
            name("");
            desc("");
            break;
        case 10:
            type("");
            name("");
            desc("");
            break;
        case 11:
            type("SCSI CD-ROM devices");
            name("");
            desc("");
            break;
        case 12:
            type("");
            name("");
            desc("");
            break;
        case 13:
            type("Previously used for the XT disk (/dev/xdN). Deleted in kernel v3.9.");
            name("");
            desc("");
            break;
        case 14:
            type("");
            name("");
            desc("");
            break;
        case 15:
            type("Sony CDU-31A/CDU-33A CD-ROM");
            name("");
            desc("");
            break;
        case 16:
            type("GoldStar CD-ROM");
            name("");
            desc("");
            break;
        case 17:
            type("Optics Storage CD-ROM");
            name("");
            desc("");
            break;
        case 18:
            type("Sanyo CD-ROM");
            name("");
            desc("");
            break;
        case 19:
            type("\"Double\" compressed disk");
            if(min < 8) {
                name("double%u", min);
                desc("%s compressed disk", Nth[min]);
            }
            if(128 < min && min < 138) {
                name("cdouble%u", min - 128);
                desc("Mirror of %s compressed disk", Nth[min - 128]);
            }
            break;
        case 20:
            type("Hitachi CD-ROM (under development)");
            if(min == 0) {
                name("hitcd");
                desc("Hitachi CD-ROM");
            }
            break;
        default:
            type("unknown");
            desc("unknown");
            name("unknown");
            break;
    }

    return 0;
}

int lschar(dev_t dev, char type[BUFLEN], char desc[BUFLEN], char name[BUFLEN]) {
    return 0;
}
