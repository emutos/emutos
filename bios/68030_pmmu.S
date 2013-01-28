/*
 * 68030_pmmu.S - assembler functions for 68030 PMMU setup
 *
 * Copyright (c) 2013 by The EmuTOS development team
 *
 * Authors:
 *  RFB   Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#if CONF_WITH_68030_PMMU
/*
 * SPECIAL NOTE
 * The PMMU tree used here replicates that used by the Falcon.  The tree
 * used by the TT differs in only one area: the 'tib2' table on the TT
 * maps the logical address range 0xf0000000-0xfeffffff to physical
 * addresses 0x00000000-0x0effffff rather than 0xf0000000-0xfeffffff.
 * Since that logical address range is not used on either machine, and
 * since Transparent Translation register 1 covers it anyway, there seems
 * to be no reason not to use the same PMMU tree for both machines.
 */

        .globl  setup_68030_pmmu

        .extern pmmutree            /* start of area for tree */

/*
 * pmmu table entry flags
 */
#define PMMU_FLAGS_PD   0x1         /* bit type: short-format page descriptor */
#define PMMU_FLAGS_TD   0x2         /* bit type: short-format table descriptor */
#define PMMU_FLAGS_CI   0x40        /* cache inhibit (page descriptors only) */

/*
 * macros for pmmu tree
 */
#define PMMU_SF_TABLE(ptr)      dc.l pmmutree+(ptr-pmmustart)+PMMU_FLAGS_TD
#define PMMU_SF_NORMAL(addr)    dc.l addr+PMMU_FLAGS_PD
#define PMMU_SF_INHIBIT(addr)   dc.l addr+PMMU_FLAGS_CI+PMMU_FLAGS_PD

/*
 * IMPORTANT: for safety, the tree must be in protected memory (below 0x800).
 * it currently starts at 0x700; therefore we must make sure that the tree
 * is no more than 0x100 bytes in length.
 */
#define PMMULENGTH  (pmmuend-pmmustart)

        .text

setup_68030_pmmu:
        lea     pmmustart,a0        // a0 -> source
        lea     pmmutree,a1         // a1 -> where to put pmmu tree

// since the tree consists of LONGs, the following way of initialising
// the loop count ensures that, if the tree length exceeds 64 longs,
// there will be an error message from the assembler

        moveq   #PMMULENGTH/2-1,d0  // length-1
        lsr.l   #1,d0               // divide by 2 for move.l
copy_tree:
        move.l  (a0)+,(a1)+
        subq.l  #1,d0
        bpl.b   copy_tree
        PMOVE_TO_CRP(root_pointer_descriptor)   // tell system where tree is
        PMOVE_TO_TC(init_tc)        // enable PMMU
        PMOVE_TO_TTR0(init_ttr0)    // override PMMU tree for accesses to
        PMOVE_TO_TTR1(init_ttr1)    //   0x01000000-0xfeffffff (see below for details)
        rts

        SECTION_RODATA

root_pointer_descriptor:
        dc.l    0x80000002,pmmutree // lower limit 0, short format, table at 'pmmutree'
init_tc:
        dc.l    0x80F04445          // enable, 32K pages, table indexes=4/4/4/5
init_ttr0:                          // transparent translation is enabled for all FCs
        dc.l    0x017E8107          //   for 0x01000000-0x7fffffff, caching allowed
init_ttr1:                          // transparent translation is enabled for all FCs
        dc.l    0x807E8507          //   for 0x80000000-0xfeffffff, *caching inhibited*

/*
 * the following PMMU tree is copied to low memory.  its main function is to
 * inhibit caching of address ranges that contain I/O addresses.  caching is
 * inhibited for the following addresses:
 *   0x00f00000-0x00ffffff
 *   0x80000000-0xffffffff
 * most logical addresses are mapped to the same physical address; however,
 * in order to reduce the size of the PMMU tree, 0x00nnnnnn and 0xffnnnnnn
 * both map to 0x00nnnnnn.
 */
pmmustart:  // TIA table for 0x00000000-0xffffffff (16 entries)
        PMMU_SF_TABLE(tib1)         // short-format table descriptor -> tib1
        PMMU_SF_NORMAL(0x10000000)  // short-format early termination page descriptors
        PMMU_SF_NORMAL(0x20000000)  //   (NOT cache-inhibited)
        PMMU_SF_NORMAL(0x30000000)
        PMMU_SF_NORMAL(0x40000000)
        PMMU_SF_NORMAL(0x50000000)
        PMMU_SF_NORMAL(0x60000000)
        PMMU_SF_NORMAL(0x70000000)
        PMMU_SF_INHIBIT(0x80000000) // short-format early-termination page descriptors
        PMMU_SF_INHIBIT(0x90000000) //   *cache-inhibited*
        PMMU_SF_INHIBIT(0xa0000000)
        PMMU_SF_INHIBIT(0xb0000000)
        PMMU_SF_INHIBIT(0xc0000000)
        PMMU_SF_INHIBIT(0xd0000000)
        PMMU_SF_INHIBIT(0xe0000000)
        PMMU_SF_TABLE(tib2)         // short-format table descriptor -> tib2

tib1:       // TIB table for 0x00000000-0x00ffffff (16 entries)
        PMMU_SF_TABLE(tic)          // short-format table descriptor -> tic
        PMMU_SF_NORMAL(0x01000000)  // short-format early termination page descriptors
        PMMU_SF_NORMAL(0x02000000)  //   NOT cache_inhibited
        PMMU_SF_NORMAL(0x03000000)
        PMMU_SF_NORMAL(0x04000000)
        PMMU_SF_NORMAL(0x05000000)
        PMMU_SF_NORMAL(0x06000000)
        PMMU_SF_NORMAL(0x07000000)
        PMMU_SF_NORMAL(0x08000000)
        PMMU_SF_NORMAL(0x09000000)
        PMMU_SF_NORMAL(0x0a000000)
        PMMU_SF_NORMAL(0x0b000000)
        PMMU_SF_NORMAL(0x0c000000)
        PMMU_SF_NORMAL(0x0d000000)
        PMMU_SF_NORMAL(0x0e000000)
        PMMU_SF_NORMAL(0x0f000000)

tib2:       // TIB table for 0xf0000000-0xffffffff (16 entries)
        PMMU_SF_INHIBIT(0xf0000000) // short-format early termination page descriptors
        PMMU_SF_INHIBIT(0xf1000000) //   *cache-inhibited*
        PMMU_SF_INHIBIT(0xf2000000)
        PMMU_SF_INHIBIT(0xf3000000)
        PMMU_SF_INHIBIT(0xf4000000)
        PMMU_SF_INHIBIT(0xf5000000)
        PMMU_SF_INHIBIT(0xf6000000)
        PMMU_SF_INHIBIT(0xf7000000)
        PMMU_SF_INHIBIT(0xf8000000)
        PMMU_SF_INHIBIT(0xf9000000)
        PMMU_SF_INHIBIT(0xfa000000)
        PMMU_SF_INHIBIT(0xfb000000)
        PMMU_SF_INHIBIT(0xfc000000)
        PMMU_SF_INHIBIT(0xfd000000)
        PMMU_SF_INHIBIT(0xfe000000)
        PMMU_SF_TABLE(tic)          // short-format table descriptor -> tic

tic:        // TIC table for 0x00000000-0x00ffffff & 0xff000000-0xffffffff (16 entries)
            // note that both logical address ranges are mapped to the same physical
        PMMU_SF_NORMAL(0x00000000)  // short-format early termination page descriptors
        PMMU_SF_NORMAL(0x00100000)  //   NOT cache-inhibited
        PMMU_SF_NORMAL(0x00200000)
        PMMU_SF_NORMAL(0x00300000)
        PMMU_SF_NORMAL(0x00400000)
        PMMU_SF_NORMAL(0x00500000)
        PMMU_SF_NORMAL(0x00600000)
        PMMU_SF_NORMAL(0x00700000)
        PMMU_SF_NORMAL(0x00800000)
        PMMU_SF_NORMAL(0x00900000)
        PMMU_SF_NORMAL(0x00a00000)
        PMMU_SF_NORMAL(0x00b00000)
        PMMU_SF_NORMAL(0x00c00000)
        PMMU_SF_NORMAL(0x00d00000)
        PMMU_SF_NORMAL(0x00e00000)
        PMMU_SF_INHIBIT(0x00f00000) // short-format early termination page descriptor
pmmuend:                            //   *cache-inhibited*
        .end
#endif
