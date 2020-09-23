/*
 * aesext.h - EmuTOS AES extensions not callable with trap
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESEXT_H
#define _AESEXT_H

#include "obdefs.h"


/* used by the VDI and by the desktop when calling the VDI */
typedef struct
{
    WORD x;
    WORD y;
} Point;

#if CONF_WITH_BACKGROUNDS
/* function used by AES and desktop, found in geminit.c */
void set_aes_background(UBYTE patcol);
#endif

/* functions used by AES and desktop, found in gemgraf.c */
void bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh);
void bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, WORD scdy, WORD scw, WORD sch);
void gsx_attr(UWORD text, UWORD mode, UWORD color);
void gsx_gclip(GRECT *pt);
void gsx_pline(WORD offx, WORD offy, WORD cnt, const Point *pts);
void gsx_sclip(const GRECT *pt);
void gsx_start(void);
void gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc);
void gsx_trans(void *saddr, UWORD swb, void *daddr, UWORD dwb, UWORD h);

/* functions used by AES and desktop, found in gemrslib.c */
void xlate_obj_array(OBJECT *obj_array, int nobj);

/* flag to display alerts in Critical Error Handler */
extern WORD enable_ceh; /* in gemdosif.S */

#endif /* _AESEXT_H */
