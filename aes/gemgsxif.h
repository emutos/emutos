/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2016 The EmuTOS development team
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
void gsx_ncode(WORD code, WORD n, WORD m);
void gsx_1code(WORD code, WORD value);

void gsx_wsclose(void);
void gsx_wsclear(void);

void ratinit(void);
void ratexit(void);
void gsx_init(void);
void gsx_exec(LONG pcspec, WORD segenv, LONG pcmdln, LONG pfcb1, LONG pfcb2);
void gsx_graphic(WORD tographic);
void bb_save(GRECT *ps);
void bb_restore(GRECT *pr);

WORD gsx_tick(void *tcode, void *ptsave);
void gsx_mfset(const MFORM *pmfnew);

void gsx_mxmy(WORD *pmx, WORD *pmy);
WORD gsx_kstate(void);
void gsx_mon(void);
void gsx_moff(void);
WORD gsx_char(void);
WORD gsx_nplanes(void);

void g_v_pline(WORD  count, WORD *pxyarray );
void vst_clip(WORD clip_flag, WORD *pxyarray );
void vst_height(WORD height, WORD *pchr_width, WORD *pchr_height,
                WORD *pcell_width, WORD *pcell_height);
void vr_recfl(WORD *pxyarray, FDB *pdesMFDB);
void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB );
void vrt_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB,
               WORD fgcolor, WORD bgcolor);
void vrn_trnfm(FDB *psrcMFDB, FDB *pdesMFDB);
void g_vsl_width(WORD width);

#if CONF_WITH_VDI_EXTENSIONS
void vex_wheelv(PFVOID new, PFVOID *old);
#endif

#endif
