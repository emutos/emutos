/*
 * blkdev.h - bios block devices
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BLKDEV_H
#define _BLKDEV_H

#include "portab.h"
#include "bios.h"


/*
 * defines
 */

#define RWABS_RETRIES   1   /* on real machine might want to increase this */

#define BLKDEVNUM   16  /* A: .. P: */
#define UNITSNUM    23  /* 2xFDC, 8xACSI, 8xSCSI, 4xIDE, 1xARAnyM */

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

#if 0   /* defined in bios.h */
struct _bpb /* bios parameter block */
{
        WORD    recsiz;         /* sector size in bytes */
        WORD    clsiz;          /* cluster size in sectors */
        WORD    clsizb;         /* cluster size in bytes */
        WORD    rdlen;          /* root directory length in records */
        WORD    fsiz;             /* fat size in records */
        WORD    fatrec;         /* first fat record (of last fat) */
        WORD    datrec;         /* first data record */
        WORD    numcl;          /* number of data clusters available */
        WORD    b_flags;
};
#endif

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
#define RW_NOTRANSLATE          8



/*
 * Prototypes
 */

/* Init block device vectors */
void blkdev_init(void);

/* general block device functions */
void blkdev_hdv_init(void);
LONG blkdev_hdv_boot(void);
LONG blkdev_getbpb(WORD dev);
LONG blkdev_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev, LONG lfirst);
LONG blkdev_mediach(WORD dev);
UWORD compute_cksum(LONG buf);




/*
 * Modes of block devices
 */

# define BDM_WP_MODE            0x01    /* write-protect bit (soft) */
# define BDM_WB_MODE            0x02    /* write-back bit (soft) */
# define BDM_REMOVABLE          0x04    /* removable media */
# define BDM_LOCKABLE           0x08    /* lockable media */
# define BDM_LRECNO             0x10    /* lrecno supported */
# define BDM_WP_HARD            0x20    /* write-protected partition */


/* unified block device identificator - partitially stolen from MiNT, hehe */

struct _blkdev
{
#if EVER_NEEDED /* take it, if you need... */
    UWORD       major;          /* XHDI */
    UWORD       minor;          /* XHDI */
    UWORD       mode;           /* some flags */

    UWORD       lock;           /* device in use */

    char        id[4];          /* XHDI partition id (GEM, BGM, RAW, \0D6, ...) */
    UWORD       key;            /* XHDI key */
#endif /* EVER_NEEDED */

    ULONG       start;          /* physical start sector */
    ULONG       size;           /* physical sectors */

    UWORD       valid;          /* device valid */
        BPB     bpb;
        GEOMETRY    geometry;   /* this should probably belong to devices */
    BYTE    serial[3];  /* the serial number taken from the bootsector */
        int             unit;           /* 0,1 = floppies, 2-9 = ACSI, 10-17 = SCSI, 18-21 = IDE */
};
typedef struct _blkdev  BLKDEV;

/* an idea how to assign partitions to a physical unit */
typedef struct _partition       PARTITION;
struct _partition
{
        BLKDEV          *blkdev;
        PARTITION       *next;
};

/* physical unit (floppy/harddisk) identificator */
struct _unit
{
    int     valid;      /* unit valid */
    ULONG       size;           /* number of physical sectors */
    ULONG       pssize;         /* physical sector size */
    LONG    last_access;/* used in mediach only */
    PARTITION *partitions;

};
typedef struct _unit UNIT;


#endif /* _BLKDEV_H */

