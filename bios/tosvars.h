/*
 * tosvars.h - name of low-memory variables
 *
 * Copyright (c) 2001 EmuTOS development team
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

#ifndef _TOSVARS_H
#define _TOSVARS_H

#include "portab.h"

extern LONG proc_lives;
extern LONG proc_dregs[];
extern LONG proc_aregs[];
extern LONG proc_enum;
extern LONG proc_usp;
extern WORD proc_stk[];

extern BYTE conterm;

extern WORD cmdload;

extern UBYTE *v_bas_ad;
//extern LONG kbdvecs[];

extern WORD *colorptr;
extern UBYTE *screenpt;
extern BYTE sshiftmod;

extern void *phystop;

extern WORD timer_ms;

extern LONG hz_200;
extern LONG dskbufp;  
extern WORD flock;
extern WORD nflops;
extern LONG drvbits;
extern WORD bootdev;
extern WORD fverify;
extern WORD seekrate;
extern BYTE diskbuf[];
extern WORD dumpflg;
extern WORD nvbls;
extern WORD vblsem;
extern LONG vbl_list[];
extern LONG *vblqueue;
//extern LONG **p_cookies


extern LONG sysbase;
extern void os_entry(void);
extern LONG os_beg;
extern LONG os_date;
extern UWORD os_dosdate;
extern WORD os_pal;
extern void (*exec_os)(void);
extern LONG end_os;
extern LONG m_start;
extern LONG m_length;

/* these symbols are automatically created by gcc + ld */
extern BYTE _etext[];     /* end of text */
extern BYTE _edata[];     /* end of data */
extern BYTE end[];        /* end of bss + comm sections */

extern LONG os_end;
extern LONG membot;
extern LONG memtop;
extern LONG themd;

/* if == 0x87654321, means that GEM is present ;
 * EmuTOS convention : if == 0x1234abcd, means that the TOS
 * was booted from an autoboot floppy, and so asks to remove
 * the floppy before going on.
 */
#define OS_MAGIC_EJECT 0x1234abcd
extern LONG os_magic;

extern LONG savptr;
extern WORD save_area[];

extern void (*prt_stat)(void);
extern void (*prt_vec)(void);
extern void (*aux_stat)(void);
extern void (*aux_vec)(void);
extern void (*dump_vec)(void);

/* indirect BIOS vectors */

LONG (*hdv_rw)(WORD rw, LONG buf, WORD cnt, WORD recnr, WORD dev, LONG lrecnr);
LONG (*hdv_bpb)(WORD dev);
LONG (*hdv_mediach)(WORD dev);
LONG (*hdv_boot)(void);
void (*hdv_init)(void);

void (*bell_hook)(void);
void (*kcl_hook)(void);

void (*etv_timer)(void);
void (*etv_critic)(void);
void (*etv_term)(void);
void (*etv_xtra)(void);


struct kbdvecs
{
    void (*midivec)( UBYTE data );  /* MIDI Input */
    void (*vkbderr)( UBYTE data );  /* IKBD Error */
    void (*vmiderr)( UBYTE data );  /* MIDI Error */
    void (*statvec)(char *buf);     /* IKBD Status */
    void (*mousevec)(char *buf);    /* IKBD Mouse */
    void (*clockvec)(char *buf);    /* IKBD Clock */
    void (*joyvec)(char *buf);      /* IKBD Joystick */
    void (*midisys)( void );        /* Main MIDI Vector */
    void (*ikbdsys)( void );        /* Main IKBD Vector */
};
extern struct kbdvecs kbdvecs;

#endif /* _TOSVARS_H */





