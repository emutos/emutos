/*
 * country.c - _AKP, _IDT and all that stuff
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
#include "detect.h"
#include "nvram.h"
#include "tosvars.h"

long cookie_idt;
long cookie_akp;

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
    { COUNTRY_CS, KEYB_CS, "cs", CHARSET_L2, IDT_24H | IDT_DDMMYY | '/' }, 
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
    /* either no NVRAM, or the NVRAM is corrupt (low battery, bad cksum), */
    /* interpret the os_pal flag in header */
    int i = get_country_index(os_pal >> 1);
    
    cookie_akp = (countries[i].country << 8) | countries[i].keyboard;
    cookie_idt = countries[i].idt;
  } else {
    cookie_akp = (buf[0] << 8) | buf[1];
    cookie_idt = (buf[2] << 8) | buf[3]; 
  }
}

int get_kbd_number(void)
{
  return cookie_akp & 0xff;
}

const char *get_lang_name(void)
{
  int i = get_country_index((cookie_akp >> 8) & 0xFF);
  return countries[i].lang_name;
}

int get_charset(void)
{
  int i = get_country_index((cookie_akp >> 8) & 0xFF);
  return countries[i].charset;
}
