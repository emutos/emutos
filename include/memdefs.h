/*
 * memdefs.h - memory management structures
 *
 * This file exists to centralise the definition of the MD & MPB, which were
 * previously defined (identically, fortunately) in two different places.
 *
 * Copyright (C) 2013-2016 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef _MEMDEFS_H
#define _MEMDEFS_H

#include "portab.h"
#include "pd.h"

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
 *  fields in Memory Descriptor
 */
#define MF_FREE 1


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

#endif  /* _MEMDEFS_H */
