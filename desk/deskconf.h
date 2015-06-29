/*
 * deskconf.h - desktop configuration header
 *
 * This file provides a place to centralise the desktop configuration.
 *
 * Copyright (c) 2011-2015 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _DESKCONF_H
#define _DESKCONF_H

#define NUM_WNODES  7               /* maximum number of desktop windows */
#define NUM_WOBS    128             /* maximum number of desktop objects */

#define NUM_SOBS    (NUM_WOBS + NUM_WNODES + 1)

#define NUM_PNODES  (NUM_WNODES+1)  /* one more than windows for unopen disk copy */

#define MAX_OBS     60              /* max number of objects that can be dragged */

#define MAX_LEVEL   8               /* max directory depth supported by the desktop */

#define LEN_FNODE   49              /* total length of highlighted text in window */
                                    /*  for selected files in "show as text" mode */

#endif  /* _DESKCONF_H */
