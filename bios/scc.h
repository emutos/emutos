/*
 * scc.h - header for SCC chip
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _SCC_H
#define _SCC_H

#if CONF_WITH_SCC
/*
 * structures
 */
typedef struct {
    UBYTE dum1;
    volatile UBYTE ctl;
    UBYTE dum2;
    volatile UBYTE data;
} SCC_PORT;
typedef struct {
    SCC_PORT portA;
    SCC_PORT portB;
} SCC;

/*
 * commands that can be written to WR0
 */
#define SCC_RESET_ES_INT    0x10
#define SCC_RESET_TX_INT    0x28
#define SCC_ERROR_RESET     0x30
#define SCC_RESET_HIGH_IUS  0x38
#endif

#endif  /* _SCC_H */
