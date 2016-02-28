/*
 * biosdefs.h - Public BIOS defines and structures
 *
 * Copyright (c) 2016 The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef BIOSDEFS_H
#define BIOSDEFS_H

#include "portab.h"

/*
 *  BPB - Bios Parameter Block
 */
struct _bpb /* bios parameter block */
{
    UWORD recsiz;       /* sector size in bytes */
    UWORD clsiz;        /* cluster size in sectors */
    UWORD clsizb;       /* cluster size in bytes */
    UWORD rdlen;        /* root directory length in records */
    UWORD fsiz;         /* FAT size in records */
    UWORD fatrec;       /* first FAT record (of last FAT) */
    UWORD datrec;       /* first data record */
    UWORD numcl;        /* number of data clusters available */
    UWORD b_flags;      /* flags (see below) */
};
typedef struct _bpb BPB;

/*
 *  flags for BPB
 */
#define B_16    1       /* device has 16-bit FATs */
#define B_FIX   2       /* device has fixed media */

#endif /* BIOSDEFS_H */
