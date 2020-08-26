/*
 * biosext.h - EmuTOS BIOS extensions not callable with trap
 *
 * Copyright (C) 2016-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSEXT_H
#define BIOSEXT_H

/*
 * Size of ST-RAM disk buffer pointed to by _dskbufp
 *
 * According to Atari documentation, this is at least two sectors in size.
 * In EmuTOS's implementation, this must be large enough to handle the
 * sectors read by flop_mediach() in order to detect floppy media change.
 * CHKSUM_SECTORS specifies the number of sectors used for that; the value
 * is (1 + the maximum FAT size).
 */
#define CHKSUM_SECTORS  6

#if CHKSUM_SECTORS > 2
# define DSKBUF_SECS     CHKSUM_SECTORS
#else
# define DSKBUF_SECS    2
#endif
#define DSKBUF_SIZE     (DSKBUF_SECS * SECTOR_SIZE)

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

#if CONF_WITH_CACHE_CONTROL
WORD cache_exists(void);
void set_cache(WORD enable);
#endif

/* bios allocation of ST-RAM */
UBYTE *balloc_stram(ULONG size, BOOL top);

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
extern UBYTE v_planes_shift; /* pixel to address helper */

/* determine monitor type, ... */
WORD get_monitor_type(void);
WORD get_palette(void);
void get_pixel_size(WORD *width,WORD *height);
int rez_changeable(void);
WORD check_moderez(WORD moderez);

/* RAM-copies of the ROM-fontheaders. See bios/fntxxx.c */
extern struct font_head fon6x6;
extern struct font_head fon8x8;
extern struct font_head fon8x16;

/* Memory map information */
#if CONF_WITH_EXTENDED_MOUSE
BOOL is_text_pointer(const void *p);
#endif

/* VIDEL routines */
WORD get_videl_mode(void);
#ifdef MACHINE_AMIGA
WORD amiga_vgetmode(void);
#endif

#endif /* BIOSEXT_H */
