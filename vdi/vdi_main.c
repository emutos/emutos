/*
 * vdimain.c - the VDI screen driver dispatcher
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "config.h"
#include "portab.h"
#include "vdi_defs.h"
#include "kprint.h"

/* forward prototypes */
void screen(void);


WORD flip_y;                    /* True if magnitudes being returned */


/* Two main jumptables for VDI functions */
static void (* const jmptb1[])(Vwk *) = {
    vdi_v_opnwk,            /*   1 */
    vdi_v_clswk,            /*   2 */
    vdi_v_clrwk,            /*   3 */
    vdi_v_nop,              /*   4 - v_updwk not yet implemented */
    vdi_v_escape,           /*   5 - each escape subfunction has its own call */
    vdi_v_pline,            /*   6 */
    vdi_v_pmarker,          /*   7 */
    vdi_v_gtext,            /*   8 */
    vdi_v_fillarea,         /*   9 */
    vdi_v_nop,              /*  10 - v_cellarray(), not usually implemented by drivers */
    vdi_v_gdp,              /*  11 */
    vdi_vst_height,         /*  12 */
    vdi_vst_rotation,       /*  13 */
    vdi_vs_color,           /*  14 */
    vdi_vsl_type,           /*  15 */
    vdi_vsl_width,          /*  16 */
    vdi_vsl_color,          /*  17 */
    vdi_vsm_type,           /*  18 */
    vdi_vsm_height,         /*  19 */
    vdi_vsm_color,          /*  20 */
    vdi_vst_font,           /*  21 */
    vdi_vst_color,          /*  22 */
    vdi_vsf_interior,       /*  23 */
    vdi_vsf_style,          /*  24 */
    vdi_vsf_color,          /*  25 */
    vdi_vq_color,           /*  26 */
    vdi_v_nop,              /*  27 - vq_cellarray, not usually implemented by drivers */
    vdi_v_locator,          /*  28 */
    vdi_v_valuator,         /*  29 */
    vdi_v_choice,           /*  30 */
    vdi_v_string,           /*  31 */
    vdi_vswr_mode,          /*  32 */
    vdi_vsin_mode,          /*  33 */
    vdi_v_nop,              /*  34 - does not exist */
    vdi_vql_attributes,     /*  35 */
    vdi_vqm_attributes,     /*  36 */
    vdi_vqf_attributes,     /*  37 */
    vdi_vqt_attributes,     /*  38 */
    vdi_vst_alignment       /*  39 */
};

static void(* const jmptb2[])(Vwk *) = {
    vdi_v_opnvwk,           /* 100 */
    vdi_v_clsvwk,           /* 101 */
    vdi_vq_extnd,           /* 102 */
    vdi_v_contourfill,      /* 103 */
    vdi_vsf_perimeter,      /* 104 */
    vdi_v_get_pixel,        /* 105 */
    vdi_vst_effects,        /* 106 */
    vdi_vst_point,          /* 107 */
    vdi_vsl_ends,           /* 108 */
    vdi_vro_cpyfm,          /* 109 */
    vdi_vr_trnfm,           /* 110 */
    vdi_vsc_form,           /* 111 */
    vdi_vsf_udpat,          /* 112 */
    vdi_vsl_udsty,          /* 113 */
    vdi_vr_recfl,           /* 114 */
    vdi_vqin_mode,          /* 115 */
    vdi_vqt_extent,         /* 116 */
    vdi_vqt_width,          /* 117 */
    vdi_vex_timv,           /* 118 */ /* in vdi_misc.c */
    vdi_vst_load_fonts,     /* 119 */
    vdi_vst_unload_fonts,   /* 120 */
    vdi_vrt_cpyfm,          /* 121 */
    vdi_v_show_c,           /* 122 */
    vdi_v_hide_c,           /* 123 */
    vdi_vq_mouse,           /* 124 */
    vdi_vex_butv,           /* 125 */ /* in vdi_mouse.c */
    vdi_vex_motv,           /* 126 */ /* in vdi_mouse.c */
    vdi_vex_curv,           /* 127 */ /* in vdi_mouse.c */
    vdi_vq_key_s,           /* 128 */
    vdi_vs_clip,            /* 129 */
    vdi_vqt_name,           /* 130 */
    vdi_vqt_fontinfo,       /* 131 */
#if CONF_WITH_VDI_EXTENSIONS
    vdi_v_nop,              /* 132 */ /* vqt_justified (PC-GEM) */
    vdi_v_nop,              /* 133 */ /* vs_grayoverride (PC-GEM/3) */
    vdi_vex_wheelv          /* 134 */ /* (Milan), also v_pat_rotate (PC-GEM/3) */
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

    if (opcode >= 1 && opcode < 1+JMPTB1_ENTRIES) {
        (*jmptb1[opcode - 1]) (vwk);
    }

    else if (opcode >= 100 && opcode < 100+JMPTB2_ENTRIES) {
        (*jmptb2[opcode - 100]) (vwk);
    }
}
