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

/*
 * concatenate the files to create the floppy image
 *
 * returns 1 if error
 */
static int mkflop(FILE *bootf, FILE *tosf, FILE *flopf)
{
    uchar buf[SECTOR_SIZE];
    struct loader *b = (struct loader *)buf;
    size_t count;
    long n;
    unsigned short sectcnt;
    int i;

    /*
     * read bootsector
     */
    count = fread(buf, 1, SECTOR_SIZE, bootf);
    if (count < SECTOR_SIZE)
    {
        if (ferror(bootf))
        {
            fprintf(stderr,"Error: can't read %s\n",BOOTNAME);
            return 1;
        }
        memset(buf+count, 0, SECTOR_SIZE-count);
    }

    /*
     * compute size of tosf, and update sectcnt
     */
    n = -1;
    if (fseek(tosf, 0, SEEK_END) == 0)
    {
        n = ftell(tosf);
        if (fseek(tosf, 0, SEEK_SET) != 0)
            n = -1;
    }
    if (n < 0)
    {
        fprintf(stderr,"Error: can't determine size of %s\n",TOSNAME);
        return 1;
    }
    count = n;

    if (count > MAX_EMUTOS_SIZE)
    {
        fprintf(stderr, "Error: %s is %ld bytes too big to fit on the floppy\n",
                        TOSNAME, (long)count-MAX_EMUTOS_SIZE);
        return 1;
    }

    sectcnt = (count+SECTOR_SIZE-1) / SECTOR_SIZE;
    b->sectcnt[0] = sectcnt>>8;
    b->sectcnt[1] = sectcnt;

    /*
     * make bootsector bootable
     */
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

    /*
     * write out bootsector
     */
    if (fwrite(buf, 1, SECTOR_SIZE, flopf) != SECTOR_SIZE)
    {
        fprintf(stderr,"Error: can't write %s\n",FLOPNAME);
        return 1;
    }

    /*
     * copy emutos starting at sector 1
     */
    for (i = 0; i < sectcnt; i++) {
        count = fread(buf, 1, SECTOR_SIZE, tosf);
        if (count < SECTOR_SIZE) {
            if (ferror(tosf))
            {
                fprintf(stderr,"Error: can't read %s\n",TOSNAME);
                return 1;
            }
            memset(buf+count, 0, SECTOR_SIZE - count);
        }
        if (fwrite(buf, 1, SECTOR_SIZE, flopf) != SECTOR_SIZE)
        {
            fprintf(stderr,"Error: can't write %s\n",FLOPNAME);
            return 1;
        }
    }

    /*
     * fill the remainder of the disk with zeroes
     */
    if (fill) {
        memset(buf, 0, SECTOR_SIZE);
        for (i = sectcnt+1; i < TOTAL_DISK_SECTORS; i++) {
            if (fwrite(buf, 1, SECTOR_SIZE, flopf) != SECTOR_SIZE)
            {
                fprintf(stderr,"Error: can't write %s\n",FLOPNAME);
                return 1;
            }
        }
    }

    /* that's it */

    return 0;
}

int main(void)
{
    FILE *bootf, *tosf, *flopf;

    bootf = fopen(BOOTNAME, "rb");
    if (!bootf)
    {
        fprintf(stderr,"Error: can't open %s\n",BOOTNAME);
        exit(EXIT_FAILURE);
    }

    tosf = fopen(TOSNAME, "rb");
    if (!tosf)
    {
        fprintf(stderr,"Error: can't open %s\n",TOSNAME);
        exit(EXIT_FAILURE);
    }

    flopf = fopen(FLOPNAME, "wb");
    if (!flopf)
    {
        fprintf(stderr,"Error: can't open %s\n",FLOPNAME);
        exit(EXIT_FAILURE);
    }

    if (mkflop(bootf, tosf, flopf))
    {
        exit(EXIT_FAILURE);     /* message already issued */
    }

    printf("done\n");
    exit(0);
}
