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

#include "portab.h"
#include "bios.h"
#include "biosmem.h"
#include "kprint.h"
#include "tosvars.h"

extern MD b_mdx;            /* found in startup.S */

static int bmem_allowed;

void bmem_init(void)
{
    m_start = os_end;
    m_length = memtop - m_start;
    themd = (LONG) &b_mdx;
    bmem_allowed = 1;
}

void * balloc(LONG size)
{
    void * ret;

    if(!bmem_allowed) {
        panic("balloc(%ld) at wrong time\n", size);
    }
    if(m_length < size) {
        panic("balloc(%ld): no memory\n", size);
    }
    ret = (void*) m_start;
    m_length -= size;
    m_start += size;
    return ret;
}

void getmpb(MPB * mpb)
{
    bmem_allowed = 0; /* BIOS memory handling not allowed past this point */

    mpb->mp_mfl = mpb->mp_rover = &b_mdx; /* free list/rover set to init MD */
    mpb->mp_mal = (MD *)0;                /* allocated list set to NULL */
}

