/*
 * tosvars.h - name of low-memory variables
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Put in this file only the low-mem vars actually used by
 * C code.
 */

#ifndef TOSVARS_H
#define TOSVARS_H

#include "portab.h"

extern LONG proc_lives;
extern LONG proc_dregs[];
extern LONG proc_aregs[];
extern LONG proc_enum;
extern LONG proc_usp;
extern WORD proc_stk[];

extern LONG memvalid;
extern LONG memval2;
extern LONG memval3;
extern BYTE conterm;

extern WORD cmdload;

extern UBYTE *v_bas_ad;

extern UWORD *colorptr;
extern UBYTE *screenpt;
extern BYTE sshiftmod;
extern BYTE defshiftmod;

extern UBYTE *phystop;

extern WORD timer_ms;

extern volatile LONG hz_200;
extern UBYTE *dskbufp;
extern volatile WORD flock;
extern WORD nflops;
extern LONG drvbits;
extern WORD bootdev;
extern WORD fverify;
extern WORD seekrate;
extern WORD dumpflg;
extern WORD nvbls;
extern volatile WORD vblsem;
extern LONG vbl_list[];
extern LONG *vblqueue;
extern volatile LONG frclock;
extern LONG *p_cookies;
extern WORD save_row;     /* saved row in escape Y command */


extern LONG sysbase;
extern void os_entry(void) NORETURN;
extern LONG os_beg;
extern LONG os_date;
extern UWORD os_dosdate;
extern WORD os_conf;
extern void (*exec_os)(void) NORETURN;
extern UBYTE *end_os;

/* these symbols are automatically created by ld */
extern UBYTE _text[];     /* start of TEXT segment */
extern UBYTE _etext[];    /* end of TEXT segment */
extern UBYTE _data[];     /* start of DATA segment */
extern UBYTE _edata[];    /* end of DATA segment */
extern UBYTE _bss[];      /* start of BSS segment */
extern UBYTE _ebss[];     /* end of BSS segment */
extern UBYTE _end_os_stram[]; /* end of the RAM used by the OS in ST-RAM */

#if CONF_WITH_STATIC_ALT_RAM
/* Static Alt-RAM is the area used by static data (BSS and maybe TEXT) */
extern UBYTE _static_altram_start[];
extern UBYTE _static_altram_end[];
#endif

extern UBYTE _endvdibss[];  /* end of VDI BSS */
#if WITH_AES
extern UBYTE _endgembss[];  /* end of GEM BSS */
#endif

extern UBYTE *membot;
extern UBYTE *memtop;

#define TTRAM_START ((UBYTE *)0x01000000)
extern UBYTE *ramtop;     /* top of TT-RAM, or NULL if no TT-RAM is present */
#define RAMVALID_MAGIC 0x1357BD13
extern LONG ramvalid;     /* if equal to RAMVALID_MAGIC, then ramtop is valid */

extern LONG os_magic;     /* if == 0x87654321, means that GEM is present */

extern LONG savptr;

extern void (*prt_stat)(void);
extern void (*prt_vec)(void);
extern void (*aux_stat)(void);
extern void (*aux_vec)(void);
extern void (*dump_vec)(void);

/* indirect BIOS vectors */

extern LONG (*bconstat_vec[])(void);
extern LONG (*bconin_vec[])(void);
extern LONG (*bconout_vec[])(WORD, WORD);
extern LONG (*bcostat_vec[])(void);

extern LONG (*hdv_rw)(WORD rw, UBYTE *buf, WORD cnt, WORD recnr, WORD dev, LONG lrecnr);
extern LONG (*hdv_bpb)(WORD dev);
extern LONG (*hdv_mediach)(WORD dev);
extern LONG (*hdv_boot)(void);
extern void (*hdv_init)(void);

extern void (*bell_hook)(void);
extern void (*kcl_hook)(void);

extern void (*etv_timer)(int);
extern LONG (*etv_critic)(WORD err,WORD dev);
extern void (*etv_term)(void);


extern void (*mousexvec)(WORD scancode);    /* Additional mouse buttons */

struct kbdvecs
{
    PFVOID midivec;     /* MIDI Input */
    PFVOID vkbderr;     /* IKBD Error */
    PFVOID vmiderr;     /* MIDI Error */
    PFVOID statvec;     /* IKBD Status */
    PFVOID mousevec;    /* IKBD Mouse */
    PFVOID clockvec;    /* IKBD Clock */
    PFVOID joyvec;      /* IKBD Joystick */
    PFVOID midisys;     /* Main MIDI Vector */
    PFVOID ikbdsys;     /* Main IKBD Vector */
};
extern struct kbdvecs kbdvecs;

#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
#define WARM_MAGIC 0x5741524D /* 'WARM' */
extern ULONG warm_magic;
#endif

extern UBYTE stkbot[]; /* BIOS internal stack */
extern UBYTE stktop[];

#endif /* TOSVARS_H */
