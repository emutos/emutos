/*
 * dsp.c - DSP routines
 *
 * Copyright (C) 2020 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "dsp.h"
#include "vectors.h"

#if CONF_WITH_DSP

struct dsp {
    UBYTE interrupt_control;    /* interrupt control register (r/w) */
    UBYTE command_vector;       /* command vector register (r/w) */
    UBYTE interrupt_status;     /* interrupt status register (r) */
    UBYTE interrupt_vector;     /* interrupt vector register (r/w) */
    UBYTE unused;
    UBYTE data_high;            /* receive/transmit high byte register (r/w) */
    UBYTE data_mid;             /* receive/transmit middle byte register (r/w) */
    UBYTE data_low;             /* receive/transmit low byte register (r/w) */
};

#define DSPBASE ((volatile struct dsp *)0xffffa200)

int has_dsp;

void detect_dsp(void)
{
    has_dsp = check_read_byte((long)&DSPBASE->interrupt_control);
    KDEBUG(("has_dsp = %d\n", has_dsp));
}
#endif /* CONF_WITH_DSP */
