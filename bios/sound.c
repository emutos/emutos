/*
 * sound.c - PSG sound routines
 *
 * Copyright (c) 2001 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "sound.h"
#include "psg.h"
#include "tosvars.h"
#include "asm.h"
#include "vectors.h"

/*
 * This is a straightforward implementation of PSG-related xbios routines.
 *
 * Note: some care has to be exerted when accessing the PSG registers.
 * Areas using the PSG are:
 * - xbios sound and floppy routines
 * - floppy VBL interrupt (to deselect floppy drives, in port A)
 * - RS232 interrupts (to handle RTS/CTS hardware flow control)
 * - timer C sound interrupt (play dosound)
 * - parallel port bios device.
 * Since acessing to a PSG register is a two step operation that requires
 * first to write in control, then read or write control or data, it is
 * necessary that no interrupt which could also use the PSG occur when
 * using the PSG.
 */

/* internal routines */

static void do_bell(void);
static void do_keyclick(void);

#if CONF_WITH_YM2149

/* data used by dosound: */

static BYTE *sndtable;    /* 0xE44 */
static UBYTE snddelay;    /* 0xE48 */
static UBYTE sndtmp;      /* 0xE49 */

#endif

void snd_init(void)
{
#if CONF_WITH_YM2149
  /* set ports A and B to output */
  PSG->control = PSG_MULTI;
  PSG->data = 0xC0;
  /* deselect both floppies */
  PSG->control = PSG_PORT_A;
  PSG->data = 0x07;
  /* dosound init */
  sndtable = NULL;
#endif
  /* set bell_hook and kcl_hook */
  bell_hook = do_bell;
  kcl_hook = do_keyclick;
}

LONG giaccess(WORD data, WORD reg)
{
#if CONF_WITH_YM2149
  WORD old_sr;
  LONG value = 0;

  old_sr = set_sr(0x2700);
  PSG->control = reg & 0xF;
  if(reg & GIACCESS_WRITE) {
    PSG->data = data;
  }
  value = PSG->control;
  set_sr(old_sr);
  return value;
#else
  return 0;
#endif
}

void ongibit(WORD data)
{
#if CONF_WITH_YM2149
  WORD old_sr;
  WORD tmp;

  old_sr = set_sr(0x2700);
  PSG->control = PSG_PORT_A;
  tmp = PSG->control;
  tmp |= data;
  PSG->data = tmp;
  set_sr(old_sr);
#endif
}

void offgibit(WORD data)
{
#if CONF_WITH_YM2149
  WORD old_sr;
  WORD tmp;

  old_sr = set_sr(0x2700);
  PSG->control = PSG_PORT_A;
  tmp = PSG->control;
  tmp &= data;
  PSG->data = tmp;
  set_sr(old_sr);
#endif
}

LONG dosound(LONG table)
{
#if CONF_WITH_YM2149
  LONG oldtable = (LONG) sndtable;
  if(table >= 0) {
    sndtable = (BYTE *) table;
    snddelay = 0;
  }
  return oldtable;
#else
  return 0;
#endif
}

#if CONF_WITH_YM2149

void sndirq(void)
{
  register BYTE *code;
  register BYTE instr;

  code = sndtable;
  if(code == 0) return;
  if(snddelay) {
    snddelay --;
    return;
  }
  while((instr = *code++) >= 0) {
    PSG->control = instr;
    if(instr == PSG_MULTI) {
      UBYTE tmp = PSG->control;
      PSG->data = (tmp & 0xC0) | (*code++ & 0x3F);
    } else {
      PSG->data = *code++;
    }
  }
  switch((UBYTE)instr) {
  case 0x80:
    sndtmp = *code++;
    break;
  case 0x81:
    PSG->control = *code++;
    sndtmp += *code++;
    PSG->data = sndtmp;
    if(sndtmp != *code++) {
      code -= 4;
    }
    break;
  default:
    /* break; ??? */
  case 0xff:
    snddelay = *code++;
    if(snddelay == 0) code = 0;
    break;
  }
  sndtable = code;
}

static const UBYTE bellsnd[] = {
  0, 0x34,    /* channel A pitch */
  1, 0,
  2, 0,       /* no channel B */
  3, 0,
  4, 0,       /* no channel C */
  5, 0,
  6, 0,       /* no noise */
  7, 0xFE,    /* no sound or noise except channel A */
  8, 0x10,    /* channel A amplitude */
  9, 0,
  10, 0,
  11, 0,      /* envelope */
  12, 16,
  13, 9,
  0xFF, 0,    /* stop sound */
};

static const UBYTE keyclicksnd[] = {
  0, 0x3B,
  1, 0,
  2, 0,
  3, 0,
  4, 0,
  5, 0,
  6, 0,
  7, 0xFE,
  8, 16,
  13, 3,
  11, 0x80,
  12, 1,
  0xFF, 0,
};

#endif /* CONF_WITH_YM2149 */

void bell(void)
{
  protect_v((PFLONG) bell_hook);
}

static void do_bell(void)
{
#if CONF_WITH_YM2149
  dosound((LONG) bellsnd);
#endif
}

static void do_keyclick(void)
{
#if CONF_WITH_YM2149
  dosound((LONG) keyclicksnd);
#endif
}
