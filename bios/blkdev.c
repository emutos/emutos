/*
 * blkdev.c - BIOS character device funtions
 *
 * Copyright (c) 2001 by
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "bios.h"
#include "gemerror.h"
#include "kprint.h"
#include "tosvars.h"
#include "floppy.h"
//#include "disk.h"
#include "blkdev.h"

/*
 * Defines
 */

#define DBGBIOSC TRUE



/*
 * Prototypes
 */

/* dummy block device functions */
LONG dummy_getbpb(WORD dev);
LONG dummy_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev);
LONG dummy_mediach(WORD dev);



/*
 * Global variables
 */

BLKDEV blkdev[BLKDEVNUM];     /* enough for now? */



/*
 * blkdevs_init - BIOS block drive initialization
 *
 * Called from bios.c, this routine will do necessary block device
 * initialization.
 */

void blkdev_init(void)
{
    int drv;                    /* device counter for init */

    /* first setup all vectors as dummy vectors */
    for (drv=0; drv<BLKDEVNUM; drv++) {
        blkdev[drv].getbpb = &dummy_getbpb;
        blkdev[drv].rwabs = &dummy_rwabs;
        blkdev[drv].mediach = &dummy_mediach;
    }

    /* setup all the bios vectors */
    hdv_boot    = blkdev_boot;
    hdv_init    = blkdev_init;
    hdv_bpb     = blkdev_getbpb;
    hdv_rw      = blkdev_rwabs;
    hdv_mediach = blkdev_mediach;

    /* floppy initialisation */
    floppy_init();

    /* harddisk initialisation */
    //disk_init();

}



/*
 * blkdev_boot - BIOS boot vector
 */

LONG blkdev_boot(void)
{
    int drv;                    /* device counter for init */

    /* first setup all vectors as dummy vectors */
    for (drv=0; drv<BLKDEVNUM; drv++) {
#if NOTYET
        if (blkdev[drv].boot() >= 0 )
            break;
#endif
    }
    return 0;
}



/*
 * blkdev_getbpb - Get BIOS parameter block function
 */

LONG blkdev_getbpb(WORD dev)
{
    return blkdev[dev].getbpb(dev);
}



/*
 * blkdev_rwabs - BIOS block device dummy read/write function
 */

LONG blkdev_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev)
{
    return blkdev[dev].rwabs(r_w, adr, numb, first, dev);
}



/*
 * blkdev_mediach - dummy BIOS media change function
 */

LONG blkdev_mediach(WORD dev)
{
    return blkdev[dev].mediach(dev);
}



/*
 * dummy_boot - dummy BIOS boot
 */

LONG dummy_boot(WORD dev)
{
    return EUNDEV;  /* unknown device */
}



/*
 * dummy_init - dummy BIOS device init function
 */

LONG dummy_init(WORD dev)
{
    return EUNDEV;  /* unknown device */
}



/*
 * dummy_getbpb - dummy get BIOS parameter block function
 */

LONG dummy_getbpb(WORD dev)
{
    return EUNDEV;  /* unknown device */
}



/*
 * dummy_rwabs - dummy BIOS block device dummy read/write function
 */

LONG dummy_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev)
{
    return EUNDEV;  /* unknown device */
}



/*
 * dummy_mediach - dummy BIOS media change function
 */

LONG dummy_mediach(WORD dev)
{
    return EUNDEV;  /* unknown device */
}
