/*      DESKWIN.H       06/11/84 - 01/04/85             Lee Lorenzen    */
/*      changed NUM_WOBS from 128 to 300        11/19/87        mdf     */
/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/
#ifndef _DESKWIN_H
#define _DESKWIN_H
#include "deskconf.h"

/*
 * items related to the text display of file folder information
 */
/*
 * macro to determine whether to use the 'wide format' when building
 * the text line to display file/folder information.  the wide format
 * actually requires about 50 bytes, so 400 pixels should be enough,
 * but for now we stick with standard Atari resolutions
 */
#define USE_WIDE_FORMAT()   (G.g_wdesk >= 640)

/*
 * total length of highlighted text for selected files in
 * text display mode
 */
#define LEN_FNODE   48


/*
 * the number of the object within G.g_screen[] that is the root
 * of the desktop itself (the desktop is a window that receives
 * special treatment within the code)
 */
#define DROOT 1


/*
 * flags in w_flags below
 */
#define WN_DESKTOP      0x01                /* the desktop pseudo-window */


/*
 * the structure used by the desktop to manage its windows
 */
typedef struct _windnode WNODE;
struct _windnode
{
        WNODE           *w_next;            /* -> next 'highest' window */
        WORD            w_flags;
        WORD            w_id;                   /* window handle id #   */
        WORD            w_obid;                 /* desktop object id    */
        WORD            w_root;                 /* pseudo root ob. in   */
                                                /*   gl_screen for this */
                                                /*   windows objects    */
        WORD            w_cvrow;                /* current virt. row    */
        WORD            w_pncol;                /* physical # of cols   */
        WORD            w_pnrow;                /* physical # of rows   */
        WORD            w_vnrow;                /* virtual # of rows    */
        PNODE           *w_path;
        BYTE            w_name[LEN_ZPATH+2];    /* allow for leading & trailing spaces */
/*
 * the following array must be large enough to hold the sprintf-formatted
 * output of the longest translated version of the STINFOST resource item.
 * as of december 2014, this is 51 bytes for the Greek-language version.
 */
        BYTE            w_info[60];
};



/* Prototypes: */
void win_view(WORD vtype, WORD isort);
void win_start(void);
void win_free(WNODE *thewin);
WNODE *win_alloc(WORD obid);
WNODE *win_find(WORD wh);
void win_top(WNODE *thewin);
WNODE *win_ontop(void);
void win_bldview(WNODE *pwin, WORD x, WORD y, WORD w, WORD h);
void win_slide(WORD wh, WORD sl_value);
void win_arrow(WORD wh, WORD arrow_type);
void win_srtall(void);
void win_bdall(void);
void win_shwall(void);
WORD win_isel(OBJECT olist[], WORD root, WORD curr);
void win_sname(WNODE *pw);
void win_sinfo(WNODE *pwin);

#endif  /* _DESKWIN_H */
