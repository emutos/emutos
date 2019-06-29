/*
 * aesvars.h - Private global AES variables
 *
 * Copyright (C) 2019 The EmuTOS development team
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

/* In Dispatch - a byte whose value is zero when not in function
 * dsptch, and 1 when between dsptch ... switchto function calls */
extern UBYTE    indisp;

extern WORD     fpt, fph, fpcnt;                /* forkq tail, head, count */

extern SPB      wind_spb;
extern CDA      *cda;
extern WORD     curpid;

#endif /* _AESVARS_H */
