/*
 * bdosdefs.h - Public definitions for BDOS system calls
 *
 * Copyright (C) 2014-2019 The EmuTOS development team
 *
 * Authors:
 *  VRI   Vincent Rivière
 *  RFB   Roger Burrows
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

/* File Attributes bits used by FAT and Fsfirst() / Fattrib() */
#define FA_RO           0x01
#define FA_HIDDEN       0x02
#define FA_SYSTEM       0x04
#define FA_VOL          0x08
#define FA_SUBDIR       0x10
#define FA_ARCHIVE      0x20

/* Values of 'wrt' for Fattrib() */
#define F_GETMOD 0x0
#define F_SETMOD 0x1

typedef struct
{
    char    d_reserved[21];     /* internal EmuTOS usage */
    char    d_attrib;           /* attributes */
    UWORD   d_time;             /* packed time */
    UWORD   d_date;             /* packed date */
    LONG    d_length;           /* size */
    char    d_fname[14];        /* name */
} DTA;

#endif /* _BDOSDEFS_H */
