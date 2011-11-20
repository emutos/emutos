/*      DESKFPD.H       06/11/84 - 03/25/85     Lee Lorenzen    */
/*      for 3.0         11/4/87                 mdf             */
/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2011 The EmuTOS development team
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

#define F_FAKE    0x40       /* these flags are *only* used in the   */
#define F_DESKTOP 0x80       /*  f_attr field of the FNODE structure */

#define E_NOERROR 0
#define E_NOFNODES 100
#define E_NOPNODES 101
#define E_NODNODES 102


#define FNODE struct filenode
FNODE
{
        FNODE           *f_next;
        BYTE            f_junk;         /* to align on even boundaries  */
        BYTE            f_attr;
/* BugFix       */
        UWORD           f_time;
        UWORD           f_date;
/* */
        LONG            f_size;
        BYTE            f_name[LEN_ZFNAME];
        WORD            f_obid;
        ANODE           *f_pa;
        WORD            f_isap;
};


#define PNODE struct pathnode
PNODE
{
        PNODE           *p_next;
        WORD            p_flags;
        WORD            p_attr;
        BYTE            p_spec[LEN_ZPATH];
        FNODE           *p_flist;
        WORD            p_count;
        LONG            p_size;
};


/* Prototypes: */
void fpd_start(void);
void fpd_parse(BYTE *pspec, WORD *pdrv, BYTE *ppath, BYTE *pname, BYTE *pext);
FNODE *fpd_ofind(FNODE *pf, WORD obj);
void pn_close(PNODE *thepath);
PNODE *pn_open(WORD  drive, BYTE *path, BYTE *name, BYTE *ext, WORD attr);
FNODE *pn_sort(WORD lstcnt, FNODE *pflist);
WORD pn_active(PNODE *thepath);

#endif  /* _DESKFPD_H */
