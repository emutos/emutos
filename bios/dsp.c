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
    union
    {
        LONG full;              /* receive/transmit as a long (r/w) */
        struct
        {
            UBYTE unused;
            UBYTE high;         /* receive/transmit high byte register (r/w) */
            UBYTE mid;          /* receive/transmit middle byte register (r/w) */
            UBYTE low;          /* receive/transmit low byte register (r/w) */
        } d;
    } data;
};

#define DSPBASE ((volatile struct dsp *)0xffffa200)

/* interrupt control register bit usage */
#define ICR_HF0     0x08        /* host flags */
#define ICR_HF1     0x10

/* interrupt status register bit usage */
#define ISR_RXDF    0x01        /* receive data register full */
#define ISR_TXDE    0x02        /* transmit data register empty */
#define ISR_HF2     0x08        /* host flags */
#define ISR_HF3     0x10

/* some useful macros */
#define DSP_RCV_READY   (DSPBASE->interrupt_status&ISR_RXDF)
#define DSP_SEND_READY  (DSPBASE->interrupt_status&ISR_TXDE)

#define DSP_WAIT_RCV()  { while(!DSP_RCV_READY) ; }
#define DSP_WAIT_SEND() { while(!DSP_SEND_READY) ; }


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
 ***** Data Transfer Routines *****
 */

/*
 * Dsp_DoBlock(): send and/or receive DSP words
 *
 * (optionally) send a block of DSP words with no handshaking, then
 * (optionally) receive a block of DSP words with no handshaking
 */
void dsp_doblock(char *send, LONG sendlen, char *rcv, LONG rcvlen)
{
    if (!has_dsp)
        return;

    /* send data first */
    if (sendlen)
    {
        /* wait for previous send to complete, then send blind */
        DSP_WAIT_SEND();
        do
        {
            DSPBASE->data.d.high = *send++;
            DSPBASE->data.d.mid = *send++;
            DSPBASE->data.d.low = *send++;
        } while(--sendlen);
    }

    if (rcvlen)
    {
        /* wait for data to be available, then receive blind */
        DSP_WAIT_RCV();
        do
        {
            *rcv++ = DSPBASE->data.d.high;
            *rcv++ = DSPBASE->data.d.mid;
            *rcv++ = DSPBASE->data.d.low;
        } while(--rcvlen);
    }
}

/*
 * Dsp_BlkHandshake(): send and/or receive DSP words with handshaking
 *
 * (optionally) send a block of DSP words with handshaking, then
 * (optionally) receive a block of DSP words with handshaking
 */
void dsp_blkhandshake(char *send, LONG sendlen, char *rcv, LONG rcvlen)
{
    if (!has_dsp)
        return;

    /* send data first */
    if (sendlen)
    {
        do
        {
            /* wait for previous send to complete */
            DSP_WAIT_SEND();
            DSPBASE->data.d.high = *send++;
            DSPBASE->data.d.mid = *send++;
            DSPBASE->data.d.low = *send++;
        } while(--sendlen);
    }

    if (rcvlen)
    {
        do
        {
            /* wait for data to be available */
            DSP_WAIT_RCV();
            *rcv++ = DSPBASE->data.d.high;
            *rcv++ = DSPBASE->data.d.mid;
            *rcv++ = DSPBASE->data.d.low;
        } while(--rcvlen);
    }
}

/*
 * Dsp_BlkUnpacked(): send and/or receive LONGs
 *
 * (optionally) send a block of LONG data with no handshaking, then
 * (optionally) receive a block of LONG data with no handshaking
 */
void dsp_blkunpacked(LONG *send, LONG sendlen, LONG *rcv, LONG rcvlen)
{
    if (!has_dsp)
        return;

    /* send data first */
    if (sendlen)
    {
        /* wait for previous send to complete, then send blind */
        DSP_WAIT_SEND();
        do
        {
            DSPBASE->data.full = *send++;
        } while(--sendlen);
    }

    if (rcvlen)
    {
        /* wait for data to be available, then receive blind */
        DSP_WAIT_RCV();
        do
        {
            *rcv++ = DSPBASE->data.full;
        } while(--rcvlen);
    }
}

/*
 * Dsp_GetWordSize
 *
 * return the size of a DSP word (always 3 for Atari)
 */
WORD dsp_getwordsize(void)
{
    if (!has_dsp)
        return 0x67;    /* unimplemented xbios call: return function # */

    return DSP_WORD_SIZE;
}

/*
 * Dsp_BlkWords(); send and/or receive WORDS
 *
 * (optionally) send a block of WORD data with no handshaking, then
 * (optionally) receive a block of WORD data with no handshaking
 */
void dsp_blkwords(WORD *send, LONG sendlen, WORD *rcv, LONG rcvlen)
{
    if (!has_dsp)
        return;

    /* send data first */
    if (sendlen)
    {
        /* wait for previous send to complete, then send blind */
        DSP_WAIT_SEND();
        do
        {
            DSPBASE->data.full = *send++;   /* sign extend by design */
        } while(--sendlen);
    }

    if (rcvlen)
    {
        UBYTE *p = (UBYTE *)rcv;

        /* wait for data to be available, then receive blind */
        DSP_WAIT_RCV();
        do
        {
            *p++ = DSPBASE->data.d.mid;     /* avoid sign issues */
            *p++ = DSPBASE->data.d.low;
        } while(--rcvlen);
    }
}

/*
 * Dsp_BlkBytes(): send and/or receive unsigned chars
 *
 * (optionally) send a block of unsigned chars with no handshaking, then
 * (optionally) receive a block of unsigned chars with no handshaking
 */
void dsp_blkbytes(UBYTE *send, LONG sendlen, UBYTE *rcv, LONG rcvlen)
{
    if (!has_dsp)
        return;

    /* send data first */
    if (sendlen)
    {
        /* wait for previous send to complete, then send blind */
        DSP_WAIT_SEND();
        do
        {
            DSPBASE->data.d.high = 0;
            DSPBASE->data.d.mid = 0;
            DSPBASE->data.d.low = *send++;
        } while(--sendlen);
    }

    if (rcvlen)
    {
        /* wait for data to be available, then receive blind */
        DSP_WAIT_RCV();
        do
        {
            UBYTE dummy;
            UNUSED(dummy);
            dummy = DSPBASE->data.d.mid;    /* like TOS, do a dummy read */
            *rcv++ = DSPBASE->data.d.low;
        } while(--rcvlen);
    }
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

/*
 * Dsp_MultBlocks(): send and/or receive multiple data blocks
 *
 * (optionally) send data blocks with no handshaking, then
 * (optionally) receive data blocks with no handshaking
 */
void dsp_multblocks(LONG sendnum, LONG rcvnum, DSPBLOCK *sendinfo, DSPBLOCK *rcvinfo)
{
    if (!has_dsp)
        return;

    /* send data first */
    if (sendnum)
    {
        /* wait for previous send to complete, then send blind */
        DSP_WAIT_SEND();
        do
        {
            LONG count = sendinfo->blocksize;
            switch(sendinfo->blocktype) {
            case BT_LONG:
                {
                    LONG *longptr = sendinfo->blockaddr;
                    do
                    {
                        DSPBASE->data.full = *longptr++;
                    } while(--count);
                }
                break;
            case BT_WORD:
                {
                    WORD *wordptr = sendinfo->blockaddr;
                    do
                    {
                        DSPBASE->data.full = *wordptr++;    /* sign extend by design */
                    } while(--count);
                }
                break;
            case BT_BYTE:
                {
                    UBYTE *byteptr = sendinfo->blockaddr;
                    do
                    {
                        DSPBASE->data.d.high = 0;
                        DSPBASE->data.d.mid = 0;
                        DSPBASE->data.d.low = *byteptr++;
                    } while(--count);
                }
                break;
            }
            sendinfo++;     /* up to next */
        } while(--sendnum);
    }

    if (rcvnum)
    {
        /* wait for data to be available, then receive blind */
        DSP_WAIT_RCV();
        do
        {
            LONG count = rcvinfo->blocksize;
            switch(rcvinfo->blocktype) {
            case BT_LONG:
                {
                    LONG *longptr = rcvinfo->blockaddr;
                    do
                    {
                        *longptr++ = DSPBASE->data.full;
                    } while(--count);
                }
                break;
            case BT_WORD:
                {
                    UBYTE *byteptr = rcvinfo->blockaddr;
                    do
                    {
                        *byteptr++ = DSPBASE->data.d.mid;   /* avoid sign issues */
                        *byteptr++ = DSPBASE->data.d.low;
                    } while(--count);
                }
                break;
            case BT_BYTE:
                {
                    UBYTE *byteptr = rcvinfo->blockaddr;
                    do
                    {
                        UBYTE dummy;
                        UNUSED(dummy);
                        dummy = DSPBASE->data.d.mid;    /* like TOS, do a dummy read */
                        *byteptr++ = DSPBASE->data.d.low;
                    } while(--count);
                }
                break;
            }
            rcvinfo++;     /* up to next */
        } while(--rcvnum);
    }
}

#endif /* CONF_WITH_DSP */
