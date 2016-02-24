/*
 * vdidef.h - Definitions for virtual workstations
 *
 * Copyright 1999 by Caldera, Inc.
 * Copyright 2005-2015 The EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef VDIDEFS_H
#define VDIDEFS_H

#include "portab.h"
#include "fonthdr.h"

#define HAVE_BEZIER 0           /* switch on bezier capability */

/* GEMDOS function numbers */
#define X_MALLOC 0x48
#define X_MFREE 0x49


/* different maximum settings */
#define MAX_PTSIN 256           /* max. # of coordinate pairs, also asm.S! */


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


/*
 * Small subset of Vwk data, used by draw_rect_common to hide VDI/Line-A
 * specific details from rectangle & polygon drawing.
 */
typedef struct {
    WORD clip;       /* polygon clipping on/off */
    WORD multifill;  /* Multi-plane fill flag   */
    UWORD patmsk;    /* Current pattern mask    */
    const UWORD *patptr;/* Current pattern pointer */
    WORD wrt_mode;   /* Current writing mode    */
    UWORD color;     /* fill color */
} VwkAttrib;


/* type that can be cast from clipping part of Wvk */
typedef struct {
    WORD xmn_clip;              /* Low x point of clipping rectangle    */
    WORD xmx_clip;              /* High x point of clipping rectangle   */
    WORD ymn_clip;              /* Low y point of clipping rectangle    */
    WORD ymx_clip;              /* High y point of clipping rectangle   */
} VwkClip;

#define VDI_CLIP(wvk) ((VwkClip*)(&(wvk->xmn_clip)))


/* Structure to hold data for a virtual workstation */

/* NOTE 1: for backwards compatibility with all versions of TOS, the
 * field 'fill_color' must remain at offset 0x1e, because the line-A
 * flood fill function uses the fill colour from the currently-open
 * virtual workstation, and it is documented that users can provide
 * a fake virtual workstation by pointing CUR_WORK to a 16-element
 * WORD array whose last element contains the fill colour.
 */
typedef struct Vwk_ Vwk;
struct Vwk_ {
    WORD chup;                  /* Character Up vector */
    WORD clip;                  /* Clipping Flag */
    const Fonthead *cur_font;   /* Pointer to current font */
    UWORD dda_inc;              /* Fraction to be added to the DDA */
    WORD multifill;             /* Multi-plane fill flag */
    UWORD patmsk;               /* Current pattern mask */
    UWORD *patptr;              /* Current pattern pointer */
    WORD pts_mode;              /* TRUE if height set in points mode */
    WORD *scrtchp;              /* Pointer to text scratch buffer */
    WORD scrpt2;                /* Offset to large text buffer */
    WORD style;                 /* Current text style */
    WORD t_sclsts;              /* TRUE if scaling up */
    WORD fill_color;            /* Current fill color (PEL value): see NOTE 1 above */
    WORD fill_index;            /* Current fill index */
    WORD fill_per;              /* TRUE if fill area outlined */
    WORD fill_style;            /* Current fill style */
    WORD h_align;               /* Current text horizontal alignment */
    WORD handle;                /* The handle this attribute area is for */
    WORD line_beg;              /* Beginning line endstyle */
    WORD line_color;            /* Current line color (PEL value) */
    WORD line_end;              /* Ending line endstyle */
    WORD line_index;            /* Current line style */
    WORD line_width;            /* Current line width */
    const Fonthead *loaded_fonts; /* Pointer to first loaded font */
    WORD mark_color;            /* Current marker color (PEL value)     */
    WORD mark_height;           /* Current marker height        */
    WORD mark_index;            /* Current marker style         */
    WORD mark_scale;            /* Current scale factor for marker data */
    Vwk *next_work;             /* Pointer to next virtual workstation  */
    WORD num_fonts;             /* Total number of faces available  */
    WORD scaled;                /* TRUE if font scaled in any way   */
    Fonthead scratch_head;      /* Holder for the doubled font data */
    WORD text_color;            /* Current text color (PEL value)   */
    WORD ud_ls;                 /* User defined linestyle       */
    WORD ud_patrn[4 * 16];      /* User defined pattern         */
    WORD v_align;               /* Current text vertical alignment  */
    WORD wrt_mode;              /* Current writing mode         */
    WORD xfm_mode;              /* Transformation mode requested (NDC) */
    WORD xmn_clip;              /* Low x point of clipping rectangle    */
    WORD xmx_clip;              /* High x point of clipping rectangle   */
    WORD ymn_clip;              /* Low y point of clipping rectangle    */
    WORD ymx_clip;              /* High y point of clipping rectangle   */
    /* newly added */
    WORD bez_qual;              /* actual quality for bezier curves */
};


typedef struct Rect_ Rect;
struct Rect_
{
    WORD x1,y1;
    WORD x2,y2;
};

typedef struct Line_ Line;
struct Line_
{
    WORD x1,y1;
    WORD x2,y2;
};

typedef struct Point_ Point;
struct Point_
{
    WORD x,y;
};



/* External definitions for internal use */
extern WORD flip_y;             /* True if magnitudes being returned */
extern WORD line_cw;            /* Linewidth for current circle */
extern WORD num_qc_lines;
extern WORD val_mode, chc_mode, loc_mode, str_mode;

/* These are still needed for text blitting */
extern const UWORD LINE_STYLE[];
extern const UWORD ROM_UD_PATRN[];
extern const UWORD SOLID;
extern const UWORD HOLLOW;

extern WORD DEV_TAB[];          /* initial intout array for open workstation */
extern WORD SIZ_TAB[];          /* initial ptsout array for open workstation */
extern WORD INQ_TAB[];          /* extended inquire values */

extern WORD *CONTRL, *INTIN, *PTSIN, *INTOUT, *PTSOUT;

extern WORD LN_MASK, LSTLIN;
extern WORD TERM_CH;

/* Line-A Bit-Blt / Copy raster form variables */
extern WORD COPYTRAN;
extern WORD multifill;

/* referenced by Line-A flood fill */
extern Vwk *CUR_WORK;           /* pointer to currently-open virtual workstation */

/* Mouse specific externals */
extern WORD GCURX;              /* mouse X position */
extern WORD GCURY;              /* mouse Y position */
extern WORD HIDE_CNT;           /* Number of levels the mouse is hidden */
extern WORD MOUSE_BT;           /* mouse button state */

/* Mouse related variables */
extern WORD     newx;           /* new mouse x&y position */
extern WORD     newy;           /* new mouse x&y position */
extern BYTE     draw_flag;      /* non-zero means draw mouse form on vblank */
extern BYTE     mouse_flag;     /* non-zero, if mouse ints disabled */
extern BYTE     cur_ms_stat;    /* current mouse status */


/* shared VDI functions & VDI line-A wrapper functions */
void undraw_sprite(void);
void draw_sprite(void);
WORD get_pix(void);
void put_pix(void);


/* Assembly Language Support Routines, ignore workstation arg */
void text_blt(Vwk * vwk);
void rectfill (Vwk * vwk, Rect * rect);


WORD gloc_key(void);

BOOL clip_line(Vwk * vwk, Line * line);
void arb_corner(Rect * rect);
void arb_line(Line * line);


/* C Support routines */
Vwk * get_vwk_by_handle(WORD);
UWORD * get_start_addr(const WORD x, const WORD y);
void set_LN_MASK(Vwk *vwk);
void st_fl_ptr(Vwk *);
void d_justified(Vwk *);

/* drawing primitives */
void draw_pline(Vwk * vwk);
void arrow(Vwk * vwk, Point * point, int count);
void draw_rect(const Vwk * vwk, Rect * rect, const UWORD fillcolor);
void polygon(Vwk * vwk, Point * point, int count);
void polyline(Vwk * vwk, Point * point, int count, WORD color);
void wideline(Vwk * vwk, Point * point, int count);

/* common drawing function */
void Vwk2Attrib(const Vwk *vwk, VwkAttrib *attr, const UWORD color);
void draw_rect_common(const VwkAttrib *attr, const Rect *rect);
void clc_flit (const VwkAttrib * attr, const VwkClip * clipper, const Point * point, WORD y, int vectors);
void abline (const Line * line, const WORD wrt_mode, UWORD color);
void contourfill(const VwkAttrib * attr, const VwkClip *clip);

/* initialization of subsystems */
void text_init(Vwk *);
void text_init2(Vwk *);
void timer_init(Vwk *);
void vdimouse_init(Vwk *);
void esc_init(Vwk *);

void vdimouse_exit(Vwk *);
void timer_exit(Vwk *);
void esc_exit(Vwk *);

/* all VDI functions */

/* As reference the TOS 1.0 start addresses are added */
void vdi_v_opnwk(Vwk *);            /* 1   - fcb53e */
void vdi_v_clswk(Vwk *);            /* 2   - fcb812 */
void vdi_v_clrwk(Vwk *);            /* 3   - fca4e8 */
/* void v_updwk(Vwk *); */          /* 4   - fca4e6 - not yet implemented */
void vdi_v_escape(Vwk *);           /* 5   - fc412e */

void vdi_v_pline(Vwk *);            /* 6   - fcb85a */
void vdi_v_pmarker(Vwk *);          /* 7   - fcb8f4 */
void vdi_v_gtext(Vwk *);            /* 8   - fcd61c */
void vdi_v_fillarea(Vwk *);         /* 9   - fcba3a */
/* void vdi_v_cellarray(Vwk *); */  /* 10  - fca4e6 - not implemented */

void vdi_v_gdp(Vwk *);              /* 11  - fcba46 */
void vdi_vst_height(Vwk *);         /* 12  - fcde96 */
void vdi_vst_rotation(Vwk *);       /* 13  - fce308 */
void vdi_vsl_type(Vwk *);           /* 15  - fcab20 */

void vdi_vsl_width(Vwk *);          /* 16  - fcab6a */
void vdi_vsl_color(Vwk *);          /* 17  - fcac26 */
void vdi_vsm_type(Vwk *);           /* 18  - fcad02 */
void vdi_vsm_height(Vwk *);         /* 19  - fcac76 */
void vdi_vsm_color(Vwk *);          /* 20  - fcad52 */

void vdi_vst_font(Vwk *);           /* 21  - fce342 */
void vdi_vst_color(Vwk *);          /* 22  - fce426 */
void vdi_vsf_interior(Vwk *);       /* 23  - fcada8 */
void vdi_vsf_style(Vwk *);          /* 24  - fcadf4 */
void vdi_vsf_color(Vwk *);          /* 25  - fcae5c */

/* void vdi_vq_cellarray(Vwk *); */ /* 27  - fca4e6 - not implemented */
void vdi_v_locator(Vwk *);          /* 28  - fcaeac */
void vdi_v_valuator(Vwk *);         /* 29  - fcb042 */
void vdi_v_choice(Vwk *);           /* 30  - fcb04a */

void vdi_v_string(Vwk *);           /* 31  - fcb0d4 */
void vdi_vswr_mode(Vwk *);          /* 32  - fcb1d8 */
void vdi_vsin_mode(Vwk *);          /* 33  - fcb232 */
void vdi_v_nop(Vwk *);              /* 34  - fca4e6 */
void vdi_vql_attributes(Vwk *);     /* 35  - fcbbf8 */

void vdi_vqm_attributes(Vwk *);     /* 36  - fcbc54 */
void vdi_vqf_attributes(Vwk *);     /* 37  - fcbcb4 */
void vdi_vqt_attributes(Vwk *);     /* 38  - fce476 */
void vdi_vst_alignment(Vwk *);      /* 39  - fce2ac */


void vdi_v_opnvwk(Vwk *);           /* 100 - fcd4d8 */

void vdi_v_clsvwk(Vwk *);           /* 101 - fcd56a */
void vdi_vq_extnd(Vwk *);           /* 102 - fcb77a */
void vdi_v_contourfill(Vwk *);      /* 103 - fd1208 */
void vdi_vsf_perimeter(Vwk *);      /* 104 - fcb306 */
void vdi_v_get_pixel(Vwk *);        /* 105 - fd1906 */

void vdi_vst_effects(Vwk *);        /* 106 - fce278 */
void vdi_vst_point(Vwk *);          /* 107 - fce132 */
void vdi_vsl_ends(Vwk *);           /* 108 - fcabca */
void vdi_vro_cpyfm(Vwk *);          /* 109 - fd0770 */
void vdi_vr_trnfm(Vwk *);           /* 110 - fd1960 */

void vdi_vsc_form(Vwk *);           /* 111 */
void vdi_vsf_udpat(Vwk *);          /* 112 - fcd5c0 */
void vdi_vsl_udsty(Vwk *);          /* 113 - fcb34c */
void vdi_vr_recfl(Vwk *);           /* 114 - fcb4be */
void vdi_vqin_mode(Vwk *);          /* 115 - fcb2a0 */

void vdi_vqt_extent(Vwk *);         /* 116 - fce4f0 */
void vdi_vqt_width(Vwk *);          /* 117 - fce6b6 */
void vdi_vex_timv(Vwk *);           /* 118 - fca530 */
void vdi_vst_load_fonts(Vwk *);     /* 119 - fcebcc */
void vdi_vst_unload_fonts(Vwk *);   /* 120 - fcec60 */

void vdi_vrt_cpyfm(Vwk *);          /* 121 - fcb486 */
void vdi_v_show_c(Vwk *);           /* 122 - fcafca */
void vdi_v_hide_c(Vwk *);           /* 123 - fcaff2 */
void vdi_vq_mouse(Vwk *);           /* 124 - fcb000 */
void vdi_vex_butv(Vwk *);           /* 125 - fd040e */

void vdi_vex_motv(Vwk *);           /* 126 - fd0426 */
void vdi_vex_curv(Vwk *);           /* 127 - fd043e */
void vdi_vq_key_s(Vwk *);           /* 128 - fcb1b4 */
void vdi_vs_clip(Vwk *);            /* 129 - fcb364 */
void vdi_vqt_name(Vwk *);           /* 130 - fce790 */

void vdi_vqt_fontinfo(Vwk *);       /* 131 - fce820 */

void vdi_vex_wheelv(Vwk *);         /* 134 */

/* not in original TOS */
void v_bez_qual(Vwk *);
void v_bez_control(Vwk *);
void v_bez(Vwk *vwk, Point * points, int count);
void v_bez_fill(Vwk *vwk, Point * points, int count);


#include "vdi_col.h"


#endif                          /* VDIDEF_H */
