/*
 * bios.h - misc internal BIOS functions and variables
 *
 * Copyright (C) 2011-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOS_H
#define BIOS_H

#include "biosdefs.h"

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

/* Line-A functions */
void linea_init(void); /* initialize variables */

/* misc BIOS variables */
extern const OSHEADER os_header;
extern struct kbdvecs kbdvecs;

/* Font offsets */
extern const UWORD off_6x6_table[];
extern const UWORD off_8x8_table[];
#define off_8x16_table off_8x8_table

/* these symbols are created by the linker script */
extern UBYTE _text[];     /* start of TEXT segment */
extern UBYTE _etext[];    /* end of TEXT segment */
extern UBYTE _data[];     /* start of DATA segment */
extern UBYTE _edata[];    /* end of DATA segment */
extern UBYTE _bss[];      /* start of BSS segment */
extern UBYTE _endvdibss[];  /* end of VDI BSS */
extern UBYTE _ebss[];     /* end of BSS segment */
extern UBYTE _end_os_stram[]; /* end of the RAM used by the OS in ST-RAM */
extern UBYTE stkbot[]; /* BIOS internal stack */
extern UBYTE stktop[];

#if CONF_WITH_STATIC_ALT_RAM
/* Static Alt-RAM is the area used by static data (BSS and maybe TEXT) */
extern UBYTE _static_altram_start[];
extern UBYTE _static_altram_end[];
#endif

#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
#define WARM_MAGIC 0x5741524D /* 'WARM' */
extern ULONG warm_magic;
#endif

#endif /* BIOS_H */
