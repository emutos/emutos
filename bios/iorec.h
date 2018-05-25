/*
 * iorec.h - Input Output RECords related things
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef IOREC_H
#define IOREC_H

#include        "portab.h"

/*==== Structs ============================================================*/

typedef struct iorec IOREC;

struct iorec {
  UBYTE *buf;           /* input buffer */
  WORD size;            /* buffer size */
  WORD head;            /* head index */
  volatile WORD tail;   /* tail index */
  WORD low;             /* low water mark */
  WORD high;            /* high water mark */
};

/* The volatile keyword below is an artificial workaround
   for the GCC bug 45052 present in GCC 4.5.x.
   Without that, bconin2() contains an infinite loop
   when USE_STOP_INSN_TO_FREE_HOST_CPU=0.
   This bug will be fixed in GCC 4.5.2.
*/
extern volatile IOREC ikbdiorec, midiiorec;

/*==== Functions ==========================================================*/

#endif /* IOREC_H */
