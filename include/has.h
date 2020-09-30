/*
 * has.h - BIOS HAS_* macros to determine if hardware is available
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef HAS_H
#define HAS_H

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

#if CONF_WITH_NOVA
extern int has_nova;    /* in nova.c */
  #define HAS_NOVA has_nova
#else
  #define HAS_NOVA 0
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

#if CONF_WITH_MAGNUM
extern int has_magnum;
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
extern int blitter_is_enabled;
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

#if CONF_WITH_DSP
extern int has_dsp;     /* in dsp.c */
  #define HAS_DSP has_dsp
#else
  #define HAS_DSP 0
#endif

#if CONF_WITH_DIP_SWITCHES
extern int has_dip_switches;
  #define HAS_DIP_SWITCHES has_dip_switches
#else
  #define HAS_DIP_SWITCHES 0
#endif

#if CONF_WITH_ALT_RAM
extern int has_alt_ram;
#endif

#if CONF_ATARI_HARDWARE
extern int has_modectl;
#endif

/* address bus width */
#if defined(__mcoldfire__)
  #define IS_BUS32 1
#elif CONF_WITH_ADVANCED_CPU
extern UBYTE is_bus32;
  #define IS_BUS32 is_bus32
#else
  #define IS_BUS32 0
#endif

#endif /* HAS_H */
