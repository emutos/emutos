/*
 * iumem.c - internal user memory management routines
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2013-2017 The EmuTOS development team
 *
 * Authors:
 *  xxx <xxx@xxx>
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


/* #define ENABLE_KDEBUG */


#include "config.h"
#include "portab.h"
#include "fs.h"
#include "mem.h"
#include "kprint.h"


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
MD *ffit(long amount, MPB *mp)
{
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
        for (maxval = 0L; q; p = q, q = p->m_link)
            if (q->m_length > maxval)
                maxval = q->m_length;

        KDEBUG(("BDOS ffit: maxval=%ld\n",maxval));
        return (MD *)maxval;
    }

    if (mp == &pmd)
        KDEBUG(("Malloc(%lu) from ST-RAM\n", amount));

    /*
     * round the size up to a multiple of 4 bytes to keep alignment;
     * alignment on long boundaries is faster in FastRAM
     */
    amount = (amount + 3) & ~3;

    /*
     * look for first free space that's large enough
     * (this could be changed to a best-fit quite easily)
     */
    for ( ; q; p = q, q = p->m_link)
    {
        if (q->m_length >= amount)
            break;
    }
    if (!q)
    {
        KDEBUG(("BDOS ffit: Not enough contiguous memory\n"));
        return NULL;
    }

    if (q->m_length == amount)
        p->m_link = q->m_link;  /* take the whole thing */
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

        /* init new MD for remaining memory on free chain */
        p1->m_length = q->m_length - amount;
        p1->m_start = q->m_start + amount;
        p1->m_link = q->m_link;
        p->m_link = p1;

        /* adjust old MD for allocated memory on allocated chain */
        q->m_length = amount;
    }

    /*
     * link allocated block into allocated list & mark owner of block
     */
    q->m_link = mp->mp_mal;
    mp->mp_mal = q;
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
    MD *f, *p, *q;

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
     * Add it to the free list.
     */
    for (p = mp->mp_mfl, q = NULL; p; q = p, p = p->m_link)
        if (f->m_start <= p->m_start)
            break;

    f->m_link = p;

    if (q)
        q->m_link = f;
    else
        mp->mp_mfl = f;

    /*
     * Update existing memory descriptor.
     */
    m->m_length = newlen;

    return 0;
}
