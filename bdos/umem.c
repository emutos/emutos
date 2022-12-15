/*
 * umem.c - user memory management interface routines
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2022 The EmuTOS development team
 *
 * Authors:
 *  KTB   Karl T. Braun (kral)
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "bdosdefs.h"
#include "fs.h"
#include "mem.h"
#include "gemerror.h"
#include "biosbind.h"
#include "biosext.h"
#include "xbiosbind.h"
#include "bdosstub.h"
#include "cookie.h"
#include "string.h"
#include "has.h"        /* for has_videl */


/*
 *  global variables
 */
MPB pmd;
#if CONF_WITH_ALT_RAM
MPB pmdalt;
int has_alt_ram;
#endif

/* value 2^n-1 to add to an address to round it */
ULONG malloc_align_stram;

/* internal variables */

static UBYTE *start_stram;
static UBYTE *end_stram;


/*
 * static functions
 */
#ifdef ENABLE_KDEBUG
#define MAX_MD_COUNT    1000    /* for loop detection */
#define MAX_MD_LIST     20      /* for dump */
static void dump_md_list(char *title,MD *md)
{
    MD *m;
    int i, n;

    for (n = 0, m = md; m && (n < MAX_MD_COUNT); m = m->m_link)
        n++;

    kprintf("| %s (%s%d entries) = %p {\n|   ",
            title, (n>=MAX_MD_COUNT)? "over ": "", n, md);

    for (i = 0, n = 0, m = md; m && (n < MAX_MD_LIST); i++, n++, m = m->m_link) {
        if (i >= 2) {
            kprintf("\n|   ");
            i = 0;
        }
        kprintf("0x%06lx[0x%06lx,0x%06lx], ", (ULONG)m, (ULONG)m->m_start, m->m_length);
    }
    kprintf("\n| }\n");
}

static void dump_mem_map(void)
{
    kprintf("===mem_dump==========================\n");
    dump_md_list("std free ", pmd.mp_mfl);
    dump_md_list("std alloc", pmd.mp_mal);
#if CONF_WITH_ALT_RAM
    if (has_alt_ram)
    {
        kprintf("| ----------------------\n");
        dump_md_list("alt free ", pmdalt.mp_mfl);
        dump_md_list("alt alloc", pmdalt.mp_mal);
    }
#endif
    kprintf("===/mem_dump==========================\n");
}
#else
#define dump_mem_map()
#endif

/*
 * find the mpb corresponding to the memory address
 *
 * returns NULL if not found
 */
static MPB *find_mpb(void *addr)
{
    if (((UBYTE *)addr >= start_stram) && ((UBYTE *)addr < end_stram))
        return &pmd;

#if CONF_WITH_ALT_RAM
    if (has_alt_ram)
        return &pmdalt;
#endif

    return NULL;
}


/*
 *  xmalloc - Function 0x48 (Malloc)
 */
void *xmalloc(long amount)
{
    void *rc;

    if (run->p_flags & PF_TTRAMMEM) {
        /* allocate TT RAM, or ST RAM if not enough TT RAM */
        rc = xmxalloc(amount, MX_PREFTTRAM);
    } else {
        /* allocate only ST RAM */
        rc = xmxalloc(amount, MX_STRAM);
    }
    KDEBUG(("BDOS: Malloc(%ld), pgmflags=0x%08lx: rc=0x%08lx\n",amount,run->p_flags,(ULONG)rc));
    return rc;
}


/*
 *  xmfree - Function 0x49 (Mfree)
 */
long xmfree(void *addr)
{
    MD *p;
    MPB *mpb;

    KDEBUG(("BDOS: Mfree(%p)\n",addr));

    mpb = find_mpb(addr);
    if (!mpb)
        return EIMBA;

    KDEBUG(("BDOS Mfree: mpb=%s\n",(mpb==&pmd)?"pmd":"pmdalt"));

    for (p = mpb->mp_mal; p; p = p->m_link)
        if (addr == p->m_start)
            break;

    if (!p)
        return EIMBA;

    freeit(p,mpb);
    dump_mem_map();

    return E_OK;
}


/*
 * xsetblk - Function 0x4A (Mshrink)
 *
 * Arguments:
 *  n   - dummy, not used
 *  blk - addr of block to free
 *  len - length of block to free
 */
long xsetblk(int n, void *blk, long len)
{
    MD *p;
    MPB *mpb;

    KDEBUG(("BDOS: Mshrink(%p,%ld)\n",blk,len));

    mpb = find_mpb(blk);
    if (!mpb)
        return EIMBA;

    KDEBUG(("BDOS Mshrink: mpb=%s\n",(mpb==&pmd)?"pmd":"pmdalt"));

    /*
     * Traverse the list of memory descriptors looking for this block.
     */
    for (p = mpb->mp_mal; p; p = p->m_link)
        if ((UBYTE *)blk == p->m_start)
            break;

    /*
     * If block address doesn't match any memory descriptor, then abort.
     */
    if (!p)
        return EIMBA;

    /*
     * Round the size up to a multiple of 2 or 4 bytes to keep alignment.
     * Alignment on long boundaries is faster in FastRAM.
     */
    if (mpb == &pmd)
        len = (len + malloc_align_stram) & ~malloc_align_stram;
    else
        len = (len + MALLOC_ALIGN_ALTRAM) & ~MALLOC_ALIGN_ALTRAM;

    KDEBUG(("BDOS Mshrink: new length=%ld\n",len));

    /*
     * Check new length.
     */
    if (p->m_length < len)
        return EGSBF;
    if (p->m_length == len)     /* nothing to do */
        return E_OK;
    if (len == 0)               /* just like Mfree() */
    {
        freeit(p,mpb);
        return E_OK;
    }

    /*
     * Shrink the existing MD & create a new one for the freed portion of memory.
     */
    if (shrinkit(p,mpb,len) < 0)
        return ERR;

    dump_mem_map();

    return E_OK;
}

/*
 *  xmxalloc - Function 0x44 (Mxalloc)
 */
void *xmxalloc(long amount, int mode)
{
    MD *m;
    void *ret_value;

    KDEBUG(("BDOS: Mxalloc(%ld,0x%04x)\n",amount,mode));

    mode &= MX_MODEMASK;    /* ignore unsupported bits */

    /*
     * if amount == -1L, return the size of the biggest block
     *
     */
    if (amount == -1L) {
        switch(mode) {
        case MX_STRAM:
            ret_value = ffit(-1L,&pmd);
            break;
#if CONF_WITH_ALT_RAM
        case MX_TTRAM:
            ret_value = ffit(-1L,&pmdalt);
            break;
#endif
        case MX_PREFSTRAM:
        case MX_PREFTTRAM:
            /*
             * for the "preferred" options, the correct behaviour is to
             * return the biggest size in either pool - verified on TOS3
             */
            {
                ret_value = ffit(-1L,&pmd);
#if CONF_WITH_ALT_RAM
                {
                    void *tmp = ffit(-1L,&pmdalt);
                    if (ret_value < tmp)
                        ret_value = tmp;
                }
#endif
            }
            break;
        default:
            /* unknown mode */
            ret_value = NULL;
        }
        goto ret;
    }

    /*
     * return NULL if asking for a negative or null amount
     */
    if (amount <= 0) {
        ret_value = NULL;
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
        if (m == NULL)
            m = ffit(amount,&pmdalt);
#endif
        break;
    case MX_PREFTTRAM:
#if CONF_WITH_ALT_RAM
        m = ffit(amount,&pmdalt);
        if (m == NULL)
#endif
            m = ffit(amount,&pmd);
        break;
    default:
        /* unknown mode */
        m = NULL;
    }

    /*
     * The internal routine returned a pointer to a memory descriptor, or NULL
     * Return its pointer to the start of the block.
     */
    if (m == NULL) {
        ret_value = NULL;
    } else {
        ret_value = m->m_start;
    }

ret:
    KDEBUG(("BDOS Mxalloc: returns 0x%08lx\n",(ULONG)ret_value));
    dump_mem_map();

    return ret_value;
}

#if CONF_WITH_VIDEL
/*
 *  srealloc - Function 0x15 (Srealloc)
 *
 *  This system call (undocumented by Atari) was introduced in Falcon TOS.
 *  It has two functions:
 *  'len' < 0:  returns the maximum amount of memory that could be used
 *              for the screen; on standard Atari systems, this is the
 *              current size of video ram plus the size of the free memory
 *              (if any) immediately below the current video ram.
 *  'len' >= 0: allocates a block of memory of size 'len' for the screen
 *              and returns a pointer to it; the memory is not part of the
 *              normal GEMDOS memory pool.  returns NULL if the memory
 *              cannot be allocated.
 *
 *  note:
 *    . the actual amount of memory allocated (by EmuTOS and TOS4) is 256
 *      bytes more than specified, for compatibility with screen memory
 *      allocation in other versions of TOS.
 */
#define FREESPACE_KLUDGE    256     /* see code below */
void *srealloc(long amount)
{
    MD *md, *last;
    LONG available;
    BOOL realloc;   /* TRUE iff reallocation is possible */

    if (!has_videl)
        return (void *)EINVFN;

    if (video_ram_size == 0)    /* unspecified */
        return NULL;

    /*
     * first, calculate available video ram size
     */

    /* find last free memory segment */
    for (md = last = pmd.mp_mfl; md; last = md, md = md->m_link)
        ;
    if (!last)                  /* "can't happen" */
        return NULL;

    /*
     * if free memory is contiguous with existing video ram, we can mess
     * (carefully) with the last free space MD.  in the very unlikely
     * event that we used up all the free space defined by the MD, we'd
     * have to free up the MD itself which would be messy.  the following
     * calculation of 'available' ensures that the MD will always have at
     * least FREESPACE_KLUDGE bytes of memory left.
     */
    if (last->m_start + last->m_length == video_ram_addr) {
        available = last->m_length + video_ram_size - EXTRA_VRAM_SIZE - FREESPACE_KLUDGE;
        realloc = TRUE;
    } else {
        available = video_ram_size - EXTRA_VRAM_SIZE;
        if (available < 0)
            available = 0;
        realloc = FALSE;    /* not contiguous, forbid reallocation */
    }

    /* if just a request for size, return it now */
    if (amount < 0L)
        return (void *)available;

    /*
     * else handle request for reallocation
     */
    if (!realloc)               /* can't reallocate */
        return NULL;

    /* round request up to next 256 bytes, then add extra, just like TOS */
    amount = (amount + 255UL) & ~255UL;
    amount += EXTRA_VRAM_SIZE;
    if (amount > available)
        return NULL;

    /* update length in MD, plus saved video ram info */
    last->m_length = last->m_length + video_ram_size - amount;
    video_ram_size = amount;
    video_ram_addr = last->m_start + last->m_length;

    /* finally, clear video ram */
    bzero(video_ram_addr, video_ram_size);

    return (void *)video_ram_addr;
}
#endif

#if CONF_WITH_ALT_RAM

#if CONF_WITH_STATIC_ALT_RAM
/* Static Alt-RAM is the area used by static data (BSS and maybe TEXT) */
extern UBYTE _static_altram_start[];
extern UBYTE _static_altram_end[];
#endif /* CONF_WITH_STATIC_ALT_RAM */

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

long xmaddalt(UBYTE *start, LONG size)
{
    MD *md, *p;

    /* shrink size to a multiple of 4 bytes */
    size &= -4L;

    /* only strictly positive sizes allowed */
    if (size <= 0)
        return -1;

    /* does it overlap with ST RAM? */
    if (((start < start_stram) && (start+size > start_stram))
     || (start < end_stram))
        return -1;

    /* try to merge blocks */
    for (p = pmdalt.mp_mfl; p; p = p->m_link) {
        if (p->m_start + p->m_length == start) {
            /* new block is just after a free one, extend it at end */
            p->m_length += size;
            return 0;
        } else if (start + size == p->m_start) {
            /* new block is just before a free one, extend it at beginning */
            p->m_start -= size;
            p->m_length += size;
            return 0;
        }
    }

#if CONF_WITH_STATIC_ALT_RAM
    if (start == _static_altram_start && _static_altram_end > _static_altram_start)
    {
        /* This region is used by EmuTOS static data.
         * Keep the lower part safe, and only add the upper part to the pool */
        start = _static_altram_end;
        size -= _static_altram_end - _static_altram_start;
    }
#endif

    md = xmgetmd();
    if (md == NULL)
        return ENSMEM;

    md->m_start = start;
    md->m_length = size;
    md->m_own = NULL;
    if (has_alt_ram) {
        /* some alternative RAM has already been registered, just insert it
         * to the beginning of the free block list.
         */
        p = pmdalt.mp_mfl;
        pmdalt.mp_mfl = md;
        md->m_link = p;
    } else {
        md->m_link = NULL;
        pmdalt.mp_mfl = md;
        pmdalt.mp_mal = NULL;
        has_alt_ram = 1;
    }

    return 0;
}

/* Get the total size of all Alt-RAM blocks */
long total_alt_ram(void)
{
    long total = 0;
    MD* md;

#if CONF_WITH_STATIC_ALT_RAM
    /* Add size of static Alt-RAM area */
    total += _static_altram_end - _static_altram_start;
#endif

    /* Add sum of free blocks */
    for(md = pmdalt.mp_mfl; md; md = md->m_link)
        total += md->m_length;

    /* Add sum of allocated blocks */
    for(md = pmdalt.mp_mal; md; md = md->m_link)
        total += md->m_length;

    return total;
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
    ULONG cookie_mch;
    MAYBE_UNUSED(cookie_mch);

    /* get the MPB */
    Getmpb((long)&pmd);

    /* derive the addresses, assuming the MPB is in clean state */
    start_stram = pmd.mp_mfl->m_start;
    end_stram = start_stram + pmd.mp_mfl->m_length;
    KDEBUG(("umem_init(): start_stram=%p, end_stram=%p\n",start_stram,end_stram));

#if CONF_WITH_ALT_RAM
    /* there is no known alternative RAM initially */
    has_alt_ram = 0;
#endif

    /*
     * Atari TOS 1 - 3 aligns memory requested from ST-RAM on multiples of
     * two bytes, whereas Atari TOS 4 aligns on multiples of four bytes.
     * As programs might (implicitly) rely on this detail, EmuTOS bases
     * its alignment on the type of machine it is running on. Note:
     * Alt-RAM memory blocks are always aligned on multiples of 4 bytes.
     */
#if CONF_WITH_VIDEL
    if (cookie_get(COOKIE_MCH, &cookie_mch) && (cookie_mch == MCH_FALCON)) {
        malloc_align_stram = 3; /* 4 byte alignment */
    } else
#endif
    {
        malloc_align_stram = 1; /* 2 byte alignment */
    }
}

/*
 * change the memory owner based on the block address
 */
void set_owner(void *addr, const PD *p)
{
    MD *m;
    MPB *mpb;

    mpb = find_mpb(addr);

    if (!mpb)       /* block address was invalid */
        return;

    for (m = mpb->mp_mal; m; m = m->m_link) {
        if (m->m_start == (UBYTE *)addr) {
            m->m_own = (PD*)p;
            return;
        }
    }
}
