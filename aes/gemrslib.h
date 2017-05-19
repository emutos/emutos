/*
 * EmuTOS AES
 *
 * Copyright (C) 2002-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMRSLIB_H
#define GEMRSLIB_H

#include "rsdefs.h"                 /* for RSHDR */

typedef struct aesglobal {
    WORD ap_version;                /* AES version */
    WORD ap_count;                  /* max # of concurrent applications */
    WORD ap_id;                     /* application id */
    LONG ap_private;                /* for use by application */
    OBJECT **ap_ptree;              /* pointer to array of tree addresses */
                                /* the following are not advertised to applications */
    RSHDR *ap_rscmem;               /* address of rsc file in memory */
    UWORD ap_rsclen;                /* length of rsc file */
    WORD ap_planes;                 /* # of colour planes on screen */
    LONG ap_3resv;                  /* ptr to AES global area D (struct THEGLO) */
    LONG ap_4resv;                  /* reserved for use in AES 4.00 */
} AESGLOBAL;

void rs_obfix(OBJECT *tree, WORD curob);
BYTE *rs_str(UWORD stnum);
WORD rs_free(AESGLOBAL *pglobal);
WORD rs_gaddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, void **rsaddr);
WORD rs_saddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, void *rsaddr);
void rs_fixit(AESGLOBAL *pglobal);
WORD rs_load(AESGLOBAL *pglobal, BYTE *rsfname);

#endif
