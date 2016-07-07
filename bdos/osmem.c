/*
 * osmem.c - allocate/release os memory
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2016 The EmuTOS development team
 *
 * Authors:
 *  KTB   Karl T. Braun (kral)
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 *  mods
 *
 *  mod no.      date      who  comments
 *  ------------ --------  ---  --------
 *  M01.01a.0708 08 Jul 86 ktb  xmgetblk wasn't checking root index
 *
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "portab.h"
#include "nls.h"
#include "fs.h"
#include "mem.h"
#include "kprint.h"

/*
 *  local constants
 */

#define LENOSM 3900     /* size of os memory pool, in words */

/*
 *  internal variables
 */

static  int     osmptr;
static  int     osmlen;
static  int     osmem[LENOSM];

/*
 *  root - root array for 'quick' pool.
 *      root is an array of ptrs to memblocks of size 'i' paragraphs,
 *      where 'i' is the index into the array (a paragraph is 16 bytes).
 *      Each list is singly linked.  Items on the list are
 *      deleted/added in LIFO order from the root.
 *
 *  note: MAXQUICK used to be 20, but 5 (indexes 0-4) is now all we need
 */

#define MAXQUICK    5
int     *root[MAXQUICK];

#define MCELLSIZE(n)    (((n)+15)>>4)

/*
 *  local debug counters
 */

static  long    dbgfreblk;
static  long    dbggtosm;
static  long    dbggtblk;


/*
 * getosm - get a block of memory from the main o/s memory pool
 * (as opposed to the 'fast' list of freed blocks).
 *
 * Treats the os pool as a large array of integers, allocating from the base.
 *
 * Arguments:
 *  n -  number of words
 */

static int *getosm(int n)
{
    int *m;

    if( n > osmlen )
    {
      /*  not enough room  */
        dbggtosm++ ;
        return 0;
    }

    m = &osmem[osmptr];         /*  start at base               */
    osmptr += n;                /*  new base                    */
    osmlen -= n;                /*  new length of free block    */
    return(m);                  /*  allocated memory            */
}



/*
 *  xmgetblk - get a block of memory from the o/s pool.
 *
 * First round up the size requested to a whole number of paragraphs
 * (where 1 paragraph = 16 bytes), and then try to get a block from 
 * the 'fast' list - a list of lists of blocks, where list[i] is a
 * list of blocks of size i paragraphs.  These lists are singly linked
 * and are deleted/removed in LIFO order from the root.  If there are
 * no free blocks on the desired list, we call getosm to get a block
 * from the os memory pool.
 *
 * Note the special handling when we are unable to obtain memory for
 * a DND or OFD.
 *
 * Arguments:
 *  blksize: size of block required, in bytes
 */
void *xmgetblk(int blksize)
{
    int i, j, w, *m, *q, **r;

    i = MCELLSIZE(blksize);     /* convert to paragraphs */
    w = i << 3;                 /* number of words */

    /*
     *  allocate block
     */

    /*  M01.01a.0708.01  */
    if( i >= MAXQUICK )
    {
        dbggtblk++ ;
        return( NULL ) ;
    }

    /*
     * we should execute the following loop a maximum of twice: the second
     * time only if we're allocating a DND or OFD & no memory is available
     */
    for (j = 0; ; j++)
    {
        if ( *(r = &root[i]) )      /* there is an item on the free list */
        {
            m = *r;                 /* get first item on list   */
            *r = *((int **) m);     /* root points to next item */
            break;
        }

        /* nothing on free list, try pool */
        if ( (m = getosm(w+1)) )    /* include size of control word */
        {
            *m++ = i;               /* put size in control word */
            break;
        }

        /* no memory available & not DND/OFD, that's (sort of) OK */
        if ((blksize != sizeof(DND)) && (blksize != sizeof(OFD)))
            break;

        /*
         * no memory for DND/OFD, try to get some
         *
         * note defensive programming: if free_available_dnds() said it
         * worked, but we're here again, then it lied and we should quit
         * to avoid an infinite loop
         */
        if ((j >= 2) || (free_available_dnds() == 0))
        {
            kcprintf(_("\033EOut of internal memory.\nUse FOLDR100.PRG to get more.\nSystem halted!\n"));
            halt();                         /*  halt system                  */
        }
    }

    /*
     *  zero out the block
     */

    if( (q = m) )
        for( j = 0 ; j < w ; j++ )
            *q++ = 0;

    return(m);
}

/*
 *  xmfreblk - free up memory allocated through mgetblk
 */

void    xmfreblk(void *m)
{
    int i;

    i = *(((int *)m) - 1);
    if( i < 0 || i >= MAXQUICK )
    {
        /*  bad index  */
        KDEBUG(("xmfreblk: bad index (0x%x), stack at 0x%p\n",i,&m));
#ifdef ENABLE_KDEBUG
        while(1)
            ;
#endif
        dbgfreblk++ ;
    }
    else
    {
        /*  ok to free up  */
        *((int **) m) = root[i];
        root[i] = m;
        if(  *((int **)m) == m )
            KDEBUG(("xmfreblk: Circular link in root[0x%x] at 0x%p\n",i,m));
    }
}

/*
 * called by bdosmain to initialise the OS memory pool.
 */

void osmem_init(void)
{
    osmlen = LENOSM;
    dbgfreblk = 0;
    dbggtosm = 0;
    dbggtblk = 0;
}
