/*
 * asmstruct.h - assembler equivalents for structures in struct.h
 *
 * This file exists to provide #defines for the offsets of fields
 * within structures defined within struct.h.  These #defines must
 * be manually kept in sync with the C language definitions, but
 * this should be easier than doing the same with hard-coded offset
 * values.
 *
 * Copyright (C) 2012-2020 The EmuTOS development team
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
#define PD_UDA          0x08        /* pointer to UDA */
#define PD_LDADDR       0x18        /* pointer to basepage */

/* UDA struct */
#define UDA_REGS                    /* offset 0: registers d0-a6 */
#define UDA_REG_A6      0x38        /* a6 */
#define UDA_SPSUPER     0x3C        /* ssp */
#define UDA_SPUSER      0x40        /* usp */
#define UDA_OLDSPSUPER  0x44        /* ssp when AES trap is entered */
