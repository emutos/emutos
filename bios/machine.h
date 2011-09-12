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
 * some useful cookies.
 */

extern long cookie_mch;
extern long cookie_vdo;
extern long cookie_fdc;
extern long cookie_snd;
extern long cookie_swi;
extern long cookie_idt;
extern long cookie_akp;
extern long cookie_frb;

/*
 * these are != 0 if the feature is present
 */

extern int has_ste_shifter;
#if CONF_WITH_TT_SHIFTER
extern int has_tt_shifter;
#endif
#if CONF_WITH_VIDEL
extern int has_videl;
#endif
extern int has_vme;
extern int has_megartc;   /* in clock.c */
extern int has_nvram;     /* in nvram.c */
extern int has_dmasound;  /* in dmasound.c */
extern int has_microwire; /* in dmasound.c */
extern int has_falcon_dmasound; /* in dmasound.c */

/*
 * other variables
 */

extern long mcpu; 
extern long fputype; 

/*
 * functions
 */

/* detect the available hardware machine */
void machine_detect(void);

/* initialise the machine, and fill the cookie jar */
void machine_init(void);

/* print the name of the machine */
const char * machine_name(void);


#endif /* MACHINE_H */

