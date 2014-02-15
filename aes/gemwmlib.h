/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2014 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMWMLIB_H
#define GEMWMLIB_H

extern LONG     gl_newdesk;
extern WORD     gl_newroot;
extern LONG     desk_tree[];
extern WORD     desk_root[];
extern OBJECT   W_TREE[];
extern OBJECT   W_ACTIVE[];
extern const WORD gl_watype[];
extern const LONG gl_waspec[];
extern TEDINFO  gl_aname;
extern TEDINFO  gl_ainfo;
extern const TEDINFO gl_asamp;
extern WORD     wind_msg[8];
extern WORD     gl_wtop;
extern LONG     gl_awind;

void w_nilit(WORD num, OBJECT olist[]);
void w_getsize(WORD which, WORD w_handle, GRECT *pt);
void w_drawdesk(GRECT *pc);
void w_setactive(void);
void w_bldactive(WORD w_handle);

void ap_sendmsg(WORD ap_msg[], WORD type, AESPD *towhom,
                WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
void w_update(WORD bottom, GRECT *pt, WORD top, WORD moved, WORD usetrue);

void wm_start(void);

WORD wm_create(WORD kind, GRECT *pt);
void wm_open(WORD w_handle, GRECT *pt);
void wm_close(WORD w_handle);
void wm_delete(WORD w_handle);
void wm_get(WORD w_handle, WORD w_field, WORD *poutwds);
void wm_set(WORD w_handle, WORD w_field, WORD *pinwds);

WORD wm_find(WORD x, WORD y);
void wm_update(WORD beg_update);
void wm_calc(WORD wtype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
             WORD *px, WORD *py, WORD *pw, WORD *ph);
void wm_new(void);

#endif
