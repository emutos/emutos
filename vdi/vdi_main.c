/*
 * vdimain.c - Many nongraphicle VDI functions
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "portab.h"
#include "lineavars.h"
#include "vdi_defs.h"
#include "kprint.h"

/* forward prototypes */
void screen(void);


#define ptsin_size 256          // max. # of elements allowed for PTSIN array
#define ptsin_max  ptsin_size/2 // max. # of coordinate pairs for PTSIN array

WORD lcl_ptsin[ptsin_size];
WORD flip_y;                    /* True if magnitudes being returned */

/* GDP variables */
WORD angle, beg_ang, del_ang, end_ang;
WORD xc, xrad, yc, yrad;


/* Two main jumptables for VDI functions */
void (*jmptb1[])(Vwk *) = {
    _v_opnwk,            /*   1 */
    _v_clswk,            /*   2 */
    _v_clrwk,            /*   3 */
    v_nop,              /*   4 - v_updwk not yet implemented */
    chk_esc,            /*   5 - each escape function has it's own call */
    _v_pline,            /*   6 */
    _v_pmarker,          /*   7 */
    d_gtext,            /*   8 */
    _v_fillarea,         /*   9 */
    _v_cellarray,        /*  10 */
    v_gdp,              /*  11 */
    dst_height,         /*  12 */
    dst_rotation,       /*  13 */
    _vs_color,           /*  14 */
    _vsl_type,           /*  15 */
    _vsl_width,          /*  16 */
    _vsl_color,          /*  17 */
    _vsm_type,           /*  18 */
    _vsm_height,         /*  19 */
    _vsm_color,          /*  20 */
    dst_font,           /*  21 */
    dst_color,          /*  22 */
    _vsf_interior,       /*  23 */
    _vsf_style,          /*  24 */
    _vsf_color,          /*  25 */
    _vq_color,           /*  26 */
    vq_cellarray,       /*  27 */
    v_locator,          /*  28 */
    v_valuator,         /*  29 */
    v_choice,           /*  30 */
    v_string,           /*  31 */
    _vswr_mode,          /*  32 */
    vsin_mode,          /*  33 */
    v_nop,              /*  34 */
    vql_attr,           /*  35 */
    vqm_attr,           /*  36 */
    vqf_attr,           /*  37 */
    dqt_attributes,     /*  38 */
    dst_alignment       /*  39 */
};

void(*jmptb2[])(Vwk *) = {
    d_opnvwk,           /* 100 */
    d_clsvwk,           /* 101 */
    _vq_extnd,           /* 102 */
    d_contourfill,      /* 103 */
    _vsf_perimeter,      /* 104 */
    _v_get_pixel,        /* 105 */
    dst_style,          /* 106 */
    dst_point,          /* 107 */
    _vsl_ends,           /* 108 */
    vdi_vro_cpyfm,      /* 109 */
    _vr_trnfm,           /* 110 */
    xfm_crfm,           /* 111 */
    dsf_udpat,          /* 112 */
    _vsl_udsty,          /* 113 */
    dr_recfl,           /* 114 */
    vqi_mode,           /* 115 */
    dqt_extent,         /* 116 */
    dqt_width,          /* 117 */
    _vex_timv,           /* 118 */ /* in vdi_misc.c */
    dt_loadfont,        /* 119 */
    dt_unloadfont,      /* 120 */
    vdi_vrt_cpyfm,      /* 121 */
    _v_show_c,           /* 122 */
    _v_hide_c,           /* 123 */
    _vq_mouse,           /* 124 */
    _vex_butv,           /* 125 */ /* in vdi_mouse.c */
    _vex_motv,           /* 126 */ /* in vdi_mouse.c */
    _vex_curv,           /* 127 */ /* in vdi_mouse.c */
    vq_key_s,           /* 128 */
    s_clip,             /* 129 */
    dqt_name,           /* 130 */
    dqt_fontinfo,       /* 131 */
    v_nop,              /* 132 */ /* vqt_justified */
    v_nop,              /* 133 */
    _vex_wheelv          /* 134 */
};



/*
 * screen - Screen driver entry point
 */

void screen(void)
{
    WORD opcode, handle;
    Vwk *vwk = NULL;

    /* get workstation handle */
    handle = CONTRL[6];

    /* no ints out & no pts out */
    CONTRL[2] = 0;
    CONTRL[4] = 0;

    flip_y = 0;
    opcode = CONTRL[0];

    /* is it open work or vwork? */
    if (opcode != 1 && opcode != 100) {
        /* Find the vwk which matches the handle, if there */
        vwk = get_vwk_by_handle(handle);
        if (!vwk)
            return;

        /* This copying is done for assembler routines */
        if (vwk->fill_style != 4)       /* multifill just for user */
            vwk->multifill = 0;
    }

    if (opcode >= 1 && opcode <= 39) {
        (*jmptb1[opcode - 1]) (vwk);
    }

    else if (opcode >= 100 && opcode <= 134) {
        (*jmptb2[opcode - 100]) (vwk);
    }
}
