/*
 * biosext.h - EmuTOS BIOS extensions not callable with trap
 *
 * Copyright (C) 2016-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSEXT_H
#define BIOSEXT_H

/* Forward declarations */
struct _mcs;

/* Bitmap of removable logical drives */
extern LONG drvrem;

/* Boot flags */
extern UBYTE bootflags;
#define BOOTFLAG_EARLY_CLI     0x01
#define BOOTFLAG_SKIP_HDD_BOOT 0x02
#define BOOTFLAG_SKIP_AUTO_ACC 0x04

ULONG initial_vram_size(void);
void flush_data_cache(void *start, long size);
void invalidate_data_cache(void *start, long size);
void invalidate_instruction_cache(void *start, long size);

#if CONF_WITH_SHUTDOWN
BOOL can_shutdown(void);
#endif

extern void (*mousexvec)(WORD scancode);    /* Additional mouse buttons */

/* Line A extensions */
extern struct _mcs *mcs_ptr; /* ptr to mouse cursor save area in use */

#endif /* BIOSEXT_H */
