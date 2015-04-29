/*
 * fonthdr.h - the Font Header
 *
 * This file exists to centralise the definition of the font header,
 * which was previously defined in two different places.
 *
 * Copyright (c) 2015 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FONTHDR_H
#define FONTHDR_H

#include "portab.h"

/* font header flags */

#define F_DEFAULT   1   /* this is the default font (face and size) */
#define F_HORZ_OFF  2   /* there is a horizontal offset table */
#define F_STDFORM   4   /* the font is in standard (Motorola) format */
#define F_MONOSPACE 8   /* the font is monospaced */

/* the font header describes a font */

#define FONT_NAME_LEN 32

typedef struct font_head Fonthead;
struct font_head {
    WORD font_id;
    WORD point;
    BYTE name[FONT_NAME_LEN];
    UWORD first_ade;
    UWORD last_ade;
    UWORD top;
    UWORD ascent;
    UWORD half;
    UWORD descent;
    UWORD bottom;
    UWORD max_char_width;
    UWORD max_cell_width;
    UWORD left_offset;          /* amount character slants left when skewed */
    UWORD right_offset;         /* amount character slants right */
    UWORD thicken;              /* number of pixels to smear when bolding */
    UWORD ul_size;              /* height of the underline */
    UWORD lighten;              /* mask for lightening  */
    UWORD skew;                 /* mask for skewing */
    UWORD flags;                /* see above */

    const UBYTE *hor_table;     /* horizontal offsets */
    const UWORD *off_table;     /* character offsets  */
    const UWORD *dat_table;     /* character definitions (raster data) */
    UWORD form_width;           /* width of raster in bytes */
    UWORD form_height;          /* height of raster in lines */

    Fonthead *next_font;        /* pointer to next font */
    UWORD font_seg;
};

#endif /* FONTHDR_H */
