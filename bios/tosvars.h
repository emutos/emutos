/*
 * tosvars.h - name of low-memory variables
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
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

/* Forward declarations */
struct _pd;

/* GEM memory usage parameters block.
 * This is actually a structure used by the BIOS
 * in order to call the main UI (i.e. AES) */

#define GEM_MUPB_MAGIC 0x87654321

typedef struct
{
    ULONG     gm_magic; /* Magical value, has to be GEM_MUPB_MAGIC */
    UBYTE     *gm_end;  /* End of the static ST-RAM used by the OS */
    PRG_ENTRY *gm_init; /* Start address of the main UI */
} GEM_MUPB;

typedef struct _osheader
{
    UWORD      os_entry;      /* BRA.S to reset handler */
    UWORD      os_version;    /* TOS version number */
    PFVOID     reseth;        /* Pointer to reset handler */
    /* The following 3 longs are actually the default GEM_MUPB structure */
    struct _osheader *os_beg; /* Base address of the operating system */
    UBYTE      *os_end;       /* End of ST-RAM statically used by the OS */
    PRG_ENTRY  *os_rsvl;      /* Entry point of default UI (unused) */
    GEM_MUPB   *os_magic;     /* GEM memory usage parameters block */
    ULONG      os_date;       /* TOS date in BCD format MMDDYYYY */
    WORD       os_conf;       /* Flag for PAL version + country */
    UWORD      os_dosdate;    /* TOS date in BDOS format */
    WORD       **os_root;     /* Pointer to the BDOS quick pool */
    UBYTE      *os_kbshift;   /* Pointer to the BIOS shifty variable */
    struct _pd **os_run;      /* Pointer to the pointer to current BASEPAGE */
    ULONG      os_dummy;      /* 'ETOS' signature */
} OSHEADER;

extern const OSHEADER os_header;

extern LONG proc_lives;
extern LONG proc_dregs[];
extern LONG proc_aregs[];
extern LONG proc_enum;
extern LONG proc_usp;
extern UWORD proc_stk[];

extern LONG memvalid;
extern LONG memval2;
extern LONG memval3;
extern UBYTE conterm;

extern WORD cmdload;

extern UBYTE *v_bas_ad;

extern const UWORD *colorptr;
extern UBYTE *screenpt;
extern UBYTE sshiftmod;

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


extern const OSHEADER *sysbase;
extern PRG_ENTRY *exec_os;
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

extern UBYTE *membot;
extern UBYTE *memtop;

#define TTRAM_START ((UBYTE *)0x01000000)
extern UBYTE *ramtop;     /* top of TT-RAM, or NULL if no TT-RAM is present */
#define RAMVALID_MAGIC 0x1357BD13
extern LONG ramvalid;     /* if equal to RAMVALID_MAGIC, then ramtop is valid */

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
