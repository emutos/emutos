/*
 * floppy.h - floppy routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef H_FLOPPY_
#define H_FLOPPY_

#include "portab.h"
 
/* bios functions */

/* Functions hdv_boot, hdv_init, rwabd, getbpb and mediach are 
 * called using variable pointers. The five flop_* functions
 * are the implementation of these provided by floppy.c.
 */

extern LONG flop_hdv_boot(void);
extern void flop_hdv_init(void);

/* xbios functions */

extern WORD floprd(LONG buf, LONG filler, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 
extern WORD flopwr(LONG buf, LONG filler, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 
extern WORD flopfmt(LONG buf, LONG filler, WORD dev, WORD spt,
                    WORD track, WORD side, WORD interleave, 
                    ULONG magic, WORD virgin); 
extern void protobt(LONG buf, LONG serial, WORD type, WORD exec);
extern WORD flopver(LONG buf, LONG filler, WORD dev, 
                    WORD sect, WORD track, WORD side, WORD count); 
extern WORD floprate(WORD dev, WORD rate);

/* internal functions */

/* call hdv_boot() and execute bootsector */
extern void do_hdv_boot(void);  

/* initialise floppy parameters */
extern void floppy_init(void);

/* get intel words */
UWORD getiword(UBYTE *addr);

/* lowlevel floppy_rwabs */
LONG floppy_rw(WORD rw, LONG buf, WORD cnt, LONG recnr, WORD spt, WORD sides, WORD dev);

#endif /* H_FLOPPY_ */
