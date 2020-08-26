/*
 * sysconf.h - system configuration header
 *
 * This file is intended to contain 'standard' system configuration
 * definitions, i.e. ones that cannot be overridden by localconf.h.
 * It is equivalent to EmuDesk's deskconf.h.
 *
 * Copyright (C) 2019-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _SYSCONF_H
#define _SYSCONF_H

/*
 * System configuration definitions
 */
#define NUM_WIN     8                   /* maximum number of windows (the     */
                                        /* desktop itself counts as 1 window) */

#define NUM_ACCS    6                   /* maximum number of desk accessory   */
                                        /* files (.ACC) that will be loaded   */
                                        /* AND the maximum number of desk     */
                                        /* accessory slots available (one     */
                                        /* slot per mn_register() call)       */

#define BLKDEVNUM   26                  /* number of block devices supported: A: ... Z: */
#define INF_FILE_NAME "A:\\EMUDESK.INF" /* path to saved desktop file */
#define INF_FILE_WILD "A:\\*.INF"       /* wildcard for desktop file */
#define ICON_RSC_NAME "A:\\EMUICON.RSC" /* path to user icon file */

/*
 * Maximum lengths for pathname, filename, and filename components
 */
#define LEN_ZPATH   114                 /* max path length, incl drive */
#define LEN_ZFNAME  13                  /* max fname length, incl '\' separator */
#define LEN_ZNODE   8                   /* max node length */
#define LEN_ZEXT    3                   /* max extension length */
#define MAXPATHLEN  (LEN_ZPATH+LEN_ZFNAME+1) /* convenient shorthand */

/*
 * Maximum coordinate supported (must fit in WORD)
 */
#define MAX_COORDINATE  (10000)         /* arbitrary, could be 32767 */

/*
 * Default keyboard auto-repeat settings: values are units of 20 msec
 */
#define KB_INITIAL  15                  /* initial delay i.e. 300 msec */
#define KB_REPEAT   2                   /* ticks between repeats, i.e. 40 msec */

/*
 * AES configuration
 */
#define SIZE_SHELBUF    4192L           /* size of shell buffer - same as TOS 1.04-> */

#endif  /* _SYSCONF_H */
