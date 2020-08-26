/*
 * vdiext.h - EmuTOS VDI extensions not callable with trap
 *
 * Copyright (C) 2019-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef _VDIEXT_H
#define _VDIEXT_H

/*
 * maximum number of vertices for v_fillarea(), v_pline(), v_pmarker()
 *
 * TOS2 allows 512, TOS3 allows 1024
 */
#ifdef TARGET_512
# define MAX_VERTICES   1024
#else
# define MAX_VERTICES   512
#endif

#ifndef ASM_SOURCE

/*
 * segment in queue structure used by contourfill()
 */
typedef struct {
    WORD y;                     /* y coordinate of segment and/or special value */
    WORD xleft;                 /* x coordinate of segment start */
    WORD xright;                /* x coordinate of segment end */
} SEGMENT;

/*
 * queue size for contourfill()
 *
 * this is currently made as large as will fit in the existing vdishare
 * area without increasing it (see below).
 *
 * in order to be guaranteed to handle all possible shapes of fill area,
 * the number of entries probably needs to be greater than or equal to
 * the current horizontal screen resolution.
 */
#define QSIZE   (sizeof(struct vsmain)/sizeof(SEGMENT))

/*
 * text scratch buffer size (in bytes)
 *
 * see vdi_text.c for how this is calculated
 */
#define SCRATCHBUF_SIZE (2*212)

/*
 * a shared area for the VDI
 *
 * if you choose to add to this, you must (manually) verify that usage of the
 * area by the new member does not conflict with usage by any other member!
 */
typedef union {
    struct vsmain {
        WORD local_ptsin[2*MAX_VERTICES];   /* used by GSX_ENTRY() - must be at offset 0 */
        WORD fill_buffer[MAX_VERTICES];     /* used by clc_flit() */
    } main;
    SEGMENT queue[QSIZE];       /* storage for contourfill() seed points  */
    WORD deftxbuf[SCRATCHBUF_SIZE/sizeof(WORD)];    /* text scratch buffer */
} VDISHARE;

/* External definitions for internal use */
extern VDISHARE vdishare;

#endif /* ASM_SOURCE */

#endif /* _VDIEXT_H */
