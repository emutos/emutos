/*
 * mkflop.c - create an auto-booting EmuTOS floppy. 
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * this tool will create a simple auto-booting FAT12 floppy,
 * called emuboot.st from bootsect.img and emutos.img.
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define FLOPNAME "emutos.st"
#define BOOTNAME "bootsect.img"
#define TOSNAME "ramtos.img"
 
typedef unsigned char uchar;
 
struct loader {
  uchar pad[0x1e];
  uchar execflg[2];
  uchar ldmode[2];
  uchar ssect[2];
  uchar sectcnt[2];
  uchar ldaddr[4];
  uchar fatbuf[4];
  uchar fname[11];
  uchar reserved;
};
 
int fill = 1;

int mkflop(FILE *bootf, FILE *tosf, FILE *flopf)
{
  uchar buf[512];
  struct loader *b = (struct loader *)buf;
  size_t count;
  unsigned short sectcnt;
  int i;
  
  /* read bootsect */
  count = 512;
  count = fread(buf, 1, count, bootf);
  if(count <= 0) return 1; /* failure */
  if(count < 512) {
    memset(buf+count, 0, 512 - count);
  }
  
  /* compute size of tosf, and update sectcnt */
  fseek(tosf, 0, SEEK_END);
  count = ftell(tosf);
  fseek(tosf, 0, SEEK_SET);
  
  sectcnt = (count + 511) / 512;
  b->sectcnt[0] = sectcnt>>8;
  b->sectcnt[1] = sectcnt;

  /* make bootsector bootable */
  {
    unsigned short a, sum;
    sum = 0;
    for(i = 0 ; i < 255 ; i++) {
      a = (buf[2*i]<<8) + buf[2*i+1];
      sum += a;
    }
    a = 0x1234 - sum;
    buf[510] = a>>8;
    buf[511] = a;
  }
  
  /* write down bootsector */
  fwrite(buf, 1, 512, flopf);
  
  /* copy the tos starting at sector 1 */
  for(i = 0 ; i < sectcnt ; i++) {
    count = fread(buf, 1, 512, tosf);
    if(count < 512) {
      memset(buf+count, 0, 512 - count);
    }
    fwrite(buf, 1, 512, flopf);
  }

  /* fill to 360 K */
  if(fill) {
    memset(buf, 0, 512);
    for(i = sectcnt+1 ; i < 720 ; i++) {
      fwrite(buf, 1, 512, flopf);
    }
  }

  /* that's it */
  
  return 0;
}

int main(int argc, char **argv)
{
  FILE *bootf, *tosf, *flopf;

  bootf = fopen(BOOTNAME, "rb");
  if(bootf == 0) goto fail;
  tosf = fopen(TOSNAME, "rb");
  if(tosf == 0) goto fail;
  flopf = fopen(FLOPNAME, "wb");
  if(flopf == 0) goto fail;
  if(mkflop(bootf, tosf, flopf)) goto fail;
  printf("done\n");
  exit(0);

fail:
  fprintf(stderr, "something failed.\n");
  exit(1);
}
  
