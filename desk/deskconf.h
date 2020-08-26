/*
 * deskconf.h - EmuDesk's configuration header
 *
 * This file provides a place to centralise the desktop configuration.
 *
 * Copyright (C) 2011-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKCONF_H
#define _DESKCONF_H

/*
 * The G.g_screen[] array is organized as follows:
 *          object                          usage
 *             0                parent of the desktop & desktop window objects
 *             1                the desktop itself, parent of the desktop icon objects
 *      2->WOBS_START-1         the desktop window objects
 *  WOBS_START-> (end)          the desktop item objects (desktop icons & items within desktop windows)
 *
 * The desktop item objects are initially on a free chain, with the number
 * of the first object in G.g_screenfree; they are chained together via
 * ob_next, and the last object has a value of NIL in ob_next.
 */
#define NUM_WNODES  7               /* maximum number of desktop windows */

#define WOBS_START  (NUM_WNODES+2)  /* first desktop item object within g_screen[] */
#define MIN_WOBS    128             /* minimum number of desktop item objects */

/*
 * the following specifies the maximum number of directory levels supported
 * for a copy/move/count/delete operation; if lower levels are found to exist,
 * the operation will be terminated with a form alert (STFO8DEE).  levels are
 * counted from the top level of the source tree.  note that this does NOT
 * restrict the number of directory levels that may exist in a file system.
 *
 * this number is arbitrary, but is the same as Atari TOS uses.
 */
#define MAX_LEVEL   12              /* max number of directory levels */

#endif  /* _DESKCONF_H */
