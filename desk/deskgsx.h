/*
 * EmuTOS desktop
 *
 * Copyright (c) 2002, 2010 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

void gsx_vclose(void);
void d_v_pline(WORD count, WORD *pxyarray);
void gsx_1code(WORD  code, WORD  value);
void gsx_vopen(void);
WORD vst_clip(WORD clip_flag, WORD *pxyarray);
void vst_height(WORD height, WORD *pchr_width, WORD *pchr_height,
                WORD *pcell_width, WORD *pcell_height);
void d_vsl_width(WORD width);
void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB);
void vrt_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB,
               WORD fgcolor, WORD bgcolor);
void vrn_trnfm(FDB *psrcMFDB, FDB *pdesMFDB);
void vr_recfl(WORD *pxyarray, FDB *pdesMFDB);
