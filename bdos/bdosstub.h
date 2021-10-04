/*
 * bdosstub.h - BDOS entry points, called by BIOS
 *
 * Copyright (C) 2019-2021 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BDOSSTUB_H
#define _BDOSSTUB_H

#include "bdosdefs.h"

/* Boot flags.
 * TODO: this should be private to the BDOS
 */
extern UBYTE bootflags;
#define BOOTFLAG_EARLY_CLI     0x01
#define BOOTFLAG_SKIP_HDD_BOOT 0x02
#define BOOTFLAG_SKIP_AUTO_ACC 0x04

/* Start the BDOS */
void bdos_bootstrap(void);

/* Pointer to the basepage of the current process.
 * Declared here because referenced by the BIOS OSHEADER,
 * and also by bios/kprint.c
 * TODO: this is dirty as it allows the BIOS to access data from an upper layer
 */
extern PD *run;

/* BDOS current date/time.
 * Declared here because also updated by XBIOS Settime()
 * TODO: this is dirty as it allows the BIOS to access data from an upper layer
 */
extern UWORD current_date, current_time;

#endif /* _BDOSSTUB_H */
