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



/* As reference the TOS 1.0 start addresses are added */

extern void v_opnwk();          /* 1   - fcb53e */
extern void v_clswk();          /* 2   - fcb812 */
extern void v_clrwk();          /* 3   - fca4e8 */
extern void v_updwk();          /* 4   - fca4e6 */
extern void chk_esc();          /* 5   - fc412e */

extern void v_pline();          /* 6   - fcb85a */
extern void v_pmarker();        /* 7   - fcb8f4 */
extern void d_gtext();          /* 8   - fcd61c */
extern void v_fillarea();       /* 9   - fcba3a */
extern void v_cellarray();      /* 10  - fca4e6 */

extern void v_gdp();            /* 11  - fcba46 */
extern void dst_height();       /* 12  - fcde96 */
extern void dst_rotation();     /* 13  - fce308 */
extern void vs_color();         /* 14  - fd1a00 */
extern void vsl_type();         /* 15  - fcab20 */

extern void vsl_width();        /* 16  - fcab6a */
extern void vsl_color();        /* 17  - fcac26 */
extern void vsm_type();         /* 18  - fcad02 */
extern void vsm_height();       /* 19  - fcac76 */
extern void vsm_color();        /* 20  - fcad52 */

extern void dst_font();         /* 21  - fce342 */
extern void dst_color();        /* 22  - fce426 */
extern void vsf_interior();     /* 23  - fcada8 */
extern void vsf_style();        /* 24  - fcadf4 */
extern void vsf_color();        /* 25  - fcae5c */

extern void vq_color();         /* 26  - fd1ab2 */
extern void vq_cellarray();     /* 27  - fca4e6 */
extern void v_locator();        /* 28  - fcaeac */
extern void v_valuator();       /* 29  - fcb042 */
extern void v_choice();         /* 30  - fcb04a */

extern void v_string();         /* 31  - fcb0d4 */
extern void vswr_mode();        /* 32  - fcb1d8 */
extern void vsin_mode();        /* 33  - fcb232 */
extern void v_nop();            /* 34  - fca4e6 */
extern void vql_attr();         /* 35  - fcbbf8 */

extern void vqm_attr();         /* 36  - fcbc54 */
extern void vqf_attr();         /* 37  - fcbcb4 */
extern void dqt_attributes();   /* 38  - fce476 */
extern void dst_alignment();    /* 39  - fce2ac */


extern void d_opnvwk();         /* 100 - fcd4d8 */

extern void d_clsvwk();         /* 101 - fcd56a */
extern void vq_extnd();         /* 102 - fcb77a */
extern void d_contourfill();    /* 103 - fd1208 */
extern void vsf_perimeter();    /* 104 - fcb306 */
extern void v_get_pixel();      /* 105 - fd1906 */

extern void dst_style();        /* 106 - fce278 */
extern void dst_point();        /* 107 - fce132 */
extern void vsl_ends();         /* 108 - fcabca */
extern void dro_cpyfm();        /* 109 - fcb454 */
extern void tran_fm();         /* 110 - fd1960 */

extern void vro_cpyfm();        /* 111 - fd0770 */
extern void dsf_udpat();        /* 112 - fcd5c0 */
extern void vsl_udsty();        /* 113 - fcb34c */
extern void dr_recfl();         /* 114 - fcb4be */
extern void vqi_mode();         /* 115 - fcb2a0 */

extern void dqt_extent();       /* 116 - fce4f0 */
extern void dqt_width();        /* 117 - fce6b6 */
extern void vex_timv();         /* 118 - fca530 */
extern void dt_loadfont();      /* 119 - fcebcc */
extern void dt_unloadfont();    /* 120 - fcec60 */

extern void drt_cpyfm();        /* 121 - fcb486 */
extern void v_show_c();         /* 122 - fcafca */
extern void v_hide_c();         /* 123 - fcaff2 */
extern void vq_mouse();         /* 124 - fcb000 */
extern void vex_butv();         /* 125 - fd040e */

extern void vex_motv();         /* 126 - fd0426 */
extern void vex_curv();         /* 127 - fd043e */
extern void vq_key_s();         /* 128 - fcb1b4 */
extern void s_clip();           /* 129 - fcb364 */
extern void dqt_name();         /* 130 - fce790 */

extern void dqt_fontinfo();     /* 131 - fce820 */


#endif                          /* JMPTBL_H */
