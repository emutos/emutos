/*
 *  chardev.c - BIOS character device functions
 *
 * Copyright (C) 2001-2020 The EmuTOS development team
 *
 * Authors:
 *  THH     Thomas Huth
 *  LVL     Laurent Vogel
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "tosvars.h"
#include "chardev.h"
#include "conout.h"
#include "vt52.h"
#include "bios.h"
#include "asm.h"
#include "serport.h"
#include "ikbd.h"
#include "midi.h"
#include "parport.h"

#define NUM_CHAR_VECS   8

static LONG (* const bconstat_init[NUM_CHAR_VECS])(void) =
    { char_dummy, bconstat1, bconstat2, bconstat3,
      char_dummy, char_dummy, char_dummy, char_dummy };

static LONG (* const bconin_init[NUM_CHAR_VECS])(void) =
    { bconin0, bconin1, bconin2, bconin3,
      char_dummy, char_dummy, char_dummy, char_dummy };

static LONG (* const bcostat_init[NUM_CHAR_VECS])(void) =
    { bcostat0, bcostat1, bcostat2, bcostat4,
      /* note that IKBD and MIDI bcostat() are swapped! */
      bcostat3, char_dummy, char_dummy, char_dummy };

static LONG (* const bconout_init[NUM_CHAR_VECS])(WORD,WORD) =
    { bconout0, bconout1, bconout2, bconout3,
      bconout4, bconout5, charout_dummy, charout_dummy };

/*
 * dummy - unimplemented functions
 */
LONG char_dummy(void)
{
    return 0L;
}

LONG charout_dummy(WORD dev,WORD b)
{
    return 0L;
}


/*==== BIOS device vector initialization =============================*/

void chardev_init(void)
{
int i;

    /* initialise bios device vectors */
    for (i = 0; i < NUM_CHAR_VECS; i++) {
        bconstat_vec[i] = bconstat_init[i];
        bconin_vec[i] = bconin_init[i];
        bcostat_vec[i] = bcostat_init[i];
        bconout_vec[i] = bconout_init[i];
    }

    /* setup serial output functions */
    aux_stat = just_rts;
    aux_vec = just_rts;

    /* setup parallel output functions */
    prt_stat = just_rts;
    prt_vec = just_rts;
    dump_vec = just_rts;
}



/* BIOS devices - bconout functions */

LONG bconout2(WORD dev, WORD b)
{
    cputc(b);
    return 1L;
}

/* bconout5 - raw console output. */
LONG bconout5(WORD dev, WORD ch)
{
#if CONF_SERIAL_CONSOLE
    /* The terminal will interpret the control characters, anyway */
    bconout(1, ch);
#endif
    ascii_out(LOBYTE(ch));
    return 1L;
}



/* BIOS devices - bcostat functions */

LONG bcostat2(void)
{
#if CONF_SERIAL_CONSOLE
    return bcostat(1);
#endif
  return -1;
}
