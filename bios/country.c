/*
 * country.c - _AKP, _IDT and country-dependant configuration
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
 * read doc/country.txt for information about country-related issues.
 */

#include "portab.h"
#include "cookie.h"
#include "country.h"
#include "nvram.h"
#include "tosvars.h"

#include "config.h"  /* included after country.h because of i18nconf.h */
#include "header.h"  /* contains the default country number */


long cookie_idt;
long cookie_akp;

#if CONF_UNIQUE_COUNTRY
const char *get_lang_name(void)
{
    return CONF_LANG;
}

void detect_akp_idt(void)
{
    cookie_akp = (OS_COUNTRY << 8) | CONF_KEYB;
    cookie_idt = IDT_24H | IDT_DDMMYY | '/';
}

#else

struct country_record {
    int country;      /* country code */
    int keyboard;     /* keyboard layout code */
    char *lang_name;  /* name used to retrieve translations */
    int charset;      /* charset code */
    int idt;         
} ;

const static struct country_record countries[] = {
    { COUNTRY_US, KEYB_US, "us", CHARSET_ST, IDT_12H | IDT_MMDDYY | '/' }, 
    { COUNTRY_DE, KEYB_DE, "de", CHARSET_ST, IDT_24H | IDT_DDMMYY | '/' }, 
    { COUNTRY_FR, KEYB_FR, "fr", CHARSET_ST, IDT_24H | IDT_DDMMYY | '/' }, 
    { COUNTRY_CZ, KEYB_CZ, "cs", CHARSET_L2, IDT_24H | IDT_DDMMYY | '/' }, 
};

/* this scheme is set to make it clear which country is partially
 * implemented and which is not 
 */
static int get_country_index(int country_code)
{
    int i;
    for(i = 0 ; i < sizeof(countries)/sizeof(*countries) ; i++) {
        if(countries[i].country == country_code) {
            return i;
        }
    }
    return 0; /* default is US */
}

void detect_akp_idt(void)
{
    char buf[4];
    int err;
  
    err = nvmaccess(0, 6, 4, (PTR) buf);
    if(err) { 
        /* either no NVRAM, or the NVRAM is corrupt (low battery, 
         * bad cksum), interpret the os_pal flag in header 
         */
        int i = get_country_index(os_pal >> 1);
    
        cookie_akp = (countries[i].country << 8) | countries[i].keyboard;
        cookie_idt = countries[i].idt;
    } else {
        cookie_akp = (buf[0] << 8) | buf[1];
        cookie_idt = (buf[2] << 8) | buf[3]; 
    }
}

static int get_kbd_number(void)
{
    return cookie_akp & 0xff;
}

const char *get_lang_name(void)
{
    int i = get_country_index((cookie_akp >> 8) & 0xFF);
    return countries[i].lang_name;
}

static int get_charset(void)
{
    int i = get_country_index((cookie_akp >> 8) & 0xFF);
    return countries[i].charset;
}

#endif

/*==== Keyboard layouts ===================================================*/

/* To add a keyboard, please do the following:
 * - read doc/country.txt
 * - create a file bios/keyb_xx.h 
 *   (you may use supplied tool dumpkbd.prg)
 * - add a #include "keyb_xx.h" below
 * - add a line in the keyboard table in bios/country.h
 * - add a line in the table below. 
 */


/* include here all available keyboard definitions */

#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_US) 
#include "keyb_us.h"
#endif
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_DE) 
#include "keyb_de.h"
#endif
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_FR) 
#include "keyb_fr.h"
#endif
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_CZ) 
#include "keyb_cz.h"
#endif

/* add the available keyboards in this table */

struct kbd_record {
    int number;
    struct keytbl *keytbl;
};

const static struct kbd_record avail_kbd[] = {
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_US) 
    { KEYB_US, &keytbl_us }, 
#endif
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_DE) 
    { KEYB_DE, &keytbl_de }, 
#endif
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_FR) 
    { KEYB_FR, &keytbl_fr },
#endif
#if (CONF_KEYB == KEYB_ALL || CONF_KEYB == KEYB_CZ) 
    { KEYB_CZ, &keytbl_cz }, 
#endif
};


void get_keytbl(struct keytbl **tbl)
{
#if ! CONF_UNIQUE_COUNTRY
    int i;
    int goal = get_kbd_number();   

    for (i = 0; i < sizeof(avail_kbd) / sizeof(*avail_kbd); i++) {
        if (avail_kbd[i].number == goal) {
            *tbl = avail_kbd[i].keytbl;
            return;
        }
    }
    /* if not found, use the default keyboard */
    *tbl = &keytbl_us;
#else
    /* use the unique keyboard anyway */
    *tbl = avail_kbd[0].keytbl;
#endif
}

/*==== Font tables ========================================================*/

/* to add a set of fonts, please do the following:
 * - read doc/country.txt
 * - add a line in the charset names in bios/country.h
 * - add extern declaration referencing the fonts below
 * - add a line in the font_sets table below
 */
 
extern const struct font_head f8x16;
extern const struct font_head f8x8;
extern const struct font_head f6x6;
extern const struct font_head latin2_8x16;
extern const struct font_head latin2_8x8;
extern const struct font_head latin2_6x6;

struct charset_fonts {
    int charset;
    const struct font_head *f6x6;
    const struct font_head *f8x8;
    const struct font_head *f8x16;
};

const static struct charset_fonts font_sets[] = {
#if (CONF_CHARSET == CHARSET_ALL || CONF_CHARSET == CHARSET_ST) 
    { CHARSET_ST, &f6x6, &f8x8, &f8x16 },
#endif
#if (CONF_CHARSET == CHARSET_ALL || CONF_CHARSET == CHARSET_L2) 
    { CHARSET_L2, &latin2_6x6, &latin2_8x8, &latin2_8x16 }, 
#endif
};

void get_fonts(struct font_head **f6x6, 
               struct font_head **f8x8, 
               struct font_head **f8x16)
{
    int j;
#if ! CONF_UNIQUE_COUNTRY
    int i;
    int charset = get_charset();
    
    /* find the index of the required charset in our font table */
    for(i = j = 0 ; i < sizeof(font_sets)/sizeof(*font_sets) ; i++) {
        if( font_sets[i].charset == charset ) {
            j = i; 
            break;
        }
    }
#else
    j = 0;
#endif
    *f6x6 = (struct font_head *) font_sets[j].f6x6;
    *f8x8 = (struct font_head *) font_sets[j].f8x8;  
    *f8x16 = (struct font_head *) font_sets[j].f8x16;
}
