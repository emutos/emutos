/*
 * gemgraf.h - header for miscellaneous EmuTOS AES graphics-related functions
 *
 * Copyright (C) 2002-2021 The EmuTOS development team
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
void gsx_blt(void *saddr, WORD sx, WORD sy,
             WORD dx, WORD dy, WORD w, WORD h,
             WORD rule, WORD fgcolor, WORD bgcolor);

void gr_inside(GRECT *pt, WORD th);
void gr_rect(UWORD icolor, UWORD ipattern, GRECT *pt);
WORD gr_just(WORD just, WORD font, char *ptext, WORD w, WORD h, GRECT *pt);
void gr_gtext(WORD just, WORD font, char *ptext, GRECT *pt);
void gr_crack(UWORD color, WORD *pbc, WORD *ptc, WORD *pip, WORD *pic, WORD *pmd);
void gr_gicon(WORD state, ICONBLK *ib, CICON *cicon);
void gr_box(WORD x, WORD y, WORD w, WORD h, WORD th);

#endif
