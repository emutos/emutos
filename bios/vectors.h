/*
 * vectors.h - exception vectors, interrupt routines and system hooks
 *
 * Copyright (C) 2001-2025 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef VECTORS_H
#define VECTORS_H

/* initialize default exception vectors */

void init_exc_vec(void);
void init_user_vec(UWORD first_boot);

/* initialise acia vectors */

void init_acia_vecs(void);

/* some exception vectors */

#if CONF_WITH_ATARI_VIDEO
void int_hbl(void);
#endif
void int_vbl(void);
void int_linea(void);
void int_timerc(void);

void biostrap(void);
void xbiostrap(void);

void just_rte(void);

#if CONF_WITH_BUS_ERROR
long check_read_byte(long);
#endif


/* */
LONG default_etv_critic(WORD err,WORD dev);
void int_illegal(void);
void int_priv(void);
void int_unimpint(void);

extern WORD trap_save_area[];

/* 680x0 exception vectors */
#define VEC_ILLEGAL (*(volatile PFVOID*)0x10) /* illegal instruction vector */
#define VEC_DIVNULL (*(volatile PFVOID*)0x14) /* division by zero exception vector */
#define VEC_PRIVLGE (*(volatile PFVOID*)0x20) /* privilege exception vector */
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
#define VEC_UNIMPINT (*(volatile PFVOID*)0xf4) /* unimplemented integer instruction exception vector */

/* MFP interrupt vectors */
#define VEC_MFP6   (*(volatile PFVOID*)0x118) /* MFP level 6 interrupt vector */

#if CONF_WITH_SCC
/* SCC interrupt vectors, default addresses */
#define VEC_SCCB_TBE (*(volatile PFVOID*)0x180) /* Channel B, transmit buffer empty */
#define VEC_SCCB_EXT (*(volatile PFVOID*)0x188) /* Channel B, external status change */
#define VEC_SCCB_RXA (*(volatile PFVOID*)0x190) /* Channel B, receive character available */
#define VEC_SCCB_SRC (*(volatile PFVOID*)0x198) /* Channel B, special receive condition */

#define VEC_SCCA_TBE (*(volatile PFVOID*)0x1a0) /* Channel A, transmit buffer empty */
#define VEC_SCCA_EXT (*(volatile PFVOID*)0x1a8) /* Channel A, external status change */
#define VEC_SCCA_RXA (*(volatile PFVOID*)0x1b0) /* Channel A, receive character available */
#define VEC_SCCA_SRC (*(volatile PFVOID*)0x1b8) /* Channel A, special receive condition */
#endif

/* Atari hardware interrupt mapping */
#define VEC_HBL     VEC_LEVEL2                /* HBL interrupt vector */
#define VEC_VBL     VEC_LEVEL4                /* VBL interrupt vector */
#define VEC_ACIA    VEC_MFP6                  /* Keyboard/MIDI interrupt vector */

/* OS exception mapping */
#define VEC_GEM     VEC_TRAP2                 /* GEM trap exception vector */
#define VEC_BIOS    VEC_TRAP13                /* BIOS trap exception vector */
#define VEC_XBIOS   VEC_TRAP14                /* XBIOS trap exception vector */

/* Non-Atari hardware vectors */
#if !CONF_WITH_MFP
extern void (*vector_5ms)(void);              /* 200 Hz system timer */
#endif

/* interrupt handlers in vectors.S */
#if CONF_WITH_MFP_RS232
void mfp_rs232_rx_interrupt(void);
void mfp_rs232_tx_interrupt(void);
#endif

#if CONF_WITH_TT_MFP
void mfp_tt_rx_interrupt(void);
void mfp_tt_tx_interrupt(void);
#endif

#if CONF_WITH_SCC
void scca_rx_interrupt(void);
void scca_tx_interrupt(void);
void scca_es_interrupt(void);
void sccb_rx_interrupt(void);
void sccb_tx_interrupt(void);
void sccb_es_interrupt(void);
#endif

#endif /* VECTORS_H */
