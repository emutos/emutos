/*
 * screen.c - low-level screen routines
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  MAD   Martin Doering
 *  THO   Thomas Huth
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "screen.h"
#include "asm.h"

/* private prototypes */

static void setphys(LONG addr);

/* determine monitor type, ... */

static WORD dflt_palette[] = {
    0x0777, 0x0707, 0x0070, 0x0770, 0x0007, 0x0707, 0x0077, 0x0555,
    0x0333, 0x0733, 0x0373, 0x0773, 0x0337, 0x0737, 0x0377, 0x0000,
};

/*
 * In the original TOS there used to be an early screen init, 
 * before memory configuration. This is not used here, and all is
 * done at the same time from C.
 */

VOID screen_init(VOID)
{
    volatile BYTE *rez_reg = (BYTE *) 0xffff8260;
    volatile WORD *col_regs = (WORD *) 0xffff8240;
    volatile struct {BYTE gpip;} *mfp = (void *)0xfffffa01;
    WORD rez;
    WORD i;
    
    *(BYTE *)0xffff820a = 2;  /* sync-mode to 50 hz pal, internal sync */
    
    for(i = 0 ; i < 16 ; i++) {
        col_regs[i] = dflt_palette[i];
    }
    
    rez = *rez_reg;

    rez &= 3;
    if(rez == 3) {
        rez = 2;
    }
    if((mfp->gpip & 0x80) != 0) {
      /* color monitor */
      if(rez == 2) rez = 0;
    } else {
      if(rez < 2) rez = 2;
    }
    
    *rez_reg = rez;
    sshiftmod = rez;
    
    if(rez == 1) {
        col_regs[3] = col_regs[15];
    }

    v_bas_ad = (BYTE *)(phystop - 0x8000L);
    setphys((LONG)v_bas_ad);
}

/* misc routines */

static void setphys(LONG addr)
{
    *(UBYTE *)0xffff8201 = ((ULONG)addr) >> 16;
    *(UBYTE *)0xffff8203 = ((ULONG)addr) >> 8;
}

/* xbios routines */

LONG physbase(void)
{
    LONG addr;

    addr = *(UBYTE *)0xffff8201;
    addr <<= 8;
    addr += *(UBYTE *)0xffff8203;
    addr <<= 8;
#if 0      /* The low byte only exists on STE, TT and Falcon */
    addr += *(UBYTE *)0xffff820D;  
#endif

    return(addr);
}

LONG logbase(void)
{
    return((ULONG)v_bas_ad);
}

WORD getrez(void)
{
    return( *(UBYTE *)0xffff8260 );
}


VOID setscreen(LONG logLoc, LONG physLoc, WORD rez)
{
    if(logLoc >= 0) {
      v_bas_ad = (char *)logLoc;
    }
    if(physLoc >= 0) {
      screenpt = (char *)physLoc;
      /* will be set up at next VBL */
    }
    if(rez >= 0) {
      /* rez ignored for now */
    }    
}

VOID setpalette(LONG palettePtr)
{
    /* next VBL will do this */
    colorptr = (WORD *)palettePtr;
}

WORD setcolor(WORD colorNum, WORD color)
{
  WORD rez = getrez();
  WORD max;
  WORD *palette = (WORD *)0xffff8240;
  switch(rez) {
  case 0:
    max = 15;
    break;
  case 1:
    max = 3;
    break;
  case 2:
    max = 1;
    break;
  default:
    max = 0;
  }
  if(colorNum >= 0 && colorNum <= max) {
    if(color == -1) {
      return palette[colorNum] & 0x777;
    } else {
      palette[colorNum] = color;
      return color & 0x777;
    }
  } else {
    return 0;
  }
}


VOID vsync(VOID)
{
    WORD old_sr;
    LONG a;
    volatile LONG frclock; 

    old_sr = set_sr(0x0300);   /* allow VBL interrupt */
    a = frclock;
    while(frclock == a) 
      ;
    set_sr(old_sr);
}

