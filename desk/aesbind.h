/*
 * EmuTOS desktop
 *
 * Copyright (C) 2002-2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESBIND_H
#define _AESBIND_H
#include "obdefs.h"

#define END_UPDATE 0        /* for wind_update() */
#define BEG_UPDATE 1
#define END_MCTRL  2
#define BEG_MCTRL  3

/*
 * Application Manager
 */
WORD appl_init(void);
WORD appl_exit(void);
/*
WORD appl_write(WORD rwid, WORD length, const void *pbuff);
WORD appl_read(WORD rwid, WORD length, void *pbuff);
WORD appl_find(const BYTE *pname);
WORD appl_tplay(const EVNTREC *tbuffer, WORD tlength, WORD tscale);
WORD appl_trecord(EVNTREC *tbuffer, WORD tlength);
*/


/*
 *  Event Manager
 */
WORD evnt_button(WORD clicks, UWORD mask, UWORD state,
                 WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks);
WORD evnt_timer(UWORD locnt, UWORD hicnt);
WORD evnt_multi(UWORD flags, UWORD bclk, UWORD bmsk, UWORD bst,
                UWORD m1flags, UWORD m1x, UWORD m1y, UWORD m1w, UWORD m1h,
                UWORD m2flags, UWORD m2x, UWORD m2y, UWORD m2w, UWORD m2h,
                WORD *mepbuff, UWORD tlc, UWORD thc, UWORD *pmx, UWORD *pmy,
                UWORD *pmb, UWORD *pks, UWORD *pkr, UWORD *pbr );
WORD evnt_dclick(WORD rate, WORD setit);
/*
UWORD evnt_keybd(void);
WORD evnt_mouse(WORD flags, WORD x, WORD y, WORD width, WORD height,
                WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks);
WORD evnt_mesag(WORD *pbuff);
*/


/*
 *  Menu Manager
 */
WORD menu_bar(OBJECT *tree, WORD showit);
WORD menu_icheck(OBJECT *tree, WORD itemnum, WORD checkit);
WORD menu_ienable(OBJECT *tree, WORD itemnum, WORD enableit);
WORD menu_tnormal(OBJECT *tree, WORD titlenum, WORD normalit);
/* WORD menu_text(OBJECT *tree, WORD inum, const BYTE *ptext); */
/* WORD menu_register(WORD pid, const BYTE *pstr); */
/* WORD menu_unregister(WORD mid); */
WORD menu_click(WORD click, WORD setit);


/*
 *  Object Manager
 */
WORD objc_add(OBJECT *tree, WORD parent, WORD child);
/* WORD objc_delete(OBJECT *tree, WORD delob); */
WORD objc_draw(OBJECT *tree, WORD drawob, WORD depth, WORD xc, WORD yc,
               WORD wc, WORD hc);
WORD objc_find(OBJECT *tree, WORD startob, WORD depth, WORD mx, WORD my);
WORD objc_order(OBJECT *tree, WORD mov_obj, WORD newpos);
WORD objc_offset(OBJECT *tree, WORD obj, WORD *poffx, WORD *poffy);
/* WORD objc_edit(OBJECT *tree, WORD obj, WORD inchar, WORD *idx, WORD kind); */
WORD objc_change(OBJECT *tree, WORD drawob, WORD depth, WORD xc, WORD yc,
                 WORD wc, WORD hc, WORD newstate, WORD redraw);


/*
 *  Form Manager
 */
WORD form_do(OBJECT *form, WORD start);
WORD form_dial(WORD dtype, WORD ix, WORD iy, WORD iw, WORD ih,
               WORD x, WORD y, WORD w, WORD h);
WORD form_alert(WORD defbut, const BYTE *astring);
WORD form_error(WORD errnum);
WORD form_center(OBJECT *tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch);
/*
WORD form_keybd(OBJECT *form, WORD obj, WORD nxt_obj, WORD thechar,
                WORD *pnxt_obj, WORD *pchar);
WORD form_button(OBJECT *form, WORD obj, WORD clks, WORD *pnxt_obj);
*/


/*
 *  Graphics Manager
 */
WORD graf_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
                 WORD *pwend, WORD *phend);
/*
WORD graf_dragbox(WORD w, WORD h, WORD sx, WORD sy, WORD xc, WORD yc,
                  WORD wc, WORD hc, WORD *pdx, WORD *pdy);
WORD graf_mbox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty);
*/
WORD graf_growbox(WORD orgx, WORD orgy, WORD orgw, WORD orgh,
                  WORD x, WORD y, WORD w, WORD h);
WORD graf_shrinkbox(WORD orgx, WORD orgy, WORD orgw, WORD orgh,
                    WORD x, WORD y, WORD w, WORD h);
/*
WORD graf_watchbox(OBJECT *tree, WORD obj, UWORD instate, UWORD outstate);
WORD graf_slidebox(OBJECT *tree, WORD parent, WORD obj, WORD isvert);
*/
WORD graf_handle(WORD *pwchar, WORD *phchar, WORD *pwbox, WORD *phbox);
WORD graf_mouse(WORD m_number, void *m_addr);
void graf_mkstate(WORD *pmx, WORD *pmy, WORD *pmstate, WORD *pkstate);


/*
 *  Scrap Manager
 */
/*
WORD scrp_read(BYTE *pscrap);
WORD scrp_write(const BYTE *pscrap);
*/


/*
 * Form Selector Manager
 */
/*
WORD fsel_input(BYTE *pipath, BYTE *pisel, WORD *pbutton);
*/


/*
 *  Window Manager
 */
WORD wind_create(UWORD kind, WORD wx, WORD wy, WORD ww, WORD wh);
WORD wind_open(WORD handle, WORD wx, WORD wy, WORD ww, WORD wh);
WORD wind_close(WORD handle);
WORD wind_delete(WORD handle);
WORD wind_get(WORD w_handle, WORD w_field, WORD *pw1, WORD *pw2, WORD *pw3, WORD *pw4);
WORD wind_get_grect(WORD w_handle, WORD w_field, GRECT *gr);
WORD wind_set(WORD w_handle, WORD w_field, ...);
WORD wind_set_grect(WORD w_handle, WORD w_field, const GRECT *gr);
WORD wind_find(WORD mx, WORD my);
WORD wind_update(WORD beg_update);
WORD wind_calc(WORD wctype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
               WORD *px, WORD *py, WORD *pw, WORD *ph);


/*
 *  Resource Manager
 */
WORD rsrc_load(const BYTE *rsname);
WORD rsrc_free(void);
/*
WORD rsrc_gaddr(WORD rstype, WORD rsid, void **paddr);
WORD rsrc_saddr(WORD rstype, WORD rsid, void *lngval);
*/
WORD rsrc_obfix(OBJECT *tree, WORD obj);
WORD rsrc_gaddr_rom(WORD rstype, WORD rsid, void **paddr);  /* see deskmain.c */


/*
 *  Shell Manager
 */
/* WORD shel_read(BYTE *pcmd, BYTE *ptail); */
WORD shel_write(WORD doex, WORD isgr, WORD iscr, BYTE *pcmd, BYTE *ptail);
WORD shel_get(void *pbuffer, WORD len);
WORD shel_put(void *pdata, WORD len);
WORD shel_find(BYTE *ppath);
/*
WORD shel_envrn(BYTE *ppath, const BYTE *psrch);
WORD shel_rdef(BYTE *lpcmd, BYTE *lpdir);
WORD shel_wdef(BYTE *lpcmd, BYTE *lpdir);
*/

#endif  /* _AESBIND_H */
