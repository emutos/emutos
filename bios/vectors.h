/*
 * vectors.h - declarations for processor type check
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _VECTORS_H
#define _VECTORS_H
 
#include "portab.h"
 
/* initialize default exception vectors */

extern void init_exc_vec(void);
extern void init_user_vec(void);
 
/* initialise acia vectors */

extern void init_acia_vecs(void);


/* some exception vectors */

extern void int_hbl(void);
extern void int_vbl(void);
extern void int_linea(void);

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
extern void brkpt(void);

extern WORD save_area[];

typedef void (*PFVOID)();

#define VEC_ILLEGAL (*(PFVOID*)0x10)    /* illegal instruction vector */
#define VEC_DIVNULL (*(PFVOID*)0x14)    /* division by zero interrupt vector */
#define VEC_LINEA   (*(PFVOID*)0x28)    /* LineA interrupt vector */
#define VEC_HBL     (*(PFVOID*)0x68)    /* HBL interrupt vector */
#define VEC_VBL     (*(PFVOID*)0x70)    /* VBL interrupt vector */
#define VEC_AES     (*(PFVOID*)0x88)    /* AES interrupt vector */
#define VEC_BIOS    (*(PFVOID*)0xb4)    /* BIOS interrupt vector */
#define VEC_XBIOS   (*(PFVOID*)0xb8)    /* XBIOS interrupt vector */
#define VEC_ACIA   (*(PFVOID*)0x118)    /* keyboard/Midi interrupt vector */

#endif /* VECTORS_H */
  
