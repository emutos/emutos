/*
 * iumem.c - internal user memory management routines
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2013-2016 The EmuTOS development team
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

#define STATIUMEM       FALSE


#if     STATIUMEM

long    ccffit;
long    ccfreeit;

#endif



/**
 *  ffit - find first fit for requested memory in ospool
 */

MD *ffit(long amount, MPB *mp)
{
    MD *p,*q,*p1;              /* free list is composed of MD's */
    BOOL maxflg;
    long maxval;

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

#if     STATIUMEM
    ++ccffit ;
#endif

    if( (q = mp->mp_rover) == 0  )      /*  get rotating pointer        */
    {
        KDEBUG(("BDOS ffit: null rover\n"));
        return( 0 ) ;
    }

    maxval = 0;
    maxflg = (amount == -1 ? TRUE : FALSE) ;

    /* Round the size to a multiple of 4 bytes to keep alignment.
       Alignment on long boundaries matters in FastRAM. */
    if (!maxflg)
        amount = (amount + 3) & ~3;

    p = q->m_link;                      /*  start with next MD          */

    do /* search the list for an MD with enough space */
    {

        if( p == 0 )
        {
            /*  at end of list, wrap back to start  */
            q = (MD *) &mp->mp_mfl ;    /*  q => mfl field  */
            p = q->m_link ;             /*  p => 1st MD     */
        }

        if ((!maxflg) && (p->m_length >= amount))
        {
            /*  big enough      */
            if (p->m_length == amount)
                /* take the whole thing */
                q->m_link = p->m_link;
            else
            {
                /* break it up - 1st allocate a new
                   MD to describe the remainder */

                /*********** TBD **********
                 *  Nicer Handling of This *
                 *        Situation       *
                 **************************/
                if( (p1=MGET(MD)) == 0 )
                {
                    KDEBUG(("BDOS ffit: null MGET\n"));
                    return(0);
                }

                /*  init new MD  */

                p1->m_length = p->m_length - amount;
                p1->m_start = p->m_start + amount;
                p1->m_link = p->m_link;

                /*  adjust allocated block  */

                p->m_length = amount;
                q->m_link = p1;
            }

            /*  link allocate block into allocated list,
             mark owner of block, & adjust rover  */

            p->m_link = mp->mp_mal;
            mp->mp_mal = p;

            p->m_own = run;

            mp->mp_rover =
                (q == (MD *) &mp->mp_mfl ? q->m_link : q);

            KDEBUG(("BDOS ffit: start=0x%08lx, length=%ld\n",(ULONG)p->m_start,p->m_length));

            return(p);  /* got some */
        }
        else if (p->m_length > maxval)
            maxval = p->m_length;

        p = ( q=p )->m_link;

    } while (q != mp->mp_rover);

    /*  return either the max, or 0 (error)  */

#ifdef ENABLE_KDEBUG
    if( maxflg )
        KDEBUG(("BDOS ffit: maxval=%ld\n",maxval));
    else
        KDEBUG(("BDOS ffit: Not enough contiguous memory\n"));
#endif
    return( maxflg ? (MD *) maxval : 0 ) ;
}


/*
 *  freeit - Free up a memory descriptor
 */

void freeit(MD *m, MPB *mp)
{
    MD *p, *q;

#if     STATIUMEM
    ++ccfreeit ;
#endif

    q = 0;

    for (p = mp->mp_mfl; p ; p = (q=p) -> m_link)
        if (m->m_start <= p->m_start)
            break;

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
    KDEBUG(("BDOS freeit: start=0x%08lx, length=%ld\n",(ULONG)m->m_start,m->m_length));

    m->m_link = p;

    if (q)
        q->m_link = m;
    else
        mp->mp_mfl = m;

    if (!mp->mp_rover)
        mp->mp_rover = m;

    if (p)
        if (m->m_start + m->m_length == p->m_start)
        { /* join to higher neighbor */
            m->m_length += p->m_length;
            m->m_link = p->m_link;

            if (p == mp->mp_rover)
                mp->mp_rover = m;

            xmfreblk(p);
        }

    if (q)
        if (q->m_start + q->m_length == m->m_start)
        { /* join to lower neighbor */
            q->m_length += m->m_length;
            q->m_link = m->m_link;

            if (m == mp->mp_rover)
                mp->mp_rover = q;

            xmfreblk(m);
        }
}
