
/*
 * natfeat.h - NatFeat header file
 *
 * Copyright (c) 2001-2003 EmuTOS development team
 *
 * Authors:
 *  joy   Petr Stehlik
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _NATFEAT_H
#define _NATFEAT_H

typedef struct
{
        long magic;
        long (* nfID)(const char *);
        long (* nfCall)(long ID, ...);
} NatFeatCookie;

extern NatFeatCookie natfeat_cookie;

#define NFID    natfeat_cookie.nfID
#define NFCall  natfeat_cookie.nfCall

extern void natfeat_init(void);
extern int has_natfeats(void);

extern long nfGetFullName(char *buffer, long size);

extern int  is_nfStdErr(void);
extern long nfStdErr(const char *text);

extern long get_xhdi_nfid(void);
extern long xhdi_vec(void);

#endif /* _NATFEAT_H */
