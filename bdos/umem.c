/*
 * umem.c - user memory management interface routines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002-2014 The EmuTOS development team
 *
 * Authors:
 *  KTB   Karl T. Braun (kral)
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


/* define ENABLE_KDEBUG */



#include "config.h"
#include "portab.h"
#include "fs.h"
#include "mem.h"
#include "gemerror.h"
#include "biosbind.h"
#include "kprint.h"



/*
 *  global variables
 */


MPB pmd;
#if CONF_WITH_ALT_RAM
MPB pmdalt;
int has_alt_ram;
#endif

/* internal variables */

long start_stram;
long end_stram;


/*
 * static functions
 */

#ifdef ENABLE_KDEBUG
static void dump_mem_map(void)
{
    MD *m;
    int i;

    kprintf("===mem_dump==========================\n");
    kprintf("| mp_mfl = 0x%08lx {\n|   ", (long)(m = pmd.mp_mfl));
    i = 0;
    for(; m != NULL ; m = m->m_link) {
        if(i >= 3) {
            kprintf("\n|   ");
            i = 0;
        }
        i++;
        kprintf("[0x%06lx, 0x%06lx], ", m->m_start, m->m_length);
    }
    kprintf("\n| }\n");
    kprintf("| mp_mal = 0x%08lx {\n|   ",  (long)(m = pmd.mp_mal));
    i = 0;
    for(; m != NULL ; m = m->m_link) {
        if(i >= 3) {
            kprintf(" \n|   ");
            i = 0;
        }
        i++;
        kprintf("[0x%06lx, 0x%06lx], ", m->m_start, m->m_length);
    }
    kprintf("\n| }\n");
    kprintf("| mp_rover = 0x%08lx\n",  (long)(pmd.mp_rover));
    kprintf("===/mem_dump==========================\n");
}
#else
#define dump_mem_map()
#endif



/*
 *  xmalloc - allocate memory
 *
 *      Function 0x48   m_alloc
 */

long    xmalloc(long amount)
{
    if( run->p_flags & PF_TTRAMMEM ) {
        /* allocate TT RAM, or ST RAM if not enough TT RAM */
        return xmxalloc(amount, MX_PREFTTRAM);
    } else {
        /* allocate only ST RAM */
        return xmxalloc(amount, MX_STRAM);
    }
}


/*
 *  xmfree - Function 0x49      m_free
 */

long    xmfree(long addr)
{
    MD *p,**q;
    MPB *mpb;

    KDEBUG(("BDOS: Mfree(0x%08lx)\n",(long)addr));

    if(addr >= start_stram && addr <= end_stram) {
        mpb = &pmd;
#if CONF_WITH_ALT_RAM
    } else if(has_alt_ram) {
        mpb = &pmdalt;
#endif
    } else {
        return EIMBA;
    }

    for (p = *(q = &mpb->mp_mal); p; p = *(q = &p->m_link))
        if (addr == p->m_start)
            break;

    if (!p)
        return(EIMBA);

    *q = p->m_link;
    freeit(p,mpb);

    return(E_OK);
}

/*
 * xsetblk - Function 0x4A      m_shrink
 *
 * Arguments:
 *  n   - dummy, not used
 *  blk - addr of block to free
 *  len - length of block to free
 */

long    xsetblk(int n, void *blk, long len)
{
    MD *m,*p;
    MPB *mpb;

    KDEBUG(("BDOS: Mshrink(0x%08lx,%ld)\n",(long)blk,len));

    if(((long)blk) >= start_stram && ((long)blk) <= end_stram) {
        mpb = &pmd;
        KDEBUG(("BDOS xsetblk: mpb=&pmd\n"));

#if CONF_WITH_ALT_RAM
    } else if(has_alt_ram) {
        mpb = &pmdalt;
        KDEBUG(("BDOS xsetblk: mpb=&pmdalt\n"));
#endif /* CONF_WITH_ALT_RAM */
    } else {
        return EIMBA;
    }

    /*
     * Traverse the list of memory descriptors looking for this block.
     */

    for (p = mpb->mp_mal; p; p = p->m_link)
        if(  (long) blk == p->m_start  )
            break;

    /*
     * If block address doesn't match any memory descriptor, then abort.
     */

    if (!p)
        return(EIMBA);

    /*
     * Round the size to a multiple of 4 bytes to keep alignment.
     * Alignment on long boundaries matters in FastRAM.
     */

    len = (len + 3) & ~3;

    KDEBUG(("BDOS xsetblk: new length=%ld\n",len));

    /*
     * If the caller is not shrinking the block size, then abort.
     */

    if (p->m_length < len)
        return(EGSBF);

    /*
     * Create a memory descriptor for the freed portion of memory.
     */

    m = MGET(MD);

#ifdef ENABLE_KDEBUG
    /* what if 0? */
    if( m == 0 )
        panic("umem.c/xsetblk: Null Return From MGET\n") ;
#endif

    m->m_start = p->m_start + len;
    m->m_length = p->m_length - len;
    p->m_length = len;
    freeit(m,mpb);

    return(E_OK);
}

/*
 *  xmxalloc - allocate memory
 *
 *      Function 0x44   m_xalloc
 */

#define MX_STRAM 0
#define MX_TTRAM 1
#define MX_PREFSTRAM 2
#define MX_PREFTTRAM 3

long    xmxalloc(long amount, int mode)
{
    MD *m;
    long ret_value;

    KDEBUG(("BDOS: xmxalloc(%ld,%d)\n",amount,mode));

    /*
     * if amount == -1L, return the size of the biggest block
     *
     */
    if(amount == -1L) {
        switch(mode) {
        case MX_STRAM:
            ret_value = (long) ffit(-1L,&pmd);
            break;
#if CONF_WITH_ALT_RAM
        case MX_TTRAM:
            ret_value = (long) ffit(-1L,&pmdalt);
            break;
#endif
        case MX_PREFSTRAM:
        case MX_PREFTTRAM:
            /* TODO - I assume that the correct behaviour is to return
             * the biggest size in either pools. The documentation is unclear.
             */
            {
                ret_value = (long) ffit(-1L,&pmd);
#if CONF_WITH_ALT_RAM
                {
                    long tmp = (long) ffit(-1L,&pmdalt);
                    if(ret_value < tmp) ret_value = tmp;
                }
#endif
            }
            break;
        default:
            /* unknown mode */
            ret_value = 0;
        }
        goto ret;
    }

    /*
     * return NULL if asking for a negative or null amount
     */

    if( amount <= 0 ) {
        ret_value = 0;
        goto ret;
    }

    /*
     * Pass the request on to the internal routine.
     */

    switch(mode) {
    case MX_STRAM:
        m = ffit(amount,&pmd);
        break;
#if CONF_WITH_ALT_RAM
    case MX_TTRAM:
        m = ffit(amount,&pmdalt);
        break;
#endif
    case MX_PREFSTRAM:
        m = ffit(amount,&pmd);
#if CONF_WITH_ALT_RAM
        if(m == NULL)
            m = ffit(amount,&pmdalt);
#endif
        break;
    case MX_PREFTTRAM:
#if CONF_WITH_ALT_RAM
        m = ffit(amount,&pmdalt);
        if(m == NULL)
#endif
            m = ffit(amount,&pmd);
        break;
    default:
        /* unknown mode */
        m = 0;
    }

    /*
     * The internal routine returned a pointer to a memory descriptor, or NULL
     * Return its pointer to the start of the block.
     */

    if(m == NULL) {
        ret_value = 0;
    } else {
        ret_value = (long) m->m_start;
    }

ret:
    KDEBUG(("BDOS xmxalloc: returns 0x%08lx\n",ret_value));
    dump_mem_map();

    return(ret_value);
}

#if CONF_WITH_ALT_RAM

/*
 * Maddalt() informs GEMDOS of the existence of additional 'alternative'
 * RAM that would not normally have been identified by the system.
 *
 * Available as of GEMDOS version 0.19 only.
 * Parameters: start indicates the starting address for the block of
 * memory to be added to the GEMDOS free list. size indicates the length
 * of this block in bytes.
 *
 * Maddalt() returns E_OK (0) if the call succeeds or a negative GEMDOS error
 * code otherwise.
 *
 * This call should only be used to identify RAM not normally identified
 * by the BIOS at startup (added through a VME-card or hardware
 * modification). Once this RAM has been identified to the system it
 * may not be removed and should only be allocated and used via the
 * standard system calls. In addition, programs wishing to use this
 * RAM must have their alternative RAM load bit set or use Mxalloc()
 * to specifically request alternative RAM.
 */

long xmaddalt( LONG start, LONG size)
{
    MD *md, *p;

    /* shrink size to a multiple of 4 bytes */
    size &= -4L;

    /* only strictly positive sizes allowed */
    if(size <= 0) return -1;

    /* does it overlap with ST RAM? */
    if((start < start_stram && start+size > start_stram) || start < end_stram)
        return -1;

    /* if the new block is just after a free one, just extend it */
    for (p = pmdalt.mp_mfl; p; p = p->m_link) {
        if (p->m_start + p->m_length == start)
            p->m_length += size;

        return 0;
    }

    md = MGET(MD);
    if(md == NULL) return ENSMEM;
    md->m_start = start;
    md->m_length = size;
    md->m_own = NULL;
    if(has_alt_ram) {
        /* some alternative RAM has already been registered, just insert it
         * to the beginning of the free block list.
         */
        p = pmdalt.mp_mfl;
        pmdalt.mp_mfl = md;
        if(pmdalt.mp_rover == p) pmdalt.mp_rover = md;
        md->m_link = p;
    } else {
        md->m_link = NULL;
        pmdalt.mp_mfl = md;
        pmdalt.mp_mal = NULL;
        pmdalt.mp_rover = md;
        has_alt_ram = 1;
    }
    return 0;
}

#endif /* CONF_WITH_ALT_RAM */

/*
 * user memory init
 * called by bdosmain at the beginning; will call getmpb and immediately
 * keep the start and end addresses to be able later to determine if
 * addresses belong to one pool or the other.
 */

void umem_init(void)
{
    /* get the MPB */
    Getmpb((long)&pmd);
    /* derive the addresses, assuming the MPB is in clean state */
    start_stram = pmd.mp_mfl->m_start;
    end_stram = start_stram + pmd.mp_mfl->m_length;
#if CONF_WITH_ALT_RAM
    /* there is no known alternative RAM initially */
    has_alt_ram = 0;
#endif
}
