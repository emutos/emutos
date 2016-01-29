/*
 * mkflop.c - create an auto-booting EmuTOS floppy.
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * this tool will create a simple auto-booting FAT12 floppy,
 * called emuboot.st from bootsect.img and emutos.img.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * manifest constants
 */
#define SECTOR_SIZE         512L
#define BOOTABLE_CHECKSUM   0x1234

#define FLOPNAME "emutos.st"
#define BOOTNAME "bootsect.img"
#define TOSNAME  "ramtos.img"
#define TOTAL_DISK_SECTORS (1 * 80 * 9) /* Single sided floppy */
#define MAX_EMUTOS_SIZE ((TOTAL_DISK_SECTORS - 1) * SECTOR_SIZE)

typedef unsigned char uchar;

struct loader {
    uchar pad[0x1e];
    uchar execflg[2];
    uchar ldmode[2];
    uchar ssect[2];
    uchar sectcnt[2];
    uchar ldaddr[4];
    uchar fatbuf[4];
    uchar fname[11];
    uchar reserved;
};

static int fill = 1;

static int mkflop(FILE *bootf, FILE *tosf, FILE *flopf)
{
    uchar buf[SECTOR_SIZE];
    struct loader *b = (struct loader *)buf;
    size_t count;
    unsigned short sectcnt;
    int i;

    /* read bootsect */
    count = SECTOR_SIZE;
    count = fread(buf, 1, count, bootf);
    if (count <= 0)
        return 1;       /* failure */
    if (count < SECTOR_SIZE) {
        memset(buf+count, 0, SECTOR_SIZE-count);
    }

    /* compute size of tosf, and update sectcnt */
    fseek(tosf, 0, SEEK_END);
    count = ftell(tosf);
    fseek(tosf, 0, SEEK_SET);

    if (count > MAX_EMUTOS_SIZE)
    {
        fprintf(stderr, "Error: %s is too big to fit on the floppy (%ld extra bytes).\n",
                        TOSNAME, (long)count-MAX_EMUTOS_SIZE);
        return 1;
    }

    sectcnt = (count+SECTOR_SIZE-1) / SECTOR_SIZE;
    b->sectcnt[0] = sectcnt>>8;
    b->sectcnt[1] = sectcnt;

    /* make bootsector bootable */
    {
        unsigned short a, sum;
        sum = 0;
        for (i = 0; i < (SECTOR_SIZE/2-1); i++) {
            a = (buf[2*i]<<8) + buf[2*i+1];
            sum += a;
        }
        a = BOOTABLE_CHECKSUM - sum;
        buf[510] = a>>8;
        buf[511] = a;
    }

    /* write down bootsector */
    fwrite(buf, 1, SECTOR_SIZE, flopf);

    /* copy the tos starting at sector 1 */
    for (i = 0; i < sectcnt; i++) {
        count = fread(buf, 1, SECTOR_SIZE, tosf);
        if (count < SECTOR_SIZE) {
            memset(buf+count, 0, SECTOR_SIZE - count);
        }
        fwrite(buf, 1, SECTOR_SIZE, flopf);
    }

    /* fill the disk with zeroes */
    if (fill) {
        memset(buf, 0, SECTOR_SIZE);
        for (i = sectcnt+1; i < TOTAL_DISK_SECTORS; i++) {
            fwrite(buf, 1, SECTOR_SIZE, flopf);
        }
    }

    /* that's it */

    return 0;
}

int main(void)
{
    FILE *bootf, *tosf, *flopf;

    bootf = fopen(BOOTNAME, "rb");
    if (bootf == 0)
        goto fail;
    tosf = fopen(TOSNAME, "rb");
    if (tosf == 0)
        goto fail;
    flopf = fopen(FLOPNAME, "wb");
    if (flopf == 0)
        goto fail;
    if (mkflop(bootf, tosf, flopf))
        goto fail;
    printf("done\n");
    exit(0);

fail:
    fprintf(stderr, "something failed.\n");
    exit(EXIT_FAILURE);
}
