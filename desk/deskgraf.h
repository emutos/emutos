
#ifndef DESKGRAF_H
#define DESKGRAF_H

#include "gsxdefs.h"

#ifndef GRECT
#include "obdefs.h"
#endif


extern WORD     gl_width;
extern WORD     gl_height;

extern WORD     gl_nrows;
extern WORD     gl_ncols;

extern WORD     gl_wchar;
extern WORD     gl_hchar;

extern WORD     gl_wschar;
extern WORD     gl_hschar;

extern WORD     gl_wptschar;
extern WORD     gl_hptschar;

extern WORD     gl_wbox;
extern WORD     gl_hbox;

extern WORD     gl_xclip;
extern WORD     gl_yclip;
extern WORD     gl_wclip;
extern WORD     gl_hclip;

extern WORD     gl_nplanes;
extern WORD     gl_handle;

extern FDB      gl_src;
extern FDB      gl_dst;

extern WS       gl_ws;
extern WORD     contrl[12];
extern WORD     intin[128];
extern WORD     ptsin[20];
extern WORD     intout[10];
extern WORD     ptsout[10];

extern LONG     ad_intin;

extern WORD     gl_mode;
extern WORD     gl_mask;
extern WORD     gl_tcolor;
extern WORD     gl_lcolor;
extern WORD     gl_fis;
extern WORD     gl_patt;
extern WORD     gl_font;

extern GRECT    gl_rscreen;
extern GRECT    gl_rfull;
extern GRECT    gl_rzero;
extern GRECT    gl_rcenter;
extern GRECT    gl_rmenu;


WORD gsx_sclip(GRECT *pt);
void gsx_gclip(GRECT *pt);
void gsx_pline(WORD offx, WORD offy, WORD cnt, WORD *pts);
void gsx_attr(UWORD text, UWORD mode, UWORD color);
void gsx_fix(FDB *pfd, LONG theaddr, WORD wb, WORD h);
void bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, WORD scdy,
          WORD scw, WORD sch);
void gsx_trans(LONG saddr, UWORD swb, LONG daddr, UWORD dwb, UWORD h);
void gsx_start();
void gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc);
void bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh);



#endif
