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

/* interrupt control register bit usage */
#define ICR_HF0     0x08        /* host flags */
#define ICR_HF1     0x10

/* interrupt status register bit usage */
#define ISR_HF2     0x08        /* host flags */
#define ISR_HF3     0x10

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

/*
 * read/write host flags 0 or 1
 *
 * note: if you do not pass the 'read' flag value (-1), Atari TOS returns
 * d0 unmodified, i.e. the return value will be the function number.
 * this applies whether you are passing 0/1 to clear/set the flag, or
 * passing an undefined value (in which case nothing is done).
 * we do the same, and also do that when there is no DSP hardware found.
 */
WORD dsp_hf0(WORD flag)
{
    WORD ret = 0x77;    /* unimplemented xbios call: return function # */

    if (has_dsp)
    {
        switch(flag) {
        case -1:
            ret = (DSPBASE->interrupt_control & ICR_HF0) ? 1 : 0;
            break;
        case 0:
            DSPBASE->interrupt_control &= ~ICR_HF0;
            break;
        case 1:
            DSPBASE->interrupt_control |= ICR_HF0;
            break;
        }
    }

    return ret;
}

WORD dsp_hf1(WORD flag)
{
    WORD ret = 0x78;    /* unimplemented xbios call: return function # */

    if (has_dsp)
    {
        switch(flag) {
        case -1:
            ret = (DSPBASE->interrupt_control & ICR_HF1) ? 1 : 0;
            break;
        case 0:
            DSPBASE->interrupt_control &= ~ICR_HF1;
            break;
        case 1:
            DSPBASE->interrupt_control |= ICR_HF1;
            break;
        }
    }

    return ret;
}

/*
 * read host flags 2 & 3
 */
WORD dsp_hf2(void)
{
    WORD ret = 0x79;    /* unimplemented xbios call: return function # */

    if (has_dsp)
        ret = (DSPBASE->interrupt_status & ISR_HF2) ? 1 : 0;

    return ret;
}

WORD dsp_hf3(void)
{
    WORD ret = 0x7a;    /* unimplemented xbios call: return function # */

    if (has_dsp)
        ret = (DSPBASE->interrupt_status & ISR_HF3) ? 1 : 0;

    return ret;
}

/*
 * read the interrupt status register
 *
 * note: as an undocumented feature, Atari TOS 4.04 returns the contents
 * of the command vector register in d1.  we currently do not emulate this.
 */
UBYTE dsp_hstat(void)
{
    WORD ret = 0x7d;    /* unimplemented xbios call: return function # */

    if (has_dsp)
        ret = DSPBASE->interrupt_status;

    return ret;
}
#endif /* CONF_WITH_DSP */
