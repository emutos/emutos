/*
 *  lowstram.c - low-memory ST-RAM variables
 *
 * Copyright (C) 2017-2019 The EmuTOS development team
 *
 * Authors:
 *  VRI    Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* Variables defined here will go as low as possible in ST-RAM */

#include "emutos.h"
#include "biosmem.h"

/* Disk buffer pointed by dskbufp
 *
 * This is placed here for two reasons:
 *  1) to ensure that it is located in ST-RAM rather than BSS.  The BSS
 *     area can get put into FastRAM  with some configurations, which
 *     can cause problems.
 *  2) to put it out of the way of any old games that might clobber the
 *     general BSS area.  Atari TOS also locates it in low memory.
 *
 * Note: Alignment on 4 bytes may be better,
 * but it is currently unsupported by GCC 4.6.4 (MiNT 20170417)
 */
UBYTE dskbuf[DSKBUF_SIZE] __attribute__ ((aligned (2)));

/* Workaround for bug in GFA Basic 3.51 startup.
 * Theoretically, we are allowed to put the shifty variable anywhere,
 * then programs may access it through the pointer present in
 * OSHEADER. However, by mistake GFA Basic only reads the low word of
 * that pointer, and sign-extends it to get the whole address.
 * Consequently, this can only be correct if shifty is located in the
 * first 32K of memory.
 * TOS puts this variable in a region accessible in user mode, so we
 * do too.
 */
UBYTE dskbuf_alignment[3]; /* FIXME: Unsafe hack to align dskbuf on 4 bytes */
UBYTE shifty; /* reflects the status up/down of mode keys */
