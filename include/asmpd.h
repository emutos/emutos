/*
 * asmpd.h - assembler equivalents for structures in pd.h
 *
 * This file exists to provide #defines for the offsets of fields
 * within structures defined within pd.h.  These #defines must be
 * manually kept in sync with the C language definitions, but this
 * should be easier than doing the same with hard-coded offset values.
 *
 * Copyright (C) 2016 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

/* BDOS PD struct */
#define P_TBASE         0x08        /* address of TEXT segment */
#define P_BBASE         0x18        /* address of BSS segment */
#define P_BLEN          0x1C        /* length of BSS */
#define P_XDTA          0x20        /* pointer to DTA area */
#define P_REGSAVE       0x68        /* registers d0,a3-a7 */
                                /* redefines of above area: */
#define P_D0SAVE        0x68        /* register d0 */
#define P_A6SAVE        0x78        /* register a6 */
#define P_A7SAVE        0x7c        /* register a7 */
