/*
 * bdosext.h - EmuTOS BDOS extensions not callable with trap
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BDOSEXT_H
#define _BDOSEXT_H

#include <pd.h>

/* BDOS initialization entry points. Called by BIOS. */
extern void osinit_before_xmaddalt(void);
extern void osinit_after_xmaddalt(void);

#if CONF_WITH_ALT_RAM
/* declare additional memory */
long xmaddalt(UBYTE *start, long size);
long total_alt_ram(void);
#endif /* CONF_WITH_ALT_RAM */

/* BDOS quick pool, referenced by BIOS OSHEADER */
#define MAXQUICK 5
extern WORD *root[MAXQUICK];

/* Pointer to the basepage of the current process.
 * Referenced by BIOS OSHEADER and bios/kprint.c */
extern PD *run;

/* BDOS date/time, synchronized from XBIOS Settime() */
extern UWORD current_time, current_date;

/* Bitmap of removable logical drives. Updated by BIOS disk_init_all() */
extern LONG drvrem;

#endif /* _BDOSEXT_H */
