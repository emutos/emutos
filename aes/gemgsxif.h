
#ifndef GEMGSXIF_H
#define GEMGSXIF_H

#include "gsxdefs.h"


extern WORD  intout[];
extern WORD  ptsout[];

extern FDB   gl_tmp;

extern LONG  old_mcode;
extern LONG  old_bcode;
extern WORD  gl_moff;
extern LONG  gl_mlen;
extern WORD  gl_graphic;


ULONG gsx_mcalc();
void gsx_malloc();
void gsx_mfree();

void gsx_mret(LONG *pmaddr, LONG *pmlen);
void gsx_ncode(WORD code, WORD n, WORD m);
void gsx_1code(WORD code, WORD value);

void gsx_wsclose();

void ratinit();
void ratexit();
void gsx_init();
void gsx_exec(LONG pcspec, WORD segenv, LONG pcmdln, LONG pfcb1, LONG pfcb2);
void gsx_graphic(WORD tographic);
void bb_save(GRECT *ps);
void bb_restore(GRECT *pr);

WORD gsx_tick(LONG tcode, LONG *ptsave);
void gsx_mfset(LONG pmfnew);

void gsx_mxmy(WORD *pmx, WORD *pmy);
WORD gsx_kstate();
void gsx_mon();
void gsx_moff();
WORD gsx_char();

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

#endif
