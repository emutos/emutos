/*
 * iorec.h - Input Output RECords related things
 *
 * Copyright (c) 2001 Laurent Vogel
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _IOREC_H
#define _IOREC_H

#include        "portab.h"

/*==== Structs ============================================================*/

#define IOREC struct iorec

IOREC {
  void *buf;            /* input buffer */
  WORD size;            /* buffer size */
  WORD head;            /* head index */
  WORD tail;            /* tail index */
  WORD low;             /* low water mark */
  WORD high;            /* high water mark */
};

extern IOREC rs232iorec, ikbdiorec, midiiorec;

/*==== Functions ==========================================================*/

#endif /* _IOREC_H */
