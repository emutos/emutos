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

#define DBG_MEM 0


extern MD themd;            /* BIOS memory descriptor (from tosvars.S) */

static int bmem_allowed;

extern int is_ramtos;           /* 1 if the TOS is running in RAM */



/*
 * bmem_init - initialize some memory related variables
 */
void bmem_init(void)
{
    LONG a;

    /* detect by looking at os_entry, if TOS in RAM */
    a = ((LONG) os_entry) & 0xffffff;
    if( a == 0xe00000L || a == 0xfc0000L ) {
        is_ramtos = 0;
    } else {
        is_ramtos = 1;
    }

#if DBG_MEM
    kprintf("_etext = 0x%08lx\n", (LONG)_etext);
    kprintf("_edata = 0x%08lx\n", (LONG)_edata);
    kprintf("end    = 0x%08lx\n", (LONG)end);
#endif
    if(is_ramtos) {
        /* patch TOS header */
        os_end = (LONG) _edata;
    }


    /* initialise some memory variables */
    end_os = os_end;
    membot = end_os;
    memtop = (long) v_bas_ad;

    /* Fill out the first memory descriptor */
    themd.m_link = (MD*) 0;     /* no next memory descriptor */
    themd.m_start = os_end;
    themd.m_length = memtop - themd.m_start;
    themd.m_own = (PD*) 0;      /* no owner's process descriptor */

    bmem_allowed = 1;
}



/*
 * balloc - simple BIOS memory allocation
 */
void * balloc(LONG size)
{
    void * ret;

    if(!bmem_allowed) {
        panic("balloc(%ld) at wrong time\n", size);
    }
    if(themd.m_length < size) {
        panic("balloc(%ld): no memory\n", size);
    }
    ret = (void*) themd.m_start;

    /* subtract needed memory from initial MD */
    themd.m_length -= size;
    themd.m_start += size;

    return ret;
}

void getmpb(MPB * mpb)
{
    bmem_allowed = 0; /* BIOS memory handling not allowed past this point */

    mpb->mp_mfl = mpb->mp_rover = &themd;   /* free list/rover set to init MD */
    mpb->mp_mal = (MD *)0;                /* allocated list set to NULL */
}

