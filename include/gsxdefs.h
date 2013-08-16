/*      GSXDEFS.H       05/06/84 - 12/08/84     Lee Lorenzen            */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2013 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#ifndef GSXDEFS_H
#define GSXDEFS_H

typedef struct wsstr
{
        WORD            ws_xres;
        WORD            ws_yres;
        WORD            ws_noscale;
        WORD            ws_wpixel;
        WORD            ws_hpixel;
        WORD            ws_ncheights;
        WORD            ws_nlntypes;
        WORD            ws_nlnwidths;
        WORD            ws_nmktypes;
        WORD            ws_nmksizes;
        WORD            ws_nfaces;
        WORD            ws_npatts;
        WORD            ws_nhatchs;
        WORD            ws_ncolors;
        WORD            ws_ngdps;
        WORD            ws_supgdps[10];
        WORD            ws_attgdps[10];
        WORD            ws_color;
        WORD            ws_rotate;
        WORD            ws_fill;
        WORD            ws_cell;
        WORD            ws_npals;
        WORD            ws_nloc;
        WORD            ws_nval;
        WORD            ws_nchoice;
        WORD            ws_nstring;
        WORD            ws_type;
        WORD            ws_pts0;
        WORD            ws_chminh;
        WORD            ws_pts2;
        WORD            ws_chmaxh;
        WORD            ws_lnminw;
        WORD            ws_pts5;
        WORD            ws_lnmaxw;
        WORD            ws_pts7;
        WORD            ws_pts8;
        WORD            ws_mkminw;
        WORD            ws_pts10;
        WORD            ws_mkmaxw;
} WS;


typedef struct fdbstr
{
        LONG            fd_addr;
        WORD            fd_w;
        WORD            fd_h;
        WORD            fd_wdwidth;
        WORD            fd_stand;
        WORD            fd_nplanes;
        WORD            fd_r1;
        WORD            fd_r2;
        WORD            fd_r3;
} FDB;


typedef struct mform
{
        WORD    mf_xhot;
        WORD    mf_yhot;
        WORD    mf_nplanes;
        WORD    mf_fg;
        WORD    mf_bg;
        WORD    mf_mask[16];
        WORD    mf_data[16];
} MFORM;

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

extern LONG     ad_intin;

extern WORD     gl_mode;
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

void gsx_gclip(GRECT *pt);
void gsx_sclip(GRECT *pt);
void gsx_pline(WORD offx, WORD offy, WORD cnt, WORD *pts);
void gsx_attr(UWORD text, UWORD mode, UWORD color);
void gsx_fix(FDB *pfd, LONG theaddr, WORD wb, WORD h);
void bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, WORD scdy, WORD scw, WORD sch);
void gsx_trans(LONG saddr, UWORD swb, LONG daddr, UWORD dwb, UWORD h);
void gsx_start(void);
void gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc);
void bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh);

#endif
