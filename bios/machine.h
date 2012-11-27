/*
 * machine.h - declarations about the machine type and capabilities
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef MACHINE_H
#define MACHINE_H

/*
 * hardware registers
 */
#define FALCON_HHT          0x00ff8282UL
#define TT_PALETTE_REGS     0x00ff8400UL
#define BLITTER_CONFIG1     0x00ff8a3cUL
#define SCC_BASE            0x00ff8c80UL
#define SYS_INT_MASK        0x00ff8e01UL
#define SCU_GPR1            0x00ff8e09UL
#define VME_INT_MASK        0x00ff8e0dUL
#define DIP_SWITCHES        0x00ff9200UL
#define TT_MFP_BASE         0x00fffa80UL

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
#if CONF_WITH_ALT_RAM
extern long cookie_frb;
#endif

/*
 * these are != 0 if the feature is present
 */

#if CONF_WITH_STE_SHIFTER
extern int has_ste_shifter;
#endif
#if CONF_WITH_TT_SHIFTER
extern int has_tt_shifter;
#endif
#if CONF_WITH_VIDEL
extern int has_videl;
#endif
#if CONF_WITH_TT_MFP
extern int has_tt_mfp;
#endif
#if CONF_WITH_SCC
extern int has_scc;
#endif
#if CONF_WITH_VME
extern int has_vme;
#endif
#if CONF_WITH_MEGARTC
extern int has_megartc;   /* in clock.c */
#endif
#if CONF_WITH_NVRAM
extern int has_nvram;     /* in nvram.c */
#endif
#if CONF_WITH_BLITTER
extern int has_blitter;
#endif
#if CONF_WITH_DMASOUND
extern int has_dmasound;  /* in dmasound.c */
extern int has_microwire; /* in dmasound.c */
extern int has_falcon_dmasound; /* in dmasound.c */
#endif

/*
 * functions
 */

/* detect the available hardware machine */
void machine_detect(void);

/* fill the cookie jar */
void fill_cookie_jar(void);

/* print the name of the machine */
const char * machine_name(void);


#endif /* MACHINE_H */

