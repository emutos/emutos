/*
 * country.h - _AKP, _IDT and all that stuff
 *
 * Copyright (c) 2001 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * dependencies: this header file must be included after:
 *
 * (no dependency)
 */
 
/* a list of country codes */

/* these are documented in the compendium */
#define COUNTRY_US  0   /* USA */
#define COUNTRY_DE  1   /* Germany */
#define COUNTRY_FR  2   /* France */
#define COUNTRY_UK  3   /* United Kingdom */
#define COUNTRY_ES  4   /* Spain */
#define COUNTRY_IT  5   /* Italy */
#define COUNTRY_SE  6   /* Sweden */
#define COUNTRY_SF  7   /* Switzerland (French) */
#define COUNTRY_SG  8   /* Switzerland (German), NOT Singapore! */

/* the other below were given by Petr */
#define COUNTRY_TR  9   /* Turkey */
#define COUNTRY_FI 10   /* Finland */
#define COUNTRY_NO 11   /* Norway */
#define COUNTRY_DK 12   /* Denmark */
#define COUNTRY_SA 13   /* Saudi Arabia */
#define COUNTRY_NL 14   /* Holland */
#define COUNTRY_CZ 15   /* Czech Republic */
#define COUNTRY_HU 16   /* Hungary */
#define COUNTRY_SK 17   /* Slovak Republic */
 
/* cookies */

extern long cookie_idt;
extern long cookie_akp;

/* used by machine.c */
void detect_akp_idt(void);

/* used by ikbd.c */
int get_kbd_number(void);

/* used by nls.c */
const char *get_lang_name(void);

