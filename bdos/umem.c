/*
 * umem.c - user memory management interface routines			
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  KTB   Karl T. Braun (kral)
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef DBGUMEM
#define DBGUMEM 1
#endif

#include "portab.h"
#include "fs.h"
#include "bios.h"
#include "mem.h"
#include "gemerror.h"
#include "../bios/kprint.h"


#ifdef DBGUMEM
static void dump_mem_map(void)
{
  MD *m;
  int i;
  
  kprintf("===mem_dump==========================\n");
  kprintf("| mp_mfl = 0x%08lx {\n|   ", m = pmd.mp_mfl);
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
  kprintf("| mp_mal = 0x%08lx {\n|   ", m = pmd.mp_mal);
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
  kprintf("| mp_rover = 0x%08lx\n", pmd.mp_rover);
  kprintf("===/mem_dump==========================\n");
}
#endif

/*
 *  xmalloc - allocate memory
 *
 *	Function 0x48	m_alloc
 */

long	xmalloc(long amount)
{
    MD *m;
    long ret_value;

    if(  amount < -1L  ) {
      ret_value = 0;
      goto ret;
    }

    /*
     * Round odd-value requests (except -1L) to next higher even number.
     */

    if ((amount != -1L) && (amount & 1))
        amount++;

    /*
     * Pass the request on to the internal routine.  If it was not able
     * to grant the request, then abort.
     */

    if (!(m = ffit(amount,&pmd))) {
      ret_value = 0;
      goto ret;
    }

    /*
     * If the request was -1L, the internal routine returned the amount
     * of available memory, rather than a pointer to a memory descriptor.
     */

    if (amount == -1L) {
      ret_value = (long) m;
      goto ret;
    }

    /*
     * The internal routine returned a pointer to a memory descriptor.
     * Return its pointer to the start of the block.
     */

    ret_value = (long) m->m_start;

ret:
#if DBGUMEM
    kprintf("BDOS: xmalloc(0x%08lx) = 0x%08lx\n", amount, ret_value);
    dump_mem_map();
#endif

    return(ret_value);
}


/*
 *  xmfree - Function 0x49	m_free
 */

long	xmfree(long addr)
{
    MD *p,**q;

    for (p = *(q = &pmd.mp_mal); p; p = *(q = &p->m_link))
        if (addr == p->m_start)
            break;

    if (!p)
        return(EIMBA);

    *q = p->m_link;
    freeit(p,&pmd);

    return(E_OK);
}

/*
 * xsetblk - Function 0x4A	m_shrink
 *
 * Arguments:
 *  n   - dummy, not used
 *  blk - addr of block to free
 *  len - length of block to free
 */

long	xsetblk(int n, char *blk, long len)
{
    MD *m,*p;

    /*
     * Traverse the list of memory descriptors looking for this block.
     */

    for (p = pmd.mp_mal; p; p = p->m_link)
        if(  (long) blk == p->m_start  )
            break;

    /*
     * If block address doesn't match any memory descriptor, then abort.
     */

    if (!p)
        return(EIMBA);

    /*
     * If the caller is not shrinking the block size, then abort.
     */

    if (p->m_length < len)
        return(EGSBF);

    /*
     * Always shrink to an even word length.
     */

    if (len & 1)
        len++;

    /*
     * Create a memory descriptor for the freed portion of memory.
     */

    m = MGET(MD);

#if	DBGUMEM
    /* what if 0? */
    if( m == 0 )
        kpanic("umem.c/xsetblk: Null Return From MGET\n") ;
#endif

    m->m_start = p->m_start + len;
    m->m_length = p->m_length - len;
    p->m_length = len;
    freeit(m,&pmd);

    return(E_OK);
}

