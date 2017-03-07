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
#define NUM_OSM_BLOCKS  118         /* more than TOS, probably larger than necessary */
#define LEN_OSM_BLOCK   (2+64)      /* in bytes */
/* size of os memory pool, in words: */
#define LENOSM          (LEN_OSM_BLOCK*NUM_OSM_BLOCKS/sizeof(WORD))


/*
 *  local typedefs
 */
#define MDS_PER_BLOCK   3

typedef struct {
    MD md;
    WORD index;         /* if used, 0-2, else -1 */
} MDEXT;

typedef struct _mdb MDBLOCK;
struct _mdb {
    MDBLOCK *mdb_next;
    MDEXT entry[MDS_PER_BLOCK];
};


/*
 *  internal variables
 */
static WORD osmptr;
static WORD osmlen;
static WORD osmem[LENOSM];


/*
 *  root - root array for 'quick' pool
 *
 *  root is an array of ptrs to memblocks of size 'i' paragraphs, where
 *  'i' is the index into the array (a paragraph is 16 bytes).  Each
 *  list is singly linked.  Items on the list are deleted/added in LIFO
 *  order from the root.
 *
 *  note: MAXQUICK used to be 20, but 5 (indexes 0-4) is now all we need,
 *  and we actually only use index 4 (all blocks are 64 bytes).
 */
#define MAXQUICK    5
WORD *root[MAXQUICK];

static MDBLOCK *mdbroot;    /* root for partially-used MDBLOCKs */


/*
 *  local debug counters
 */
static LONG dbgfreblk;
static LONG dbggtosm;
static LONG dbggtblk;


/*
 * getosm - get a block of memory from the main o/s memory pool
 * (as opposed to the 'fast' list of freed blocks).
 *
 * Treats the os pool as a large array of integers, allocating from the base.
 *
 * Arguments:
 *  n -  number of words
 */
static WORD *getosm(WORD n)
{
    WORD *m;

    if (n > osmlen)
    {
        /* not enough room */
        dbggtosm++;
        return 0;
    }

    m = &osmem[osmptr];         /*  start at base               */
    osmptr += n;                /*  new base                    */
    osmlen -= n;                /*  new length of free block    */
    return m;                   /*  allocated memory            */
}


/*
 *  unlink_mdblock - unlinks an MDBLOCK from the mdb chain
 *
 *  returns -1 iff the MDBLOCK is not on the mdb chain
 */
static WORD unlink_mdblock(MDBLOCK *mdb)
{
    MDBLOCK *prev, *next;

    next = mdb->mdb_next;
    mdb->mdb_next = NULL;   /* neatness */

    if (mdb == mdbroot)     /* first on mdb chain? */
    {
        mdbroot = next;     /* yes, just point root to next */
        return 0;
    }

    /*
     * find previous MDBLOCK
     */
    for (prev = mdbroot; prev; prev = prev->mdb_next)
    {
        if (prev->mdb_next == mdb)
        {
            prev->mdb_next = next; /* just snip it out */
            return 0;
        }
    }

    KDEBUG(("unlink_mdblock(): cannot unlink MDBLOCK at %p, not on mdb chain\n",mdb));
    return -1;
}


/*
 *  xmgetmd - get an MD
 *
 *  To create a single pool for all osmem requests, MDs are grouped in
 *  blocks of 3 called MDBLOCKs which occupy 58 bytes.  MDBLOCKs are
 *  handled as follows:
 *    . they are linked in a chain, initially empty
 *    . when the first MD is required, an MDBLOCK is obtained via
 *      xmgetblk() and put on the chain, and the first slot is allocated
 *    . MDs are obtained from existing partially-used MDBLOCKS
 *    . when an MDBLOCK is full, it is removed from the chain
 *    . when an MD in a full MDBLOCK is freed, the MDBLOCK is put back
 *      on the chain
 *    . when the MDBLOCK is totally unused, it is put back on the normal
 *      free chain
 */
MD *xmgetmd(void)
{
    MDBLOCK *mdb = mdbroot;
    MD *md;
    WORD i, avail;

    if (!mdb)
    {
        mdb = MGET(MDBLOCK);
        if (!mdb)
            return NULL;

        /* initialise new MDBLOCK */
        mdb->mdb_next = NULL;
        for (i = 0; i < MDS_PER_BLOCK; i++)
            mdb->entry[i].index = -1;   /* unused */
        mdbroot = mdb;
        KDEBUG(("xmgetmd(): got new MDBLOCK at %p\n",mdb));
    }

    /*
     * allocate MD from MDBLOCK
     */
    for (i = 0, avail = 0, md = NULL; i < MDS_PER_BLOCK; i++)
    {
        if (mdb->entry[i].index < 0)
        {
            if (!md)                /* not yet allocated */
            {
                mdb->entry[i].index = i;
                md = &mdb->entry[i].md;
                KDEBUG(("xmgetmd(): got MD at %p\n",md));
            }
            else avail++;
        }
    }

    if (!md)
    {
        KDEBUG(("xmgetmd(): MDBLOCK at %p is invalid, no free entries\n",mdb));
        return NULL;
    }

    /*
     * remove full MDBLOCK from mdb chain
     */
    if (avail == 0)
    {
        KDEBUG(("xmgetmd(): MDBLOCK at %p is now full\n",mdb));
        if (unlink_mdblock(mdb) == 0)
            KDEBUG(("xmgetmd(): removed MDBLOCK at %p from mdb chain\n",mdb));
    }

    return md;
}


/*
 *  xmfremd - free an MD
 */
void xmfremd(MD *md)
{
    MDBLOCK *mdb;
    MDEXT *entry;
    WORD i, avail;

    i = *(WORD *)(md+1);            /* word following MD */
    if ((i < 0) || (i >= MDS_PER_BLOCK))
    {
        KDEBUG(("xmfremd(): MD at %p not freed, invalid index %d\n",md,i));
        return;
    }

    entry = (MDEXT *)md - i;        /* point to first entry */
    mdb = (MDBLOCK *)((char *)entry - sizeof(MDBLOCK *));

    mdb->entry[i].index = -1;       /* mark as free */
    KDEBUG(("xmfremd(): MD at %p freed\n",md));

    for (i = 0, avail = 0; i < MDS_PER_BLOCK; i++)
        if (mdb->entry[i].index < 0)
            avail++;

    switch(avail) {
    case 3:             /* remove from mdb chain & put on free chain */
        KDEBUG(("xmfremd(): MDBLOCK at %p is now empty\n",mdb));
        if (unlink_mdblock(mdb) == 0)
        {
            xmfreblk(mdb);          /* move to free chain */
            KDEBUG(("xmfremd(): MDBLOCK at %p moved to free chain\n",mdb));
        }
        break;
    case 2:
        break;
    case 1:             /* add to mdb chain */
        mdb->mdb_next = mdbroot;
        mdbroot = mdb;
        KDEBUG(("xmfremd(): MDBLOCK at %p now has free entry, moved to mdb chain\n",mdb));
        break;
    default:
        KDEBUG(("xmfremd(): MDBLOCK at %p is invalid, %d free entries\n",mdb,avail));
        break;
    }
}


/*
 *  xmgetblk - get a block of memory from the o/s pool.
 *
 * First try to get a block from the 'fast' list, anchored by root[4],
 * a list of blocks of size 64 bytes.  This list is singly linked and
 * blocks are deleted/removed in LIFO order from the root.  If there
 * are no free blocks on the list, we call getosm to get a block from
 * the os memory pool.
 *
 * If we cannot get memory for an MDBLOCK, we return NULL (the request
 * will fail).  Otherwise we will attempt to free up DNDs to make space
 * and if that fails, the system will be halted.
 *
 * Arguments:
 *  memtype: the type of request
 */
void *xmgetblk(WORD memtype)
{
    WORD i, j, w, *m, *q, **r;

    if ((memtype < MEMTYPE_MDBLOCK) || (memtype > MEMTYPE_OFD))
    {
        dbggtblk++;
        return NULL;
    }

    /*
     *  allocate block
     */
    i = 4;                          /* always from root[4] */
    w = 32;                         /* number of words */

    /*
     * we should execute the following loop a maximum of twice: the second
     * time only if we're allocating a DMD/DND/OFD & no memory is available
     */
    for (j = 0; ; j++)
    {
        if ( *(r = &root[i]) )      /* there is an item on the free list */
        {
            m = *r;                 /* get first item on list   */
            *r = *((WORD **) m);    /* root points to next item */
            break;
        }

        /* nothing on free list, try pool */
        if ( (m = getosm(w+1)) )    /* include size of control word */
        {
            *m++ = i;               /* put size in control word */
            break;
        }

        /* no memory available for an MDBLOCK, that's (sort of) OK */
        if (memtype == MEMTYPE_MDBLOCK)
            break;

        /*
         * no memory for DMD/DND/OFD, try to get some
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

    if ( (q = m) )
        for (j = 0; j < w; j++)
            *q++ = 0;

    return m;
}


/*
 *  xmfreblk - free up memory allocated through xmgetblk
 */
void xmfreblk(void *m)
{
    WORD i;

    i = *(((WORD *)m) - 1);

    if (i != 4)
    {
        /*  bad index  */
        KDEBUG(("xmfreblk: bad index (0x%x), stack at 0x%p\n",i,&m));
#ifdef ENABLE_KDEBUG
        while(1)
            ;
#endif
        dbgfreblk++;
    }
    else
    {
        /*  ok to free up  */
        *((WORD **) m) = root[i];
        root[i] = m;
        if (*((WORD **)m) == m)
            KDEBUG(("xmfreblk: Circular link in root[0x%x] at 0x%p\n",i,m));
    }
}


/*
 * called by bdosmain to initialise the OS memory pool
 */
void osmem_init(void)
{
    osmlen = LENOSM;
    mdbroot = NULL;
    dbgfreblk = 0;
    dbggtosm = 0;
    dbggtblk = 0;
}
