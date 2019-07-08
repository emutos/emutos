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
struct font_head;

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

/* print a panic message both via kprintf and cprintf, then halt */
void panic(const char *fmt, ...) PRINTF_STYLE NORETURN;

/* halt the machine */
void halt(void) NORETURN;

#if CONF_WITH_SHUTDOWN
BOOL can_shutdown(void);
#endif

#if CONF_WITH_EXTENDED_MOUSE
extern void (*mousexvec)(WORD scancode);    /* Additional mouse buttons */
#endif

/* Line A extensions */
extern struct _mcs *mcs_ptr; /* ptr to mouse cursor save area in use */

/* determine monitor type, ... */
WORD get_monitor_type(void);
void get_pixel_size(WORD *width,WORD *height);
int rez_changeable(void);
WORD check_moderez(WORD moderez);

/* RAM-copies of the ROM-fontheaders. See bios/fntxxx.c */
extern struct font_head fon6x6;
extern struct font_head fon8x8;
extern struct font_head fon8x16;

/* Memory map information */
#if CONF_WITH_EXTENDED_MOUSE
BOOL is_text_pointer(void *p);
#endif

#endif /* BIOSEXT_H */
