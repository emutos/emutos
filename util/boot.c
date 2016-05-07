/*
 * boot.c - standalone PRG to load EmuTOS in RAM
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
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

#define DBG_BOOT 0

/* last part of the loader is in util/bootram.S */
extern void bootram(const UBYTE *src, ULONG size, ULONG cpu) NORETURN;

/* ramtos.img is embedded in util/ramtos.S */
extern const UBYTE ramtos[];
extern const UBYTE end_ramtos[];

/*
 * cookie stuff
 */
#define _p_cookies  *(struct cookie **)0x5a0
#define _CPU        0x5f435055L

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

/* return value from _CPU cookie */
static ULONG get_cpu_cookie(void)
{
  struct cookie *cptr = _p_cookies;

  if (cptr) {
    do {
      if (cptr->id == _CPU)
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
  Cconin();
#endif

  /* supervisor */

  Super(0);

  cpu = get_cpu_cookie();

  /* do the rest in assembler */

  bootram(ramtos, count, cpu);

  return 1;
}
