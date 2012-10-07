/*
 *  chardev.c - BIOS character device funtions
 *
 * Copyright (c) 2001-2008 by the EmuTOS Development Team
 *
 * Authors:
 *  THO     Thomas Huth
 *  LVL     Laurent Vogel
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "config.h"
#include "portab.h"
#include "gemerror.h"
#include "kprint.h"
#include "tosvars.h"
#include "chardev.h"
#include "conout.h"
#include "vt52.h"
#include "mfp.h"


/*==== Defines ============================================================*/

#define DBGBIOSC TRUE

/*==== BIOS initialization ================================================*/
/*
 * called from startup.s, this routine will do necessary bios initialization
 * that can be done in hi level lang.  startup.s has the rest.
 */

void chardev_init(void)
{
    /* initialise bios device functions */

    bconstat_vec[0] = bconstat0;
    bconstat_vec[1] = bconstat1;
    bconstat_vec[2] = bconstat2;
    bconstat_vec[3] = bconstat4;    /* IKBD and MIDI bconstat swapped */
    bconstat_vec[4] = bconstat3;
    bconstat_vec[5] = bconstat5;
    bconstat_vec[6] = bconstat6;
    bconstat_vec[7] = bconstat7;

    bconin_vec[0] = bconin0;
    bconin_vec[1] = bconin1;
    bconin_vec[2] = bconin2;
    bconin_vec[3] = bconin3;
    bconin_vec[4] = bconin4;
    bconin_vec[5] = bconin5;
    bconin_vec[6] = bconin6;
    bconin_vec[7] = bconin7;
    
    bconout_vec[0] = bconout0;
    bconout_vec[1] = bconout1;
    bconout_vec[2] = bconout2;
    bconout_vec[3] = bconout3;
    bconout_vec[4] = bconout4;
    bconout_vec[5] = bconout5;
    bconout_vec[6] = bconout6;
    bconout_vec[7] = bconout7;

    bcostat_vec[0] = bcostat0;
    bcostat_vec[1] = bcostat1;
    bcostat_vec[2] = bcostat2;
    bcostat_vec[3] = bcostat3;
    bcostat_vec[4] = bcostat4;
    bcostat_vec[5] = bcostat5;
    bcostat_vec[6] = bcostat6;
    bcostat_vec[7] = bcostat7;

    /* setup serial output functions */
    aux_stat = dummy;
    aux_vec = dummy;

    /* setup parallel output functions */
    prt_stat = dummy;
    prt_vec = dummy;
    dump_vec = dummy;
}



/* BIOS devices - bconstat functions */

LONG bconstat4(void)
{
  return 0;
}

LONG bconstat5(void)
{
  return 0;
}

LONG bconstat6(void)
{
  return 0;
}

LONG bconstat7(void)
{
  return 0;
}



/* BIOS devices - bconin functions */

LONG bconin4(void)
{
  return 0;
}
LONG bconin5(void)
{
  return 0;
}
LONG bconin6(void)
{
  return 0;
}
LONG bconin7(void)
{
  return 0;
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
    ascii_out(ch);
    return 1L;
}

LONG bconout6(WORD dev, WORD b)
{
    return 1L;
}

LONG bconout7(WORD dev, WORD b)
{
    return 1L;
}



/* BIOS devices - bcostat functions */

LONG bcostat2(void)
{
  return -1;
}

LONG bcostat5(void)
{
  return -1;
}

LONG bcostat6(void)
{
  return -1;
}

LONG bcostat7(void)
{
  return -1;
}



/*
 * dummy - not implemented functions
 */

void dummy(void)
{
    return;
}
