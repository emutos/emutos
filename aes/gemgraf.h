/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMGRAF_H
#define GEMGRAF_H

#include "gsxdefs.h"

WORD gsx_chkclip(GRECT *pt);
void gsx_cline(UWORD x1, UWORD y1, UWORD x2, UWORD y2);
void gsx_xbox(GRECT *pt);
void gsx_xcbox(GRECT *pt);
void gsx_blt(LONG saddr, UWORD sx, UWORD sy, UWORD swb,
             LONG daddr, UWORD dx, UWORD dy, UWORD dwb, UWORD w, UWORD h,
             UWORD rule, WORD fgcolor, WORD bgcolor);

void gr_inside(GRECT *pt, WORD th);
void gr_rect(UWORD icolor, UWORD ipattern, GRECT *pt);
WORD gr_just(WORD just, WORD font, LONG ptext, WORD w, WORD h, GRECT *pt);
void gr_gtext(WORD just, WORD font, LONG ptext, GRECT *pt);
void gr_crack(UWORD color, WORD *pbc, WORD *ptc, WORD *pip, WORD *pic, WORD *pmd);
void gr_gicon(WORD state, WORD *pmask, WORD *pdata, BYTE *ptext, WORD ch,
              WORD chx, WORD chy, GRECT *pi, GRECT *pt);
void gr_box(WORD x, WORD y, WORD w, WORD h, WORD th);

#endif
