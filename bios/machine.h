/*
 * machine.h - declarations about the machine type and capabilities
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MACHINE_H
#define MACHINE_H

#include "memory.h"

/*
 * hardware registers
 */
#define FALCON_BUS_CTL      0xffff8007UL
#define FALCON_HHT          0xffff8282UL
#define TT_PALETTE_REGS     0xffff8400UL
#define BLITTER_CONFIG1     0xffff8a3cUL
#define SCC_BASE            0xffff8c80UL
#define SYS_INT_MASK        0xffff8e01UL
#define SCU_GPR1            0xffff8e09UL
#define VME_INT_MASK        0xffff8e0dUL
#define DIP_SWITCHES        0xffff9200UL
#define MONSTER_REG         0xfffffe00UL

/*
 * some useful cookies.
 */

extern long cookie_mch;
extern long cookie_vdo;
#if CONF_WITH_FDC
extern long cookie_fdc;
#endif
extern long cookie_snd;
#if CONF_WITH_DIP_SWITCHES
extern long cookie_swi;
#endif
extern long cookie_idt;
extern long cookie_akp;

/*
 * these are != 0 if the feature is present
 */

/* Convenience macro to test if first boot. See MEMINIT_BIT_FIRST_BOOT. */
#define FIRST_BOOT (meminit_flags & MEMINIT_FIRST_BOOT)

#if CONF_WITH_ARANYM
extern int is_aranym;
  #define IS_ARANYM is_aranym
#else
  #define IS_ARANYM 0
#endif

#if CONF_WITH_STE_SHIFTER
extern int has_ste_shifter;
  #define HAS_STE_SHIFTER has_ste_shifter
#else
  #define HAS_STE_SHIFTER 0
#endif

#if CONF_WITH_TT_SHIFTER
extern int has_tt_shifter;
  #define HAS_TT_SHIFTER has_tt_shifter
#else
  #define HAS_TT_SHIFTER 0
#endif

#if CONF_WITH_VIDEL
extern int has_videl;
  #define HAS_VIDEL has_videl
#else
  #define HAS_VIDEL 0
#endif

#if CONF_WITH_TT_MFP
extern int has_tt_mfp;
  #define HAS_TT_MFP has_tt_mfp
#else
  #define HAS_TT_MFP 0
#endif

#if CONF_WITH_SCC
extern int has_scc;
#endif

#if CONF_WITH_VME
extern int has_vme;
  #define HAS_VME has_vme
#else
  #define HAS_VME 0
#endif

#if CONF_WITH_MONSTER
extern int has_monster;
extern int has_monster_rtc;
  #define HAS_MONSTER_RTC has_monster_rtc
#else
  #define HAS_MONSTER_RTC 0
#endif

#if CONF_WITH_ICDRTC
extern int has_icdrtc;    /* in clock.c */
  #define HAS_ICDRTC has_icdrtc
#else
  #define HAS_ICDRTC 0
#endif

#if CONF_WITH_MEGARTC
extern int has_megartc;   /* in clock.c */
  #define HAS_MEGARTC has_megartc
#else
  #define HAS_MEGARTC 0
#endif

#if CONF_WITH_NVRAM
extern int has_nvram;     /* in nvram.c */
  #define HAS_NVRAM has_nvram
#else
  #define HAS_NVRAM 0
#endif

/* convenience macro: TRUE iff any kind of real time clock */
#define HAS_RTC (HAS_NVRAM || HAS_MEGARTC || HAS_ICDRTC || HAS_MONSTER_RTC)

#if CONF_WITH_BLITTER
extern int has_blitter;
  #define HAS_BLITTER has_blitter
#else
  #define HAS_BLITTER 0
#endif

#if CONF_WITH_DMASOUND
extern int has_dmasound;  /* in dmasound.c */
extern int has_microwire; /* in dmasound.c */
extern int has_falcon_dmasound; /* in dmasound.c */
  #define HAS_DMASOUND has_dmasound
  #define HAS_MICROWIRE has_microwire
  #define HAS_FALCON_DMASOUND has_falcon_dmasound
#else
  #define HAS_DMASOUND 0
  #define HAS_MICROWIRE 0
  #define HAS_FALCON_DMASOUND 0
#endif

#if CONF_WITH_DIP_SWITCHES
extern int has_dip_switches;
  #define HAS_DIP_SWITCHES has_dip_switches
#else
  #define HAS_DIP_SWITCHES 0
#endif

/*
 * functions
 */

/* address bus width */
#if defined(__mcoldfire__)
  #define IS_BUS32 1
#elif CONF_WITH_ADVANCED_CPU
BOOL detect_32bit_address_bus(void);
extern UBYTE is_bus32;
  #define IS_BUS32 is_bus32
#else
  #define IS_BUS32 0
#endif

/* detect the available hardware */
void machine_detect(void);

/* perform machine-specific initialisation */
void machine_init(void);

/* fill the cookie jar */
void fill_cookie_jar(void);

/* print the name of the machine */
const char * machine_name(void);


#endif /* MACHINE_H */
