/*
 * dumpkbd.c : dump the TOS keyboard tables into a KEYTBL.TBL file format
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include <osbind.h>
#include "btools.h"

struct ktbl {
  int magic;
  char norm[128];
  char shft[128];
  char caps[128];
  char fake_alt[3];
} ktbl;

struct ktable {
  char **norm;
  char **shft;
  char **caps;
};

int main(int argc, char **argv)
{
  int fh;
  long old;
  struct ktable *ktable;

  ktbl.magic = 0x2771;
  ktable = (struct ktable *) Keytbl(-1L, -1L, -1L);

  old = Super(0);

  memmove(ktbl.norm, ktable->norm, 128);
  memmove(ktbl.shft, ktable->shft, 128);
  memmove(ktbl.caps, ktable->caps, 128);

  Super(old);

  ktbl.fake_alt[0] = 0;
  ktbl.fake_alt[1] = 0;
  ktbl.fake_alt[2] = 0;

  fh = Fcreate("KEYTBL.TBL", 0);
  if(fh < 0) {
    Cconws("couldn't open file\015\012");
    exit(1);
  }
  Fwrite(fh, (long) sizeof(ktbl), (long) &ktbl);
  Fclose(fh);
  Cconws("Done\015\012");
  exit(0);
}
  
