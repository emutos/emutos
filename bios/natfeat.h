
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

typedef struct
{
	long magic;
	long (* nfID)(const char *);
	long (* nfCall)(long ID, ...);
} NatFeatCookie;

extern void detect_native_features(void);
extern NatFeatCookie natfeat_cookie;

#define NFID	natfeat_cookie.nfID
#define NFCall	natfeat_cookie.nfCall

extern long xhdi_vec(void);
