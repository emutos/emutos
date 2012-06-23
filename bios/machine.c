/*
 * machine.c - detection of machine type
 *
 * Copyright (c) 2001 - 2011 EmuTOS development team.
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#define DBG_MACHINE 0

#include "config.h"
#include "portab.h"
#include "cookie.h"
#include "machine.h"
#include "biosmem.h"
#include "vectors.h"
#include "nvram.h"
#include "tosvars.h"
#include "country.h"
#include "clock.h"
#include "natfeat.h"
#include "xhdi.h"
#include "string.h"
#include "dmasound.h"
#include "kprint.h"

long cookie_vdo;
#if CONF_WITH_FDC
long cookie_fdc;
#endif
long cookie_snd;
long cookie_mch;
#if CONF_WITH_DIP_SWITCHES
long cookie_swi;
#endif
#if CONF_WITH_ALT_RAM
long cookie_frb;
#endif


/*
 * test specific hardware features
 */

#if CONF_WITH_STE_SHIFTER
int has_ste_shifter;
#endif
#if CONF_WITH_TT_SHIFTER
int has_tt_shifter;
#endif
#if CONF_WITH_VIDEL
int has_videl;
#endif
 
/*
 * Tests video capabilities (STEnhanced Shifter, TT Shifter and VIDEL)
 */
static void detect_video(void)
{
#if CONF_WITH_STE_SHIFTER
 /* test if we have an STe Shifter by testing that register 820d
  * works (put a value, read other reg, read again, and compare)
  */
  volatile BYTE *ste_reg = (BYTE *) 0xffff820d;
  volatile BYTE *other_reg1 = (BYTE *) 0xffff8203;
  volatile WORD *other_reg2 = (WORD *) 0xffff8240;

  has_ste_shifter = 0;  
  if(!check_read_byte((long)ste_reg)) return;
  *ste_reg = 90;
  *other_reg1; /* force register read (really useful ?) */
  if(*ste_reg == 90) {
    *ste_reg = 0;
    *other_reg2; /* force register read (really useful ?) */
    if(*ste_reg == 0) {
      has_ste_shifter = 1;
    } 
  }

#if DBG_MACHINE
  kprintf("has_ste_shifter = %d\n", has_ste_shifter);
#endif
#endif

#if CONF_WITH_TT_SHIFTER
  /* test if we have a TT Shifter by testing for TT color palette */
  has_tt_shifter = 0;
  if (check_read_byte(0xffff8400))
    has_tt_shifter = 1;

#if DBG_MACHINE
  kprintf("has_tt_shifter = %d\n", has_tt_shifter);
#endif
#endif

#if CONF_WITH_VIDEL
  /* test if we have Falcon VIDEL by testing for f030_xreg */
  has_videl = 0;
  if (check_read_byte(0xffff8282))
    has_videl = 1;

#if DBG_MACHINE
  kprintf("has_videl = %d\n", has_videl);
#endif
#endif
}

#if CONF_WITH_VME

/* vme */

int has_vme;

static void detect_vme(void)
{
  volatile BYTE *vme_mask = (BYTE *) 0xffff8e0d;
  volatile BYTE *sys_mask = (BYTE *) 0xffff8e01;
  
  if(check_read_byte(0xffff8e09)) {
    *vme_mask = 0x40;  /* ??? IRQ3 from VMEBUS/soft */
    *sys_mask = 0x14;  /* ??? set VSYNC and HSYNC */
    has_vme = 1;
  } else {
    has_vme = 0;
  }

#if DBG_MACHINE
  kprintf("has_vme = %d\n", has_vme);
#endif
}

#endif /* CONF_WITH_VME */


#if CONF_WITH_BLITTER

/* blitter */

int has_blitter;

static void detect_blitter(void)
{
  has_blitter = 0;

  if (check_read_byte(0xffff8a3c))
    has_blitter = 1;

#if DBG_MACHINE
  kprintf("has_blitter = %d\n",has_blitter);
#endif
}

#endif /* CONF_WITH_BLITTER */


#if CONF_WITH_DIP_SWITCHES

/* DIP switches */

static void setvalue_swi(void)
{
  cookie_swi = 0x7F;
  if (check_read_byte(0xffff9201)) {
    cookie_swi = (*(volatile WORD *)0xffff9200)>>8;
  }

#if DBG_MACHINE
  kprintf("cookie_swi = 0x%08lx\n", cookie_swi);
#endif
}

#endif /* CONF_WITH_DIP_SWITCHES */

/* video type */

static void setvalue_vdo(void)
{
#if CONF_WITH_VIDEL
  if(has_videl) {
    cookie_vdo = 0x00030000L;
  }
  else
#endif
#if CONF_WITH_TT_SHIFTER
  if(has_tt_shifter) {
    cookie_vdo = 0x00020000L;
  }
  else
#endif
#if CONF_WITH_STE_SHIFTER
  if(has_ste_shifter) {
    cookie_vdo = 0x00010000L;
  }
  else
#endif
  {
    cookie_vdo = 0x00000000L;
  } 

#if DBG_MACHINE
  kprintf("cookie_vdo = 0x%08lx\n", cookie_vdo);
#endif
}

#if CONF_ATARI_HARDWARE
/* detect ARAnyM */
#define ARANYM_NAME     "aranym"
static int is_it_aranym(void)
{
  return (strncasecmp(machine_name(), ARANYM_NAME, strlen(ARANYM_NAME)) == 0);
}
#endif /* CONF_ATARI_HARDWARE */

/* machine type */
static void setvalue_mch(void)
{
#if CONF_ATARI_HARDWARE
  if (is_it_aranym()) {
    cookie_mch = MCH_ARANYM;
  }
#if CONF_WITH_VIDEL
  else if(has_videl) {
    cookie_mch = MCH_FALCON;
  }
#endif
#if CONF_WITH_TT_SHIFTER
  else if(has_tt_shifter) {
    cookie_mch = MCH_TT;
  }
#endif
#if CONF_WITH_STE_SHIFTER
  else if(has_ste_shifter) {
#if CONF_WITH_VME
    if(has_vme) {
      cookie_mch = MCH_MSTE;
    }
    else
#endif /* CONF_WITH_VME */
    {
      cookie_mch = MCH_STE;
    }
  }
#endif
  else {
    cookie_mch = MCH_ST;
  }
#else
  cookie_mch = MCH_NOHARD;
#endif /* CONF_ATARI_HARDWARE */

#if DBG_MACHINE
  kprintf("cookie_mch = 0x%08lx\n", cookie_mch);
#endif
}

/* SND */

static void setvalue_snd(void)
{
  cookie_snd = 0;

#if CONF_WITH_YM2149
  cookie_snd |= SND_PSG;
#endif

#if CONF_WITH_DMASOUND
  if (has_dmasound) {
    cookie_snd |= SND_8BIT;
  }

  if (has_falcon_dmasound) {
    /* FIXME: Enable the following when Falcon XBIOS functions are implemented */
    /*cookie_snd |= SND_16BIT | SND_MATRIX;*/
  }
#endif

#if DBG_MACHINE
  kprintf("cookie_snd = 0x%08lx\n", cookie_snd);
#endif
}
  

#if CONF_WITH_FDC

/* FDC */

static void setvalue_fdc(void)
{
  /* LVL - This is what I understood of my search on 
   * comp.sys.atari.st.tech, archives, but I do not claim 
   * this to be fully accurate. 
   */
#if CONF_WITH_DIP_SWITCHES
  if(cookie_swi & 0x40) {
    cookie_fdc = FDC_1ATC;
  } else
#endif
  {
    cookie_fdc = FDC_0ATC;
  } 

#if DBG_MACHINE
  kprintf("cookie_fdc = 0x%08lx\n", cookie_fdc);
#endif
}

#endif /* CONF_WITH_FDC */

void machine_detect(void)
{
  detect_video();
#if CONF_WITH_VME
  detect_vme();
#endif
#if CONF_WITH_MEGARTC
  detect_megartc();
#if DBG_MACHINE
  kprintf("has_megartc = %d\n", has_megartc);
#endif /* DBG_MACHINE */
#endif /* CONF_WITH_MEGARTC */
#if CONF_WITH_NVRAM
  detect_nvram();
#endif
#if CONF_WITH_DMASOUND
  detect_dmasound();
#endif
#if CONF_WITH_BLITTER
  detect_blitter();
#endif
}
  
void machine_init(void)
{
#ifdef __mcoldfire__
  cookie_add(COOKIE_COLDFIRE, 0);
#else
  /* this is detected by detect_cpu(), called from processor_init() */
  cookie_add(COOKIE_CPU, mcpu);
#endif

  /* _VDO
   * This cookie represents the revision of the video shifter present. 
   * Currently valid values are: 
   * 0x00000000  ST 
   * 0x00010000  STe 
   * 0x00020000  TT030 
   * 0x00030000  Falcon030 
   */

  setvalue_vdo();
  cookie_add(COOKIE_VDO, cookie_vdo);

#ifndef __mcoldfire__
  /* this is detected by detect_fpu(), called from processor_init() */
  cookie_add(COOKIE_FPU, fputype);
#endif

  /* _MCH */
  setvalue_mch();
  cookie_add(COOKIE_MCH, cookie_mch);

#if CONF_WITH_DIP_SWITCHES
  /* _SWI  On machines that contain internal configuration dip switches, 
   * this value specifies their positions as a bitmap. Dip switches are 
   * generally used to indicate the presence of additional hardware which 
   * will be represented by other cookies.  
   */
  if (cookie_mch == MCH_MSTE || cookie_mch == MCH_TT
      || cookie_mch == MCH_FALCON) {
    setvalue_swi();
    cookie_add(COOKIE_SWI, cookie_swi);
  }
#endif

  /* _SND
   * This cookie contains a bitmap of sound features available to the 
   * system as follows:  
   * 0x01 GI Sound Chip (PSG) 
   * 0x02 1 Stereo 8-bit Playback 
   * 0x04 DMA Record (w/XBIOS) 
   * 0x08 16-bit CODEC 
   * 0x10 DSP 
   */
  
  setvalue_snd();
  cookie_add(COOKIE_SND, cookie_snd);

#if CONF_WITH_ALT_RAM
  /* _FRB  This cookie is present when alternative RAM is present. It 
   * points to a 64k buffer that may be used by DMA device drivers to 
   * transfer memory between alternative RAM and ST RAM for DMA operations.  
   */
  if (ramtop > 0) {
    cookie_frb = (long)balloc(64 * 1024UL);
#if DBG_MACHINE
    kprintf("cookie_frb = 0x%08lx\n", cookie_frb);
#endif
    cookie_add(COOKIE_FRB, cookie_frb);
  }
  else {
    cookie_frb = 0;
  }
#endif /* CONF_WITH_ALT_RAM */
   
  /* _FLK  The presence of this cookie indicates that file and record 
   * locking extensions to GEMDOS exist. The value field is a version 
   * number currently undefined.  
   */
  
  /* _IDT This cookie defines the currently configured date and time 
   * format, Bits #0-7 contain the ASCII code of the date separator. 
   * Bits #8-11 contain a value indicating the date display format as 
   * follows:  
   *   0 MM-DD-YY 
   *   1 DD-MM-YY 
   *   2 YY-MM-DD 
   *   3 YY-DD-MM
   * Bits #12-15 contain a value indicating the time format as follows:  
   *   0 12 hour 
   *   1 24 hour
   * Note: The value of this cookie does not affect any of the internal 
   * time functions. It is intended for informational use by applications 
   */

  /* _AKP  This cookie indicates the presence of an Advanced Keyboard 
   * Processor. The high word of this cookie is currently reserved. 
   * The low word indicates the language currently used by TOS for 
   * keyboard interpretation and alerts. 
   */
  
  detect_akp();
#if DBG_MACHINE
    kprintf("cookie_akp = 0x%08lx\n", cookie_akp);
#endif
  cookie_add(COOKIE_AKP, cookie_akp);

  detect_idt();
#if DBG_MACHINE
    kprintf("cookie_idt = 0x%08lx\n", cookie_idt);
#endif
  cookie_add(COOKIE_IDT, cookie_idt);
  
#if CONF_WITH_FDC
  /* Floppy Drive Controller 
   * Most significant byte means: 
   * 0 - DD (Normal floppy interface)
   * 1 - HD (1.44 MB with 3.5")
   * 2 - ED (2.88 MB with 3.5")
   * the 3 other bytes are the Controller ID:
   * 0 - No information available
   * 'ATC' - Fully compatible interface built in a way that
   * behaves like part of the system.
   */

  setvalue_fdc();
  cookie_add(COOKIE_FDC, cookie_fdc);
#endif

#if DETECT_NATIVE_FEATURES
  if (has_natfeats()) {
    cookie_add(COOKIE_NATFEAT, (long)&natfeat_cookie);
  }
#endif

#if CONF_WITH_XHDI
  create_XHDI_cookie();
#endif
}

const char * machine_name(void)
{
#ifdef MACHINE_FIREBEE
  return "FireBee";
#elif defined(MACHINE_AMIGA)
  return "Amiga";
#else

#if DETECT_NATIVE_FEATURES
  static char buffer[80];
  long bufsize;

  bufsize = nfGetFullName(buffer, sizeof(buffer)-1);
  if (bufsize > 0)
    return buffer;
#endif

  switch(cookie_mch) {
  case MCH_ST:
#if CONF_WITH_MEGARTC
    if(has_megartc) {
      return "MegaST";
    }
    else
#endif /* CONF_WITH_MEGARTC */
    {
      return "ST";
    }
  case MCH_STE: return "STe";
  case MCH_MSTE: return "MegaSTe";
  case MCH_TT: return "TT";
  case MCH_FALCON: return "Falcon";
  default: return "unknown";
  } 
#endif
}
