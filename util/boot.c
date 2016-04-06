/*
 * boot.c - standalone PRG to load EmuTOS in RAM
 *
 * Copyright (c) 2001-2013 The EmuTOS development team
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

/* last part of the loader is in util/bootasm.S */
extern void bootasm(UBYTE *dest, UBYTE *src, ULONG count, ULONG cpu);

/* ramtos.img is embedded in util/ramtos.S */
extern UBYTE ramtos[];
extern UBYTE end_ramtos[];

/*
 * reset vector stuff
 */
#define resvalid    (ULONG *)0x426

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
  UBYTE *address;
  ULONG cpu;

  /* get the file size */

  count = end_ramtos - ramtos;

  /* get final address */

  address = *((UBYTE **)(ramtos + 8));

#if DBG_BOOT
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

  cpu = get_cpu_cookie();   /* used by bootasm code */

#if CONF_DETECT_FIRST_BOOT_WITHOUT_MEMCONF
  /* this boot is a first boot */
  *(ULONG*)0x6fc = 0; /* warm_magic */
#endif

  *resvalid = 0L;           /* prevent startup.S from calling now-invalid reset vector */

  /* do the rest in assembler */

  bootasm(address, ramtos, count, cpu);

  return 1;
}
