/*
 * deskconf.h - desktop configuration header
 *
 * This file provides a place to centralise the desktop configuration.
 *
 * Copyright (c) 2011 EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKCONF_H
#define _DESKCONF_H

#ifdef DESK1
#define NUM_WNODES  7               /* maximum number of desktop windows */
#define NUM_WOBS    128             /* maximum number of desktop objects */
#else
#define NUM_WNODES  2
#define NUM_WOBS    300
#endif

#define NUM_SOBS    (NUM_WOBS + NUM_WNODES + 1)

#define NUM_FNODES  400
#define NUM_PNODES  (NUM_WNODES+1)  /* one more than windows for unopen disk copy */

#define MAX_OBS     60              /* max number of objects that can be dragged */

#define MAX_LEVEL   8               /* max directory depth supported by the desktop */

#endif  /* _DESKCONF_H */
