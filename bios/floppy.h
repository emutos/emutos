/*
 * floppy.h - floppy routines
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FLOPPY_H
#define FLOPPY_H

/* bios functions */

/* Functions hdv_init, rwabs, getbpb and mediach are
 * called using variable pointers. The flop_* functions
 * are the implementation of these provided by floppy.c.
 */

void flop_hdv_init(void);
WORD flop_boot_read(void);
LONG flop_mediach(WORD dev);

/* xbios functions */

LONG floprd(UBYTE *buf, LONG filler, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count);
LONG flopwr(const UBYTE *buf, LONG filler, WORD dev,
                   WORD sect, WORD track, WORD side, WORD count);
LONG flopfmt(UBYTE *buf, WORD *skew, WORD dev, WORD spt,
                    WORD track, WORD side, WORD interleave,
                    ULONG magic, WORD virgin);
void protobt(UBYTE *buf, LONG serial, WORD type, WORD exec);
LONG flopver(WORD *buf, LONG filler, WORD dev,
                    WORD sect, WORD track, WORD side, WORD count);
LONG floprate(WORD dev, WORD rate);

/* internal functions */

void flop_checksum(int floppy, UBYTE *buf);

#if CONF_WITH_FDC

void flopvbl(void);

#endif /* CONF_WITH_FDC */

/* lowlevel floppy_rwabs */
LONG floppy_rw(WORD rw, UBYTE *buf, WORD cnt, LONG recnr, WORD spt, WORD sides, WORD dev);

#endif /* FLOPPY_H */
