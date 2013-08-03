/*
 * EmuTOS aes
 *
 * Copyright (c) 2002-2013 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMRSLIB_H
#define GEMRSLIB_H

typedef struct aesglobal {
    WORD ap_version;
    WORD ap_count;
    WORD ap_id;
    LONG ap_private;
    LONG ap_ptree;
    LONG ap_1resv;
    UWORD ap_2resv[2];
    LONG ap_3resv;
    LONG ap_4resv;
} AESGLOBAL;

void rs_obfix(LONG tree, WORD curob);
BYTE *rs_str(UWORD stnum);
WORD rs_free(AESGLOBAL *pglobal);
WORD rs_gaddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG *rsaddr);
WORD rs_saddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG rsaddr);
void rs_fixit(AESGLOBAL *pglobal);
WORD rs_load(AESGLOBAL *pglobal, LONG rsfname);

#endif
