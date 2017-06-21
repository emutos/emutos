/*
 * blitter.h - header for blitter routines
 *
 * Copyright (C) 2017 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef _BLITTER_H
#define _BLITTER_H

#if CONF_WITH_BLITTER
BOOL blitter_hline(void);
#endif

#endif
