/*
 * boot.c - standalone boot.prg to load EmuTOS in RAM
 *
 * Copyright (c) 2001 by following authors
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <osbind.h>
#include "btools.h"   /* memmove */
#include "asm.h"      /* set_sr  */

extern void bootasm(long dest, long src, long count);
extern long getbootsize(void);

/*
 * the file TOS_FILENAME, of maximum length TOS_SIZE, 
 * will be loaded in memory, then copied at the address
 * indicated by its header and finally be executed by
 * jumping at that address.
 */

#define TOS_FILENAME "ramtos.img"
#define TOS_SIZE     (192*1024L)

static void putl(unsigned long u)
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
  Cconws(c);
}

static void fatal(const char *s)
{
  Cconws("Fatal: ");
  Cconws(s);
  Cconws("\012\015hit any key.");
  exit(1);
}

int main()
{
  int fh;
  long count;
  char *buf;
  long address;
  
  /* allocate temp buffer */
  
  count = TOS_SIZE;
  buf = (char *) Malloc(count + getbootsize());
  if(buf == 0) {
    fatal("cannot allocate memory");
  }
  
  /* load the file */
  
  fh = Fopen(TOS_FILENAME, 0);
  if(fh < 0) {
    fatal("cannot open file " TOS_FILENAME);
  }
  count = Fread(fh, count, buf);
  if(count < 0) {
    fatal("error reading file");
  }
  Fclose(fh);
  
  /* get final address */
  
  address = *((long *)(buf + 8));
  
  Cconws("\012\015address = 0x");
  putl(address);
  Cconws(".\012\015");
  
  /* check that the address is in first 512K RAM */

  if((address <= 0x400L) || (address + TOS_SIZE >= 0x80000L)) {
    fatal("bad address in header");
  }

  /* hit a key to let the user remove any floppy */

  Cconws("Hit RETURN to boot " TOS_FILENAME);
  Cconin();
  
  /* supervisor */
  
  Super(0);
  
  /* do the rest in assembler */
  
  bootasm(address, (long)buf, count);
  
  return 1;
}
