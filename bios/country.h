/*
 * country.h - _AKP, _IDT and country-dependant configuration
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef COUNTRY_H
#define COUNTRY_H

#include "ikbd.h"
#include "font.h"

/* a list of country codes */
#include "ctrycodes.h"

/* a list of keyboard layout codes */
#define KEYB_ALL -1
#define KEYB_US 0
#define KEYB_DE 1
#define KEYB_FR 2
#define KEYB_CZ 3
#define KEYB_GR 4

/* charset codes - names in [brackets] are understood by GNU recode */
#define CHARSET_ALL -1
#define CHARSET_ST 0   /* original [atarist] */
#define CHARSET_L2 1   /* [ISO-Latin-2] charset */
#define CHARSET_GR 2   /* Greek charset */

/* IDT flag 24 hour: 0 = 12am/pm or 1 = 24 hour */
#define IDT_12H   0x0000
#define IDT_24H   0x1000
#define IDT_TMASK 0x1000  /* time mask */

/* IDT format for printing date */
#define IDT_MMDDYY 0x000
#define IDT_DDMMYY 0x100
#define IDT_YYMMDD 0x200
#define IDT_YYDDMM 0x300
#define IDT_DMASK  0x300
#define IDT_SMASK   0xFF  /* date mask */


/* cookies */

extern long cookie_idt;
extern long cookie_akp;

/* used by machine.c */
void detect_akp_idt(void);

/* used by ikbd.c */
void get_keytbl(struct keytbl **tbl);

/* used by nls.c */
const char *get_lang_name(void);

/* used by initlinea.c */
void get_fonts(struct font_head **f6x6, 
               struct font_head **f8x8, 
               struct font_head **f8x16);

#endif /* COUNTRY_H */
