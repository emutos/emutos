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

/* miscellaneous function prototypes */
void detect_dsp(void);
void dsp_init(void);
void dsp_inout_handler(void);   /* interrupt handler for Dsp_InStream() & Dsp_OutStream() */
void dsp_io_handler(void);      /* interrupt handler for Dsp_IOStream() */
void dsp_sv_handler(void);      /* interrupt handler for Dsp_SetVectors() */

/* XBIOS DSP functions */
void dsp_doblock(const UBYTE *send, LONG sendlen, char *rcv, LONG rcvlen);
void dsp_blkhandshake(const UBYTE *send, LONG sendlen, char *rcv, LONG rcvlen);
void dsp_blkunpacked(LONG *send, LONG sendlen, LONG *rcv, LONG rcvlen);
void dsp_instream(char *data, LONG datalen, LONG numblocks, LONG *blocksdone);
void dsp_outstream(char *data, LONG datalen, LONG numblocks, LONG *blocksdone);
void dsp_iostream(char *send, char *rcv, LONG sendlen, LONG rcvlen, LONG numblocks, LONG *blocksdone);
void dsp_removeinterrupts(WORD mask);
WORD dsp_getwordsize(void);
WORD dsp_lock(void);
void dsp_unlock(void);
void dsp_available(LONG *xavailable, LONG *yavailable);
WORD dsp_reserve(LONG xreserve, LONG yreserve);
WORD dsp_loadprog(char *filename, WORD ability, void *buffer);
void dsp_execprog(const UBYTE *codeptr, LONG codesize, WORD ability);
void dsp_execboot(const UBYTE *codeptr, LONG codesize, WORD ability);
LONG dsp_lodtobinary(char *filename, char *outbuf);
void dsp_triggerhc(WORD vector);
WORD dsp_requestuniqueability(void);
WORD dsp_getprogability(void);
void dsp_flushsubroutines(void);
WORD dsp_loadsubroutine(const UBYTE *codeptr, LONG size, WORD ability);
WORD dsp_inqsubrability(WORD ability);
WORD dsp_runsubroutine(WORD handle);
WORD dsp_hf0(WORD flag);
WORD dsp_hf1(WORD flag);
WORD dsp_hf2(void);
WORD dsp_hf3(void);
void dsp_blkwords(WORD *send, LONG sendlen, WORD *rcv, LONG rcvlen);
void dsp_blkbytes(UBYTE *send, LONG sendlen, UBYTE *rcv, LONG rcvlen);
UBYTE dsp_hstat(void);
void dsp_setvectors(void (*receiver)(LONG data), LONG (*transmitter)(void));
void dsp_multblocks(LONG sendnum, LONG rcvnum, DSPBLOCK *sendinfo, DSPBLOCK *rcvinfo);

/* functions in dsp2.S */
void dsp_inout_asm(void);       /* wrapper for dsp_inout_handler() */
void dsp_io_asm(void);          /* wrapper for dsp_io_handler() */
void dsp_sv_asm(void);          /* wrapper for dsp_sv_handler() */

#endif /* CONF_WITH_DSP */

#endif /* _DSP_H */
