/*
 *  biosmem.h - dumb bios-level memory management
 *
 * Copyright (c) 2002 by
 *
 * Authors:
 *  LVL    Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef H_BIOSMEM_
#define H_BIOSMEM_

#include "portab.h"
#include "bios.h"



/*
 *  MD - Memory Descriptor
 */

#define MD struct _md

MD
{
        MD      *m_link;  /* next MD, or NULL */
        long    m_start;  /* start address of memory block */
        long    m_length; /* number of bytes in memory block*/
        PD      *m_own;   /* owner's process descriptor */
} ;

/*
 *  fields in Memory Descriptor
 */

#define MF_FREE 1


/*
 *  MPB - Memory Partition Block
 */

#define MPB struct _mpb

MPB
{
        MD      *mp_mfl;   /* memory free list */
        MD      *mp_mal;   /* memory allocated list */
        MD      *mp_rover; /* roving pointer */
} ;


/* Prototypes */
void bmem_init(void);
void *balloc(long size);

/* BIOS function */

void getmpb(MPB *mpb);

#endif /* H_BIOSMEM_ */
