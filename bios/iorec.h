/*
 * iorec.h - Input Output RECords related things
 *
 * Copyright (C) 2001-2021 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IOREC_H
#define IOREC_H

/*==== Structs ============================================================*/

typedef struct iorec IOREC;

struct iorec {
    UBYTE *buf;         /* input buffer */
    WORD size;          /* buffer size */
    WORD head;          /* head index */
    volatile WORD tail; /* tail index */
    WORD low;           /* low water mark */
    WORD high;          /* high water mark */
};

extern IOREC ikbdiorec, midiiorec;

/*==== Functions ==========================================================*/

#endif /* IOREC_H */
