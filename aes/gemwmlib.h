/*
 * gemwmlib.h - header for EmuTOS AES Window Library functions
 *
 * Copyright (C) 2002-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMWMLIB_H
#define GEMWMLIB_H

#define DESKWH  0       /* window handle for desktop */

/* gadgets classified by type (location) */
#define TGADGETS    (NAME | CLOSER | FULLER | MOVER)    /* for 'top' window gadgets */
#define VGADGETS    (UPARROW | DNARROW | VSLIDE)        /* for vertical scroll bar */
#define HGADGETS    (LFARROW | RTARROW | HSLIDE)        /* for horizontal scroll bar */

extern WORD     gl_wtop;
extern OBJECT   *gl_awind;

void w_getsize(WORD which, WORD w_handle, GRECT *pt);
void w_drawdesk(GRECT *pc);
void w_setactive(void);
void w_bldactive(WORD w_handle);

void ap_sendmsg(WORD ap_msg[], WORD type, AESPD *towhom,
                WORD w3, WORD w4, WORD w5, WORD w6, WORD w7);
void w_update(WORD bottom, GRECT *pt, WORD top, BOOL moved);

void wm_start(void);
void wm_init(void);

WORD wm_create(WORD kind, GRECT *pt);
BOOL wm_open(WORD w_handle, GRECT *pt);
BOOL wm_close(WORD w_handle);
BOOL wm_delete(WORD w_handle);
BOOL wm_get(WORD w_handle, WORD w_field, WORD *poutwds, WORD *pinwds);
BOOL wm_set(WORD w_handle, WORD w_field, WORD *pinwds);

WORD wm_find(WORD x, WORD y);
void wm_update(WORD beg_update);
void wm_calc(WORD wtype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
             WORD *px, WORD *py, WORD *pw, WORD *ph);
void wm_new(void);

#if CONF_WITH_3D_OBJECTS
void w_redraw_desktop(GRECT *pt);
#endif

#endif
