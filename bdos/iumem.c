/*
 * iumem.c - internal user memory management routines
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2013-2022 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


/* #define ENABLE_KDEBUG */


#include "emutos.h"
#include "fs.h"
#include "mem.h"
#include "bdosstub.h"


/*
 *  STATIUMEM - cond comp; set to true to count calls to these routines
 */
#define STATIUMEM   FALSE

#if STATIUMEM
long ccffit;
long ccfreeit;
#endif


/*
 *  ffit - find first fit for requested memory in ospool
 */
MD *ffit(ULONG amount, MPB *mp) 
{
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
    return find_first_large_enough_free_block(amount, mp, (UBYTE*)0L);
}


MD *find_first_large_enough_free_block(ULONG amount, MPB *mp, UBYTE *start_address)
{
    MD *middle_block; 
    LONG s;
#endif
    MD *p, *q, *p1;     /* free list is composed of MD's */
    LONG maxval;

#ifdef ENABLE_KDEBUG
    if (mp == &pmd)
        KDEBUG(("BDOS ffit: mp=&pmd\n"));
#if CONF_WITH_ALT_RAM
    else if (mp == &pmdalt)
        KDEBUG(("BDOS ffit: mp=&pmdalt\n"));
#endif /* CONF_WITH_ALT_RAM */
    else
        KDEBUG(("BDOS ffit: mp=%p\n",mp));
#endif
    KDEBUG(("BDOS ffit: requested=%ld\n",amount));

#if STATIUMEM
    ++ccffit;
#endif

    p = (MD *)mp;
    if ((q = mp->mp_mfl) == NULL)   /* get free list pointer */
    {
        KDEBUG(("BDOS ffit: null free list ptr\n"));
        return NULL;
    }

    /*
     * handle request for maximum free block
     */
    if (amount == -1L)
    {
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
        for (maxval = 0L; q && (start_address == 0 || (s = ((LONG)start_address - (LONG)q->m_start)) >= 0); p = q, q = p->m_link)
        {
            ULONG size = q->m_length;
            if (start_address)
                size -= s;
            if (size > maxval)
                maxval = size;
        }
#else
        for (maxval = 0L; q; p = q, q = p->m_link)
            if (q->m_length > maxval)
                maxval = q->m_length;
#endif
        KDEBUG(("BDOS ffit: maxval=%ld\n",maxval));      
        return (MD *)maxval;
    }

    if (mp == &pmd)
        KDEBUG(("Malloc(%lu) from ST-RAM\n", amount));

    /*
     * round the size up to a multiple of 2 or 4 bytes to keep alignment;
     * alignment on long boundaries is faster in FastRAM
     */
    if (mp == &pmd)
        amount = (amount + malloc_align_stram) & ~malloc_align_stram;
    else
        amount = (amount + MALLOC_ALIGN_ALTRAM) & ~MALLOC_ALIGN_ALTRAM;

    /*
     * look for first free space that's large enough
     * (this could be changed to a best-fit quite easily)
     */
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
    for ( ; q && (start_address == 0 || (s = ((LONG)start_address - (LONG)q->m_start)) >= 0); p = q, q = p->m_link)
    {
        LONG size = q->m_length;
        if (start_address)
            size -= s;
        if (size >= amount)
            break;
    }
#else
    for ( ; q; p = q, q = p->m_link)
    {
        if (q->m_length >= amount)
            break;
    }
#endif
    if (!q)
    {
        KDEBUG(("BDOS ffit: Not enough contiguous memory\n"));
        return NULL;
    }

    if (q->m_length == amount)
    {
        p->m_link = q->m_link;  /* take the whole thing */
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
        /* link allocated block into allocated list & mark owner of block */
        q->m_link = mp->mp_mal;
        mp->mp_mal = q;
#endif
    }
    else
    {
        /* break it up - 1st allocate a new MD to describe the remainder */

        /*********** TBD **********
         * Nicer Handling of This *
         *        Situation       *
         **************************/
        if ((p1=xmgetmd()) == NULL)
        {
            KDEBUG(("BDOS ffit: null MGET\n"));
            return NULL;
        }

#if CONF_WITH_NON_RELOCATABLE_SUPPORT

        /* s = q->m_start - start_address, can only be <= 0 otherwise it means we didn't find a suitable block (q) */
        if (start_address == 0 || s == 0)
        {
            /* best case scenario (also happens when start_address==0), we only need to trim the
             * block we found to the requested length */

#endif
            /* init new MD for remaining memory on free chain */
            p1->m_length = q->m_length - amount;
            p1->m_start = q->m_start + amount;
            p1->m_link = q->m_link; 
            p->m_link = p1;

            /* adjust old MD for allocated memory on allocated chain */
            q->m_length = amount;

            /*
            * link allocated block into allocated list & mark owner of block
            */
            q->m_link = mp->mp_mal;
            mp->mp_mal = q;
        }
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
        else
        {
            /* worst case scenario, we have to allocate something in the middle of our block, and
             * have fringe free blocks on both sides */
            if ((middle_block=xmgetmd()) == NULL)
            {
                KDEBUG(("BDOS ffit: null MGET\n"));
                xmfremd(p1);
                return NULL;
            }
            /* q is the left fringe, then we have middle_block, then remainder */
            q->m_length = s;
            middle_block->m_length = amount;
            p1->m_length = q->m_length - amount - s;

            middle_block->m_start = start_address; /* that's the whole point */
            p1->m_start = start_address + amount;
            
            /* insert middle_block to allocated blocks list */
            middle_block->m_link = mp->mp_mal;
            mp->mp_mal = middle_block;
            
            /* insert p1 in the list of free blocks, immediately after the free block
             * we found so free blocks are still sorted by ascending address */
            q->m_link = p1;
            p1->m_link = q->m_link;

            middle_block->m_own = run;
            return middle_block;
        }
    }
#endif

    /* if we take all block, shrink block or start_address = free block address */ 
    q->m_own = run;

    KDEBUG(("BDOS ffit: start=%p, length=%ld\n",q->m_start,q->m_length));
    return q;
}


/*
 *  freeit - Free up a memory descriptor
 */
void freeit(MD *m, MPB *mp)
{
    MD *p, *q, *f;

#ifdef ENABLE_KDEBUG
    if (mp == &pmd)
        KDEBUG(("BDOS freeit: mp=&pmd\n"));
#if CONF_WITH_ALT_RAM
    else if (mp == &pmdalt)
        KDEBUG(("BDOS freeit: mp=&pmdalt\n"));
#endif /* CONF_WITH_ALT_RAM */
    else
        KDEBUG(("BDOS freeit: mp=%p\n",mp));
#endif
    KDEBUG(("BDOS freeit: start=%p, length=%ld\n",m->m_start,m->m_length));

#if STATIUMEM
    ++ccfreeit;
#endif

    /*
     * first, find it in the allocated list
     */
    for (p = mp->mp_mal, q = NULL; p; q = p, p = p->m_link)
        if (m->m_start == p->m_start)
            break;

    if (!p)
    {
        KDEBUG(("BDOS freeit: invalid MD address %p\n",m));
        return;
    }

    /*
     * snip it out
     */
    if (q)
        q->m_link = p->m_link;
    else
        mp->mp_mal = p->m_link;

    /*
     * find where to add it to the free list
     * (the free list is maintained in ascending sequence)
     *
     * p -> MD to be added
     */
    for (f = mp->mp_mfl, q = NULL; f; q = f, f = f-> m_link)
        if (p->m_start <= f->m_start)
            break;

    /*
     * insert it
     */
    p->m_link = f;          /* f -> next higher block */
    if (q)
        q->m_link = p;
    else
        mp->mp_mfl = p;

    /*
     * finally, coalesce free blocks if possible
     */
    if (f)
        if (p->m_start + p->m_length == f->m_start)
        { /* join to higher neighbor */
            p->m_length += f->m_length;
            p->m_link = f->m_link;
            xmfremd(f);
        }

    if (q)
        if (q->m_start + q->m_length == p->m_start)
        { /* join to lower neighbor */
            q->m_length += p->m_length;
            q->m_link = p->m_link;
            xmfremd(p);
        }
}


/*
 *  shrinkit - Shrink a memory descriptor
 */
WORD shrinkit(MD *m, MPB *mp, LONG newlen)
{
    MD *f;

    /*
     * Create a memory descriptor for the freed portion of memory.
     */
    f = xmgetmd();
    if (!f)
    {
        KDEBUG(("BDOS shrinkit: not enough OS memory for new MD\n"));
        return -1;
    }

    f->m_start = m->m_start + newlen;
    f->m_length = m->m_length - newlen;

    /*
     * Add it to the allocated list.
     */
    f->m_link = mp->mp_mal;
    mp->mp_mal = f;

    /*
     * Update existing memory descriptor.
     */
    m->m_length = newlen;

    /*
     * Free new memory descriptor via freeit() which takes care of
     * coalescing free blocks (important!).
     */
    freeit(f, mp);

    return 0;
}
