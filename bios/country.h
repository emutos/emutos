/*
 * country.h - _AKP, _IDT and country-dependant configuration
 *
 * Copyright (c) 2001-2005 The EmuTOS development team
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

/* charset codes - names in [brackets] are understood by GNU recode */
#define CHARSET_ALL -1
#define CHARSET_ST 0   /* original [atarist] */
#define CHARSET_L2 1   /* [ISO-Latin-2] charset */
#define CHARSET_L9 2   /* [ISO-8859-15] (latin-9) charset */
#define CHARSET_GR 3   /* Greek charset */
#define CHARSET_RU 4   /* Russian charset */

/* IDT flag 24 hour: 0 = 12am/pm or 1 = 24 hour */
#define IDT_12H   0x0000
#define IDT_24H   0x1000
#define IDT_TMASK 0x1000  /* time mask */

/* IDT format for printing date */
#define IDT_BIT_DM 0x100 /* day before month */
#define IDT_BIT_YM 0x200 /* year before month */
#define IDT_MMDDYY 0x000
#define IDT_DDMMYY IDT_BIT_DM
#define IDT_YYMMDD IDT_BIT_YM
#define IDT_YYDDMM (IDT_BIT_YM | IDT_BIT_DM)
#define IDT_DMASK  (IDT_BIT_YM | IDT_BIT_DM)
#define IDT_SMASK   0xFF  /* date mask */


/* cookies */

extern long cookie_akp;
extern long cookie_idt;

/* used by machine.c */
void detect_akp(void);
void detect_idt(void);

/* used by ikbd.c */
void get_keytbl(const struct keytbl **tbl);

/* used by nls.c */
const char *get_lang_name(void);

/* used by initlinea.c */
void get_fonts(const Fonthead **f6x6,
               const Fonthead **f8x8,
               const Fonthead **f8x16);

#endif /* COUNTRY_H */
