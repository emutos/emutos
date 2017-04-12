/*
 * rsdefs.h - RSC file definitions
 *
 * Copyright (C) 2016-2017 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef RSDEFS_H
#define RSDEFS_H

typedef struct rshdr
{
    UWORD   rsh_vrsn;       /* see below */
    WORD    rsh_object;
    WORD    rsh_tedinfo;
    WORD    rsh_iconblk;    /* list of ICONBLKS */
    WORD    rsh_bitblk;
    WORD    rsh_frstr;
    WORD    rsh_string;
    WORD    rsh_imdata;     /* image data */
    WORD    rsh_frimg;
    WORD    rsh_trindex;
    WORD    rsh_nobs;       /* counts of various structs */
    WORD    rsh_ntree;
    WORD    rsh_nted;
    WORD    rsh_nib;
    WORD    rsh_nbb;
    WORD    rsh_nstring;
    WORD    rsh_nimages;
    WORD    rsh_rssize;     /* total bytes in resource */
} RSHDR;

/* definitions for rsh_vrsn */
#define NEW_FORMAT_RSC  0x0004          /* this bit set indicates a new-format */
                                        /* resource file (not yet supported)   */

#endif /* RSDEFS_H */
