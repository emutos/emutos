/*
 * jmptbl.h - External declarations for VDI function jump table
 *
 * Copyright (c) 1999 Caldera, Inc. and Authors:
 *
 *
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef _JMPTBL_H
#define _JMPTBL_H

#include "portab.h"

extern void v_nop();            /* 0   */

extern void v_opnwk();          /* 1   */
extern void v_clswk();          /* 2   */
extern void v_clrwk();          /* 3   */
extern void v_updwk();          /* 4   */
extern void CHK_ESC();          /* 5   */

extern void v_pline();          /* 6   */
extern void v_pmarker();        /* 7   */
extern void d_gtext();          /* 8   */
extern void v_fillarea();       /* 9   */
extern void v_cellarray();      /* 10  */

extern void v_gdp();            /* 11  */
extern void dst_height();       /* 12  */
extern void dst_rotation();     /* 13  */
extern void vs_color();         /* 14  */
extern void vsl_type();         /* 15  */

extern void vsl_width();        /* 16  */
extern void vsl_color();        /* 17  */
extern void vsm_type();         /* 18  */
extern void vsm_height();       /* 19  */
extern void vsm_color();        /* 20  */

extern void dst_font();         /* 21  */
extern void dst_color();        /* 22  */
extern void vsf_interior();     /* 23  */
extern void vsf_style();        /* 24  */
extern void vsf_color();        /* 25  */

extern void vq_color();         /* 26  */
extern void vq_cellarray();     /* 27  */
extern void v_locator();        /* 28  */
extern void v_valuator();       /* 29  */
extern void v_choice();         /* 30  */

extern void v_string();         /* 31  */
extern void vswr_mode();        /* 32  */
extern void vsin_mode();        /* 33  */
extern void vql_attr();         /* 35  */

extern void vqm_attr();         /* 36  */
extern void vqf_attr();         /* 37  */
extern void dqt_attributes();   /* 38  */
extern void dst_alignment();    /* 39  */

extern void d_opnvwk();         /* 100  */

extern void d_clsvwk();         /* 101  */
extern void vq_extnd();         /* 102  */
extern void d_contourfill();    /* 103  */
extern void vsf_perimeter();    /* 104  */
extern void v_get_pixel();      /* 105  */

extern void dst_style();        /* 106  */
extern void dst_point();        /* 107  */
extern void vsl_ends();         /* 108  */
extern void dro_cpyfm();        /* 109  */
extern void TRAN_FM();          /* 110  */

extern void XFM_CRFM();         /* 111  */
extern void dsf_udpat();        /* 112  */
extern void vsl_udsty();        /* 113  */
extern void dr_recfl();         /* 114  */
extern void vqi_mode();         /* 115  */

extern void dqt_extent();       /* 116  */
extern void dqt_width();        /* 117  */
extern void EX_TIMV();          /* 118  */
extern void dt_loadfont();      /* 119  */
extern void dt_unloadfont();    /* 120  */

extern void drt_cpyfm();        /* 121  */
extern void v_show_c();         /* 122  */
extern void v_hide_c();         /* 123  */
extern void vq_mouse_status();  /* 124  */
extern void vex_butv();         /* 125  */

extern void vex_motv();         /* 126  */
extern void vex_curv();         /* 127  */
extern void vq_key_s();         /* 128  */
extern void s_clip();           /* 129  */
extern void dqt_name();         /* 130  */

extern void dqt_fontinfo();     /* 131  */


#endif                          /* JMPTBL_H */
