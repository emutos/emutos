/*
 * tosvars.h - declarations of low-memory system variables
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * The system variables are only accessible from supervisor mode.
 * Each variable has a fixed address documented by Atari.
 * Actual addresses are defined in tosvars.ld
 */

#ifndef TOSVARS_H
#define TOSVARS_H

#include "biosdefs.h"

/* Forward declarations */
struct _md;
struct _bcb;

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
extern UBYTE defshiftmod;
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
extern WORD nvbls; /* Number of slots in the array pointed by vblqueue */
extern volatile WORD vblsem;
extern PFVOID *vblqueue; /* Pointer to the VBL queue array */
extern volatile LONG frclock;
extern LONG *p_cookies;
extern WORD save_row;     /* saved row in escape Y command */

extern const OSHEADER *sysbase;
extern PRG_ENTRY *exec_os;
extern UBYTE *end_os;

extern UBYTE *membot;
extern UBYTE *memtop;

#define TTRAM_START ((UBYTE *)0x01000000)
extern UBYTE *ramtop;     /* top of TT-RAM, or NULL if no TT-RAM is present */
#define RAMVALID_MAGIC 0x1357BD13
extern LONG ramvalid;     /* if equal to RAMVALID_MAGIC, then ramtop is valid */

extern LONG savptr;

extern struct _md themd;  /* BIOS memory descriptor */
extern struct _bcb *bufl[2]; /* buffer lists - two lists:  FAT and dir/data */

extern void (*con_state)(WORD); /* state of conout state machine */

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

extern ETV_TIMER_T etv_timer;
extern LONG (*etv_critic)(WORD err,WORD dev);
extern void (*etv_term)(void);

extern void (*swv_vec)(void);

#endif /* TOSVARS_H */
