/*
 * prz_program_loader - loader for Foenix non-relocatable program format.
 *
 * Copyright (C) 2022 The EmuTOS development team
 *
 * Author:
 *      Vincent Barrilliot
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


#include "program_loader.h"
#include "gemerror.h"
#include "config.h"
#include "portab.h"
#include "fs.h"
#include "kprint.h"
#include "mem.h"
#include "pghdr.h"
#include "proc.h"
#include "string.h"
#include "sysconf.h"

#if CONF_WITH_NON_RELOCATABLE_SUPPORT

#define MAX_SEGMENTS 8 /* max number of segments we will process. This is limited by stack size */
#define PGZ_MAGIC 'Z'0x601a
#define SIZEOF_MAGIC 1L


typedef struct {
    UBYTE address[3];
    UBYTE size[3];
} SEGMENT_HDR;


static WORD can_load(FH fh)
{
    WORD r;
    char magic_maybe;

    r = xread(fh, SIZEOF_MAGIC, &magic_maybe);
    if (r < 0L)
        return r;
    if (r != SIZEOF_MAGIC)
        return EPLFMT;

    return magic_maybe == 'z' || magic_maybe == 'Z';
}

static WORD get_program_info(FH fh, LOAD_STATE *lstate) {
    lstate->relocatable_size = 0L;
    lstate->flags = PF_STANDARD;
    return E_OK;
}

/* AABBCC to 00CCBBAA . TODO there must be a more efficient way to do that in asm */
static ULONG word24to32(const UBYTE *b) {
    ULONG r = 0;
    UBYTE *rb = (UBYTE*)r;
    rb[1] = b[2];
    rb[2] = b[1];
    rb[3] = b[0];
    return r;
} 

static LONG pgz_load_program_into_memory(FH fh, PD *pd, LOAD_STATE *lstate)
{
    SEGMENT_HDR seghdr;
    LONG size;
    UBYTE *addr;
    MD *dst;
    WORD r;
    int segcount; /* number of already allocated segments in memory */
    UBYTE *segadr[8];
    segcount = 0;

    for (segcount = 0; segcount < sizeof(segadr)/sizeof(UBYTE*) ; segcount++)
    {
        r = xread(fh, sizeof(SEGMENT_HDR), &seghdr);
        if (r < sizeof(SEGMENT_HDR))
            return EPLFMT;
        
        size = word24to32(seghdr.size);
        if (size == 0L)
            return segcount == 0 ? EPLFMT : E_OK;
        
        addr = (UBYTE*)word24to32(seghdr.address);
        
        /* allocate memory */
        dst = find_first_large_enough_free_block(size, &pmd, addr);
        if (dst == 0)
            break;
        set_owner(dst->m_start, lstate->owner);
        segadr[segcount++] = dst->m_start;
        
        /* read segment into memory */
        r = xread(fh, size , dst->m_start);
        if (r < 0L)
            return r;
        if (r != size)
            return EPLFMT;
    }

    /* if we get here it means we failed to allocate memory and we need to cleanup
     * what is already allocated */
    while (segcount) 
        xmfree(segadr[segcount--]);
    return ENSMEM;
}

PROGRAM_LOADER pgz_program_loader = {
    can_load,
    get_program_info,
    pgz_load_program_into_memory
};

#endif /* CONF_WITH_NON_RELOCATABLE_SUPPORT */