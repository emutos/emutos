/*
 * gemgsxif.h - header for EmuTOS AES's interface to the VDI
 *
 * Copyright (C) 2002-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMGSXIF_H
#define GEMGSXIF_H

#include "gsxdefs.h"


extern WORD  intout[];
extern WORD  ptsout[];

extern WORD  gl_moff;

void gsx_malloc(void);
void gsx_mfree(void);

void gsx_mret(LONG *pmaddr, LONG *pmlen);
void gsx_0code(WORD code);
void gsx_1code(WORD code, WORD value);

void gsx_wsclose(void);
void gsx_wsclear(void);

void ratinit(void);
void ratexit(void);
void gsx_init(void);
void gsx_graphic(BOOL tographic);
void bb_save(GRECT *ps);
void bb_restore(GRECT *pr);

WORD gsx_tick(void *tcode, void *ptsave);
void gsx_mfset(const MFORM *pmfnew);

void gsx_mxmy(WORD *pmx, WORD *pmy);
WORD gsx_kstate(void);
void gsx_mon(void);
void gsx_moff(void);
WORD gsx_char(void);
void gsx_setmousexy(WORD x, WORD y);
WORD gsx_nplanes(void);
void gsx_textsize(WORD *charw, WORD *charh, WORD *cellw, WORD *cellh);

void gsx_fix(FDB *pfd, void *theaddr, WORD wb, WORD h);
void gsx_fix_screen(FDB *pfd);
void v_pline(WORD count, WORD *pxyarray);
void vs_clip(WORD clip_flag, WORD *pxyarray );
void vst_height(WORD height, WORD *pchr_width, WORD *pchr_height,
                WORD *pcell_width, WORD *pcell_height);
void vr_recfl(WORD *pxyarray);
void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB );
void vrt_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB,
               WORD fgcolor, WORD bgcolor);
void vrn_trnfm(FDB *psrcMFDB, FDB *pdesMFDB);
void vsl_width(WORD width);

#if CONF_WITH_EXTENDED_MOUSE
void vex_wheelv(PFVOID new, PFVOID *old);
#endif

/*
 * use #defines for simple functions
 */
#define vsf_color(x)    gsx_1code(SET_FILL_COLOR, x)
#define vsf_interior(x) gsx_1code(SET_FILL_INTERIOR, x)
#define vsf_style(x)    gsx_1code(SET_FILL_STYLE, x)
#define vsl_type(x)     gsx_1code(SET_LINE_TYPE, x)
#define vsl_udsty(x)    gsx_1code(SET_UD_LINE_STYLE, x)

#endif
