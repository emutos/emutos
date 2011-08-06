/*
 * bootasm.S - assembler stuff for boot.prg
 *
 * Copyright (c) 2001 by Authors:
 *
 *  LVL  Laurent Vogel
 *  
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

        .global _bootasm
        .global _getbootsize
        
        .text
/*
 * void bootasm(long dest, long src, long count);
 * will move count bytes from address src to address dest,
 * and will jump in dest.
 *
 * It needs to be done in assembler, because the copy
 * routine needs to be first copied out of the destination
 * zone in memory.
 */

_getbootsize:
        move.l  #bootend-boot, d0
        rts
        
_bootasm:
        move.w  #0x2700,sr
        move.l  4(sp),a3
        move.l  8(sp),a2
        move.l  12(sp),d2
        
        /* align the size to a multiple of 4 */
        
        btst    #0,d2
        beq     even
        add.l   #1,d2
even:
        btst    #1,d2
        beq     four
        add.l   #2,d2
four:

        /* copy the boot routine after the TOS buffer */
  
        lea     (a2,d2),a4
        move.l  a4,a1
        lea     boot,a0
        bsr     _getbootsize
        lsr.l   #2,d0
copy:
        move.l  (a0)+,(a1)+
#ifdef __mcoldfire__
        subq.l  #1,d0
        bpl.s   copy
#else
        dbra    d0,copy
#endif
        
        /* jump into boot routine */
        
        /* the instruction cache should be invalidated here */
        jmp     (a4)
        
boot:
        /* copy the TOS to destination */

        lsr.l   #2,d2
        sub.l   #1,d2
        move.l  a3,a0
bcopy:
        move.l  (a2)+,(a3)+
#ifdef __mcoldfire__
        subq.l  #1,d2
        bpl.s   bcopy
#else
        dbra    d2,bcopy
        sub.l   #0x10000,d2 
        bhi     bcopy 
#endif
        
        /* jump into it */

        /* the instruction cache should be invalidated here */
        jmp     (a0)
bootend:
        .end

