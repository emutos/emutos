/*
 * mkflop.c - create an auto-booting EmuTOS floppy.
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * this tool will create a simple auto-booting FAT12 floppy
 * called emutos.st from bootsect.img and ramtos.img.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * manifest constants
 */
#define SECTOR_SIZE         512L
#define ENTRY_SIZE          32      /* size of directory entry */
#define BOOTABLE_CHECKSUM   0x1234

/*
 * input & output filenames
 */
const char *bootname;
const char *tosname;
const char *flopname;

/*
 * the following values are used for the root dir entry of
 * the created filesystem
 */
#define FILE_NAME   "EMUTOS  SYS"   /* must be in disk format! */
#define FILE_ATTR   0x06            /* SYSTEM and HIDDEN */
#define FILE_CLUS   2               /* starting cluster */

typedef unsigned char uchar;
typedef unsigned short uword;
typedef unsigned long ulong;

struct loader {
    uchar pad[11];
    uchar bps[2];       /* bytes per sector */
    uchar spc;          /* sectors per cluster */
    uchar res[2];       /* reserved sectors */
    uchar fat;          /* number of FATs */
    uchar dir[2];       /* root dir entries */
    uchar sec[2];       /* total number of sectors */
    uchar media;        /* media descriptor */
    uchar spf[2];       /* sectors per FAT */
    uchar spt[2];       /* sectors per track */
    uchar sides[2];     /* number of sides */
    uchar hid[2];       /* number of hidden sectors */
                    /* the following values must be big-endian, since   */
                    /* they are used for communication with the loader  */
    uchar ssect[2];     /* starting sector */
    uchar sectcnt[2];   /* number of sectors */
    uchar ldaddr[4];
};

struct dir_entry {
    uchar name[11];     /* filename */
    uchar attrib;       /* attribute */
    uchar fill[10];
    uchar time[2];      /* DOS time & date */
    uchar date[2];
    uchar clus[2];      /* starting cluster */
    uchar length[4];    /* file length */
};


/*
 * routines to convert to & from little-endian ('intel') values
 */
static uword getiword(uchar *b)
{
    return 256 * b[1] + b[0];
}

static void putiword(uchar *b,uword n)
{
    b[0] = n & 0xff;
    b[1] = (n >> 8) & 0xff;
}

static void putilong(uchar *b,ulong n)
{
    b[0] = n & 0xff;
    b[1] = (n >> 8) & 0xff;
    b[2] = (n >> 16) & 0xff;
    b[3] = (n >> 24) & 0xff;
}

/*
 * routines to convert to & from big-endian ('motorola') values
 */
static uword getmword(uchar *b)
{
    return 256 * b[0] + b[1];
}

static void putmword(uchar *b,uword n)
{
    b[0] = (n >> 8) & 0xff;
    b[1] = n & 0xff;
}

/*
 * get size of file
 *
 * return -1 iff error
 */
static long get_filesize(FILE *fp)
{
    long n;

    if (fseek(fp,0,SEEK_END) != 0)
        return -1L;

    n = ftell(fp);
    if (n < 0)
        return -1L;

    if (fseek(fp,0,SEEK_SET) != 0)
        return -1L;

    return n;
}

/*
 * make bootsector bootable, then write it
 *
 * return -1 iff error
 */
static int write_bootsec(FILE *fp,uchar *bootbuf)
{
    int i;
    uword sum;

    /* first, make bootsector bootable */
    for (i = 0, sum = 0; i < (SECTOR_SIZE/2-1); i++)
        sum += getmword(bootbuf+2*i);
    putmword(bootbuf+SECTOR_SIZE-2,BOOTABLE_CHECKSUM-sum);

    /* then write it */
    if (fwrite(bootbuf,1,SECTOR_SIZE,fp) != SECTOR_SIZE)
        return -1;

    return 0;
}

/*
 * create 12-bit FAT entry
 *
 * inserts value 'fatval' into entry 'fatnum' of 'fat'
 * assumes 'fatval' <= 0xfff
 */
static void set_fat_entry(uchar *fat,uword fatnum,uword fatval)
{
    uword i = 3 * fatnum / 2;

    if (fatnum&1)       /* odd-numbered entries */
    {
        fat[i] |= (fatval & 0x0f) << 4;
        fat[i+1] = fatval >> 4;
    } else {            /* even-numbered */
        fat[i] = fatval & 0xff;
        fat[i+1] |= fatval >> 8;
    }
}

/*
 * write FATs
 *
 * return -1 iff error
 */
static int write_fats(FILE *fp,struct loader *b,uword clusters)
{
    uchar *fatbuf;
    int i, rc = 0;
    unsigned int fatlen;

    fatlen = getiword(b->spf) * SECTOR_SIZE;
    fatbuf = malloc(fatlen);
    if (!fatbuf)
        return -1;
    memset(fatbuf,0x00,fatlen);

    set_fat_entry(fatbuf,0,0xff9);      /* put standard values in first 2 entries */
    set_fat_entry(fatbuf,1,0xfff);

    for (i = 0; i < clusters-1; i++)
        set_fat_entry(fatbuf,i+2,i+3);  /* n+1 is the next entry in the chain after n */
    set_fat_entry(fatbuf,i+2,0xfff);    /* terminate the chain */ 

    /* write the fats */
    for (i = 0; i < b->fat; i++)
    {
        if (fwrite(fatbuf,1,fatlen,fp) != fatlen)
        {
            rc = -1;
            break;
        }
    }

    free(fatbuf);

    return rc;
}

/*
 * get TOS-compatible date/time
 *
 * returns current local date & time
 */
static void get_time(uword *curdate,uword *curtime)
{
    time_t now;
    struct tm *t;

    now = time(NULL);
    t = localtime(&now);

    *curdate = (uword)(((t->tm_year-80)<<9) + ((t->tm_mon+1)<<5) + t->tm_mday);
    *curtime = (uword)((t->tm_hour<<11) + (t->tm_min<<5) + t->tm_sec/2);
}

/*
 * write root dir
 *
 * return -1 iff error
 */
static int write_root(FILE *fp,struct loader *b,long file_size)
{
    uchar *rootbuf;
    struct dir_entry *entry;
    int rc = 0;
    long n;
    uword rootlen, d, t;

    n = (getiword(b->dir)*ENTRY_SIZE + SECTOR_SIZE - 1) / SECTOR_SIZE;
    rootlen = n * SECTOR_SIZE;

    rootbuf = malloc(rootlen);
    if (!rootbuf)
        return -1;
    memset(rootbuf,0x00,rootlen);

    get_time(&d,&t);

    /* fill in first root dir entry */
    entry = (struct dir_entry *)rootbuf;
    memcpy(entry->name,FILE_NAME,11);
    entry->attrib = FILE_ATTR;
    putiword(entry->time,t);
    putiword(entry->date,d);
    putiword(entry->clus,FILE_CLUS);
    putilong(entry->length,file_size);

    if (fwrite(rootbuf,1,rootlen,fp) != rootlen)
        rc = -1;

    free(rootbuf);

    return rc;
}

/*
 * create the floppy image
 *
 * returns 1 if error
 */
static int mkflop(FILE *bootf, FILE *tosf, FILE *flopf)
{
    uchar bootbuf[SECTOR_SIZE], buf[SECTOR_SIZE];
    struct loader *b = (struct loader *)bootbuf;
    size_t count;
    long n;
    uword startsec, sectcnt, clusters;
    int i;

    /*
     * read bootsector
     */
    count = fread(bootbuf, 1, SECTOR_SIZE, bootf);
    if (count < SECTOR_SIZE)
    {
        if (ferror(bootf))
        {
            fprintf(stderr,"mkflop error: can't read %s\n",bootname);
            return 1;
        }
        memset(bootbuf+count, 0, SECTOR_SIZE-count);
    }

    /*
     * compute file system parameters, and update starting sector
     */
    startsec = getiword(b->res) + b->fat*getiword(b->spf)
                + ((getiword(b->dir)*ENTRY_SIZE)+SECTOR_SIZE-1)/SECTOR_SIZE;
    putmword(b->ssect,startsec);    /* make it big-endian */

    /*
     * compute size of tosf, and update sector count
     */
    n = get_filesize(tosf);
    if (n < 0)
    {
        fprintf(stderr,"mkflop error: can't determine size of %s\n",tosname);
        return 1;
    }
    count = n;

    n = count - (getiword(b->sec)-startsec) * SECTOR_SIZE;  /* calculate excess if any */
    if (n > 0)
    {
        fprintf(stderr,"mkflop error: %s is %ld bytes too big to fit on the floppy\n",tosname,n);
        return 1;
    }

    sectcnt = (count+SECTOR_SIZE-1) / SECTOR_SIZE;
    putmword(b->sectcnt,sectcnt);   /* make it big-endian */
    clusters = (sectcnt+b->spc-1) / b->spc;

    /*
     * write out bootsector, FATs, root dir
     */
    if (write_bootsec(flopf,bootbuf) < 0)
    {
        fprintf(stderr,"mkflop error: can't write bootsector to %s\n",flopname);
        return 1;
    }
    if (write_fats(flopf,b,clusters) < 0)
    {
        fprintf(stderr,"mkflop error: can't write FATs to %s\n",flopname);
        return 1;
    }
    if (write_root(flopf,b,count) < 0)
    {
        fprintf(stderr,"mkflop error: can't write root dir to %s\n",flopname);
        return 1;
    }

    /*
     * copy emutos image
     */
    for (i = 0; i < sectcnt; i++) {
        count = fread(buf, 1, SECTOR_SIZE, tosf);
        if (count < SECTOR_SIZE) {
            if (ferror(tosf))
            {
                fprintf(stderr,"mkflop error: can't read %s\n",tosname);
                return 1;
            }
            memset(buf+count, 0, SECTOR_SIZE - count);
        }
        if (fwrite(buf, 1, SECTOR_SIZE, flopf) != SECTOR_SIZE)
        {
            fprintf(stderr,"mkflop error: can't write EmuTOS image to %s\n",flopname);
            return 1;
        }
    }

    /*
     * fill the remainder of the disk with zeros
     */
    memset(buf, 0, SECTOR_SIZE);
    for (i = startsec+sectcnt; i < getiword(b->sec); i++) {
        if (fwrite(buf, 1, SECTOR_SIZE, flopf) != SECTOR_SIZE)
        {
            fprintf(stderr,"mkflop error: can't write zeros to %s\n",flopname);
            return 1;
        }
    }

    /* that's it */

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *bootf, *tosf, *flopf;

    if (argc != 4)
    {
        fprintf(stderr, "usage: %s <bootsect.img> <ramtos.img> <floppy.st>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    bootname = argv[1];
    tosname = argv[2];
    flopname = argv[3];

    bootf = fopen(bootname, "rb");
    if (!bootf)
    {
        fprintf(stderr,"mkflop error: can't open %s\n",bootname);
        exit(EXIT_FAILURE);
    }

    tosf = fopen(tosname, "rb");
    if (!tosf)
    {
        fprintf(stderr,"mkflop error: can't open %s\n",tosname);
        exit(EXIT_FAILURE);
    }

    flopf = fopen(flopname, "wb");
    if (!flopf)
    {
        fprintf(stderr,"mkflop error: can't open %s\n",flopname);
        exit(EXIT_FAILURE);
    }

    if (mkflop(bootf, tosf, flopf))
    {
        exit(EXIT_FAILURE);     /* message already issued */
    }

    printf("mkflop done\n");
    exit(0);
}
