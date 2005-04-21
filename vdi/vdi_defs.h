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



#ifndef VDIDEFS_H
#define VDIDEFS_H

#include "portab.h"


#define HAVE_BEZIER 0   // switch on bezier capability

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

typedef struct Fonthead_ Fonthead;
struct Fonthead_ {              /* descibes a font */
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

    Fonthead *next_font;        /* pointer to next font */
    UWORD font_seg;
};


/* Structure to hold data for a virtual workstation */
typedef struct Vwk_ Vwk;
struct Vwk_ {
    WORD chup;                  /* Character Up vector */
    WORD clip;                  /* Clipping Flag */
    Fonthead *cur_font;         /* Pointer to current font */
    WORD dda_inc;               /* Fraction to be added to the DDA */
    WORD multifill;             /* Multi-plane fill flag */
    UWORD patmsk;               /* Current pattern mask */
    UWORD *patptr;              /* Current pattern pointer */
    WORD pts_mode;              /* TRUE if height set in points mode */
    WORD *scrtchp;              /* Pointer to text scratch buffer */
    WORD scrpt2;                /* Offset to large text buffer */
    WORD style;                 /* Current text style */
    WORD t_sclsts;              /* TRUE if scaling up */
    WORD fill_color;            /* Current fill color (PEL value) */
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
    Fonthead *loaded_fonts;     /* Pointer to first loaded font     */
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


typedef struct Hzline_ Hzline;
struct Hzline_
{
    UWORD *addr;
    int dx;
    int leftpart;
    int rightpart;
    WORD patind;
    int patadd;
    UWORD color;
    UWORD rightmask;
    UWORD leftmask;
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
extern WORD num_qc_lines, q_circle[];
extern WORD val_mode, chc_mode, loc_mode, str_mode;
extern BYTE shft_off;           /* once computed Offset into a Scan Line */

/* gdp area variables */
extern WORD xc, yc, xrad, yrad, del_ang, beg_ang, end_ang;
extern WORD angle, n_steps;

/* attribute environment save variables */
extern WORD s_begsty, s_endsty, s_fil_col;

/* These are still needed for text blitting */
extern UWORD LINE_STYLE[];
extern UWORD DITHER[];
extern UWORD HATCH0[], HATCH1[], OEMPAT[];
extern UWORD ROM_UD_PATRN[];
extern UWORD SOLID;
extern UWORD HOLLOW;
extern UWORD HAT_0_MSK, HAT_1_MSK;
extern UWORD DITHRMSK, OEMMSKPAT;

extern WORD DEV_TAB[];          /* initial intout array for open workstation */
extern WORD SIZ_TAB[];          /* initial ptsout array for open workstation */
extern WORD INQ_TAB[];          /* extended inquire values */

extern WORD *CONTRL, *INTIN, *PTSIN, *INTOUT, *PTSOUT;

extern WORD LN_MASK, LSTLIN;
extern WORD REQ_COL[3][MAX_COLOR];
extern WORD MAP_COL[], REV_MAP_COL[];
extern WORD TERM_CH;

/* Bit-Blt variables */
extern WORD COPYTRAN;

/* Mouse specific externals */
extern WORD GCURX;              // mouse X position
extern WORD GCURY;              // mouse Y position
extern WORD HIDE_CNT;           // Number of levels the mouse is hidden
extern WORD MOUSE_BT;           // mouse button state

/* Mouse related variables */
extern WORD     newx;           // new mouse x&y position
extern WORD     newy;           // new mouse x&y position
extern BYTE     draw_flag;      // non-zero means draw mouse form on vblank
extern BYTE     mouse_flag;     // non-zero, if mouse ints disabled
extern BYTE     cur_ms_stat;    /* current mouse status */



/* Assembly Language Support Routines NEWLY ADDED */
void text_blt(void);
void xfm_crfm(Vwk * vwk);
void rectfill (Vwk * vwk, Rect * rect);


WORD gloc_key(void);
WORD gchc_key(void);
WORD gchr_key(void);
WORD gshift_s(void);

BOOL clip_line(Vwk * vwk, Line * line);
WORD vec_len(WORD x, WORD y);
void arb_corner(Rect * rect);
void arb_line(Line * line);


/* C Support routines */
Vwk * get_vwk_by_handle(WORD);
void * get_start_addr(const WORD x, const WORD y);

void v_show_c(Vwk *);
void v_hide_c(Vwk *);
void v_clrwk(Vwk *);
void chk_esc(Vwk *);
void st_fl_ptr(Vwk *);
void d_justified(Vwk *);
void r_fa_attr(Vwk *);
void s_fa_attr(Vwk *);
void vex_butv(Vwk *);
void vex_motv(Vwk *);
void vex_curv(Vwk *);
void vex_timv(Vwk *);

/* drawing primitives */
void abline (Vwk * vwk, Line * line);
void draw_pline(Vwk * vwk);

void horzline(const Vwk * vwk, Line * line);
void draw_rect(const Vwk * vwk, const Rect * rect, const UWORD fillcolor);
void polygon(Vwk * vwk, Point * point, int count);
void polyline(Vwk * vwk, Point * point, int count);
void wideline(Vwk * vwk, Point * point, int count);

WORD Isqrt(ULONG x);

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
void v_opnwk(Vwk *);          /* 1   - fcb53e */
void v_clswk(Vwk *);          /* 2   - fcb812 */
void v_clrwk(Vwk *);          /* 3   - fca4e8 */
void v_updwk(Vwk *);          /* 4   - fca4e6 */
void chk_esc(Vwk *);          /* 5   - fc412e */

void v_pline(Vwk *);          /* 6   - fcb85a */
void v_pmarker(Vwk *);        /* 7   - fcb8f4 */
void d_gtext(Vwk *);          /* 8   - fcd61c */
void v_fillarea(Vwk *);       /* 9   - fcba3a */
void v_cellarray(Vwk *);      /* 10  - fca4e6 */

void v_gdp(Vwk *);            /* 11  - fcba46 */
void dst_height(Vwk *);       /* 12  - fcde96 */
void dst_rotation(Vwk *);     /* 13  - fce308 */
void vs_color(Vwk *);         /* 14  - fd1a00 */
void vsl_type(Vwk *);         /* 15  - fcab20 */

void vsl_width(Vwk *);        /* 16  - fcab6a */
void vsl_color(Vwk *);        /* 17  - fcac26 */
void vsm_type(Vwk *);         /* 18  - fcad02 */
void vsm_height(Vwk *);       /* 19  - fcac76 */
void vsm_color(Vwk *);        /* 20  - fcad52 */

void dst_font(Vwk *);         /* 21  - fce342 */
void dst_color(Vwk *);        /* 22  - fce426 */
void vsf_interior(Vwk *);     /* 23  - fcada8 */
void vsf_style(Vwk *);        /* 24  - fcadf4 */
void vsf_color(Vwk *);        /* 25  - fcae5c */

void vq_color(Vwk *);         /* 26  - fd1ab2 */
void vq_cellarray(Vwk *);     /* 27  - fca4e6 */
void v_locator(Vwk *);        /* 28  - fcaeac */
void v_valuator(Vwk *);       /* 29  - fcb042 */
void v_choice(Vwk *);         /* 30  - fcb04a */

void v_string(Vwk *);         /* 31  - fcb0d4 */
void vswr_mode(Vwk *);        /* 32  - fcb1d8 */
void vsin_mode(Vwk *);        /* 33  - fcb232 */
void v_nop(Vwk *);            /* 34  - fca4e6 */
void vql_attr(Vwk *);         /* 35  - fcbbf8 */

void vqm_attr(Vwk *);         /* 36  - fcbc54 */
void vqf_attr(Vwk *);         /* 37  - fcbcb4 */
void dqt_attributes(Vwk *);   /* 38  - fce476 */
void dst_alignment(Vwk *);    /* 39  - fce2ac */


void d_opnvwk(Vwk *);         /* 100 - fcd4d8 */

void d_clsvwk(Vwk *);         /* 101 - fcd56a */
void vq_extnd(Vwk *);         /* 102 - fcb77a */
void d_contourfill(Vwk *);    /* 103 - fd1208 */
void vsf_perimeter(Vwk *);    /* 104 - fcb306 */
void v_get_pixel(Vwk *);      /* 105 - fd1906 */

void dst_style(Vwk *);        /* 106 - fce278 */
void dst_point(Vwk *);        /* 107 - fce132 */
void vsl_ends(Vwk *);         /* 108 - fcabca */
void dro_cpyfm(Vwk *);        /* 109 - fcb454 */
void vr_trnfm(Vwk *);         /* 110 - fd1960 */

void vdi_vro_cpyfm(Vwk *);    /* 111 - fd0770 */
void dsf_udpat(Vwk *);        /* 112 - fcd5c0 */
void vsl_udsty(Vwk *);        /* 113 - fcb34c */
void dr_recfl(Vwk *);         /* 114 - fcb4be */
void vqi_mode(Vwk *);         /* 115 - fcb2a0 */

void dqt_extent(Vwk *);       /* 116 - fce4f0 */
void dqt_width(Vwk *);        /* 117 - fce6b6 */
void vex_timv(Vwk *);         /* 118 - fca530 */
void dt_loadfont(Vwk *);      /* 119 - fcebcc */
void dt_unloadfont(Vwk *);    /* 120 - fcec60 */

void vdi_vrt_cpyfm(Vwk *);    /* 121 - fcb486 */
void v_show_c(Vwk *);         /* 122 - fcafca */
void v_hide_c(Vwk *);         /* 123 - fcaff2 */
void vq_mouse(Vwk *);         /* 124 - fcb000 */
void vex_butv(Vwk *);         /* 125 - fd040e */

void vex_motv(Vwk *);         /* 126 - fd0426 */
void vex_curv(Vwk *);         /* 127 - fd043e */
void vq_key_s(Vwk *);         /* 128 - fcb1b4 */
void s_clip(Vwk *);           /* 129 - fcb364 */
void dqt_name(Vwk *);         /* 130 - fce790 */

void dqt_fontinfo(Vwk *);     /* 131 - fce820 */

void vex_wheelv(Vwk *);       /* 134 */

/* not in original TOS */
void v_bez_qual(Vwk *);
void v_bez_control(Vwk *);
void v_bez(Vwk *vwk, Point * points, int count);
void v_bez_fill(Vwk *vwk, Point * points, int count);


#endif                          /* VDIDEF_H */
