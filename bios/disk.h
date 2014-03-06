/*
 * disk.h - disk routines
 *
 * Copyright (c) 2001-2014 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef DISK_H
#define DISK_H

#include "portab.h"

/* defines */

#define SECTOR_SIZE     512 /* standard for floppy, hard disk */
#define NUMFLOPPIES     2   /* max number of floppies supported */

#define ACSI_BUS            0
#define SCSI_BUS            1
#define IDE_BUS             2
#define SDMMC_BUS           3

#define MAX_BUS             SDMMC_BUS
#define DEVICES_PER_BUS     8

#define UNITSNUM            (NUMFLOPPIES+(DEVICES_PER_BUS*(MAX_BUS+1)))

#define GET_BUS(major)          ((major)/DEVICES_PER_BUS)
#define IS_ACSI_DEVICE(major)   (GET_BUS(major) == ACSI_BUS)
#define IS_SCSI_DEVICE(major)   (GET_BUS(major) == SCSI_BUS)
#define IS_IDE_DEVICE(major)    (GET_BUS(major) == IDE_BUS)
#define IS_SDMMC_DEVICE(major)  (GET_BUS(major) == SDMMC_BUS)

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

/* read/write flags */
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

extern UNIT units[];

/* physical disk functions */

LONG disk_inquire(UWORD unit, ULONG *blocksize, ULONG *deviceflags, char *productname, UWORD stringlen);
LONG disk_get_capacity(UWORD unit, ULONG *blocks, ULONG *blocksize);
LONG disk_rw(UWORD unit, UWORD rw, ULONG sector, UWORD count, void *buf);

/* xbios functions */

extern LONG DMAread(LONG sector, WORD count, LONG buf, WORD major);
extern LONG DMAwrite(LONG sector, WORD count, LONG buf, WORD major);

/* partition detection */

void disk_init_all(void);
LONG disk_mediach(UWORD unit);
void disk_rescan(int major);
void byteswap(UBYTE *buffer, ULONG size);

#endif /* DISK_H */
