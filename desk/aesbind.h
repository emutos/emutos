
#ifndef H_AESBIND_
#define H_AESBIND_

/* AES prototypes: */

WORD appl_init();
WORD appl_exit();
/*
WORD appl_write(WORD rwid, WORD length, LONG pbuff);
WORD appl_read(WORD rwid, WORD length, LONG pbuff);
*/
WORD appl_find(LONG pname);
/*
WORD appl_tplay(LONG tbuffer, WORD tlength, WORD tscale);
WORD appl_trecord(LONG tbuffer, WORD tlength);
*/

WORD evnt_button(WORD clicks, UWORD mask, UWORD state,
                 WORD *pmx, WORD *pmy, WORD *pmb, WORD *pks);
WORD evnt_timer(UWORD locnt, UWORD hicnt);
WORD evnt_multi(UWORD flags, UWORD bclk, UWORD bmsk, UWORD bst,
                UWORD m1flags, UWORD m1x, UWORD m1y, UWORD m1w, UWORD m1h, 
                UWORD m2flags, UWORD m2x, UWORD m2y, UWORD m2w, UWORD m2h,
                LONG mepbuff, UWORD tlc, UWORD thc, UWORD *pmx, UWORD *pmy,
                UWORD *pmb, UWORD *pks, UWORD *pkr, UWORD *pbr );
WORD evnt_dclick(WORD rate, WORD setit);

WORD menu_bar(LONG tree, WORD showit);
WORD menu_icheck(LONG tree, WORD itemnum, WORD checkit);
WORD menu_ienable(LONG tree, WORD itemnum, WORD enableit);
WORD menu_tnormal(LONG tree, WORD titlenum, WORD normalit);
WORD menu_text(LONG tree, WORD inum, LONG ptext);

WORD objc_add(LONG tree, WORD parent, WORD child);
/*WORD objc_delete(LONG tree, WORD delob);*/
WORD objc_draw(LONG tree, WORD drawob, WORD depth, WORD xc, WORD yc,
               WORD wc, WORD hc);
WORD objc_find(LONG tree, WORD startob, WORD depth, WORD mx, WORD my);
WORD objc_order(LONG tree, WORD mov_obj, WORD newpos);
WORD objc_offset(LONG tree, WORD obj, WORD *poffx, WORD *poffy);
WORD objc_change(LONG tree, WORD drawob, WORD depth, WORD xc, WORD yc,
                 WORD wc, WORD hc, WORD newstate, WORD redraw);

WORD form_do(LONG form, WORD start);
WORD form_dial(WORD dtype, WORD ix, WORD iy, WORD iw, WORD ih,
               WORD x, WORD y, WORD w, WORD h);
WORD form_alert(WORD defbut, LONG astring);
WORD form_error(WORD errnum);
WORD form_center(LONG tree, WORD *pcx, WORD *pcy, WORD *pcw, WORD *pch);


WORD graf_rubbox(WORD xorigin, WORD yorigin, WORD wmin, WORD hmin,
                 WORD *pwend, WORD *phend);
WORD graf_dragbox(WORD w, WORD h, WORD sx, WORD sy, WORD xc, WORD yc,
                  WORD wc, WORD hc, WORD *pdx, WORD *pdy);
WORD graf_mbox(WORD w, WORD h, WORD srcx, WORD srcy, WORD dstx, WORD dsty);
WORD graf_watchbox(LONG tree, WORD obj, UWORD instate, UWORD outstate);
WORD graf_slidebox(LONG tree, WORD parent, WORD obj, WORD isvert);
WORD graf_handle(WORD *pwchar, WORD *phchar, WORD *pwbox, WORD *phbox);
WORD graf_mouse(WORD m_number, WORD m_addr);
void graf_mkstate(WORD *pmx, WORD *pmy, WORD *pmstate, WORD *pkstate);

/*
WORD fsel_input(LONG pipath, LONG pisel, WORD *pbutton);
*/

WORD wind_create(UWORD kind, WORD wx, WORD wy, WORD ww, WORD wh);
WORD wind_open(WORD handle, WORD wx, WORD wy, WORD ww, WORD wh);
WORD wind_close(WORD handle);
WORD wind_delete(WORD handle);
WORD wind_get(WORD w_handle, WORD w_field, WORD *pw1, WORD *pw2, WORD *pw3, WORD *pw4);
WORD wind_set(WORD w_handle, WORD w_field, ...);
WORD wind_find(WORD mx, WORD my);
WORD wind_update(WORD beg_update);
WORD wind_calc(WORD wctype, UWORD kind, WORD x, WORD y, WORD w, WORD h,
               WORD *px, WORD *py, WORD *pw, WORD *ph);


WORD rsrc_load(LONG rsname);
WORD rsrc_free();
WORD rsrc_gaddr(WORD rstype, WORD rsid, LONG *paddr);
WORD rsrc_obfix(LONG tree, WORD obj);

WORD shel_read(LONG pcmd, LONG ptail);
WORD shel_write(WORD doex, WORD isgr, WORD iscr, LONG pcmd, LONG ptail);
WORD shel_get(LONG pbuffer, WORD len);
WORD shel_put(LONG pdata, WORD len);
WORD shel_find(LONG ppath);
WORD shel_envrn(LONG ppath, LONG psrch);

#endif
