/*
 * vectors.h - exception vectors, interrupt routines and system hooks
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef VECTORS_H
#define VECTORS_H
 
#include "portab.h"
 
/* initialize default exception vectors */

extern void init_exc_vec(void);
extern void init_user_vec(void);
 
/* initialise acia vectors */

extern void init_acia_vecs(void);


/* some exception vectors */

#if CONF_WITH_SHIFTER
extern void int_hbl(void);
#endif
extern void int_vbl(void);
extern void int_linea(void);
extern void int_timerc(void);

extern void gemtrap(void);
extern void biostrap(void);
extern void xbiostrap(void);

extern void just_rte(void);
extern void just_rts(void);

long check_read_byte(long);

/* are these useful ? */
extern void print_stat(void);
extern void print_vec(void);
extern void serial_stat(void);
extern void serial_vec(void);
extern void dump_scr(void);
extern void print_vec(void);

/* */
extern void criter1(void);
extern void int_illegal(void);
extern void int_priv(void);

extern WORD trap_save_area[];

/* pointer to function returning LONG */
typedef LONG (*PFLONG)(void);

/* pointer to function returning VOID */
typedef void (*PFVOID)(void);

/* 680x0 exception vectors */
#define VEC_ILLEGAL (*(volatile PFVOID*)0x10) /* illegal instruction vector */
#define VEC_DIVNULL (*(volatile PFVOID*)0x14) /* division by zero exception vector */
#define VEC_PRIVLGE (*(volatile PFVOID*)0x20) /* priviledge exception vector */
#define VEC_LINEA   (*(volatile PFVOID*)0x28) /* LineA exception vector */
#define VEC_LEVEL1  (*(volatile PFVOID*)0x64) /* Level 1 interrupt vector */
#define VEC_LEVEL2  (*(volatile PFVOID*)0x68) /* Level 2 interrupt vector */
#define VEC_LEVEL3  (*(volatile PFVOID*)0x6c) /* Level 3 interrupt vector */
#define VEC_LEVEL4  (*(volatile PFVOID*)0x70) /* Level 4 interrupt vector */
#define VEC_LEVEL5  (*(volatile PFVOID*)0x74) /* Level 5 interrupt vector */
#define VEC_LEVEL6  (*(volatile PFVOID*)0x78) /* Level 6 interrupt vector */
#define VEC_LEVEL7  (*(volatile PFVOID*)0x7c) /* Level 7 interrupt (not maskable) */
#define VEC_TRAP1   (*(volatile PFVOID*)0x84) /* TRAP #1 exception vector */
#define VEC_TRAP2   (*(volatile PFVOID*)0x88) /* TRAP #2 exception vector */
#define VEC_TRAP13  (*(volatile PFVOID*)0xb4) /* TRAP #13 exception vector */
#define VEC_TRAP14  (*(volatile PFVOID*)0xb8) /* TRAP #14 exception vector */

/* MFP interrupt vectors */
#define VEC_MFP6   (*(volatile PFVOID*)0x118) /* MFP level 6 interrupt vector */

/* Atari hardware interrupt mapping */
#define VEC_HBL     VEC_LEVEL2                /* HBL interrupt vector */
#define VEC_VBL     VEC_LEVEL4                /* VBL interrupt vector */
#define VEC_ACIA    VEC_MFP6                  /* Keyboard/MIDI interrupt vector */

/* OS exception mapping */
#define VEC_AES     VEC_TRAP2                 /* AES trap exception vector */
#define VEC_BIOS    VEC_TRAP13                /* BIOS trap exception vector */
#define VEC_XBIOS   VEC_TRAP14                /* XBIOS trap exception vector */

/* protect d2/a2 when calling external user-supplied code */

LONG protect_v(LONG (*func)(void));
LONG protect_w(LONG (*func)(WORD), WORD);
LONG protect_ww(LONG (*func)(void), WORD, WORD);
LONG protect_wlwwwl(LONG (*func)(void), WORD, LONG, WORD, WORD, WORD, LONG);

#endif /* VECTORS_H */
  
