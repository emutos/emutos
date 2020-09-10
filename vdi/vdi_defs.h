/*
 * vdi_defs.h - Definitions for virtual workstations
 *
 * Copyright 1999 by Caldera, Inc.
 * Copyright 2005-2020 The EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef VDIDEFS_H
#define VDIDEFS_H

#include "fonthdr.h"
#include "aesext.h"
#include "vdiext.h"

#define HAVE_BEZIER 0           /* switch on bezier capability */

/*
 * some VDI opcodes
 */
#define V_OPNWK_OP      1
#define V_CLSWK_OP      2
#define V_OPNVWK_OP     100
#define V_CLSVWK_OP     101


/*
 * some minima and maxima
 */
#define MIN_LINE_STYLE  1       /* for vsl_type() */
#define MAX_LINE_STYLE  7
#define DEF_LINE_STYLE  1

#define MIN_END_STYLE   SQUARED /* for vsl_ends() */
#define MAX_END_STYLE   ROUND
#define DEF_END_STYLE   SQUARED

#define MAX_LINE_WIDTH  40

#define MIN_MARK_STYLE  1       /* for vsm_type() */
#define MAX_MARK_STYLE  6
#define DEF_MARK_STYLE  3

#define MIN_FILL_STYLE  0       /* for vsf_interior() */
#define FIS_HOLLOW      0
#define FIS_SOLID       1
#define FIS_PATTERN     2
#define FIS_HATCH       3
#define FIS_USER        4
#define MAX_FILL_STYLE  4
#define DEF_FILL_STYLE  FIS_HOLLOW

#define MIN_FILL_HATCH  1       /* for vsf_style() when fill style is hatch */
#define MAX_FILL_HATCH  12
#define DEF_FILL_HATCH  1

#define MIN_FILL_PATTERN 1      /* for vsf_style() when fill style is pattern */
#define MAX_FILL_PATTERN 24
#define DEF_FILL_PATTERN 1

#define MIN_WRT_MODE    1       /* for vswr_mode() */
#define MD_REPLACE      1
#define MD_TRANS        2
#define MD_XOR          3
#define MD_ERASE        4
#define MAX_WRT_MODE    4
#define DEF_WRT_MODE    MD_REPLACE

#define MIN_ARC_CT      32      /* min # of points to use when drawing circle/ellipse */
#define MAX_ARC_CT      128     /* max # of points ... (must not exceed MAX_VERTICES) */


/* line ending types */
#define SQUARED     0
#define ARROWED     1
#define ROUND       2

/* aliases for different table positions */
#define xres        DEV_TAB[0]
#define yres        DEV_TAB[1]
#define xsize       DEV_TAB[3]
#define ysize       DEV_TAB[4]
#define numcolors   DEV_TAB[13]

#define DEF_LWID    SIZ_TAB[4]
#define DEF_CHHT    SIZ_TAB[1]
#define DEF_CHWT    SIZ_TAB[0]
#define DEF_MKWD    SIZ_TAB[8]
#define DEF_MKHT    SIZ_TAB[9]
#define MAX_MKWD    SIZ_TAB[10]
#define MAX_MKHT    SIZ_TAB[11]

/* Defines for CONTRL[] */
#define ROUTINE     0
#define N_PTSIN     1
#define N_PTSOUT    2
#define N_INTIN     3
#define N_INTOUT    4
#define SUBROUTINE  5
#define VDI_HANDLE  6

/* text style bits: for vwk->style (and also line-A variable STYLE) */
#define F_THICKEN   1
#define F_LIGHT     2
#define F_SKEW      4
#define F_UNDER     8
#define F_OUTLINE   16
#define F_SHADOW    32

/* thickness of outline (documentation only, relies on assembler code in vdi_tblit.S) */
#define OUTLINE_THICKNESS   1

/*
 * Small subset of Vwk data, used by draw_rect_common to hide VDI/Line-A
 * specific details from rectangle & polygon drawing.
 */
typedef struct {
    WORD clip;                  /* polygon clipping on/off */
    WORD multifill;             /* Multi-plane fill flag   */
    UWORD patmsk;               /* Current pattern mask    */
    const UWORD *patptr;        /* Current pattern pointer */
    WORD wrt_mode;              /* Current writing mode    */
    UWORD color;                /* fill color */
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

/*
 * the following values are used for 'wrt_mode' in the Vwk structure above
 */
#define WM_REPLACE      (MD_REPLACE-1)
#define WM_TRANS        (MD_TRANS-1)
#define WM_XOR          (MD_XOR-1)
#define WM_ERASE        (MD_ERASE-1)


typedef struct {
    WORD x1,y1;
    WORD x2,y2;
} Rect;

typedef struct {
    WORD x1,y1;
    WORD x2,y2;
} Line;


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

extern WORD SIZ_TAB[];          /* initial ptsout array for open workstation */
extern WORD INQ_TAB[];          /* extended inquire values */

extern WORD *CONTRL, *INTIN, *PTSIN, *INTOUT, *PTSOUT;

extern WORD LN_MASK, LSTLIN;
extern WORD TERM_CH;

extern WORD MAP_COL[], REV_MAP_COL[];

/* Line-A Bit-Blt / Copy raster form variables */
extern WORD COPYTRAN;
extern WORD MFILL;

/* referenced by Line-A flood fill */
extern Vwk *CUR_WORK;           /* pointer to currently-open virtual workstation */
extern WORD (*SEEDABORT)(void); /* ptr to function called to signal early abort */

/* Mouse specific externals */
extern WORD GCURX;              /* mouse X position */
extern WORD GCURY;              /* mouse Y position */
extern WORD HIDE_CNT;           /* Number of levels the mouse is hidden */

/* Mouse related variables */
extern WORD     newx;           /* new mouse x&y position */
extern WORD     newy;           /* new mouse x&y position */
extern UBYTE    draw_flag;      /* non-zero means draw mouse form on vblank */
extern UBYTE    mouse_flag;     /* non-zero, if mouse ints disabled */
extern UBYTE    cur_ms_stat;    /* current mouse status */


BOOL clip_line(Vwk *vwk, Line *line);
void arb_corner(Rect *rect);
void arb_line(Line *line);


/* C Support routines */
Vwk *get_vwk_by_handle(WORD);
UWORD *get_start_addr(const WORD x, const WORD y);
void set_LN_MASK(Vwk *vwk);
void st_fl_ptr(Vwk *);
void gdp_justified(Vwk *);
WORD validate_color_index(WORD colnum);

/* drawing primitives */
void draw_pline(Vwk *vwk);
void arrow(Vwk *vwk, Point *point, int count);
void draw_rect(const Vwk *vwk, Rect *rect, const UWORD fillcolor);
void polygon(Vwk *vwk, Point *point, int count);
void polyline(Vwk *vwk, Point *point, int count, WORD color);
void wideline(Vwk *vwk, Point *point, int count);

/* common drawing function */
void Vwk2Attrib(const Vwk *vwk, VwkAttrib *attr, const UWORD color);
void draw_rect_common(const VwkAttrib *attr, const Rect *rect);
void clc_flit (const VwkAttrib *attr, const VwkClip *clipper, const Point *point, WORD y, int vectors);
void abline (const Line *line, const WORD wrt_mode, UWORD color);
void contourfill(const VwkAttrib *attr, const VwkClip *clip);

/* initialization of subsystems */
void init_colors(void);
void text_init(void);
void text_init2(Vwk *);
void timer_init(void);
void vdimouse_init(void);
void esc_init(Vwk *);

void vdimouse_exit(void);
void timer_exit(void);
void esc_exit(Vwk *);

/* all VDI functions */

void vdi_v_opnwk(Vwk *);            /* 1 */
void vdi_v_clswk(Vwk *);            /* 2 */
void vdi_v_clrwk(Vwk *);            /* 3 */
/* void v_updwk(Vwk *); */          /* 4 - not implemented */
void vdi_v_escape(Vwk *);           /* 5 */

void vdi_v_pline(Vwk *);            /* 6 */
void vdi_v_pmarker(Vwk *);          /* 7 */
void vdi_v_gtext(Vwk *);            /* 8 */
void vdi_v_fillarea(Vwk *);         /* 9 */
/* void vdi_v_cellarray(Vwk *); */  /* 10 - not implemented */

void vdi_v_gdp(Vwk *);              /* 11 */
void vdi_vst_height(Vwk *);         /* 12 */
void vdi_vst_rotation(Vwk *);       /* 13 */
void vdi_vs_color(Vwk *);           /* 14 */
void vdi_vsl_type(Vwk *);           /* 15 */

void vdi_vsl_width(Vwk *);          /* 16 */
void vdi_vsl_color(Vwk *);          /* 17 */
void vdi_vsm_type(Vwk *);           /* 18 */
void vdi_vsm_height(Vwk *);         /* 19 */
void vdi_vsm_color(Vwk *);          /* 20 */

void vdi_vst_font(Vwk *);           /* 21 */
void vdi_vst_color(Vwk *);          /* 22 */
void vdi_vsf_interior(Vwk *);       /* 23 */
void vdi_vsf_style(Vwk *);          /* 24 */
void vdi_vsf_color(Vwk *);          /* 25 */

void vdi_vq_color(Vwk *vwk);        /* 26 */
/* void vdi_vq_cellarray(Vwk *); */ /* 27 - not implemented */
void vdi_v_locator(Vwk *);          /* 28 */
void vdi_v_choice(Vwk *);           /* 30 */

void vdi_v_string(Vwk *);           /* 31 */
void vdi_vswr_mode(Vwk *);          /* 32 */
void vdi_vsin_mode(Vwk *);          /* 33 */
void vdi_vql_attributes(Vwk *);     /* 35 */

void vdi_vqm_attributes(Vwk *);     /* 36 */
void vdi_vqf_attributes(Vwk *);     /* 37 */
void vdi_vqt_attributes(Vwk *);     /* 38 */
void vdi_vst_alignment(Vwk *);      /* 39 */


void vdi_v_opnvwk(Vwk *);           /* 100 */

void vdi_v_clsvwk(Vwk *);           /* 101 */
void vdi_vq_extnd(Vwk *);           /* 102 */
void vdi_v_contourfill(Vwk *);      /* 103 */
void vdi_vsf_perimeter(Vwk *);      /* 104 */
void vdi_v_get_pixel(Vwk *);        /* 105 */

void vdi_vst_effects(Vwk *);        /* 106 */
void vdi_vst_point(Vwk *);          /* 107 */
void vdi_vsl_ends(Vwk *);           /* 108 */
void vdi_vro_cpyfm(Vwk *);          /* 109 */
void vdi_vr_trnfm(Vwk *);           /* 110 */

void vdi_vsc_form(Vwk *);           /* 111 */
void vdi_vsf_udpat(Vwk *);          /* 112 */
void vdi_vsl_udsty(Vwk *);          /* 113 */
void vdi_vr_recfl(Vwk *);           /* 114 */
void vdi_vqin_mode(Vwk *);          /* 115 */

void vdi_vqt_extent(Vwk *);         /* 116 */
void vdi_vqt_width(Vwk *);          /* 117 */
void vdi_vex_timv(Vwk *);           /* 118 */
void vdi_vst_load_fonts(Vwk *);     /* 119 */
void vdi_vst_unload_fonts(Vwk *);   /* 120 */

void vdi_vrt_cpyfm(Vwk *);          /* 121 */
void vdi_v_show_c(Vwk *);           /* 122 */
void vdi_v_hide_c(Vwk *);           /* 123 */
void vdi_vq_mouse(Vwk *);           /* 124 */
void vdi_vex_butv(Vwk *);           /* 125 */

void vdi_vex_motv(Vwk *);           /* 126 */
void vdi_vex_curv(Vwk *);           /* 127 */
void vdi_vq_key_s(Vwk *);           /* 128 */
void vdi_vs_clip(Vwk *);            /* 129 */
void vdi_vqt_name(Vwk *);           /* 130 */

void vdi_vqt_fontinfo(Vwk *);       /* 131 */

#if CONF_WITH_EXTENDED_MOUSE
void vdi_vex_wheelv(Vwk *);         /* 134 */
#endif

#if CONF_WITH_VDI_TEXT_SPEEDUP
void direct_screen_blit(WORD count, WORD *str);
#endif

/* not in original TOS */
void v_bez_qual(Vwk *);
void v_bez_control(Vwk *);
void v_bez(Vwk *vwk, Point *points, int count);
void v_bez_fill(Vwk *vwk, Point *points, int count);

#endif                          /* VDIDEF_H */
