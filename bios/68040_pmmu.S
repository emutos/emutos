/*
 * 68040_pmmu.S - MMU initialization for 68040 (taken from set_mmu.prg).
 *
 * Copyright 2013-2019 by Odd Skancke and Miro Kropáček.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"

#if CONF_WITH_68040_PMMU

                .globl  _setup_68040_pmmu
                .extern _ramtop
                .extern _balloc_stram

                .arch   68040

//Translate control register bit meanings
tc_enable       = 15    //Translation Enable (0 = disabled)

//Cache settings
c_writetrough   = 0b00
c_copyback      = 0b01
c_precise       = 0b10
c_imprecise     = 0b11

//Descriptor bit definitions
d_cache_pos     = 5     //Position of cache setting bits in descriptors
d_writeprotect  = 2
d_super         = 7
d_u0            = 8
d_u1            = 9
d_global        = 10
d_ur0           = 11

msk_rootp_addr  = 0b11111111111111111111111000000000
msk_pnttab_addr = 0b11111111111111111111111000000000
msk_4kpt_addr   = 0b11111111111111111111111100000000
msk_4kpd_paddr  = 0b11111111111111111111000000000000

MEMORY_SIZE_ST_RAM  = 14*1024*1024
MEMORY_SIZE_TOS_ROM = 1024*1024
MEMORY_SIZE_IO1     = 1024*1024
MEMORY_SIZE_IO2     = 1024*1024

                .text

_setup_68040_pmmu:
                movem.l d2-d7/a2-a6,-(sp)

                jbsr    turn_off_all

                // find out how much TT RAM we have
                move.l  _ramtop.w,d0
                jeq     no_ttram1
                sub.l   #0x01000000,d0
                move.l  d0,tt_ram_size
no_ttram1:
                add.l   #MEMORY_SIZE_ST_RAM+MEMORY_SIZE_TOS_ROM+MEMORY_SIZE_IO1+MEMORY_SIZE_IO2,d0

                // calculate amount of memory needed for the tables and descriptors:

                // 1) Each of the 128 root-level table descriptors corresponds to
                //    a 32-Mbyte block of memory
                // 2) Each of the 128 pointer-level table descriptors corresponds to
                //    a 256-Kbyte block of memory.
                // 3) There are 64 4-Kbyte page descriptors in a page-level table.

                // page descriptors to allocate    = ⌈size / 4096⌉
                // pointer descriptors to allocate = ⌈number of page descriptors / 64⌉
                // root descriptors to allocate    = ⌈number of pointer descriptors / 128⌉

                // However, perhaps to avoid unnecessary code complexity,
                // create_pointertable() always assumes 128 pointer descriptors
                // and create_pagetable() always assumes 64 page descriptors,
                // regardless of whether they are going to be used or not.
                // Also, there are 128 root descriptors assumed in mmusetup().
                // A possible C version of this function could easily remove those
                // shortcuts but that would be beneficial only if it made sense to
                // assume RAM size was not divisible by 256 KB.

                lsr.l   #8,d0                   // memory size / 4096 (no remainder)
                lsr.l   #4,d0
                move.l  d0,d1                   // add number of page descriptors

                lsr.l   #6,d1                   // number of page descriptors / 64 (no remainder)
                add.l   d1,d0                   // add number of pointer descriptors

                add.l   #128,d0                 // add number of root descriptors

                lsl.l   #2,d0                   // each descriptor is 4 bytes

                add.l   #~msk_rootp_addr,d0     // padding

                clr.w   -(sp)                   // FALSE
                move.l  d0,-(sp)
                jsr     _balloc_stram
                addq.l  #6,sp

                add.l   #~msk_rootp_addr,d0
                and.l   #msk_rootp_addr,d0
                move.l  d0,root_table

                jbsr    mmusetup
                jcc     error_terminate

                jbsr    mmu_start
                movem.l (sp)+,d2-d7/a2-a6
                moveq   #0,d0
                rts

error_terminate:movem.l (sp)+,d2-d7/a2-a6
                moveq   #1,d0
                rts

mmu_start:      moveq   #0,d0
                movec   d0,cacr
                cpusha  bc
                nop

                moveq   #0,d0
                movec   d0,tc
                pflusha
                nop
                moveq   #0,d0
                movec   d0,itt0
                movec   d0,dtt0
                movec   d0,itt1
                movec   d0,dtt1

                move.l  root_table,d0
                movec   d0,urp
                movec   d0,srp

                // we want zeroes here
                moveq   #0,d0
                movec   d0,itt0
                movec   d0,dtt0
                movec   d0,itt1
                movec   d0,dtt1

                move.l  #(1<<31)+(1<<15),d0     // enable data and instruction caches
                movec   d0,cacr

                move.l  #(1<<tc_enable),d0

                // 4K
                movec   d0,tc

                rts

turn_off_all:   moveq   #0,d0
                movec   d0,cacr
                cpusha  bc
                nop

                moveq   #0,d0
                movec   d0,tc
                move.l  #1<<6,d0
                movec   d0,itt0
                movec   d0,itt1
                movec   d0,dtt0
                movec   d0,dtt1
                pflusha
                nop

                rts

//=======================================================

mmusetup:
                move.l  root_table,a0
                move.l  a0,d0
                add.l   #~msk_rootp_addr+1,d0
                move.l  d0,next_free
                moveq   #127,d0
.clear_root:    clr.l   (a0)+
                dbra    d0,.clear_root

                move.l  root_table,a0

                // ST RAM
                move.l  #0x00000000,d0          // logical
                move.l  #0x00000000,d1          // physical
                move.l  #MEMORY_SIZE_ST_RAM,d2  // size
                move.l  #c_writetrough<<d_cache_pos,d3  // flags
                jbsr    create_table
                jcc     .error

                // TOS ROM
                move.l  #0x00e00000,d0          // logical
                move.l  #0x00e00000,d1          // physical
                move.l  #MEMORY_SIZE_TOS_ROM,d2 // size
                move.l  #c_precise<<d_cache_pos,d3      // flags
                jbsr    create_table
                jcc     .error

                // I/O space
                move.l  #0x00f00000,d0          // logical
                move.l  #0x00f00000,d1          // physical
                move.l  #MEMORY_SIZE_IO1,d2     // size
                move.l  #c_precise<<d_cache_pos,d3      // flags
                jbsr    create_table
                jcc     .error

                // TT RAM
                move.l  tt_ram_size,d2          // size
                jeq     no_ttram2
                move.l  #0x01000000,d0          // logical
                move.l  #0x01000000,d1          // physical
                move.l  #c_copyback<<d_cache_pos,d3     // flags
                jbsr    create_table
                jcc     .error
no_ttram2:
                // I/O space (shadow)
                move.l  #0xfff00000,d0          // logical
                move.l  #0x00f00000,d1          // physical
                move.l  #MEMORY_SIZE_IO2,d2     // size
                move.l  #c_precise<<d_cache_pos,d3      // flags
                jbsr    create_table
                jcc     .error

                or.b    #1,ccr
                rts

.error:         and.b   #-2,ccr
                rts

//===============================================================================

//Input
        //D0 = Logical start address
        //D1 = Physical start address
        //D2 = Size of block (must be divisible by page size)
        //D3 = Mode of this block.

create_table:   movem.l d0-a6,-(sp)
        //Clear the bits we dont want to be set in the page-descriptors.
                and.l   #(1<<d_writeprotect)+(3<<d_cache_pos)+(1<<d_super)+(1<<d_u0)+(1<<d_u1)+(1<<d_global)+(1<<d_ur0),d3

        //Take the logical address and create offsets into root and pointer tables
        //D4 = index into root table
        //D5 = Index into pointer table
.create_more:   move.l  d0,d4
                rol.l   #7,d4
                and.l   #0b1111111,d4
                lsl.w   #2,d4
                move.l  d0,d5
                swap    d5
                and.l   #0b111111100,d5

        //Get root-table descriptor, contains base-address of the pointer table to use.
                move.l  (a0,d4.l),d6

        //Check if the descriptor is valid (or resident).
                move.b  d6,d7
                and.b   #2,d7                   //Check udp field.
                jne     .pnttab_there           //This descriptor is valid
        //The pointer table has to be created..
                jbsr    create_pointertable

        //The root descriptor is the base address of the pointer table.
        //Now get the pointer table descriptor, using the index in D5
.pnttab_there:  and.l   #msk_pnttab_addr,d6     //Mask out the pointer table base address in root descriptor
                move.l  d6,a1                   //Pointer table base address in A1
                move.l  (a1,d5.l),d6            //Get pointer table descriptor out of the pointer table

        //Is this pointer table descriptor valid? Now it means that the page table does not
        //yet exist if not valid.
                move.b  d6,d7
                and.b   #2,d7
                jne     .pagtab_there
        //Create new page table
                jbsr    create_pagetable

        //The pointer table descriptor contains the base address of the page table we need.
.pagtab_there:
        //4k pages
        //Ok, now mask out the base address of the page table in the pointer table descriptor
                and.l   #msk_4kpt_addr,d6
                move.l  d6,a2                   //A2 = Base address of page table
                move.l  d0,d6                   //We need the index into the page table,
                                                //taken from the logical address

        //Bits 12 - 17 of the logical address contains the page table index for 4K pages.
                lsl.l   #6,d6                   //Shift bit 12 to bit 18 (into high word + 2 to mul by 4)
                swap    d6                      //Now page index is at correct bit location
                and.l   #0b11111100,d6          //Mask out the index
                move.l  (a2,d6.l),d4            //Get page descriptor
                and.b   #3,d4                   //Is this descriptor already used?
                jne     .overlap_error          //Yes, we can't allow that.

        //Now we can setup this page descriptor to point to the desired physical address
        //and OR in the desired descriptor bits/modes (cache mode and such)
                move.l  d1,d4                   //D1 contains the physical address
                and.l   #msk_4kpd_paddr,d4      //Mask off excessive bits
                or.b    #1,d4                   //Make descriptor valid
                or.w    d3,d4                   //OR in the mode
                move.l  d4,(a2,d6.l)            //Store this descriptor

        //Now we add the size of the page to the logical/physical addresses
        //and subtract page size from size of block to setup in this pass.
                add.l   #1024*4,d0              //Next logical address
                add.l   #1024*4,d1              //Next physical address
                sub.l   #1024*4,d2              //Block of memory covered by this page
                jhi     .create_more            //There's more, just loop

.ok_done:       movem.l (sp)+,d0-a6
                or.b    #1,ccr
                rts

.overlap_error: and.b   #-2,ccr
                movem.l (sp)+,d0-a6
                rts

//Create a new pagetable (pointer level descriptor), clear it and return the
//address in D6 and store address in the pointer table at (a1,d5.l)
//      A0 = root table
//      A1 = pointer table
//      D4 = root table index
//      D5 = pointer table index
create_pagetable:
                move.l  next_free,d7            //Address of next free location
                move.l  d7,d6

        //Create 4k page table
.4kp:           and.l   #~msk_4kpt_addr,d6      //Already on a "4K page-table" boundary?
                jeq     .4kp_nf_ok              //Yes
                add.l   #(~msk_4kpt_addr)+1,d7  //Add size of page table..
                and.l   #msk_4kpt_addr,d7       //..then align to size of pagetable.
.4kp_nf_ok:     move.l  d7,d6                   //D6 = Base address of new page table
                add.l   #(~msk_4kpt_addr)+1,d7  //Update the next_free pointer
                move.l  d7,next_free            //Save
                move.l  d6,a2                   //A2 = Base address of new page table
                or.b    #2,d6                   //OR in %10 in the PDT field
                move.l  d6,(a1,d5.l)            //Store descriptor in ptr table (pointer level)
                moveq   #((((~msk_4kpt_addr)+1)&0xffffffff)/4)-1,d7  //Size of page table / 4
.4kp_clr_pt:    clr.l   (a2)+                   //Invalidate all descriptors in new page table
                dbra    d7,.4kp_clr_pt
                rts

        //A0 = root table base
        //D4 = root table index
        //D5 = Pointer table index

        //Return new pointertable base in D6, and store it in the root-table at (a0,d4.l)
create_pointertable:
                move.l  next_free,d7            //Get next free location
                move.l  d7,d6
                and.l   #~msk_pnttab_addr,d6    //Already on 512 byte boundary?
                jeq     .nf_ok                  //yes
        //Fix address to next whole 512 byte boundary
                add.l   #(~msk_pnttab_addr)+1,d7
                and.l   #msk_pnttab_addr,d7
.nf_ok:         move.l  d7,d6
                add.l   #(~msk_pnttab_addr)+1,d7    //Add the bytes needed (512)
                move.l  d7,next_free
                move.l  d6,a1
                or.b    #2,d6
                move.l  d6,(a0,d4.l)
                moveq   #((((~msk_pnttab_addr)+1)&0xffffffff)/4)-1,d7

.clr_pt:        clr.l   (a1)+
                dbra    d7,.clr_pt
                rts

                .bss
                .even
root_table:     .ds.l 1
next_free:      .ds.l 1
tt_ram_size:    .ds.l 1

#endif /* CONF_WITH_68040_PMMU */
