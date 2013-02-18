/*
 * asmstruct.h - assembler equivalents for structures in struct.h
 *
 * This file exists to provide #defines for the offsets of fields
 * within structures defined within struct.h.  These #defines must
 * be manually kept in sync with the C language definitions, but
 * this should be easier than doing the same with hard-coded offset
 * values.
 *
 * Copyright (c) 2012 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

/*
 * defines
 */

/* AES PD struct */
#define PD_UDA          0x08        // pointer to UDA
#define PD_LDADDR       0x18        // pointer to basepage

/* BDOS PD struct */
#define P_TBASE         0x08        // address of TEXT segment
#define P_BBASE         0x18        // address of BSS segment
#define P_BLEN          0x1C        // length of BSS
#define P_XDTA          0x20        // pointer to DTA buffer ???

/* UDA struct */
#define UDA_INSUPER                 // offset 0: the 'in supervisor' flag
#define UDA_REGS        0x02        // registers d0-a6
#define UDA_REG_A6      0x3A        // a6
#define UDA_SPSUPER     0x3E        // ssp
#define UDA_SPUSER      0x42        // usp
#define UDA_OLDSPSUPER  0x46        // ssp when AES trap is entered
