/*
 * boot.c - standalone PRG to load EmuTOS in RAM
 *
 * Copyright (C) 2001-2019 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *  VRI     Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include <osbind.h>
#include <stdlib.h>

#ifndef GENERATING_DEPENDENCIES
/* Defines generated from emutos.map */
#include "../obj/ramtos.h"
#endif

#define DBG_BOOT 0

/* last part of the loader is in util/bootram.S */
void bootram(const UBYTE *src, ULONG size, ULONG cpu) NORETURN;

/* emutos.img is embedded in util/ramtos.S */
extern const UBYTE ramtos[];
extern const UBYTE end_ramtos[];

/*
 * cookie stuff
 */
#define _p_cookies  *(struct cookie **)0x5a0
#define _CPU        0x5f435055L
#define _MCH        0x5f4d4348L
#define MCH_TT      0x00020000L

struct cookie {
  ULONG id;
  ULONG value;
};

#if DBG_BOOT
static void putl(ULONG u)
{
  int i;
  char c[9];
  unsigned a;
  c[8] = 0;
  for(i = 0 ; i < 8 ; i++) {
    a = u & 0xf;
    u >>= 4L;
    c[7-i] = (a < 10) ? (a + '0') : (a + 'a' - 10);
  }
  (void)Cconws(c);
}

static void fatal(const char *s)
{
  (void)Cconws("Fatal: ");
  (void)Cconws(s);
  (void)Cconws("\012\015hit any key.");
  Cconin();
  exit(1);
}
#endif

/* return value from cookie */
static ULONG get_cookie(ULONG id)
{
  struct cookie *cptr = _p_cookies;

  if (cptr) {
    do {
      if (cptr->id == id)
        return cptr->value;
    } while ((++cptr)->id);
  }

  return 0L;
}

int main(void)
{
  ULONG count;
  ULONG cpu;
#if DBG_BOOT
  UBYTE *address;
#endif

  /* get the file size */

  count = end_ramtos - ramtos;

#if DBG_BOOT
  /* get final address */

  address = *((UBYTE **)(ramtos + 8));

  (void)Cconws("src = 0x");
  putl((ULONG)ramtos);
  (void)Cconws("\012\015");

  (void)Cconws("dst = 0x");
  putl((ULONG)address);
  (void)Cconws("\012\015");

  /* check that the address is realistic */

  if((address <= (UBYTE *)0x800L) || (address >= (UBYTE *)0x80000)) {
    fatal("bad address in header");
  }

  (void)Cconws("Hit RETURN to boot EmuTOS");
  (void)Cconin();
#endif

  /* supervisor */

  Super(0);

  cpu = get_cookie(_CPU);

#ifdef TARGET_256
  /* prg256 variants don't support TT and beyond */
  if (get_cookie(_MCH) >= MCH_TT) {
    (void)Cconws("\033EEMU256*.PRG supports ST/MegaST/STe/MegaSTe.\r\n");
    (void)Cconws("Use EMUTOS*.PRG instead.\r\n");
    (void)Cconws("Hit RETURN to exit");
    (void)Cconin();
    return 1;
  }
#endif

  /* do the rest in assembler */

  bootram(ramtos, count, cpu);

  return 1;
}
