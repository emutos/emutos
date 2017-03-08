/*
 * pd.h - the Process Descriptor
 *
 * This file exists to centralise the definition of the process descriptor,
 * which was previously defined in several different places.
 *
 * Copyright (C) 2011-2015 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef PD_H
#define PD_H

#include "portab.h"
#include "dta.h"

#define NUMSTD      6       /* number of standard files */

/*
 *  PD - Process Descriptor
 */

#define PDCLSIZE    0x80    /*  size of command line in bytes  */
#define NUMCURDIR   BLKDEVNUM   /* number of entries in curdir array */

typedef struct _pd PD;
struct _pd
{
/* 0x00 */
    LONG    p_lowtpa;       /* pointer to start of TPA */
    LONG    p_hitpa;        /* pointer to end of TPA+1 */
    LONG    p_tbase;        /* pointer to base of text segment */
    LONG    p_tlen;         /* length of text segment */
/* 0x10 */
    LONG    p_dbase;        /* pointer to base of data segment */
    LONG    p_dlen;         /* length of data segment */
    LONG    p_bbase;        /* pointer to base of bss segment */
    LONG    p_blen;         /* length of bss segment */
/* 0x20 */
    DTA     *p_xdta;
    PD      *p_parent;      /* parent PD */
    ULONG   p_flags;        /* see below */
    BYTE    *p_env;         /* pointer to environment string */
/* 0x30 */
    BYTE    p_uft[NUMSTD];  /* index into sys file table for std files */
    BYTE    p_lddrv;
    BYTE    p_curdrv;
    LONG    p_1fill[2];
/* 0x40 */
    BYTE    p_curdir[NUMCURDIR];    /* index into sys dir table */
    BYTE    p_2fill[32-NUMCURDIR];
/* 0x60 */
    LONG    p_3fill[2];
    LONG    p_dreg[1];      /* dreg[0] */
    LONG    p_areg[5];      /* areg[3..7] */
/* 0x80 */
    BYTE    p_cmdlin[PDCLSIZE];     /* command line image */
};

/* p_flags values: */
#define PF_FASTLOAD     0x0001
#define PF_TTRAMLOAD    0x0002
#define PF_TTRAMMEM     0x0004
#define PF_STANDARD     (PF_FASTLOAD | PF_TTRAMLOAD | PF_TTRAMMEM)

extern PD *run; /* Pointer to the basepage of the current process */

#endif /* PD_H */
