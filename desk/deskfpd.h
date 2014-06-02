/*      DESKFPD.H       06/11/84 - 03/25/85     Lee Lorenzen    */
/*      for 3.0         11/4/87                 mdf             */
/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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

#define OP_COUNT 0
#define OP_DELETE 1
#define OP_COPY 2

#define D_PERM 0x0001

#define V_ICON 0
#define V_TEXT 1

#define S_NAME 0
#define S_DATE 1
#define S_SIZE 2
#define S_TYPE 3
#define S_DISK 4

#define E_NOERROR 0
#define E_NOFNODES 100
#define E_NOPNODES 101
#define E_NODNODES 102


#define FNODE struct filenode
FNODE
{
        FNODE           *f_next;
        BYTE            f_junk;         /* to align on even boundaries  */
        BYTE            f_attr;             /* NOTE: f_attr thru f_name[]  */
        UWORD           f_time;             /*  MUST be the same size & in */
        UWORD           f_date;             /*   the same sequence as the  */
        LONG            f_size;             /*    corresponding items in   */
        BYTE            f_name[LEN_ZFNAME]; /*     the DTA structure!      */
        WORD            f_obid;
        ANODE           *f_pa;
        WORD            f_isap;
};


#define PNODE struct pathnode
PNODE
{
        PNODE           *p_next;
        WORD            p_flags;
        WORD            p_attr;     /* attribs used in Fsfirst() */
        BYTE            p_spec[LEN_ZPATH];
        FNODE           *p_fbase;   /* start of malloc'd fnodes */
        FNODE           *p_flist;   /* linked list of fnodes */
        WORD            p_count;    /* number of items (fnodes) */
        LONG            p_size;     /* total size of items */
};


/* Prototypes: */
void fpd_start(void);
void fpd_parse(BYTE *pspec, WORD *pdrv, BYTE *ppath, BYTE *pname, BYTE *pext);
FNODE *fpd_ofind(FNODE *pf, WORD obj);
void pn_close(PNODE *thepath);
PNODE *pn_open(WORD  drive, BYTE *path, BYTE *name, BYTE *ext, WORD attr);
FNODE *pn_sort(PNODE *pn);
WORD pn_active(PNODE *thepath);

#endif  /* _DESKFPD_H */
