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



#include	"portab.h"

/*==== Defines ============================================================*/

/*==== Structs ============================================================*/

#define IOREC struct iorec

IOREC {
  void *ibuf;            /* input buffer */
  WORD ibufsize;          /* buffer size */
  WORD ibifhd;            /* head index */
  WORD ibuftl;            /* tail index */
  WORD ibuflow;           /* low water mark */
  WORD ibufhi;            /* high water mark */
};

IOREC *rs232iorec, *ikbdiorec, *midiiorec;

/*==== Functions ==========================================================*/

