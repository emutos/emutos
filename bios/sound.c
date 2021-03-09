/*
 * sound.c - PSG sound routines
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
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

static const UBYTE *sndtable;
static UBYTE snddelay;
static UBYTE sndtmp;

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

LONG dosound(const UBYTE *table)
{
#if CONF_WITH_YM2149
    const UBYTE *oldtable = sndtable;

    if ((LONG)table >= 0)
    {
        sndtable = table;
        snddelay = 0;
    }

    return (LONG)oldtable;
#else
    return 0;
#endif
}

#if CONF_WITH_YM2149
void sndirq(void)
{
    const UBYTE *code;
    UBYTE instr, data;

    code = sndtable;
    if (code == NULL)
        return;

    if (snddelay)
    {
        snddelay--;
        return;
    }

    while((instr = *code++) <= 0x80)
    {
        data = *code++;
        if (instr == 0x80)
        {
            sndtmp = data;          /* starting register value for 0x81 command */
            continue;
        }
        /* other values are assumed to be PSG register numbers (0-15) */
        PSG->control = instr;
        if (instr == PSG_MULTI)
        {
            UBYTE tmp = PSG->control;
            PSG->data = (tmp & PSG_PORT_MASK) | (data & PSG_MIXER_MASK);
        } else {
            PSG->data = data;
        }
    }

    if (instr == 0x81)
    {
        PSG->control = *code++;     /* register number */
        sndtmp += *code++;          /* increment register value */
        PSG->data = sndtmp;         /*  & send to register      */
        if (sndtmp != *code++)      /* if current value != ending value, */
            code -= 4;              /*  rewind to run again next time    */
    }
    else                    /* all remaining commands (> 0x81) just set the delay */
    {
        snddelay = *code++;
        if (snddelay == 0)
            code = NULL;        /* end play */
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
  0xFF, 0    /* stop sound */
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
  0xFF, 0
};

static const UBYTE coldbootsnd[] = {
  13, 00,  /* Env: attack */
  11, 0,
  12, 12,
  2, 0xef, /* C5 */
  3, 0x00,
  7, 0xfd, /* B ON */
  9,0x12,  /* Volume */
  0x82, 6,
  13,00,
  2,0xdd, /* C4 */
  3,0x01,
  0x82,6,
  13,00,
  2,0x3e, /* G4 */
  3,0x01,
  0x82,6,
  13,00,
  2,0xef,
  3,0x00,	
  0x82,6,
  7,0xf7,
  0xff,00
};

static const UBYTE warmbootsnd[] = {
  11,0,
  12,10,
  13,00,	/* Env: attack */
  2,0xef,	/* C5 */
  3,0x00,
  7,0xfd,	/* B ON */
  9,0x12,	/* Volume */
  0x82, 6,
  13,0,
  0x82,6,
  13, 0,
  0x82, 6,
  0xff, 00	
};

#endif /* CONF_WITH_YM2149 */

void bell(void)
{
    protect_v((PFLONG) bell_hook);
}

static void do_bell(void)
{
#if CONF_WITH_YM2149
    dosound(bellsnd);
#endif
}

static void do_keyclick(void)
{
#if CONF_WITH_YM2149
    dosound(keyclicksnd);
#endif
}

void coldbootsound(void) {
#if CONF_WITH_YM2149
    dosound(coldbootsnd);
#endif
}

void warmbootsound(void) {
#if CONF_WITH_YM2149
    dosound(warmbootsnd);
#endif
}