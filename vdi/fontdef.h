/*
 * fontdef.h - font-header definitions
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _FONTDEF_H
#define _FONTDEF_H

#include "portab.h"

/* fh_flags   */

#define F_DEFAULT 1                             /* this is the default font (face and size) */
#define F_HORZ_OFF  2                   /* there are left and right offset tables */
#define F_STDFORM  4                    /* is the font in standard format */
#define F_MONOSPACE 8                   /* is the font monospaced */

/* style bits */

#define F_THICKEN 1
#define F_LIGHT 2
#define F_SKEW  4
#define F_UNDER 8
#define F_OUTLINE 16
#define F_SHADOW        32

struct font_head {                              /* descibes a font */
        WORD font_id;
        WORD point;
        BYTE name[32];
        UWORD first_ade;
        UWORD last_ade;
        UWORD top;
        UWORD ascent;
        UWORD half;
        UWORD descent;
        UWORD bottom;
        UWORD max_char_width;
        UWORD max_cell_width;
        UWORD left_offset;                      /* amount character slants left when skewed */
        UWORD right_offset;                     /* amount character slants right */
        UWORD thicken;                          /* number of pixels to smear */
        UWORD ul_size;                          /* size of the underline */
        UWORD lighten;                          /* mask to and with to lighten  */
        UWORD skew;                                     /* mask for skewing */
        UWORD flags;

        UBYTE *hor_table;                       /* horizontal offsets */
        UWORD *off_table;                       /* character offsets  */
        UWORD *dat_table;                       /* character definitions */
        UWORD form_width;
        UWORD form_height;

        struct font_head *next_font;    /* pointer to next font */
        UWORD font_seg;
};

#endif                                                  /* _FONTDEF_H */
