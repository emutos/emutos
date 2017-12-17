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
/* Note: Alignment on 4 bytes may be better,
 * but it is currently unsupported by GCC 4.6.4 (MiNT 20170417) */
UBYTE dskbuf[DSKBUF_SIZE] __attribute__ ((aligned (2)));

/* Workaround for bug in GFA Basic 3.51 startup.
 * Theoretically, we are allowed to put the shifty variable anywhere,
 * then programs may access it through the pointer present in
 * OSHEADER. However, by mistake the GFA only reads the low word of
 * that pointer, and sign-extends it to get the whole address.
 * Consequently, this can only be correct if shifty is located in the
 * first 32K of memory.
 * TOS puts this variable in a region accessible in user mode, so we
 * do.
 */
UBYTE shifty; /* reflects the status up/down of mode keys */
