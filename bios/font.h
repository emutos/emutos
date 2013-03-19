/*
 * font.h - font specific definitions
 *
 * Copyright (c) 2001 Lineo, Inc.
 * Copyright (c) 2004 by Authors:
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef FONT_H
#define FONT_H

#include "portab.h"

/* font header flags */

#define F_DEFAULT 1     /* this is the default font (face and size) */
#define F_HORZ_OFF  2   /* there are left and right offset tables */
#define F_STDFORM  4    /* is the font in standard format */
#define F_MONOSPACE 8   /* is the font monospaced */

/* font style bits */

#define F_THICKEN 1
#define F_LIGHT 2
#define F_SKEW  4
#define F_UNDER 8
#define F_OUTLINE 16
#define F_SHADOW        32

/* font specific linea variables */

extern const UWORD *v_fnt_ad;   /* address of current monospace font */
extern const UWORD *v_off_ad;   /* address of font offset table */
extern UWORD v_fnt_nd;          /* ascii code of last cell in font */
extern UWORD v_fnt_st;          /* ascii code of first cell in font */
extern UWORD v_fnt_wr;          /* font cell wrap */

/* character cell specific linea variables */

extern UWORD    v_cel_ht;       /* cell height (width is 8) */
extern UWORD    v_cel_mx;       /* needed by MiNT: columns on the screen minus 1 */
extern UWORD    v_cel_my;       /* needed by MiNT: rows on the screen minus 1 */
extern UWORD    v_cel_wr;       /* needed by MiNT: length (in bytes) of a line of characters */

/*
 * font_ring is a struct of four pointers, each of which points to
 * a list of font headers linked together to form a string.
 */

extern struct font_head *font_ring[4];  /* Ring of available fonts */
extern WORD font_count;                 /* all three fonts and NULL */

/* the font header descibes a font */

struct font_head {
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
    UWORD left_offset;          /* amount character slants left when skewed */
    UWORD right_offset;         /* amount character slants right */
    UWORD thicken;              /* number of pixels to smear */
    UWORD ul_size;              /* size of the underline */
    UWORD lighten;              /* mask to and with to lighten  */
    UWORD skew;                 /* mask for skewing */
    UWORD flags;

    const UBYTE *hor_table;     /* horizontal offsets */
    const UWORD *off_table;     /* character offsets  */
    const UWORD *dat_table;     /* character definitions */
    UWORD form_width;
    UWORD form_height;

    struct font_head *next_font;/* pointer to next font */
    UWORD font_seg;
};



/* prototypes */

void font_init(void);           /* initialize BIOS font ring */
void font_set_default(void);    /* choose the default font */

#endif /* FONT_H */
