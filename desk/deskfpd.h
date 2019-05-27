/*      DESKFPD.H       06/11/84 - 03/25/85     Lee Lorenzen    */
/*      for 3.0         11/4/87                 mdf             */
/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2019 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/
#ifndef _DESKFPD_H
#define _DESKFPD_H
#include "deskconf.h"
#include "desk_rsc.h"           /* for ICONITEM, NAMEITEM etc */

#define OP_COUNT 0
#define OP_DELETE 1
#define OP_COPY 2
#define OP_MOVE 3

#define D_PERM 0x0001

#define V_ICON (ICONITEM-ICONITEM)  /* view as icons */
#define V_TEXT (TEXTITEM-ICONITEM)  /* view as text */
#define START_VIEW  V_ICON      /* default */

/*
 * sort sequence for display
 *
 * Note: folders are always listed before files, except for 'no sort'
 */
#define S_NAME (NAMEITEM-NAMEITEM)  /* file name (ascending) */
#define S_TYPE (TYPEITEM-NAMEITEM)  /* file extension (ascending), then name */
#define S_SIZE (SIZEITEM-NAMEITEM)  /* size (largest first), then name */
#define S_DATE (DATEITEM-NAMEITEM)  /* date (newest first), then name */
#define S_NSRT (NSRTITEM-NAMEITEM)  /* no sort (directory sequence) */
#define START_SORT  S_NAME      /* default */

#define E_NOERROR 0
#define E_NOFNODES 100
#define E_NOPNODES 101
#define E_NODNODES 102


typedef struct _filenode FNODE;
struct _filenode
{
    FNODE *f_next;
    char  f_selected;       /* if TRUE, file/folder has been selected */
                            /* note: we arrange to align f_attr on an odd boundary */
    char  f_attr;               /* NOTE: f_attr thru f_name[]  */
    UWORD f_time;               /*  MUST be the same size & in */
    UWORD f_date;               /*   the same sequence as the  */
    LONG  f_size;               /*    corresponding items in   */
    char  f_name[LEN_ZFNAME];   /*     the DTA structure!      */
    WORD  f_seq;            /* sequence within directory */
    WORD  f_obid;           /* index into G.g_screen[] for this object */
    ANODE *f_pa;            /* ANODE to get icon# from */
    WORD  f_isap;           /* if TRUE, use a_aicon in ANODE, else use a_dicon */
};


typedef struct _pathnode PNODE;
struct _pathnode
{
    WORD  p_attr;           /* attribs used in Fsfirst() */
    char  p_spec[LEN_ZPATH];/* dir path containing the FNODEs below */
    FNODE *p_fbase;         /* start of malloc'd fnodes */
    FNODE *p_flist;         /* linked list of fnodes */
    WORD  p_count;          /* number of items (fnodes) */
    LONG  p_size;           /* total size of items */
};


typedef struct _windnode WNODE; /* see deskwin.h */


/* Prototypes: */
void pn_clear(WNODE *pw);
void pn_close(PNODE *thepath);
PNODE *pn_open(char *pathname, WNODE *pw);
FNODE *pn_sort(PNODE *pn);
WORD pn_active(PNODE *thepath, BOOL include_folders);

#endif  /* _DESKFPD_H */
