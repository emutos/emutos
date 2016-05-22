/*
 * bios.h - misc BIOS function prototypes
 *
 * Copyright (C) 2011-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOS_H
#define BIOS_H

void biosmain(void) NORETURN;
LONG bios_do_unimpl(WORD number);

/* misc BIOS functions */
LONG bconstat(WORD handle);
LONG bconin(WORD handle);
LONG bconout(WORD handle, WORD what);
LONG lrwabs(WORD r_w, UBYTE *adr, WORD numb, WORD first, WORD drive, LONG lfirst);
LONG setexc(WORD num, LONG vector);
LONG tickcal(void);
LONG getbpb(WORD drive);
LONG bcostat(WORD handle);
LONG mediach(WORD drv);
LONG drvmap(void);

/* utility functions */
#if CONF_SERIAL_CONSOLE_ANSI
void bconout_str(WORD handle, const char* str);
#endif

#endif /* BIOS_H */
