
#ifndef GEMGSXIF_H
#define GEMGSXIF_H

#include "gsxdefs.h"


extern WORD  intout[];
extern WORD  ptsout[];

extern FDB   gl_tmp;

extern LONG  old_mcode;
extern LONG  old_bcode;
extern WORD  gl_moff;extern LONG  gl_mlen;
extern WORD  gl_graphic;


void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB );
void g_v_pline(WORD  count, WORD *pxyarray );
void g_vsl_width(WORD width);
void gsx_mon();
void gsx_moff();

#endif
