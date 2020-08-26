/*
 * blitter.h - header for blitter routines
 *
 * Copyright (C) 2017-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BLITTER_H
#define _BLITTER_H

#if CONF_WITH_BLITTER

/*
 * architectural definitions
 */
#define BLITTER     ((BLIT *)0xFFFF8A00L)

typedef struct
{
    UWORD           halftone[16];       /* halftone RAM */
    WORD            src_x_incr;         /* source X increment */
    WORD            src_y_incr;         /* source Y increment */
    volatile UWORD  *src_addr;          /* source address */
    UWORD           endmask_1;          /* for first write of line */
    UWORD           endmask_2;          /* for other writes */
    UWORD           endmask_3;          /* for last write of line */
    WORD            dst_x_incr;         /* destination X increment */
    WORD            dst_y_incr;         /* destination Y increment */
    volatile UWORD  *dst_addr;          /* destination address */
    volatile UWORD  x_count;            /* X count */
    volatile UWORD  y_count;            /* Y count */
    UBYTE           hop;                /* HOP */
    UBYTE           op;                 /* OP */
    volatile UBYTE  status;             /* status bits & line# */
    UBYTE           skew;               /* FXSR, NFSR, & skew */
} BLIT;

#endif

/* the following are used by the blitter and the blitter emulation code */

/*
 * values for hop
 */
#define HOP_ALL_ONES            0
#define HOP_HALFTONE_ONLY       1
#define HOP_SOURCE_ONLY         2
#define HOP_SOURCE_AND_HALFTONE 3

/*
 * values for status
 */
#define BUSY        0x80
#define HOG         0x40
#define SMUDGE      0x20
#define LINENO      0x0f

/*
 * values for skew
 */
#define FXSR    0x80
#define NFSR    0x40
#define SKEW    0x0f

#endif
