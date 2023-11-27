/*
 * aesvars.h - Private global AES variables
 *
 * Copyright (C) 2019-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _AESVARS_H
#define _AESVARS_H

#include "struct.h"

/* Ready List Root - a list of AESPDs linked by the p_link field, terminated
 * by zero [see gempd.c function insert_process] */
extern AESPD    *rlr;

extern AESPD    *drl, *nrl;
extern EVB      *eul, *dlr, *zlr;

/* Convert an EVB list root pointer to a fake EVB,
 * as if it was the e_link field of the struct.
 * Other fields are invalid and must not be used.
 * Such fake EVB are used as e_pred for the first item of the lists.
 * Of course this is extremely bad practice. */
static __inline__ EVB *FAKE_EVB(EVB **root)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    UBYTE *fake_evb_start = ((UBYTE *)root) - offsetof(EVB, e_link);
#pragma GCC diagnostic pop
    return (EVB *)fake_evb_start;
}

/* In Dispatch - a byte whose value is zero when not in function
 * dsptch, and 1 when between dsptch ... switchto function calls */
extern UBYTE    indisp;

extern WORD     fpt, fph, fpcnt;                /* forkq tail, head, count */

extern SPB      wind_spb;
extern WORD     curpid;

#endif /* _AESVARS_H */
