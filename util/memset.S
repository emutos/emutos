/*
 * memset.S - a quick memset() for 68000.
 *
 * Copyright (c) 2001 by Authors:
 *
 *  LVL  Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 * This was inspired from Minix 1.5 kernel source copy68k.s.
 * Laurent Vogel, 14 february 2001.
 *
 * Altered to m68k-atari-mint-as 68k syntax 12 september 2001
 */
 
 
 
// ==== Defines ==============================================================

        .global _memset
        .global _bzero
           
       	.text
//
// void bzero(void *address, unsigned long size)
// 
_bzero:
        move.l   4(sp),a0
        move.l   8(sp),d0
        move.l   d7,-(sp)
        move.l   #0,d7
        bra      memset
//
// void *memset(void *address, short c, unsigned long size)
// fills with byte c, returns the given address.

_memset:
        move.l   4(sp),a0
        move.l   d7,-(sp)
        move.w   12(sp),d0
        move.b   d0,d7
        lsl.w    #8,d7
        move.b   d0,d7
        move.w   d7,d0
        swap     d7
        move.w   d0,d7
        move.l   14(sp),d0
        ble      end
           
// at this point, a0=block, d7=pattern, d0=size, (sp)=saved d7
// and we do not read the stack args any more.
memset:           
        move.l   a0,d1
        btst     #0,d1
        beq      even
        move.b   d7,(a0)+
        sub.l    #1,d0
even:
        move.l   #63,d1                // 
        cmp.l    d1,d0                 // 
        ble      zero4                 // count < 64
        movem.l  d2-d6/a2-a5,-(a7)     // save regs for movem use
        move.l   d7,d2
        move.l   d7,d3
        move.l   d7,d4
        move.l   d7,d5
        move.l   d7,d6
        move.l   d7,a1
        move.l   d7,a2
        move.l   d7,a3
        move.l   d7,a4
        move.l   d7,a5
        move.b   d0,d1                 // count mod 256
        lsr.l    #8,d0                 // count div 256
        bra      end256
loop256:
        lea      256(a0),a0           
        movem.l  d2-d7/a1-a5,-(a0)     // zero 11x4 bytes
        movem.l  d2-d7/a1-a5,-(a0)     // zero 11x4 bytes
        movem.l  d2-d7/a1-a5,-(a0)     // zero 11x4 bytes
        movem.l  d2-d7/a1-a5,-(a0)     // zero 11x4 bytes
        movem.l  d2-d7/a1-a5,-(a0)     // zero 11x4 bytes
        movem.l  d2-d7/a1-a3,-(a0)     // zero 9x4 bytes
        lea      256(a0),a0           
end256:
        dbra     d0,loop256            // decrement count, test and loop
        move.l   d1,d0                 // remainder becomes new count
        beq      done                  // more? no.
        and.b    #0x3F,d1              // + count mod 64
        lsr.b    #6,d0                 // + count div 64
        bra      end64
done:
        movem.l  (a7)+,d2-d6/a2-a5     // restore regs for movem use
        bra end
loop64:
        movem.l  d2-d7/a4-a5,(a0)      // zero 8x4 bytes
        movem.l  d2-d7/a4-a5,32(a0)    // zero 8x4 bytes
        lea      64(a0),a0           
end64:
        dbra     d0,loop64             // decrement count, test and loop
        movem.l  (a7)+,d2-d6/a2-a5     // restore regs for movem use
        move.l   d1,d0                 // remainder becomes new count
           
zero4:
        move.b   d0,d1                 // +
        and.b    #3,d1                 // +
        lsr.b    #2,d0                 // +
        bra      end4
loop4:
        move.l   d7,(a0)+
end4:
        dbra     d0,loop4              // decrement count, test and loop
        move.l   d1,d0                 // remainder becomes new count
        bra      end1
loop1:
        move.b   d7,(a0)+
end1:
        dbra     d0,loop1              // decrement count, test and loop
end:
        move.l   (sp)+,d7
        move.l   4(sp),d0              // return the address.
        rts


