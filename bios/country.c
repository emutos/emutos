/*
 * country.c - _AKP, _IDT and country-dependent configuration
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * read doc/country.txt for information about country-related issues.
 */

#include "config.h"
#include "portab.h"
#include "cookie.h"
#include "country.h"
#include "nvram.h"
#include "tosvars.h"
#include "header.h"

/*
 * country tables - we define the data structures here, then include the
 * actual tables. The code reading these tables is below.
 */

#if CONF_MULTILANG

struct country_record {
    int country;            /* country code */
    const char *lang_name;  /* name used to retrieve translations */
    int keyboard;           /* keyboard layout code */
    int charset;            /* charset code */
    int idt;                /* international date and time */
};

#endif

struct charset_fonts {
    const Fonthead *f6x6;
    const Fonthead *f8x8;
    const Fonthead *f8x16;
};

/*
 * ctables.h contains the following tables:
 * static const struct country_record countries[];
 * static const struct keytbl *keytables[];
 * static const struct charset_fonts font_sets[];
 */

#include "ctables.h"

/*
 *
 */

long cookie_idt;
long cookie_akp;

/* Get the default country code according to OS header. */
static int get_default_country(void)
{
    if (os_conf == OS_CONF_MULTILANG)
    {
        /* No country specified in OS header.
         * Default to the value of the COUNTRY Makefile variable. */
        return OS_COUNTRY;
    }
    else
    {
        /* Default to the country specified in OS header */
        return os_conf >> 1;
    }
}

#if CONF_MULTILANG

static const struct country_record *get_country_record(int country_code)
{
    int default_country = get_default_country();
    int default_country_index = 0;
    int i;

    for(i = 0 ; i < ARRAY_SIZE(countries) ; i++) {
        if(countries[i].country == country_code) {
            return &countries[i]; /* Found */
        }

        if(countries[i].country == default_country) {
            default_country_index = i;
        }
    }

    /* Not found */
    return &countries[default_country_index];
}

/* Get the country code used for display: fonts and language */
static int get_current_country_display(void)
{
    return (cookie_akp >> 8) & 0xff;
}

/* Get the country code used for input: keyboard layout */
static int get_current_country_input(void)
{
    return cookie_akp & 0xff;
}

static int get_kbd_index(void)
{
    int country_code = get_current_country_input();
    const struct country_record *cr = get_country_record(country_code);
    return cr->keyboard;
}

const char *get_lang_name(void)
{
    int country_code = get_current_country_display();
    const struct country_record *cr = get_country_record(country_code);
    return cr->lang_name;
}

static int get_charset_index(void)
{
    int country_code = get_current_country_display();
    const struct country_record *cr = get_country_record(country_code);
    return cr->charset;
}

#endif

/*
 * initialise the _AKP cookie
 *
 * the default values are taken from os_conf;
 *
 * if configured for multilanguage and NVRAM, and NVRAM is readable,
 * we override the defaults with the values from NVRAM
 */
void detect_akp(void)
{
    int country = get_default_country();
    int keyboard = country;

#if CONF_WITH_NVRAM && CONF_MULTILANG
    {
        UBYTE buf[2];
        int err;

        err = nvmaccess(0, 6, 2, buf);
        if (err == 0)
        {
            /* Override with the NVRAM settings */
            country = buf[0];
            keyboard = buf[1];
        }
    }
#endif

    cookie_akp = (country << 8) | keyboard;
}

void detect_idt(void)
{
#if CONF_WITH_NVRAM
    UBYTE buf[2];
    int err;

    err = nvmaccess(0, 8, 2, buf);
    if (err == 0)
    {
        cookie_idt = (buf[0] << 8) | buf[1];
    }
    else
#endif
    {
        /* either no NVRAM, or the NVRAM is corrupt (low battery,
         * bad cksum), get the value from the current country. */
#if CONF_MULTILANG
        int country_code = get_current_country_display();
        const struct country_record *cr = get_country_record(country_code);
        cookie_idt = cr->idt;
#else
        cookie_idt = CONF_IDT;
#endif
    }
}

/*
 * get_keytbl - get current country keyboard layout
 */

const struct keytbl *get_keytbl(void)
{
    int j;
#if CONF_MULTILANG
    j = get_kbd_index();
#else
    /* use the unique keyboard anyway */
    j = 0;
#endif
    return keytables[j];
}

/*
 * get_fonts - initialize country dependant font tables
 */

void get_fonts(const Fonthead **f6x6,
               const Fonthead **f8x8,
               const Fonthead **f8x16)
{
    int j;
#if CONF_MULTILANG
    j = get_charset_index();
#else
    /* use the unique charset anyway */
    j = 0;
#endif
    *f6x6 = font_sets[j].f6x6;
    *f8x8 = font_sets[j].f8x8;
    *f8x16 = font_sets[j].f8x16;
}
