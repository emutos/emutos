/*
 * country.h - _AKP, _IDT and country-dependant configuration
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
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
#include "i18nconf.h"
#include "ctrycodes.h"

/* used by machine.c */
void detect_akp(void);
void detect_idt(void);

/* used by ikbd.c */
const struct keytbl *get_keytbl(void);

/* used by nls.c */
const char *get_lang_name(void);

/* used by initlinea.c */
void get_fonts(const Fonthead **f6x6,
               const Fonthead **f8x8,
               const Fonthead **f8x16);

#endif /* COUNTRY_H */
