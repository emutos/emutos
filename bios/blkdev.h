/*
 * chardev.h - bios devices
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BLKDEV_H
#define _BLKDEV_H

#include        "portab.h"


/*
 * defines
 */

#define BLKDEVNUM 16



/*
 * Prototypes
 */

/* Init block device vectors */
void blkdev_init(void);

/* general block device functions */
void blkdev_hdv_init(void);
LONG blkdev_hdv_boot(void);
LONG blkdev_getbpb(WORD dev);
LONG blkdev_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev);
LONG blkdev_mediach(WORD dev);



/*
 * Modes of block devices
 */

# define BDM_WP_MODE		0x01	/* write-protect bit (soft) */
# define BDM_WB_MODE		0x02	/* write-back bit (soft) */
# define BDM_REMOVABLE		0x04	/* removable media */
# define BDM_LOCKABLE		0x08	/* lockable media */
# define BDM_LRECNO		0x10	/* lrecno supported */
# define BDM_WP_HARD		0x20	/* write-protected partition */


/* unified block device identificator - partitially stolen from MiNT, hehe */

typedef struct blkdev	BLKDEV;
struct blkdev
{
#if EVER_NEEDED /* take it, if you need... */
    UWORD	major;		/* XHDI */
    UWORD	minor;		/* XHDI */
    UWORD	mode;		/* some flags */

    ULONG	start;		/* physical start sector */
    ULONG	size;		/* physical sectors */
    ULONG	pssize;		/* physical sector size */

    UWORD	valid;		/* device valid */
    UWORD	lock;		/* device in use */

    char	id[4];		/* XHDI partition id (GEM, BGM, RAW, \0D6, ...) */
    UWORD	key;		/* XHDI key */
#endif /* EVER_NEEDED */

    LONG	(*boot)         (WORD dev);
    LONG	(*init)		(WORD dev);
    LONG 	(*getbpb)	(WORD dev);
    LONG	(*rwabs)        (WORD r_w, LONG adr, WORD numb,
                                 WORD first, WORD dev);
    LONG	(*mediach)      (WORD dev);
};



#endif /* _BLKDEV_H */

