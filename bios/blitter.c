/*
 * blitter.c - blitter routines
 *
 * Copyright (C) 2017 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"

#include "lineavars.h"
#include "machine.h"
#include "../vdi/vdi_defs.h"
#include "kprint.h"

#if CONF_WITH_BLITTER

#define BLITTER     ((BLIT *)0xFFFF8A00L)

typedef struct {
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
    volatile WORD   x_count;            /* X count */
    volatile WORD   y_count;            /* Y count */
    UBYTE           hop;                /* HOP */
    UBYTE           op;                 /* OP */
    volatile BYTE   status;             /* status bits & line# */
    BYTE            skew;               /* FXSR, NFSR, & skew */
} BLIT;

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
#define BUSY    0x80
#define HOG     0x40
#define SMUDGE  0x20
#define LINENO  0x0f


/*
 * blitter op lookup table for different values of WRT_MODE & COLBITn
 *
 * for WRT_MODE = m, use entry m*2 if COLBITn = 0, entry m*2+1 if COLBITn != 0
 */
static const UBYTE op_lookup[8] = { 0x00, 0x03, 0x04, 0x07, 0x06, 0x06, 0x01, 0x0d };


/*
 * blitter version of linea4 (horizontal line)
 *
 * if blitter available & enabled, uses it and returns TRUE
 * else returns FALSE
 */
BOOL blitter_hline(void)
{
    UWORD *screen_addr;
    WORD *colbit;
    UWORD endmask1, endmask3;
    WORD i, n, width, patindex;

    if (!blitter_is_enabled)
        return FALSE;

    /*
     * here we may wish to check source/dest addresses when running
     * on non-Atari hardware (e.g. the FireBee)
     */

    /* determine line width & endmasks */
    width = ((X2 >> 4) - (X1 >> 4)) + 1;
    endmask1 = 0xffff >> (X1 & 0x0f);
    endmask3 = 0xffff << (15 - (X2 & 0x0f));
    if (width == 1)
        endmask1 &= endmask3;

    BLITTER->src_x_incr = 0;
    BLITTER->endmask_1 = endmask1;
    BLITTER->endmask_2 = 0xffff;
    BLITTER->endmask_3 = endmask3;
    BLITTER->dst_x_incr = v_planes * sizeof(WORD);
    BLITTER->x_count = width;

    patindex = Y1 & PATMSK;
    screen_addr = get_start_addr(X1, Y1);

    for (i = 0, colbit = &COLBIT0; i < v_planes; i++)
    {
        BLITTER->halftone[0] = PATPTR[patindex];
        if (MFILL)
            patindex += 16;
        BLITTER->dst_addr = screen_addr++;
        BLITTER->y_count = 1;
        BLITTER->hop = HOP_HALFTONE_ONLY;
        n = WRT_MODE * 2 + (*colbit++ ? 1 : 0);
        BLITTER->op = op_lookup[n];

        /*
         * we run the blitter in the Atari-recommended way: use no-HOG mode,
         * and manually restart the blitter until it's done.
         */
        BLITTER->status = BUSY;     /* no-HOG mode */
        __asm__ __volatile__(
        "lea    0xFFFF8A3C,a0\n\t"
        "0:\n\t"
        "tas    (a0)\n\t"
        "nop\n\t"
        "jbmi   0b\n\t"
        :
        :
        : "a0", "memory", "cc"
        );
    }

    return TRUE;
}

#endif /* CONF_WITH_BLITTER */
