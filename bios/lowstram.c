/*
 *  lowstram.c - low-memory ST-RAM variables
 *
 * Copyright (C) 2017 The EmuTOS development team
 *
 * Authors:
 *  VRI    Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* Variables defined here will go as low as possible in ST-RAM */

#include "config.h"
#include "portab.h"
#include "biosmem.h"

/* Disk buffer pointed by dskbufp */
UBYTE dskbuf[DSKBUF_SIZE];
