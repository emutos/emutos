/*      GSXDEFS.H       05/06/84 - 12/08/84     Lee Lorenzen            */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*       Copyright (C) 2002-2022 The EmuTOS development team
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
        void           *fd_addr;
        WORD            fd_w;
        WORD            fd_h;
        WORD            fd_wdwidth;
        WORD            fd_stand;
        WORD            fd_nplanes;
        WORD            fd_r1;
        WORD            fd_r2;
        WORD            fd_r3;
} FDB;


extern WORD     gl_width;       /* screen width */
extern WORD     gl_height;      /* screen height */

extern WORD     gl_wchar;       /* width of character cell (normal font) */
extern WORD     gl_hchar;       /* height of character cell (normal font) */

extern WORD     gl_wschar;      /* width of character cell (small font) */
extern WORD     gl_hschar;      /* height of character cell (small font) */

extern WORD     gl_wbox;        /* box width */
extern WORD     gl_hbox;        /* box height */

extern GRECT    gl_clip;        /* global clipping rectangle */

extern WORD     gl_nplanes;     /* number of bit planes */
extern WORD     gl_handle;      /* physical workstation handle */

extern FDB      gl_src;
extern FDB      gl_dst;

extern WS       gl_ws;
extern WORD     contrl[12];
extern WORD     intin[128];
extern WORD     ptsin[20];

extern GRECT    gl_rscreen;     /* the entire screen */
extern GRECT    gl_rfull;       /* the screen except the menu bar */
extern GRECT    gl_rzero;       /* 0,0,0,0 */
extern GRECT    gl_rcenter;     /* a box centered in the 'gl_rfull' area */
extern GRECT    gl_rmenu;       /* the menu bar */

#endif
