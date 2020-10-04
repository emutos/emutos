/*
 * dsp.h - DSP routines
 *
 * Copyright (C) 2020 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DSP_H
#define _DSP_H

#if CONF_WITH_DSP

/* structure used by Dsp_MultBlocks() only */
typedef struct {
    WORD blocktype;             /* type of data element (see below) */
    LONG blocksize;             /* number of data elements */
    void *blockaddr;            /* ptr to first data element */
} DSPBLOCK;

/* values for 'blocktype' above */
#define BT_LONG 0
#define BT_WORD 1
#define BT_BYTE 2

/* function prototypes */
void detect_dsp(void);
void dsp_init(void);

/* XBIOS DSP functions */
void dsp_doblock(char *send, LONG sendlen, char *rcv, LONG rcvlen);
void dsp_blkhandshake(char *send, LONG sendlen, char *rcv, LONG rcvlen);
void dsp_blkunpacked(LONG *send, LONG sendlen, LONG *rcv, LONG rcvlen);
WORD dsp_getwordsize(void);
WORD dsp_lock(void);
void dsp_unlock(void);
WORD dsp_hf0(WORD flag);
WORD dsp_hf1(WORD flag);
WORD dsp_hf2(void);
WORD dsp_hf3(void);
void dsp_blkwords(WORD *send, LONG sendlen, WORD *rcv, LONG rcvlen);
void dsp_blkbytes(UBYTE *send, LONG sendlen, UBYTE *rcv, LONG rcvlen);
UBYTE dsp_hstat(void);
void dsp_multblocks(LONG sendnum, LONG rcvnum, DSPBLOCK *sendinfo, DSPBLOCK *rcvinfo);

#endif /* CONF_WITH_DSP */

#endif /* _DSP_H */
