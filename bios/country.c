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
 * To add a country:
 * - add a line in country.h
 * - add a keyboard (follow instructions in ikbd.c)
 * - add a line in the countries table below
 * - update the table of country names in tools/mkheader.c
 */

#include "portab.h"
#include "cookie.h"
#include "country.h"
#include "detect.h"
#include "nvram.h"
#include "tosvars.h"
#include "keyboard.h"

long cookie_idt;
long cookie_akp;
int the_country;

/* IDT flag 24 hour: 0 = 12am/pm or 1 = 24 hour */
#define IDT_12H   0x0000
#define IDT_24H   0x1000

/* IDT format for printing date */
#define IDT_MMDDYY 0x000
#define IDT_DDMMYY 0x100
#define IDT_YYMMDD 0x200
#define IDT_YYDDMM 0x300

static struct {
  int country;      /* country code defined in country.h */
  int keyboard;     /* keyboard layout code defined in keyboard.h */
  char *lang_name;  /* name used to retrieve translations */
  int idt;         
} countries[] = {
  {  COUNTRY_US, KEYB_US, "us", IDT_12H | IDT_MMDDYY | '/' }, 
  {  COUNTRY_DE, KEYB_DE, "de", IDT_24H | IDT_DDMMYY | '/' }, 
  {  COUNTRY_FR, KEYB_FR, "fr", IDT_24H | IDT_DDMMYY | '/' }, 
};

void detect_akp_idt(void)
{
  char buf[4];
  int err;
  
  err = nvmaccess(0, 6, 4, (PTR) buf);
  if(err) { 
    /* either no NVRAM, or the NVRAM is corrupt (low battery, bad cksum), */
    /* interpret the os_pal flag in header */
    int country = os_pal >> 1;
    int i, j;
    j = 0;
    for(i = 0 ; i < sizeof(countries)/sizeof(*countries) ; i++) {
      if(countries[i].country == country) {
        j = i;
        break;
      }
    }
    cookie_akp = (countries[j].country << 8) | countries[j].keyboard;
    cookie_idt = countries[j].idt;
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
  return countries[(cookie_akp >> 8) & 0xFF].lang_name;
}

