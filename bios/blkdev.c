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

#define DBG_BLKDEV 1



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

    /* first setup all vectors for floppy A: and B: */
    for (drv=0; drv<=2; drv++) {
        blkdev[drv].getbpb = &flop_getbpb;
        blkdev[drv].rwabs = &flop_rwabs;
        blkdev[drv].mediach = &flop_mediach;
    }

    /* Then setup all vectors as dummy vectors - later harddisk */
    for (drv=2; drv<BLKDEVNUM; drv++) {
        blkdev[drv].getbpb = &dummy_getbpb;
        blkdev[drv].rwabs = &dummy_rwabs;
        blkdev[drv].mediach = &dummy_mediach;
    }

    /* setup booting related vectors */
    hdv_boot    = blkdev_hdv_boot;
    hdv_init    = blkdev_hdv_init;

    /* setup general block device vectors */
    hdv_bpb     = blkdev_getbpb;
    hdv_rw      = blkdev_rwabs;
    hdv_mediach = blkdev_mediach;

    /* floppy initialisation */
    floppy_init();

    /* harddisk initialisation */
    //disk_init();

}



/*
 * blkdev_hdv_init - BIOS boot vector
 *
 * For now just floppy...
 */

void blkdev_hdv_init(void)
{
    /* call the real */
    flop_hdv_init();
}



/*
 * blkdev_hdv_boot - BIOS boot vector
 */

LONG blkdev_hdv_boot(void)
{
    /* call hdv_init using the pointer - maybe vector is overloaded */
    hdv_init();                 	/* is flop_hdv_init in real... */

    return(flop_hdv_boot());


#if IMPLEMENTED
    int drv;                    	/* device counter for init */

    /* Loop through all devices, see, if they boot */
    for (drv=0; drv<BLKDEVNUM; drv++) {
        if (blkdev[drv].boot() >= 0 )   /* See, if device did boot */
            break;                      /* if yes, break */
    }
    return 0;
#endif
}



/*
 * blkdev_getbpb - Get BIOS parameter block vector
 */

LONG blkdev_getbpb(WORD dev)
{
    return blkdev[dev].getbpb(dev);
}



/*
 * blkdev_rwabs - BIOS block device dummy read/write vector
 */

LONG blkdev_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev)
{
    return blkdev[dev].rwabs(r_w, adr, numb, first, dev);
}



/*
 * blkdev_mediach - dummy BIOS media change vector
 */

LONG blkdev_mediach(WORD dev)
{
    return blkdev[dev].mediach(dev);
}



/*
 * dummy_getbpb - dummy get BIOS parameter block function
 */

LONG dummy_getbpb(WORD dev)
{
#if DBG_BLKDEV
    kprintf("BIOS: dummy_getbpb from drive %d\n", dev);
#endif
    return EUNDEV;  /* unknown device */
}



/*
 * dummy_rwabs - dummy BIOS block device dummy read/write function
 */

LONG dummy_rwabs(WORD r_w, LONG adr, WORD numb, WORD first, WORD dev)
{
#if DBG_BLKDEV
    kprintf("BIOS: dummy_rwabs from drive %d\n", dev);
#endif
    return EUNDEV;  /* unknown device */
}



/*
 * dummy_mediach - dummy BIOS media change function
 */

LONG dummy_mediach(WORD dev)
{
#if DBG_BLKDEV
    kprintf("BIOS: dummy_mediach from drive %d\n", dev);
#endif
    return EUNDEV;  /* unknown device */
}
