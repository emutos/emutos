/*
 * blkdev.h - bios block devices
 *
 * Copyright (c) 2001-2014 The EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BLKDEV_H
#define BLKDEV_H

#include "portab.h"
#include "bpb.h"


/*
 * defines
 */
#define SECTOR_SIZE     512 /* standard for floppy, hard disk */
#define MAX_FAT12_CLUSTERS  4078    /* architectural constants */
#define MAX_FAT16_CLUSTERS  65518

#define RWABS_RETRIES   1   /* on real machine might want to increase this */

#define NUMFLOPPIES     2   /* max number of floppies supported */

#define ACSI_BUS            0
#define SCSI_BUS            1
#define IDE_BUS             2
#define SDMMC_BUS           3

#define MAX_BUS             SDMMC_BUS
#define DEVICES_PER_BUS     8
#define UNITSNUM            (NUMFLOPPIES+(DEVICES_PER_BUS*(MAX_BUS+1)))

#define GET_BUS(n)          ((n)/DEVICES_PER_BUS)
#define IS_ACSI_DEVICE(n)   (GET_BUS(n) == ACSI_BUS)
#define IS_SCSI_DEVICE(n)   (GET_BUS(n) == SCSI_BUS)
#define IS_IDE_DEVICE(n)    (GET_BUS(n) == IDE_BUS)
#define IS_SDMMC_DEVICE(n)  (GET_BUS(n) == SDMMC_BUS)


/*
 * commands used for internal xxx_ioctl() calls
 */
#define GET_DISKINFO        20  /* get disk info for specified drive: */
                                /* arg -> array of two ULONGS:        */
                                /*   [0] capacity (in sectors)        */
                                /*   [1] sector size (in bytes)       */
#define GET_DISKNAME        21  /* get name of specified drive:       */
                                /* arg -> return data (max 40 chars)  */
#define GET_MEDIACHANGE     30  /* return status as per Mediach() call*/
                                /* arg is NULL                        */


/* Original FAT12 bootsector */
struct bs {
  /*   0 */  UBYTE bra[2];
  /*   2 */  UBYTE loader[6];
  /*   8 */  UBYTE serial[3];
  /*   b */  UBYTE bps[2];    /* bytes per sector */
  /*   d */  UBYTE spc;       /* sectors per cluster */
  /*   e */  UBYTE res[2];    /* number of reserved sectors */
  /*  10 */  UBYTE fat;       /* number of FATs */
  /*  11 */  UBYTE dir[2];    /* number of DIR root entries */
  /*  13 */  UBYTE sec[2];    /* total number of sectors */
  /*  15 */  UBYTE media;     /* media descriptor */
  /*  16 */  UBYTE spf[2];    /* sectors per FAT */
  /*  18 */  UBYTE spt[2];    /* sectors per track */
  /*  1a */  UBYTE sides[2];  /* number of sides */
  /*  1c */  UBYTE hid[2];    /* number of hidden sectors */
  /*  1e */  UBYTE data[0x1e0];
  /* 1fe */  UBYTE cksum[2];
};

/* Extended FAT12/FAT16 bootsector */
struct fat16_bs {
  /*   0 */  UBYTE bra[2];
  /*   2 */  UBYTE loader[6];
  /*   8 */  UBYTE serial[3];
  /*   b */  UBYTE bps[2];    /* bytes per sector */
  /*   d */  UBYTE spc;       /* sectors per cluster */
  /*   e */  UBYTE res[2];    /* number of reserved sectors */
  /*  10 */  UBYTE fat;       /* number of FATs */
  /*  11 */  UBYTE dir[2];    /* number of DIR root entries */
  /*  13 */  UBYTE sec[2];    /* total number of sectors */
  /*  15 */  UBYTE media;     /* media descriptor */
  /*  16 */  UBYTE spf[2];    /* sectors per FAT */
  /*  18 */  UBYTE spt[2];    /* sectors per track */
  /*  1a */  UBYTE sides[2];  /* number of sides */
  /*  1c */  UBYTE hid[4];    /* number of hidden sectors (earlier: 2 bytes) */
  /*  20 */  UBYTE sec2[4];   /* total number of sectors (if not in sec) */
  /*  24 */  UBYTE ldn;       /* logical drive number */
  /*  25 */  UBYTE dirty;     /* dirty filesystem flags */
  /*  26 */  UBYTE ext;       /* extended signature */
  /*  27 */  UBYTE serial2[4]; /* extended serial number */
  /*  2b */  UBYTE label[11]; /* volume label */
  /*  36 */  UBYTE fstype[8]; /* file system type */
  /*  3e */  UBYTE data[0x1c0];
  /* 1fe */  UBYTE cksum[2];
};


struct _pun_info
{
        unsigned short  puns;                   /* Number of HD's */
        unsigned char   pun [16];               /* AND with masks below: */

# define PUN_DEV        0x1f                    /* device number of HD */
# define PUN_UNIT       0x7                     /* Unit number */
# define PUN_SCSI       0x8                     /* 1=SCSI 0=ACSI */
# define PUN_IDE        0x10                    /* Falcon IDE */
# define PUN_REMOVABLE  0x40                    /* Removable media */
# define PUN_VALID      0x80                    /* zero if valid */

        long            partition_start [16];
        long            cookie;                 /* 'AHDI' if following valid */
        long            *cookie_ptr;            /* Points to 'cookie' */
        unsigned short  version_num;            /* AHDI version */
        unsigned short  max_sect_siz;           /* Max logical sec size */
        long            reserved[16];           /* Reserved */
};

typedef struct _pun_info PUN_INFO;


struct _geometry        /* disk parameter block */
{
    WORD spt;          /* number of sectors per track */
    WORD sides;        /* number of sides */
};

typedef struct _geometry GEOMETRY;


#define RW_READ             0
#define RW_WRITE            1
/* bit masks */
#define RW_RW               1
#define RW_NOMEDIACH        2
#define RW_NORETRIES        4
#define RW_NOTRANSLATE      8

/*
 *  return codes
 */

#define DEVREADY        -1L             /*  device ready                */
#define DEVNOTREADY     0L              /*  device not ready            */
#define MEDIANOCHANGE   0L              /*  media def has not changed   */
#define MEDIAMAYCHANGE  1L              /*  media may have changed      */
#define MEDIACHANGE     2L              /*  media def has changed       */





/*
 * Prototypes
 */

/* Init block device vectors */
void blkdev_init(void);

/* general block device functions */
LONG blkdev_hdv_boot(void);
LONG blkdev_getbpb(WORD dev);
LONG blkdev_drvmap(void);
LONG blkdev_avail(WORD dev);
WORD get_shift(ULONG blocksize);

int add_partition(int dev, LONG *devices_available, char id[], ULONG start, ULONG size);


/*
 * Modes of block devices
 */

#define BDM_WP_MODE            0x01    /* write-protect bit (soft) */
#define BDM_WB_MODE            0x02    /* write-back bit (soft) */
#define BDM_REMOVABLE          0x04    /* removable media */
#define BDM_LOCKABLE           0x08    /* lockable media */
#define BDM_LRECNO             0x10    /* lrecno supported */
#define BDM_WP_HARD            0x20    /* write-protected partition */


/* unified block device identificator - partitially stolen from MiNT, hehe */

struct _blkdev
{
    char        id[4];          /* XHDI partition id (GEM, BGM, RAW, \0D6, ...) */
    ULONG       start;          /* physical start sector */
    ULONG       size;           /* physical sectors */
    UBYTE       valid;          /* device valid */
    UBYTE       mediachange;    /* current mediachange status */
    BPB         bpb;
    GEOMETRY    geometry;       /* this should probably belong to units */
    UBYTE       serial[3];      /* the serial number taken from the bootsector */
    UBYTE       serial2[4];     /* serial number 2 (MSDOS) from the bootsector */
    int         unit;           /* 0,1 = floppies, 2-9 = ACSI, 10-17 = SCSI, */
                                /*  18-25 = IDE, 26-33 = SD/MMC              */
};
typedef struct _blkdev  BLKDEV;

/* physical unit (floppy/harddisk) identificator */
struct _unit
{
    BYTE    valid;          /* unit valid */
    BYTE    byteswap;       /* unit is byteswapped */
    ULONG   size;           /* number of physical sectors */
    WORD    psshift;        /* shift left amount to convert sectors to bytes */
    LONG    last_access;    /* used in mediach only */
    LONG    drivemap;       /* bitmap of logical devices on this physical unit */
    UBYTE   features;       /* see below */
#define UNIT_REMOVABLE  0x80    /* unit uses removable media */
    UBYTE   status;         /* see below */
#define UNIT_CHANGED    0x01    /* 0 => physical media has not changed */
};
typedef struct _unit UNIT;

/*
 * defined in blkdev.c, also used in floppy.c
 */

extern BLKDEV blkdev[];
extern UNIT units[];


#endif /* BLKDEV_H */
