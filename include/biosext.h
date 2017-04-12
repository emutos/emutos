/*
 * biosext.h - EmuTOS BIOS extensions not callable with trap
 *
 * Copyright (C) 2016-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSEXT_H
#define BIOSEXT_H

/* Boot flags */
extern UBYTE bootflags;
#define BOOTFLAG_EARLY_CLI     0x01
#define BOOTFLAG_SKIP_HDD_BOOT 0x02
#define BOOTFLAG_SKIP_AUTO_ACC 0x04

void invalidate_instruction_cache(void *start, long size);

#if CONF_WITH_SHUTDOWN
BOOL can_shutdown(void);
#endif

#endif /* BIOSEXT_H */
