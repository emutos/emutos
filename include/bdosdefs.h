/*
 * bdosdefs.h - Public definitions for BDOS system calls
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _BDOSDEFS_H
#define _BDOSDEFS_H

/* Values for Mxalloc() mode */
#define MX_STRAM 0
#define MX_TTRAM 1
#define MX_PREFSTRAM 2
#define MX_PREFTTRAM 3
#define MX_MODEMASK  0x03   /* mask for supported mode bits */

/* Values of 'mode' for Pexec() */
#define PE_LOADGO     0
#define PE_LOAD       3
#define PE_GO         4
#define PE_BASEPAGE   5
#define PE_GOTHENFREE 6
#define PE_BASEPAGEFLAGS 7
#if DETECT_NATIVE_FEATURES
#define PE_RELOCATE   50    /* required for NatFeats support only, not in Atari TOS */
#endif

#endif /* _BDOSDEFS_H */
