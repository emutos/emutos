/*
 * vectors.h - declarations for exception vectors.
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

extern VOID init_exc_vec(VOID);
extern VOID init_user_vec(VOID);
 
/* initialise acia vectors */

extern VOID init_acia_vecs(VOID);


/* some exception vectors */

extern VOID int_hbl(VOID);
extern VOID int_vbl(VOID);
extern VOID int_linea(VOID);
extern VOID dummyaes(VOID);
extern VOID biostrap(VOID);
extern VOID xbiostrap(VOID);
extern VOID just_rte(VOID);

/* are these useful ? */
extern VOID print_stat(VOID);
extern VOID print_vec(VOID);
extern VOID serial_stat(VOID);
extern VOID serial_vec(VOID);
extern VOID dump_scr(VOID);
extern VOID print_vec(VOID);

/* */
extern VOID just_rts(VOID);
extern VOID criter1(VOID);
extern VOID brkpt(VOID);

typedef VOID (*PFVOID)();

#define VEC_DIVNULL (*(PFVOID*)0x14)    /* division by zero interrupt vector */
#define VEC_LINEA   (*(PFVOID*)0x28)    /* LineA interrupt vector */
#define VEC_HBL     (*(PFVOID*)0x68)    /* HBL interrupt vector */
#define VEC_VBL     (*(PFVOID*)0x70)    /* VBL interrupt vector */
#define VEC_AES     (*(PFVOID*)0x88)    /* AES interrupt vector */
#define VEC_BIOS    (*(PFVOID*)0xb4)    /* BIOS interrupt vector */
#define VEC_XBIOS   (*(PFVOID*)0xb8)    /* XBIOS interrupt vector */
#define VEC_ACIA   (*(PFVOID*)0x118)    /* keyboard/Midi interrupt vector */

#endif /* VECTORS_H */
  
