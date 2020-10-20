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
#include "asm.h"
#include "dsp.h"
#include "psg.h"
#include "tosvars.h"
#include "vectors.h"
#include "gemdos.h"
#include "bdosdefs.h"
#include "string.h"

#if CONF_WITH_DSP

#define DSP_WORD_SIZE   3       /* architectural */
#define DSP_VECTOR_SIZE 2

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
#define ICR_RREQ    0x01        /* enable receive interrupt (DSP is ready to send) */
#define ICR_TREQ    0x02        /* enable transmit interrupt (DSP is ready to receive) */
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

/* DSP interrupt vector - use the same one as TOS4.04 */
#define DSP_INTNUM  0xff
#define VEC_DSP     (*(volatile PFVOID*)(DSP_INTNUM*sizeof(LONG)))

/* time (in ticks) to assert DSP RESET signal */
#define DSP_RESET_TIME  2

/* maximum sizes (in DSP words) */
#define DSP_BOOTSTRAP_SIZE  512     /* for Dsp_ExecBoot() */
#define DSP_SUBROUTINE_SIZE 1024    /* for Dsp_LoadSubroutine() */

/* number of DSP subroutines supported */
#define NUM_SYSTEM_SUBROUTINES  2
#define NUM_USER_SUBROUTINES    8

/* callable DSP vector info */
#define LOADSUB_VECTOR      0x15        /* used by system */
#define BLOCKMOVE_VECTOR    0x16
#define FIRST_SUBRTN_VECTOR 0x17        /* for users */
#define NUM_DSP_VECTORS     (NUM_SYSTEM_SUBROUTINES+NUM_USER_SUBROUTINES)
#define SIZE_DSP_VECTORS    (NUM_DSP_VECTORS*DSP_VECTOR_SIZE)
#define LOADSUB_ADDR        (LOADSUB_VECTOR*DSP_VECTOR_SIZE)
#define FIRST_SUBRTN_ADDR   (FIRST_SUBRTN_VECTOR*DSP_VECTOR_SIZE)

/* DSP memory sizes */
#define YSIZE       0x4000      /* 16K, overlapping the bottom of P memory */
#define PSIZE       0x7ea9      /* must agree with the value hardcoded in   */
                                /* dspboot.c/dspprog.c/dspstart.c, which is */
                                /* the value PROGLOAD in tools/dsp/dsp.inc  */

/*
 * DSP code
 */
static const UBYTE dspboot[] = {    /* loaded into DSP low memory by firmware bootstrap */
#include "dspboot.c"
};
#define DSPBOOT_SIZE    (sizeof(dspboot) / DSP_WORD_SIZE)

static const UBYTE dspprog[] = {    /* loaded into DSP high memory by dspboot */
#include "dspprog.c"
};
#define DSPPROG_SIZE    (sizeof(dspprog) / DSP_WORD_SIZE)

static const UBYTE dspstart[] = {   /* start/restart DSPPROG */
#include "dspstart.c"
};
#define DSPSTART_SIZE   (sizeof(dspstart) / DSP_WORD_SIZE)

static const UBYTE dspvect[] = {    /* vectors to be loaded using DSPPROG */
                                        /* header expected by DSPPROG */
    0x00, 0x00, 0x00,                       /* memory type = P */
    0x00, 0x00, LOADSUB_ADDR,               /* memory address */
    0x00, 0x00, SIZE_DSP_VECTORS,           /* size in DSP words */
#include "dspvect.c"                    /* skeleton vector data for vectors $15->$1e */
    0x00, 0x00, 0x03                    /* end of transmission indicator */
};
#define DSPVECT_SIZE    (sizeof(dspvect) / DSP_WORD_SIZE)
/* offset to user subroutine vector data, in DSP words */
#define DSPVECT_USERVEC_OFFSET  (3+NUM_SYSTEM_SUBROUTINES*DSP_VECTOR_SIZE)

/*
 * subroutine information structure
 */
typedef struct {
    UWORD start;        /* start address */
    UWORD size;         /* length */
    WORD ability;       /* associated 'ability' */
} SUBINFO;


/*
 * some global/local variables
 */
int has_dsp;

static BOOL dsp_is_locked;
static WORD program_ability;
static WORD unique_ability;
static LONG program_top;        /* end address of program + 1 */
static LONG subroutine_bottom;  /* start address of lowest subroutine */
static WORD oldest_sub;         /* index of next subroutine to replace */

static UBYTE dspvect_ram[DSPVECT_SIZE*DSP_WORD_SIZE];

/*
 * subroutine information table
 *
 * this is a circular queue, whose oldest member (the next one to be
 * replaced) always references the topmost subroutine in memory.
 * note:
 * . the handle for a subroutine is the same as its associated vector
 * . the vector for a subroutine in entry i is (i+FIRST_SUBRTN_VECTOR)
 */
static SUBINFO subroutine[NUM_USER_SUBROUTINES];

/* the following private structure is used by interrupt handling */
static struct {
                    /* send info */
    char *send;         /* buffer ptr */
    LONG sendlen;       /* length */
    LONG sendblocks;    /* blocks to send */
    LONG *senddone;     /* ptr to 'blocks sent' */
                    /* receive info */
    char *rcv;          /* buffer ptr */
    LONG rcvlen;        /* length */
    LONG rcvblocks;     /* blocks to receive */
    LONG *rcvdone;      /* ptr to 'blocks received' */
} ih_args;

/* function pointers used by Dsp_SetVectors() */
static void (*user_rcv)(LONG data);     /* called to pass data from DSP to user */
static LONG (*user_send)(void);         /* called to get data to pass to DSP */

/****************************************************
 *                                                  *
 *  I N I T I A L I S A T I O N   R O U T I N E S   *
 *                                                  *
 ****************************************************/
void detect_dsp(void)
{
    has_dsp = check_read_byte((long)&DSPBASE->interrupt_control);
    KDEBUG(("has_dsp = %d\n", has_dsp));
}

void dsp_init(void)
{
    if (!has_dsp)
        return;

    dsp_is_locked = FALSE;
    memcpy(dspvect_ram, dspvect, sizeof(dspvect));  /* initialise RAM copy of vector data */
    dsp_flushsubroutines();     /* reset subroutine info table & memory info */
    program_top = 0;
    program_ability = 0;
    unique_ability = -32768;    /* just like TOS */

    /* load the program loader into DSP memory */
    dsp_execboot(dspboot, DSPBOOT_SIZE, 0);
    dsp_doblock(dspprog, DSPPROG_SIZE, NULL, 0);
}

/****************************************************
 *                                                  *
 *  D A T A   T R A N S F E R   R O U T I N E S     *
 *                                                  *
 ****************************************************/

/*
 * Dsp_DoBlock(): send and/or receive DSP words
 *
 * (optionally) send a block of DSP words with no handshaking, then
 * (optionally) receive a block of DSP words with no handshaking
 */
void dsp_doblock(const UBYTE *send, LONG sendlen, char *rcv, LONG rcvlen)
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
 * Dsp_BlkHandShake(): send and/or receive DSP words with handshaking
 *
 * (optionally) send a block of DSP words with handshaking, then
 * (optionally) receive a block of DSP words with handshaking
 */
void dsp_blkhandshake(const UBYTE *send, LONG sendlen, char *rcv, LONG rcvlen)
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
 * Dsp_InStream(): send DSP words (no handshaking) via an interrupt handler
 */
void dsp_instream(char *data, LONG datalen, LONG numblocks, LONG *blocksdone)
{
    if (!has_dsp)
        return;

    /* save args for use by interrupt handler */
    ih_args.send = data;
    ih_args.sendlen = datalen;
    ih_args.sendblocks = numblocks;
    ih_args.senddone = blocksdone;

    *blocksdone = 0L;
    if (datalen && numblocks)
    {
        /* set up interrupt handler */
        VEC_DSP = dsp_inout_asm;
        DSPBASE->interrupt_vector = DSP_INTNUM;
        DSPBASE->interrupt_control |= ICR_TREQ;
    }
}

/*
 * Dsp_OutStream(): receive DSP words (no handshaking) via an interrupt handler
 */
void dsp_outstream(char *data, LONG datalen, LONG numblocks, LONG *blocksdone)
{
    if (!has_dsp)
        return;

    /* save args for use by interrupt handler */
    ih_args.rcv = data;
    ih_args.rcvlen = datalen;
    ih_args.rcvblocks = numblocks;
    ih_args.rcvdone = blocksdone;

    *blocksdone = 0L;
    if (datalen && numblocks)
    {
        /* set up interrupt handler */
        VEC_DSP = dsp_inout_asm;
        DSPBASE->interrupt_vector = DSP_INTNUM;
        DSPBASE->interrupt_control |= ICR_RREQ;
    }
}

/*
 * dsp_inout_handler(): C portion of interrupt handler for Dsp_InStream()/Dsp_OutStream()
 */
void dsp_inout_handler(void)
{
    char *data;
    LONG datalen;

    /*
     * handle receive data (if any)
     */
    if (DSP_RCV_READY)
    {
        data = ih_args.rcv;
        datalen = ih_args.rcvlen;
        do
        {
            *data++ = DSPBASE->data.d.high;
            *data++ = DSPBASE->data.d.mid;
            *data++ = DSPBASE->data.d.low;
        } while(--datalen);
        ih_args.rcv = data;         /* update buffer pointer */
        (*ih_args.rcvdone)++;       /*  & 'blocks received' count */
        if (*ih_args.rcvdone == ih_args.rcvblocks)      /* if done, */
            DSPBASE->interrupt_control &= ~ICR_RREQ;    /* disable rcv data interrupt */
    }

    /*
     * handle send data (if any)
     */
    if (DSP_SEND_READY)
    {
        data = ih_args.send;
        datalen = ih_args.sendlen;
        do
        {
            DSPBASE->data.d.high = *data++;
            DSPBASE->data.d.mid = *data++;
            DSPBASE->data.d.low = *data++;
        } while(--datalen);
        ih_args.send = data;        /* update buffer pointer */
        (*ih_args.senddone)++;      /*  & 'blocks sent' count */
        if (*ih_args.senddone == ih_args.sendblocks)    /* if done, */
            DSPBASE->interrupt_control &= ~ICR_TREQ;    /* disable send data interrupt */
    }
}

/*
 * Dsp_IOStream(): send/receive DSP words (no handshaking) via an interrupt handler
 *
 * we send the first block, then install an interrupt handler to service
 * 'output is ready' from the DSP.  on each interrupt, we receive a block
 * and send the next one.
 */
void dsp_iostream(char *send, char *rcv, LONG sendlen, LONG rcvlen, LONG numblocks, LONG *blocksdone)
{
    if (!has_dsp)
        return;

    /* save args for use by interrupt handler */
    ih_args.send = send;
    ih_args.rcv = rcv;
    ih_args.sendlen = sendlen;
    ih_args.rcvlen = rcvlen;
    ih_args.rcvblocks = numblocks;
    ih_args.rcvdone = blocksdone;

    *blocksdone = 0L;

    do
    {
        DSPBASE->data.d.high = *send++;
        DSPBASE->data.d.mid = *send++;
        DSPBASE->data.d.low = *send++;
    } while(--sendlen);
    ih_args.send = send;        /* update buffer pointer */

    /* set up interrupt handler */
    VEC_DSP = dsp_io_asm;
    DSPBASE->interrupt_vector = DSP_INTNUM;
    DSPBASE->interrupt_control |= ICR_RREQ;
}

/*
 * dsp_io_handler(): C portion of interrupt handler for Dsp_IOStream()
 */
void dsp_io_handler(void)
{
    char *data;
    LONG datalen;

    /* receive the data that's ready */
    data = ih_args.rcv;
    datalen = ih_args.rcvlen;
    do
    {
        *data++ = DSPBASE->data.d.high;
        *data++ = DSPBASE->data.d.mid;
        *data++ = DSPBASE->data.d.low;
    } while(--datalen);
    ih_args.rcv = data;         /* update buffer ptr */
    (*ih_args.rcvdone)++;       /*  & 'blocks received' count */
    if (*ih_args.rcvdone == ih_args.rcvblocks)      /* if done, */
    {
        DSPBASE->interrupt_control &= ~ICR_RREQ;    /* disable rcv data interrupt */
        return;                                     /*  and exit */
    }

    /* not done, so send the next block */
    data = ih_args.send;
    datalen = ih_args.sendlen;
    do
    {
        DSPBASE->data.d.high = *data++;
        DSPBASE->data.d.mid = *data++;
        DSPBASE->data.d.low = *data++;
    } while(--datalen);
    ih_args.send = data;        /* update buffer ptr */
}

/*
 * Dsp_RemoveInterrupts(): disable send and/or receive interrupts from DSP
 */
void dsp_removeinterrupts(WORD mask)
{
    if (!has_dsp)
        return;

    mask &= 0x03;               /* only bits 0 & 1 are valid */
    DSPBASE->interrupt_control &= ~mask;
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

/********************************************************
 *                                                      *
 *  P R O G R A M   C O N T R O L   R O U T I N E S     *
 *                                                      *
 ********************************************************/

/*
 * Dsp_Lock() / Dsp_Unlock(): software lock/unlock for DSP
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
 * Dsp_Available(): return memory availability
 */
void dsp_available(LONG *xavailable, LONG *yavailable)
{
    *xavailable = subroutine_bottom - YSIZE;
    *yavailable = YSIZE;    /* TOS returns 1 less, probably a bug */
}

/*
 * Dsp_Reserve(): reserve program memory
 */
WORD dsp_reserve(LONG xreserve, LONG yreserve)
{
    LONG requested;

    if (!has_dsp)
        return 0x6b;    /* unimplemented xbios call: return function # */

    if (yreserve > YSIZE)
        return -1;

    requested = xreserve + YSIZE;
    if (requested > subroutine_bottom)
        return -1;

    program_top = requested;

    return 0;
}

/*
 * Dsp_LoadProg(): load & run .LOD file from disk
 */
WORD dsp_loadprog(char *filename, WORD ability, void *buffer)
{
    LONG size;

    if (!has_dsp)
        return 0x6c;    /* unimplemented xbios call: return function # */

    /* read .LOD file & convert to binary in 'buffer' */
    size = dsp_lodtobinary(filename, buffer);
    if (size < 0L)
        return -1;

    dsp_execprog(buffer, size, ability);

    return 0;
}

/*
 * Dsp_ExecProg(): load & run binary DSP program
 */
void dsp_execprog(const UBYTE *codeptr, LONG codesize, WORD ability)
{
    if (!has_dsp)
        return;

    /* start up our program loader */
    dsp_execboot(dspstart, DSPSTART_SIZE, ability);

    /* send the program */
    dsp_blkhandshake(codeptr, codesize, NULL, 0);

    /* send the standard set of vectors & the transmission terminator */
    dsp_blkhandshake(dspvect_ram, DSPVECT_SIZE, NULL, 0);

    program_ability = ability;  /* remember this */
}

/*
 * Dsp_ExecBoot(): load bootstrap program
 *
 * we reset the DSP via a bit on PSG port A.  this triggers DSP bootstrap
 * firmware to load 512 DSP words and run them.
 */
void dsp_execboot(const UBYTE *codeptr, LONG codesize, WORD ability)
{
    ULONG end;
    WORD old_sr, n;
    UBYTE value;

    if (!has_dsp)
        return;

    /* assert RESET */
    old_sr = set_sr(0x2700);
    PSG->control = PSG_PORT_A;
    value = PSG->control;
    PSG->data = value & ~0x10;      /* clear DSP reset bit */
    PSG->data = value | 0x10;       /* assert bit to start DSP reset */
    set_sr(old_sr);

    /* delay to allow the DSP to reset */
    end = hz_200 + DSP_RESET_TIME;  /* like TOS4, but this seems excessive */
    while(hz_200 < end)
        ;

    /* unassert RESET to start the firmware bootstrap */
    old_sr = set_sr(0x2700);
    PSG->control = PSG_PORT_A;
    value = PSG->control;
    PSG->data = value & ~0x10;      /* unassert reset bit */
    set_sr(old_sr);

    /* load up to 512 DSP words (zero-fill if necessary) */
    if (codesize > DSP_BOOTSTRAP_SIZE)
        codesize = DSP_BOOTSTRAP_SIZE;
    for (n = 0; n < codesize; n++)
    {
        DSPBASE->data.d.high = *codeptr++;
        DSPBASE->data.d.mid = *codeptr++;
        DSPBASE->data.d.low = *codeptr++;
    }
    for ( ; n < DSP_BOOTSTRAP_SIZE; n++)
    {
        DSPBASE->data.d.high = 0;
        DSPBASE->data.d.mid = 0;
        DSPBASE->data.d.low = 0;
    }
}


/*
 * helper routines for Dsp_LodToBinary()
 */

/* convert two hex digits to unsigned byte */
static UBYTE hex(char *p)
{
    WORD i;
    UBYTE n;

    for (i = 0, n = 0; i < 2; i++, p++)
    {
        n <<= 4;
        if ((*p >= '0') && (*p <= '9'))
            n += *p - '0';
        else if ((*p >= 'A') && (*p <= 'F'))
            n += *p - 'A' + 10;
        else if ((*p >= 'a') && (*p <= 'f'))
            n += *p - 'a' + 10;
    }

    return n;
}

/* convert 6 hex digits to 3 unsigned bytes in an array */
static char *inword(char *value, char *p)
{
    WORD i;

    for (i = 0; i < DSP_WORD_SIZE; i++, p += 2, value++)
        *value = hex(p);

    return p;
}

/* convert LONG value to 3 unsigned bytes in an array */
static void outword(char *q, LONG value)
{
    WORD i;

    for (i = 0, q += DSP_WORD_SIZE-1; i < DSP_WORD_SIZE; i++, q--, value >>= 8)
        *q = value;
}

/* test for end-of-line character */
static BOOL iseol(char *p)
{
    if ((*p == '\r') || (*p == '\n'))
        return TRUE;

    return FALSE;
}

/* skip up to last eol character (skips empty lines too) */
static char *skiptoeol(char *p)
{
    while (!iseol(p))
        p++;

    while(iseol(p))
        p++;

    return p-1;
}

/* skip spaces */
static char *skipspaces(char *p)
{
    while(*p == ' ')
        p++;

    return p;
}

/* convert _DATA section of .LOD file to binary */
static WORD handle_DATA(char **pptr, char **qptr)
{
    char *p = *pptr + 6;                /* point after "_DATA " */
    char *q = *qptr;
    char *qstart;
    LONG type, addr, dsp_words;

    switch(*p) {
    case 'P':
        type = 0;
        break;
    case 'X':
        type = 1;
        break;
    case 'Y':
        type = 2;
        break;
    default:
        return -1;
    }

    p = skipspaces(p+1);
    addr = (hex(p) << 8) + hex(p+2);
    p = skiptoeol(p);

    /* initialise 'header' */
    outword(q, type);       /* first DSP word is type */
    q += DSP_WORD_SIZE;
    outword(q, addr);       /* second word is start address */
    q += 2*DSP_WORD_SIZE;   /* third word will be filled in later */
    qstart = q;             /* start of DSP code */

    /* convert rest of _DATA section to binary */
    for ( ; *p && (*p != '_'); p++)
    {
        if (iseol(p))       /* ignore empty lines */
        {
            p = skiptoeol(p);
            continue;
        }
        while(!iseol(p))
        {
            p = inword(q, p);   /* convert a DSP word */
            q += DSP_WORD_SIZE;
            p = skipspaces(p);  /* skip trailing spaces */
        }
    }
    dsp_words = (q - qstart) / DSP_WORD_SIZE;

    /* complete binary data 'header' with DSP word count */
    outword(qstart-DSP_WORD_SIZE, dsp_words);

    /* back up to end of section if necessary */
    if (*p == '_')
        --p;

    /* update input ptrs */
    *pptr = p;
    *qptr = q;

    return dsp_words + 3;
}

/* convert in-memory LOD file to binary */
static LONG convert_lod(char *outbuf, char *inbuf)
{
    char *p, *q;

    /*
     * scan buffer, looking for _DATA sections
     */
    for (p = inbuf, q = outbuf; *p; p++)
    {
        if (strncasecmp(p, "_END", 4) == 0)     /* done if _END is found */
            break;
        if (strncasecmp(p, "_DATA ", 6) == 0)   /* got _DATA */
        {
            if (handle_DATA(&p, &q) < 0)
                return -1L;
            continue;
        }
        p = skiptoeol(p);
    }

    return (q - outbuf) / DSP_WORD_SIZE;
}

/* return filesize of specified file */
static LONG filesize(char *filename)
{
    DTA dta, *dtasave;
    LONG size = -1L;

    dtasave = dos_gdta();
    dos_sdta(&dta);

    if (dos_sfirst(filename, 0) == 0)
        size = dta.d_length;

    dos_sdta(dtasave);

    return size;
}

/*
 * Dsp_LodToBinary(): convert .LOD file to binary
 */
LONG dsp_lodtobinary(char *filename, char *outbuf)
{
    LONG fsize, numwords = 0L;
    char *inbuf;

    if (!has_dsp)
        return 0x6f;    /* unimplemented xbios call: return function # */

    fsize = filesize(filename);
    if (fsize <= 0L)
        return -1L;

    inbuf = dos_alloc_stram(fsize);
    if (!inbuf)
        return -1L;

    if (dos_load_file(filename, fsize, inbuf) < 0L)
        return -1L;

    numwords = convert_lod(outbuf, inbuf);

    dos_free(inbuf);

    return numwords;
}

/*
 * Dsp_TriggerHC(): trigger user host command vector
 *
 * users are only supposed to use vectors $13 and $14, but TOS does not
 * enforce this, so (for now at least) we don't either.
 */
void dsp_triggerhc(WORD vector)
{
    if (!has_dsp)
        return;

    DSPBASE->command_vector = 0x80 | vector;
}

/*
 * Dsp_RequestUniqueAbility(): return unique ability identifier
 */
WORD dsp_requestuniqueability(void)
{
    if (!has_dsp)
        return 0x71;    /* unimplemented xbios call: return function # */

    return ++unique_ability;
}

/*
 * Dsp_GetProgAbility(): return ability identifier of current program
 */
WORD dsp_getprogability(void)
{
    if (!has_dsp)
        return 0x72;    /* unimplemented xbios call: return function # */

    return program_ability;
}

/*
 * Dsp_FlushSubroutines(): remove all subroutines from memory
 */
void dsp_flushsubroutines(void)
{
    if (!has_dsp)
        return;

    bzero(subroutine, sizeof(subroutine));  /* clear out info table */
    oldest_sub = 0;                         /* reset circular pointer */
    subroutine_bottom = PSIZE;              /* reset start of subroutine area */
}

/*
 * helper functions for Dsp_Load_Subroutine()
 */

/* invoke the LOADSUB function of our program loader */
static void load_sub(const UBYTE *codeptr, WORD dest, WORD len)
{
    DSPBASE->command_vector = 0x80 | LOADSUB_VECTOR;
    while(DSPBASE->command_vector & 0x80)   /* wait for DSP response */
        ;

    DSPBASE->data.d.high = 0;           /* send destination address */
    DSPBASE->data.d.mid = HIBYTE(dest);
    DSPBASE->data.d.low = LOBYTE(dest);

    DSPBASE->data.d.high = 0;           /* send length */
    DSPBASE->data.d.mid = HIBYTE(len);
    DSPBASE->data.d.low = LOBYTE(len);

    dsp_doblock(codeptr, len, NULL, 0); /* send actual subroutine */
}

/* invoke the BLOCKMOVE function of our program loader */
static void move_block(UWORD dest, UWORD src, UWORD len)
{
    DSPBASE->command_vector = 0x80 | BLOCKMOVE_VECTOR;
    while(DSPBASE->command_vector & 0x80)   /* wait for DSP response */
        ;

    DSPBASE->data.d.high = 0;           /* send source address */
    DSPBASE->data.d.mid = HIBYTE(src);
    DSPBASE->data.d.low = LOBYTE(src);

    DSPBASE->data.d.high = 0;           /* send destination address */
    DSPBASE->data.d.mid = HIBYTE(dest);
    DSPBASE->data.d.low = LOBYTE(dest);

    DSPBASE->data.d.high = 0;           /* send length */
    DSPBASE->data.d.mid = HIBYTE(len);
    DSPBASE->data.d.low = LOBYTE(len);
}

/* update the start address of a user subroutine in our copy of the vector table */
static void update_dspvect(WORD uservec, WORD start)
{
    UBYTE *p;

    /* the start address is contained in the word after the JSR opcode */
    p = dspvect_ram + (DSPVECT_USERVEC_OFFSET+DSP_VECTOR_SIZE*uservec+1) * DSP_WORD_SIZE;

    *p++ = 0;
    *p++ = HIBYTE(start);
    *p = LOBYTE(start);
}

/* send the user subroutine vector table to the DSP */
static void send_uservect(void)
{
    UBYTE *p;
    WORD len = NUM_USER_SUBROUTINES * DSP_VECTOR_SIZE;

    DSPBASE->command_vector = 0x80 | LOADSUB_VECTOR;
    while(DSPBASE->command_vector & 0x80)   /* wait for DSP response */
        ;

    DSPBASE->data.d.high = 0;           /* send destination address */
    DSPBASE->data.d.mid = 0;
    DSPBASE->data.d.low = FIRST_SUBRTN_ADDR;

    DSPBASE->data.d.high = 0;           /* send length */
    DSPBASE->data.d.mid = 0;
    DSPBASE->data.d.low = len;

    /* send the user vectors */
    p = dspvect_ram + DSPVECT_USERVEC_OFFSET*DSP_WORD_SIZE;
    do
    {
        DSPBASE->data.d.high = *p++;
        DSPBASE->data.d.mid = *p++;
        DSPBASE->data.d.low = *p++;
    } while(--len);
}

/*
 * Dsp_LoadSubroutine(): install subroutine into DSP memory
 */
WORD dsp_loadsubroutine(const UBYTE *codeptr, LONG size, WORD ability)
{
    SUBINFO *p, *oldest;
    WORD i, adjust, handle;

    if (!has_dsp)
        return 0x74;    /* unimplemented xbios call: return function # */

    if (size > DSP_SUBROUTINE_SIZE)
        return 0;

    if (program_top + size > subroutine_bottom)
        return 0;

    /*
     * initially, 'oldest_sub' points to the first of 8 empty slots, so we
     * will fill them in order.  as a consequence, they will be sequenced
     * in decreasing order of start address, with the first one at the top
     * of available memory.  when we wrap around and come to the first one
     * again, we will move the remaining subroutines up in DSP memory and
     * then increment 'oldest_sub'; thus 'oldest_sub' will always point to
     * the subroutine at the top of available memory.
     */
    oldest = &subroutine[oldest_sub];
    if (oldest->size)   /* all slots are in use, must kick this subroutine out */
    {
        adjust = oldest->size;
        /*
         * move subroutines upwards in DSP memory: note that the addresses
         * for BLOCKMOVE are the *last bytes* of the regions concerned
         */
        move_block(oldest->start+adjust-1, oldest->start-1, oldest->start-subroutine_bottom);
        /*
         * fix up start addresses in subroutine info and vector table
         */
        for (i = 0, p = subroutine; i < NUM_USER_SUBROUTINES; i++, p++)
        {
            p->start += adjust;         /* adjust start of all subroutines */
            update_dspvect(i, p->start);/* in vector table too */
        }
        subroutine_bottom += adjust;    /* fix up start of subroutine area */
    }

    subroutine_bottom -= size;          /* adjust pointer to start of subroutines */
    oldest->start = subroutine_bottom;  /* & update table with subroutine info */
    oldest->size = size;
    oldest->ability = ability;
    update_dspvect(oldest_sub, oldest->start);  /* in vector table too */
    handle = FIRST_SUBRTN_VECTOR + oldest_sub;  /* remember for return code */

    send_uservect();                            /* send user subroutine vector table */

    load_sub(codeptr, subroutine_bottom, size); /* actually load the subroutine */

    if (++oldest_sub >= NUM_USER_SUBROUTINES)    /* update queue ptr */
        oldest_sub = 0;

    return handle;
}

/*
 * Dsp_InqSubrAbility(): get handle for installed subroutine
 */
WORD dsp_inqsubrability(WORD ability)
{
    WORD i;
    SUBINFO *p;

    if (!has_dsp)
        return 0x75;    /* unimplemented xbios call: return function # */

    for (i = 0, p = subroutine; i < NUM_USER_SUBROUTINES; i++, p++)
    {
        if (p->ability == ability)
            return i + FIRST_SUBRTN_VECTOR;
    }

    return 0;
}

/*
 * Dsp_RunSubroutine(): execute DSP-resident subroutine
 */
WORD dsp_runsubroutine(WORD handle)
{
    WORD n, addr;

    if (!has_dsp)
        return 0x75;    /* unimplemented xbios call: return function # */

    n = handle - FIRST_SUBRTN_VECTOR;
    if ((n < 0) || (n >= NUM_USER_SUBROUTINES))
        return -1;

    /*
     * set up the start address of the DSP routine in the host port
     * so that the DSP code can read it when it starts
     */
    addr = subroutine[n].start;
    DSPBASE->data.d.high = 0;
    DSPBASE->data.d.mid = HIBYTE(addr);
    DSPBASE->data.d.low = LOBYTE(addr);

    DSPBASE->command_vector = 0x80 | handle;    /* wake up subroutine */

    return 0;
}

/*
 * Dsp_Hf0() / Dsp_Hf1(): read/write host flags 0 or 1
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
 * Dsp_Hf2() / Dsp_Hf3(): read host flags 2 & 3
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
 * Dsp_Hstat(): read the interrupt status register
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
 * Dsp_SetVectors(): install user interrupt handler
 */
void dsp_setvectors(void (*receiver)(LONG data), LONG (*transmitter)(void))
{
    if (!has_dsp)
        return;

    user_rcv = NULL;        /* initialise ptrs */
    user_send = NULL;

    if (receiver)
    {
        user_rcv = receiver;
        VEC_DSP = dsp_sv_asm;
        DSPBASE->interrupt_vector = DSP_INTNUM;
        DSPBASE->interrupt_control |= ICR_RREQ;
    }

    if (transmitter)
    {
        user_send = transmitter;
        VEC_DSP = dsp_sv_asm;
        DSPBASE->interrupt_vector = DSP_INTNUM;
        DSPBASE->interrupt_control |= ICR_TREQ;
    }
}

/*
 * dsp_sv_handler(): C portion of interrupt handler for Dsp_SetVectors()
 */
void dsp_sv_handler(void)
{
    LONG data;

    if (DSP_RCV_READY && user_rcv)
    {
        data = DSPBASE->data.full;
        user_rcv(data);
    }

    if (DSP_SEND_READY && user_send)
    {
        data = user_send();
        if (data)       /* user has something to send? */
            DSPBASE->data.full = data;
    }
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
