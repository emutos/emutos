/*
 * vdidef.h - Definitions for virtual workstations
 *
 * Copyright (c) 1999 Caldera, Inc.
 *
 * Authors:
 *  xxx <xxx@xxx>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef VDIDEF_H
#define VDIDEF_H

#include "portab.h"



/* different maximum settings */

#define MAX_COLOR       16
#define MX_LN_STYLE     7
#define MX_LN_WIDTH     40
#define MAX_MARK_INDEX  6
#define MAX_FONT        1
#define MX_FIL_STYLE    4
#define MX_FIL_HAT_INDEX        12
#define MX_FIL_PAT_INDEX        24
#define MAX_MODE        3
#define MAX_ARC_CT      70      /* maximum number of points on circle */


/* aliases for different values */

#define SQUARED 0
#define ARROWED 1
#define CIRCLED 2

#define LLUR 0
#define ULLR 1


/* aliases for different table positions */

#define xres            DEV_TAB[0]
#define yres            DEV_TAB[1]
#define xsize           DEV_TAB[3]
#define ysize           DEV_TAB[4]

#define DEF_LWID        SIZ_TAB[4]
#define DEF_CHHT        SIZ_TAB[1]
#define DEF_CHWT        SIZ_TAB[0]
#define DEF_MKWD        SIZ_TAB[8]
#define DEF_MKHT        SIZ_TAB[9]
#define MAX_MKWD        SIZ_TAB[10]
#define MAX_MKHT        SIZ_TAB[11]

/* Defines for CONTRL[] */
#define ROUTINE 0
#define N_PTSIN 1
#define N_PTSOUT 2
#define N_INTIN 3
#define N_INTOUT 4
#define SUBROUTINE 5
#define VDI_HANDLE 6

/* gsx write modes */
#define MD_REPLACE  1
#define MD_TRANS    2
#define MD_XOR      3
#define MD_ERASE    4



/* font-header definitions */

/* fh_flags   */

#define F_DEFAULT 1             /* this is the default font (face and size) */
#define F_HORZ_OFF  2           /* there are left and right offset tables */
#define F_STDFORM  4            /* is the font in standard format */
#define F_MONOSPACE 8           /* is the font monospaced */

/* style bits */

#define F_THICKEN 1
#define F_LIGHT 2
#define F_SKEW  4
#define F_UNDER 8
#define F_OUTLINE 16
#define F_SHADOW        32

struct font_head {              /* descibes a font */
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

    UBYTE *hor_table;           /* horizontal offsets */
    UWORD *off_table;           /* character offsets  */
    UWORD *dat_table;           /* character definitions */
    UWORD form_width;
    UWORD form_height;

    struct font_head *next_font;        /* pointer to next font */
    UWORD font_seg;
};


/* Structure to hold data for a virtual workstation */

struct attribute {
    WORD chup;                  /* Character Up vector          */
    WORD clip;                  /* Clipping Flag            */
    struct font_head *cur_font; /* Pointer to current font      */
    WORD dda_inc;               /* Fraction to be added to the DDA  */
    WORD multifill;             /* Multi-plane fill flag        */
    UWORD patmsk;               /* Current pattern mask         */
    UWORD *patptr;              /* Current pattern pointer      */
    WORD pts_mode;              /* TRUE if height set in points mode    */
    WORD *scrtchp;              /* Pointer to text scratch buffer   */
    WORD scrpt2;                /* Offset to large text buffer      */
    WORD style;                 /* Current text style           */
    WORD t_sclsts;              /* TRUE if scaling up           */
    WORD fill_color;            /* Current fill color (PEL value)   */
    WORD fill_index;            /* Current fill index           */
    WORD fill_per;              /* TRUE if fill area outlined       */
    WORD fill_style;            /* Current fill style           */
    WORD h_align;               /* Current text horizontal alignment    */
    WORD handle;                /* The handle this attribute area is for */
    WORD line_beg;              /* Beginning line endstyle      */
    WORD line_color;            /* Current line color (PEL value)   */
    WORD line_end;              /* Ending line endstyle         */
    WORD line_index;            /* Current line style           */
    WORD line_width;            /* Current line width           */
    struct font_head *loaded_fonts;     /* Pointer to first loaded font     */
    WORD mark_color;            /* Current marker color (PEL value)     */
    WORD mark_height;           /* Current marker height        */
    WORD mark_index;            /* Current marker style         */
    WORD mark_scale;            /* Current scale factor for marker data */
    struct attribute *next_work;        /* Pointer to next virtual
                                           workstation  */
    WORD num_fonts;             /* Total number of faces available  */
    WORD scaled;                /* TRUE if font scaled in any way   */
    struct font_head scratch_head;      /* Holder for the doubled font data */
    WORD text_color;            /* Current text color (PEL value)   */
    WORD ud_ls;                 /* User defined linestyle       */
    WORD ud_patrn[4 * 16];      /* User defined pattern         */
    WORD v_align;               /* Current text vertical alignment  */
    WORD wrt_mode;              /* Current writing mode         */
    WORD xfm_mode;              /* Transformation mode requested    */
    WORD xmn_clip;              /* Low x point of clipping rectangle    */
    WORD xmx_clip;              /* High x point of clipping rectangle   */
    WORD ymn_clip;              /* Low y point of clipping rectangle    */
    WORD ymx_clip;              /* High y point of clipping rectangle   */
};

/* Raster definitions */
typedef struct {
    void *fd_addr;
    WORD fd_w;
    WORD fd_h;
    WORD fd_wdwidth;
    WORD fd_stand;
    WORD fd_nplanes;
    WORD fd_r1;
    WORD fd_r2;
    WORD fd_r3;
} MFDB;

typedef struct
{
    WORD x1,y1;
    WORD x2,y2;
} RECT;

#endif                          /* VDIDEF_H */
