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

/*
 *  PD - Process Descriptor (a.k.a. BASEPAGE)
 */

#define NUMSTD      6       /* number of standard files */
#define NUMCURDIR   BLKDEVNUM   /* number of entries in curdir array */
#define PDCLSIZE    0x80    /*  size of command line in bytes  */

typedef struct _pd PD;
struct _pd
{
/* 0x00 */
    UBYTE   *p_lowtpa;      /* pointer to start of TPA */
    UBYTE   *p_hitpa;       /* pointer to end of TPA+1 */
    UBYTE   *p_tbase;       /* pointer to base of text segment */
    LONG    p_tlen;         /* length of text segment */
/* 0x10 */
    UBYTE   *p_dbase;       /* pointer to base of data segment */
    LONG    p_dlen;         /* length of data segment */
    UBYTE   *p_bbase;       /* pointer to base of bss segment */
    LONG    p_blen;         /* length of bss segment */
/* 0x20 */
    DTA     *p_xdta;
    PD      *p_parent;      /* parent PD */
    ULONG   p_flags;        /* see below */
    char    *p_env;         /* pointer to environment string */
/* 0x30 */
    SBYTE   p_uft[NUMSTD];  /* index into sys file table for std files */
    char    p_lddrv;
    UBYTE   p_curdrv;
    LONG    p_1fill[2];
/* 0x40 */
    UBYTE   p_curdir[NUMCURDIR];    /* index into sys dir table */
    char    p_2fill[32-NUMCURDIR];
/* 0x60 */
    LONG    p_3fill[2];
    LONG    p_dreg[1];      /* dreg[0] */
    LONG    p_areg[5];      /* areg[3..7] */
/* 0x80 */
    char    p_cmdlin[PDCLSIZE];     /* command line image */
};

/* p_flags values: */
#define PF_FASTLOAD     0x0001
#define PF_TTRAMLOAD    0x0002
#define PF_TTRAMMEM     0x0004
#define PF_STANDARD     (PF_FASTLOAD | PF_TTRAMLOAD | PF_TTRAMMEM)

/*
 *  MD - Memory Descriptor
 */
typedef struct _md MD;
struct _md
{
        MD      *m_link;    /* next MD, or NULL */
        UBYTE   *m_start;   /* start address of memory block */
        LONG    m_length;   /* number of bytes in memory block*/
        PD      *m_own;     /* owner's process descriptor */
};

/*
 *  MPB - Memory Partition Block
 */
typedef struct _mpb MPB;
struct _mpb
{
        MD      *mp_mfl;    /* memory free list */
        MD      *mp_mal;    /* memory allocated list */
        MD      *mp_rover;  /* roving pointer - no longer used */
};

#define PATH_ENV "PATH="    /* PATH environment variable */

#endif /* _BDOSDEFS_H */
