/*
 * vdimain.c - the VDI screen driver dispatcher
 *
 * Copyright (C) 1999 Caldera, Inc.
 *               2002-2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "vdi_defs.h"
#include "lineavars.h"
#include "asm.h"

/* forward prototypes */
void screen(void);


WORD flip_y;                    /* True if magnitudes being returned */

typedef void (*VDI_OP_T)(Vwk *); /* Pointer type to VDI operation */
#define vdi_v_nop ((VDI_OP_T)just_rts) /* VDI dummy operation */

struct vdi_jmptab {
    unsigned char nptsout;
    unsigned char nintout;
    VDI_OP_T op;
};

/* Two main jumptables for VDI functions */
static struct vdi_jmptab const jmptb1[] = {
    { 6, 45, vdi_v_opnwk },            /*   1 */
    { 0,  0, vdi_v_clswk },            /*   2 */
    { 0,  0, vdi_v_clrwk },            /*   3 */
    { 0,  0, vdi_v_nop },              /*   4 - v_updwk not yet implemented */
    { 0,  0, vdi_v_escape },           /*   5 - each escape subfunction has its own call */
    { 0,  0, vdi_v_pline },            /*   6 */
    { 0,  0, vdi_v_pmarker },          /*   7 */
    { 0,  0, vdi_v_gtext },            /*   8 */
    { 0,  0, vdi_v_fillarea },         /*   9 */
    { 0,  0, vdi_v_nop },              /*  10 - v_cellarray(), not usually implemented by drivers */
    { 0,  0, vdi_v_gdp },              /*  11 */
    { 2,  0, vdi_vst_height },         /*  12 */
    { 0,  1, vdi_vst_rotation },       /*  13 */
    { 0,  0, vdi_vs_color },           /*  14 */
    { 0,  1, vdi_vsl_type },           /*  15 */
    { 1,  0, vdi_vsl_width },          /*  16 */
    { 0,  1, vdi_vsl_color },          /*  17 */
    { 0,  1, vdi_vsm_type },           /*  18 */
    { 1,  0, vdi_vsm_height },         /*  19 */
    { 0,  1, vdi_vsm_color },          /*  20 */
    { 0,  1, vdi_vst_font },           /*  21 */
    { 0,  1, vdi_vst_color },          /*  22 */
    { 0,  1, vdi_vsf_interior },       /*  23 */
    { 0,  1, vdi_vsf_style },          /*  24 */
    { 0,  1, vdi_vsf_color },          /*  25 */
    { 0,  4, vdi_vq_color },           /*  26 */
    { 0,  0, vdi_v_nop },              /*  27 - vq_cellarray, not usually implemented by drivers */
    { 0,  0, vdi_v_locator },          /*  28 */
    { 0,  0, vdi_v_nop },              /*  29 - vdi_v_valuator, not usually implemented by drivers */
    { 0,  1, vdi_v_choice },           /*  30 */
    { 0,  0, vdi_v_string },           /*  31 */
    { 0,  1, vdi_vswr_mode },          /*  32 */
    { 0,  1, vdi_vsin_mode },          /*  33 */
    { 0,  0, vdi_v_nop },              /*  34 - does not exist */
    { 1,  3, vdi_vql_attributes },     /*  35 */
    { 1,  3, vdi_vqm_attributes },     /*  36 */
    { 0,  5, vdi_vqf_attributes },     /*  37 */
    { 2,  6, vdi_vqt_attributes },     /*  38 */
    { 0,  2, vdi_vst_alignment }       /*  39 */
};

static struct vdi_jmptab const jmptb2[] = {
    { 6, 45, vdi_v_opnvwk },           /* 100 */
    { 0,  0, vdi_v_clsvwk },           /* 101 */
    { 6, 45, vdi_vq_extnd },           /* 102 */
    { 0,  0, vdi_v_contourfill },      /* 103 */
    { 0,  1, vdi_vsf_perimeter },      /* 104 */
    { 0,  2, vdi_v_get_pixel },        /* 105 */
    { 0,  1, vdi_vst_effects },        /* 106 */
    { 2,  1, vdi_vst_point },          /* 107 */
    { 0,  2, vdi_vsl_ends },           /* 108 */
    { 0,  0, vdi_vro_cpyfm },          /* 109 */
    { 0,  0, vdi_vr_trnfm },           /* 110 */
    { 0,  0, vdi_vsc_form },           /* 111 */
    { 0,  0, vdi_vsf_udpat },          /* 112 */
    { 0,  0, vdi_vsl_udsty },          /* 113 */
    { 0,  0, vdi_vr_recfl },           /* 114 */
    { 0,  1, vdi_vqin_mode },          /* 115 */
    { 4,  0, vdi_vqt_extent },         /* 116 */
    { 3,  1, vdi_vqt_width },          /* 117 */
    { 0,  1, vdi_vex_timv },           /* 118 */ /* in vdi_misc.c */
    { 0,  1, vdi_vst_load_fonts },     /* 119 */
    { 0,  0, vdi_vst_unload_fonts },   /* 120 */
    { 0,  0, vdi_vrt_cpyfm },          /* 121 */
    { 0,  0, vdi_v_show_c },           /* 122 */
    { 0,  0, vdi_v_hide_c },           /* 123 */
    { 1,  1, vdi_vq_mouse },           /* 124 */
    { 0,  0, vdi_vex_butv },           /* 125 */ /* in vdi_mouse.c */
    { 0,  0, vdi_vex_motv },           /* 126 */ /* in vdi_mouse.c */
    { 0,  0, vdi_vex_curv },           /* 127 */ /* in vdi_mouse.c */
    { 0,  1, vdi_vq_key_s },           /* 128 */
    { 0,  0, vdi_vs_clip },            /* 129 */
    { 0, 33, vdi_vqt_name },           /* 130 */
    { 5,  2, vdi_vqt_fontinfo },       /* 131 */
#if CONF_WITH_EXTENDED_MOUSE
    { 0,  0, vdi_v_nop },              /* 132 */ /* vqt_justified (PC-GEM) */
    { 0,  0, vdi_v_nop },              /* 133 */ /* vs_grayoverride (PC-GEM/3) */
    { 0,  0, vdi_vex_wheelv }          /* 134 */ /* (Milan), also v_pat_rotate (PC-GEM/3) */
#endif
};

#define JMPTB1_ENTRIES  ARRAY_SIZE(jmptb1)
#define JMPTB2_ENTRIES  ARRAY_SIZE(jmptb2)


/*
 * screen - Screen driver entry point
 */
void screen(void)
{
    WORD opcode, handle;
    WORD *contrl = CONTRL;
    const struct vdi_jmptab *jmptab;
    Vwk *vwk = NULL;

    /* get workstation handle */
    handle = CONTRL[6];

    /* no ints out & no pts out */
    contrl[2] = 0;
    contrl[4] = 0;

    flip_y = 0;
    opcode = contrl[0];

    /* is it open work or vwork? */
    if ((opcode != V_OPNWK_OP) && (opcode != V_OPNVWK_OP)) {
        /* Find the vwk which matches the handle, if there */
        vwk = get_vwk_by_handle(handle);
        if (!vwk)
            return;

        /* This copying is done for assembler routines */
        if (vwk->fill_style != 4)       /* multifill just for user */
            vwk->multifill = 0;
    }

    if ((opcode >= V_OPNWK_OP) && (opcode < V_OPNWK_OP+JMPTB1_ENTRIES)) {
        jmptab = &jmptb1[opcode - V_OPNWK_OP];
    }
    else if ((opcode >= V_OPNVWK_OP) && (opcode < V_OPNVWK_OP+JMPTB2_ENTRIES)) {
        jmptab = &jmptb2[opcode - V_OPNVWK_OP];
    } else {
        return;
    }
    contrl[2] = jmptab->nptsout;
    contrl[4] = jmptab->nintout;
    (*jmptab->op) (vwk);

    /*
     * at this point, for v_opnwk() and v_opnvwk(), vwk is NULL.  we
     * must fix this before we use it to set the lineA variables below.
     * fortunately, v_opnwk() and v_opnvwk() have set CUR_WORK to a
     * valid value (see vdi_control.c).  so we use this to set vwk.
     */
    if ((opcode == V_OPNWK_OP) || (opcode == V_OPNVWK_OP))
    {
        vwk = CUR_WORK;
    }

    /*
     * set some line-A variables from the vwk info (as long as
     * the workstation is valid)
     */
    if ((opcode != V_CLSWK_OP) && (opcode != V_CLSVWK_OP)) {
        /*
         * the following assignments are not required by EmuTOS, but
         * ensure that the values in the line-A variables mirror those
         * in the current virtual workstation, just like in Atari TOS.
         */
        CUR_FONT = vwk->cur_font;
        WRT_MODE = vwk->wrt_mode;
        CLIP = vwk->clip;
        XMINCL = vwk->xmn_clip;
        YMINCL = vwk->ymn_clip;
        XMAXCL = vwk->xmx_clip;
        YMAXCL = vwk->ymx_clip;
        font_ring[2] = vwk->loaded_fonts;
        CUR_WORK = vwk;
    }
}
