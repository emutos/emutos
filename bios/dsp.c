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

#define DSP_WORD_SIZE   3       /* architectural */

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

static BOOL dsp_is_locked;

/*
 * Initialisation Routines
 */
void detect_dsp(void)
{
    has_dsp = check_read_byte((long)&DSPBASE->interrupt_control);
    KDEBUG(("has_dsp = %d\n", has_dsp));
}

void dsp_init(void)
{
    dsp_is_locked = FALSE;
}

/*
 * Data Transfer Routines
 */
WORD dsp_getwordsize(void)
{
    if (!has_dsp)
        return 0x67;    /* unimplemented xbios call: return function # */

    return DSP_WORD_SIZE;
}

/*
 * Program Control Routines
 */
WORD dsp_lock(void)
{
    if (!has_dsp)
        return 0x68;    /* unimplemented xbios call: return function # */

    if (dsp_is_locked)
        return -1;

    dsp_is_locked = TRUE;
    return 0;
}

void dsp_unlock(void)
{
    dsp_is_locked = FALSE;
}
#endif /* CONF_WITH_DSP */
