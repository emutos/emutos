/*
 * jmptbl.c - Jumptable for VDI functions
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include    "gsxdef.h"
#include    "portab.h"
#include    "gsxextrn.h"
#include    "jmptbl.h"

void (*jmptb1[])() = {
    v_opnwk,            /*   1 */
    v_clswk,            /*   2 */
    v_clrwk,            /*   3 */
    v_nop,              /*   4 - v_updwk not yet implemented */
    chk_esc,            /*   5 - each escape function has it's own call */
    v_pline,            /*   6 */
    v_pmarker,          /*   7 */
    d_gtext,            /*   8 */
    v_fillarea,         /*   9 */
    v_cellarray,        /*  10 */
    v_gdp,              /*  11 */
    dst_height,         /*  12 */
    dst_rotation,       /*  13 */
    vs_color,           /*  14 */
    vsl_type,           /*  15 */
    vsl_width,          /*  16 */
    vsl_color,          /*  17 */
    vsm_type,           /*  18 */
    vsm_height,         /*  19 */
    vsm_color,          /*  20 */
    dst_font,           /*  21 */
    dst_color,          /*  22 */
    vsf_interior,       /*  23 */
    vsf_style,          /*  24 */
    vsf_color,          /*  25 */
    vq_color,           /*  26 */
    vq_cellarray,       /*  27 */
    v_locator,          /*  28 */
    v_valuator,         /*  29 */
    v_choice,           /*  30 */
    v_string,           /*  31 */
    vswr_mode,          /*  32 */
    vsin_mode,          /*  33 */
    v_nop,              /*  34 */
    vql_attr,           /*  35 */
    vqm_attr,           /*  36 */
    vqf_attr,           /*  37 */
    dqt_attributes,     /*  38 */
    dst_alignment       /*  39 */
};

void(*jmptb2[])() = {
    d_opnvwk,
    d_clsvwk,
    vq_extnd,
    d_contourfill,
    vsf_perimeter,
    v_get_pixel,
    dst_style,
    dst_point,
    vsl_ends,
    dro_cpyfm,
    tran_fm,
    xfm_crfm,
    dsf_udpat,
    vsl_udsty,
    dr_recfl,
    vqi_mode,
    dqt_extent,
    dqt_width,
    vex_timv,           /* in lisagem.S */
    dt_loadfont,
    dt_unloadfont,
    drt_cpyfm,
    v_show_c,
    v_hide_c,
    vq_mouse,
    vex_butv,           /* in vdimouse.S */
    vex_motv,           /* in vdimouse.S */
    vex_curv,           /* in vdimouse.S */
    vq_key_s,
    s_clip,
    dqt_name,
    dqt_fontinfo
};



/*
 * screen - Screen driver entry point
 */

void screen()
{
    REG WORD opcode, r, *control;
    REG struct attribute *work_ptr;
    BYTE found;

    control = CONTRL;
    r = *(control + 6);

    opcode = *control;

    //cprintf("SCREEN opcode=%d\n", opcode);

    /* no ints out & no pts out */

    *(control + 2) = 0;
    *(control + 4) = 0;

    FLIP_Y = 0;

    if (opcode != 1 && opcode != 100) {

        /* Find the attribute area which matches the handle */

        work_ptr = &virt_work;

        found = 0;
        do {
            found = (r == work_ptr->handle);
        } while (!found && (work_ptr = work_ptr->next_work));

        /* handle is invalid if we fall through, so exit */

        if (!found)
            return;

        cur_work = work_ptr;
        INQ_TAB[19] = CLIP = work_ptr->clip;
        XMN_CLIP = work_ptr->xmn_clip;
        YMN_CLIP = work_ptr->ymn_clip;
        XMX_CLIP = work_ptr->xmx_clip;
        YMX_CLIP = work_ptr->ymx_clip;

        WRT_MODE = work_ptr->wrt_mode;

        patptr = work_ptr->patptr;
        patmsk = work_ptr->patmsk;

        if (work_ptr->fill_style == 4)
            multifill = work_ptr->multifill;
        else
            multifill = 0;

        font_ring[2] = work_ptr->loaded_fonts;

        DEV_TAB[10] = work_ptr->num_fonts;

        DDA_INC = work_ptr->dda_inc;
        T_SCLSTS = work_ptr->t_sclsts;
        DOUBLE = work_ptr->scaled;

        cur_font = work_ptr->cur_font;

        MONO_STATUS = F_MONOSPACE & cur_font->flags;
        scrpt2 = work_ptr->scrpt2;
        scrtchp = work_ptr->scrtchp;
        STYLE = work_ptr->style;
        h_align = work_ptr->h_align;
        v_align = work_ptr->v_align;
        CHUP = work_ptr->chup;

    }
    /* end if open work or vwork */
    if (opcode >= 1 && opcode <= 39) {
        opcode--;
        (*jmptb1[opcode]) ();
    }

    else if (opcode >= 100 && opcode <= 131) {
        opcode -= 100;
        (*jmptb2[opcode]) ();
    }

}
