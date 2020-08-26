/* A program to show or change the optional boot delay configured
 * in an EmuTOS ROM image.
 *
 * Copyright (C) 2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ETOS_MAGIC_OFFS (0x2C)
#define OSXH_MAGIC_OFFS (0x34)
#define BOOTDELAY_OFFS  (OSXH_MAGIC_OFFS+0x18)

#define ETOS_MAGIC      "ETOS"
#define OSXH_MAGIC      "OSXH"
#define OSXH_MIN_VERS   (0x19)

#define BIGENDIAN_LONG(x) ((((unsigned long)x[0]) << 24ul) | (((unsigned long)x[1]) << 16ul) | \
                           (((unsigned long)x[2]) << 8ul)  | (((unsigned long)x[3])))

int main(int argc, char *argv[])
{
    FILE *fp;
    unsigned char magic[4];
    unsigned char olddelay;
    unsigned char newdelay;
    int setdelay = 0;
    int temp;

    if (argc < 2) {
        printf("usage: %s <EmuTOS ROM file> [boot delay]\r\n", *argv);
        printf("\r\nShow or change optional boot delay in EmuTOS ROM.\r\n");
        printf("Permissible delay values: 1-255 seconds, or 0 to deactivate delay.\r\n");
        return 1;
    }

    if (argc == 3) {
        temp = atoi(argv[2]);
        if ((temp < 0) || (temp > 255)) {
            printf("Permissible delay values: 1-255 seconds, or 0 to deactivate delay.\r\n");
            return 1;
        }
        newdelay = (unsigned char)temp;
        setdelay = 1;
    }

    /* Open file for reading & writing only when setting a new value. */
    fp = fopen(argv[1], setdelay?"rb+":"rb");
    if (!fp) {
        perror("Error opening EmuTOS ROM file");
        return -1;
    }

    if (fseek(fp, ETOS_MAGIC_OFFS, SEEK_SET) != 0) {
        perror("Error seeking to EmuTOS magic");
        return -1;
    }
    if (fread(&magic, sizeof(magic), 1, fp) != 1) {
        perror("Error reading EmuTOS magic");
        return -1;
    }
    if (memcmp(magic, ETOS_MAGIC, sizeof(magic)) != 0) {
        printf("%s is not an EmuTOS ROM image.\r\n", argv[1]);
        return -1;
    }

    if (fseek(fp, OSXH_MAGIC_OFFS, SEEK_SET) != 0) {
        perror("Error seeking to OSXH magic");
        return -1;
    }
    if (fread(&magic, sizeof(magic), 1, fp) != 1) {
        perror("Error reading OSXH magic");
        return -1;
    }
    if (memcmp(magic, OSXH_MAGIC, sizeof(magic)) != 0) {
        printf("This EmuTOS version does not support an optional boot delay.\r\n");
        return -1;
    }

    /* OSXH header length also indicates its version. */
    if (fread(&magic, sizeof(magic), 1, fp) != 1) {
        perror("Error reading OSXH header length");
        return -1;
    }
    if (BIGENDIAN_LONG(magic) < OSXH_MIN_VERS) {
        printf("This EmuTOS version does not support an optional boot delay.\r\n");
        return -1;
    }

    if (fseek(fp, BOOTDELAY_OFFS, SEEK_SET) != 0) {
        perror("Error seeking to boot delay");
        return -1;
    }
    if (fread(&olddelay, sizeof(olddelay), 1, fp) != 1) {
        perror("Error reading boot delay");
        return -1;
    }

    if (olddelay > 0) {
        printf("%s boot delay: %d seconds.\r\n", setdelay?"Old":"Current", olddelay);
    } else {
        printf("%s boot delay: off.\r\n", setdelay?"Old":"Current");
    }

    if (setdelay) {
        if (fseek(fp, BOOTDELAY_OFFS, SEEK_SET) != 0) {
            perror("Error seeking to boot delay");
            return -1;
        }
        if (fwrite(&newdelay, sizeof(newdelay), 1, fp) != 1) {
            perror("Error writing boot delay");
            return -1;
        }
        if (newdelay > 0) {
            printf("New boot delay: %d seconds.\r\n", newdelay);
        } else {
            printf("New boot delay: off.\r\n");
        }
    }

    fclose(fp);
    return 0;
}
