/*
 * memmove.S - a fast memmove and bcopy in 68000.
 *
 * Copyright (c) 2001 by Authors:
 *
 *  LVL  Laurent Vogel
 *  DAL  Dale Schumacher
 *  KS   Klamer Schutte
 *  RAL  ???
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */
 
 
 
// This is taken from Minix 1.5 copy_68k.s fast copy routine.
// extracted phys_copy() and converted to modern syntax, 
// Laurent Vogel, october 2001.
//
//****************************************************************************
//
//     C O P Y _ 6 8 K . S                                       M I N I X
//
//     Basic fast copy routines used by the kernel 
//****************************************************************************
//
// Contents:
//
//   flipclicks   exchange two block of clicks
//   zeroclicks   zero a block of clicks
//   copyclicks   copy a block of clicks
//   phys_copy    copy a block of bytes
//
//============================================================================
// Edition history
//
//  #    Date                         Comments                       By
// --- -------- ---------------------------------------------------- --- 
//   1 13.06.89 fast phys_copy by Dale Schumacher                    DAL
//   2 16.06.89 bug fixes and code impromvement by Klamer Schutte    KS
//   3 12.07.89 bug fix and further code improvement to phys_copy    RAL
//   4 14.07.89 flipclicks,zeroclicks,copyclicks added               RAL
//   5 15.07.89 fast copy routine for messages added to phys_copy    RAL
//   6 03.08.89 clr.l <ea> changed to move.l #0,<ea> (= moveq )      RAL
//
//****************************************************************************



        .global _memmove
        .global _memcpy
        .global _bcopy

        .text

//
// void * bcopy(void * src, void *dst, size_t size);
// same as memmove save for the order of arguments.

_bcopy:
        move.l   4(sp),a0        // load source pointer
        move.l   8(sp),a1        // load destination pointer     
        bra      memmove
        
// 
// void * memmove(void * dst, void * src, size_t length);
// moves length bytes from src to dst, performing correctly 
// if the two regions overlap. returns dst as passed.

//
// void * memcpy(void * dst, void * src, size_t length);
// moves length bytes from src to dst. returns dst as passed.
// the behaviour is undefined if the two regions overlap.


	
_memcpy:
_memmove:
        move.l   8(sp),a0        // load source pointer
        move.l   4(sp),a1        // load destination pointer      
memmove:
        cmp.l    a1,a0
        bgt      memcopy         // if src > dst, copy will do fine.
        beq      end             // if src == dst, nothing to do.
// now, src < dst.
        move.l   a0,d0
        move.l   a1,d1
        eor.b    d1,d0
        btst     #0,d0           // pointers mutually aligned?
        bne      back1          
        move.l   12(sp),d0       // size
        beq      end             // if size == 0, nothing to do 
        add.l    d0,a0
        add.l    d0,a1
        move.l   a0,d1
        btst     #0,d1
        beq      bcheck64
        move.b   -(a0),-(a1)
        sub.l    #1,d0
        beq      end
bcheck64:        
        move.l   #63,d1                // +
        cmp.l    d1,d0                 // +
        ble      back4                 // + count < 64
        movem.l  d2-d7/a2-a6,-(sp)     // save regs for movem use
        move.b   d0,d1                 // count mod 256
        lsr.l    #8,d0                 // count div 256
        bra      bend256
bloop256:
        lea      -256(a0),a0
        lea      -256(a1),a1
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,44(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,88(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,132(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,176(a1)
        movem.l  (a0)+,d2-d7/a2-a4     // copy  9x4 bytes
        movem.l  d2-d7/a2-a4,220(a1)
        lea      -256(a0),a0
bend256:
        dbra     d0,bloop256           // decrement count, test and loop
        move.l   d1,d0                 // remainder becomes new count
        beq      done                  // more to copy? no!
        and.b    #0x3F,d1              // + count mod 64
        lsr.b    #6,d0                 // + count div 64
        bra      bend64
bloop64:
        lea      -64(a0),a0
        lea      -64(a1),a1
        movem.l  (a0)+,d2-d7/a4-a5     // copy 8x4 bytes
        movem.l  d2-d7/a4-a5,(a1)
        movem.l  (a0)+,d2-d7/a4-a5     // copy 8x4 bytes
        movem.l  d2-d7/a4-a5,32(a1)
        lea      -64(a0),a0
bend64:
        dbra     d0,bloop64            // decrement count, test and loop
        movem.l  (a7)+,d2-d7/a2-a6     // restore regs for movem use
        move.l   d1,d0                 // remainder becomes new count
back4:
        move.b   d0,d1                 // +
        and.b    #3,d1                 // +
        lsr.b    #2,d0                 // +
        bra      bend4
bloop4:
        move.l   -(a0),-(a1)
bend4:
        dbra     d0,bloop4              // decrement count, test and loop
        move.l   d1,d0                 // remainder becomes new count
        bra      bend1
bloop1:
        move.b   -(a0),-(a1)
bend1:
        dbra     d0,bloop1              // decrement count, test and loop
end:    rts

// backwards, when pointers are not aligned.
back1:
        move.l   12(sp),d0       // size
        beq      end             // if size == 0, nothing to do 
// backwards, but 16 bytes by 16 bytes forward.        
        add.l    d0,a0
        add.l    d0,a1
        move.l   #16,d1          
        cmp.l    d1,d0            
        blt      bend1
back16:
        move.b   d0,d1
        and.b    #0x0F,d1
        lsr.l    #4,d0
        bra      bend16
bloop16:
        lea      -16(a0),a0
        lea      -16(a1),a1
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        lea      -16(a0),a0
        lea      -16(a1),a1
bend16:
        dbra     d0,bloop16
        sub.l    #0x10000,d0           // count can even be bigger (>1MB)
        bhi      bloop16               // (dbra handles only word counters)
        move.l   d1,d0
        bra      bend1
        
memcopy:
        move.l   a0,d0
        move.l   a1,d1
        eor.b    d1,d0
        btst     #0,d0                 // pointers mutually aligned?
        bne      copy1                 // +
        move.l   12(sp),d0             // +
        beq      end                   // if cnt == 0 && pointers both odd ...
        btst     #0,d1                 // pointers aligned, but odd?
        beq      check64               // no
        move.b   (a0)+,(a1)+           // copy odd byte
        sub.l    #1,d0                 // decrement count
check64:
        move.l   #63,d1                // +
        cmp.l    d1,d0                 // +
        ble      copy4                 // + count < 64
        movem.l  d2-d7/a2-a6,-(a7)     // save regs for movem use
        move.b   d0,d1                 // count mod 256
        lsr.l    #8,d0                 // count div 256
        bra      end256
loop256:
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,44(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,88(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,132(a1)
        movem.l  (a0)+,d2-d7/a2-a6     // copy 11x4 bytes
        movem.l  d2-d7/a2-a6,176(a1)
        movem.l  (a0)+,d2-d7/a2-a4     // copy  9x4 bytes
        movem.l  d2-d7/a2-a4,220(a1)
        lea      256(a1),a1
end256:
        dbra     d0,loop256            // decrement count, test and loop
        move.l   d1,d0                 // remainder becomes new count
        beq      done                  // more to copy? no!
        and.b    #0x3F,d1              // + count mod 64
        lsr.b    #6,d0                 // + count div 64
        bra      end64
done:
        movem.l  (a7)+,d2-d7/a2-a6     // restore regs for movem use
        bra end

loop64:
        movem.l  (a0)+,d2-d7/a4-a5     // copy 8x4 bytes
        movem.l  d2-d7/a4-a5,(a1)
        movem.l  (a0)+,d2-d7/a4-a5     // copy 8x4 bytes
        movem.l  d2-d7/a4-a5,32(a1)
        lea      64(a1),a1
end64:
        dbra     d0,loop64             // decrement count, test and loop
        movem.l  (a7)+,d2-d7/a2-a6     // restore regs for movem use
        move.l   d1,d0                 // remainder becomes new count
copy4:
        move.b   d0,d1                 // +
        and.b    #3,d1                 // +
        lsr.b    #2,d0                 // +
        bra      end4
loop4:
        move.l   (a0)+,(a1)+
end4:
        dbra     d0,loop4              // decrement count, test and loop
        move.l   d1,d0                 // remainder becomes new count
        bra      end1
loop1:
        move.b   (a0)+,(a1)+
end1:
        dbra     d0,loop1              // decrement count, test and loop
        rts

copy1:
        move.l   12(sp),d0
                                       // count can be big; test on it !
        move.l   #16,d1                // == moveq; 4
        cmp.l    d1,d0                 // 6
        blt      end1
copy16:
        move.b   d0,d1
        and.b    #0x0F,d1
        lsr.l    #4,d0
        bra      end16
loop16:
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
        move.b   (a0)+,(a1)+
end16:
        dbra     d0,loop16
        sub.l    #0x10000,d0           // count can even be bigger (>1MB)
        bhi      loop16                // (dbra handles only word counters)
        move.l   d1,d0
        bra      end1


