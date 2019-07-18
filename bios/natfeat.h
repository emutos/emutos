/*
 * natfeat.h - NatFeat header file
 *
 * Copyright (C) 2001-2016 The EmuTOS development team
 *
 * Authors:
 *  PES   Petr Stehlik
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
extern BOOL has_natfeats(void);

extern long nfGetFullName(char *buffer, long size);

extern BOOL is_nfStdErr(void);
extern long nfStdErr(const char *text);

extern long get_xhdi_nfid(void);

/* terminate the execution of the emulator if possible, else no-op */
extern void nf_shutdown(void);
extern BOOL has_nf_shutdown(void);

/* load a new OS kernel into the memory to 'addr' ('size' bytes available) */
extern long nf_bootstrap(UBYTE *addr, long size);
/* return the OS kernel arguments to be passed to 'addr' ('size' bytes available) */
extern long nf_getbootstrap_args(char *addr, long size);
extern UWORD nf_getbootdrive(void);

/* check if the host is emulating the MMU */
extern BOOL mmu_is_emulated(void);
#endif /* _NATFEAT_H */
