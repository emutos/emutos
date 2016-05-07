/*
 * sound.c - PSG sound routines
 *
 * Copyright (c) 2001-2016 The EmuTOS development team
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
#include "machine.h"
#include "cookie.h"

/*
 * This is a straightforward implementation of PSG-related xbios routines.
 *
 * Note: some care has to be exerted when accessing the PSG registers.
 * Areas using the PSG are:
 * - xbios sound and floppy routines
 * - floppy VBL interrupt (to deselect floppy drives, in port A)
 * - RS232 interrupts (to handle RTS/CTS hardware flow control)
 * - timer C sound interrupt (play dosound)
 * - parallel port bios device
 * - IDE reset (on Falcon), currently not used
 * - LAN/serial port select on TT/Mega STe.
 * Since accessing a PSG register is a two step operation that requires
 * first writing control, then reading or writing control or data, it is
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

/*
 * Bit 7 of PSG port A was unused on the original ST, but has
 * conflicting meanings for subsequent systems:
 *  . for the TT & Mega STe, it is normally set, to enable the serial
 *    port rather than the LAN port
 *  . for the Falcon, it is normally clear; setting it applies a reset
 *    to the IDE interface
 *
 * Ideally we would detect the LAN interface, but there seems to be no
 * way to do this directly; we just know that it's present on the TT
 * and the Mega STe.
 */
#define HAS_LAN_PORT    ((cookie_mch==MCH_TT) || (cookie_mch==MCH_MSTE))

#endif

void snd_init(void)
{
#if CONF_WITH_YM2149
    UBYTE porta_init = 0x07;    /* deselect both floppies & select side 0 */

    if (HAS_LAN_PORT)
        porta_init |= 0x80;     /* select serial, not LAN */

    /* set ports A and B to output */
    PSG->control = PSG_MULTI;
    PSG->data = PSG_PORTB_OUTPUT | PSG_PORTA_OUTPUT;

    /* initialise port A */
    PSG->control = PSG_PORT_A;
    PSG->data = porta_init;

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
    if (reg & GIACCESS_WRITE)
    {
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

    if (table >= 0)
    {
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
    BYTE *code;
    BYTE instr;

    code = sndtable;
    if (code == 0)
        return;

    if (snddelay)
    {
        snddelay--;
        return;
    }

    while((instr = *code++) >= 0)
    {
        PSG->control = instr;
        if (instr == PSG_MULTI)
        {
            UBYTE tmp = PSG->control;
            PSG->data = (tmp & PSG_PORT_MASK) | (*code++ & PSG_MIXER_MASK);
        } else {
            PSG->data = *code++;
        }
    }

    switch((UBYTE)instr) 
    {
    case 0x80:
        sndtmp = *code++;
        break;
    case 0x81:
        PSG->control = *code++;
        sndtmp += *code++;
        PSG->data = sndtmp;
        if (sndtmp != *code++)
        {
            code -= 4;
        }
        break;
    default:
        /* break; ??? */
    case 0xff:
        snddelay = *code++;
        if (snddelay == 0)
            code = 0;
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
