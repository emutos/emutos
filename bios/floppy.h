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

#ifndef FLOPPY_H
#define FLOPPY_H

#include "portab.h"
 
/* bios functions */

/* Functions hdv_boot, hdv_init, rwabd, getbpb and mediach are 
 * called using variable pointers. The five flop_* functions
 * are the implementation of these provided by floppy.c.
 */

extern LONG flop_hdv_boot(void);
extern void flop_hdv_init(void);

/* xbios functions */

extern LONG floprd(LONG buf, LONG filler, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 
extern LONG flopwr(LONG buf, LONG filler, WORD dev, 
                   WORD sect, WORD track, WORD side, WORD count); 
extern LONG flopfmt(LONG buf, LONG filler, WORD dev, WORD spt,
                    WORD track, WORD side, WORD interleave, 
                    ULONG magic, WORD virgin); 
extern void protobt(LONG buf, LONG serial, WORD type, WORD exec);
extern LONG flopver(LONG buf, LONG filler, WORD dev, 
                    WORD sect, WORD track, WORD side, WORD count); 
extern LONG floprate(WORD dev, WORD rate);

#if CONF_WITH_FDC

/* internal functions */

extern void flopvbl(void);

#endif /* CONF_WITH_FDC */

/* lowlevel floppy_rwabs */
LONG floppy_rw(WORD rw, LONG buf, WORD cnt, LONG recnr, WORD spt, WORD sides, WORD dev);

#endif /* FLOPPY_H */
