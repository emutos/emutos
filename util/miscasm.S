/*
 * miscasm.S - Some small helper functions in assembler
 *
 * Copyright (c) 2001 by following authors
 *
 * Authors:
 *  SCC     Steven C. Cavender
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



        .global _swp68w         // swap bytes in a word 
        .global _swp68l         // swap word & bytes in long

        .global _trap1          // call GEMDOS
        .global _trap13         // call BIOS
        .global _trap14         // call XBIOS
        .global _trap1_pexec    // Reentrant Pexec call


        .text



/*
 * _s68 - swap bytes in a word 
 */

_swp68w:
        link    a6,#0
        move.l  8(a6),a0
        move    (a0),d0
        ror     #8,d0
        move    d0,(a0)
        unlk    a6
        rts



/*
 * _s68 - swap word & bytes in long
 */

_swp68l:
        link    a6,#0
        move.l  8(a6),a0
        move.l  (a0),d0
        ror     #8,d0
        swap    d0
        ror     #8,d0
        move.l  d0,(a0)
        unlk    a6
        rts



/*
 * trap1 - trap 1 (GEMDOS) entry point
 */
        .text
_trap1:
        move.l  (sp)+,t1spsav   // save return address
        movem.l d2/a2,t1regsav
        trap    #1              // call bdos call
        movem.l t1regsav,d2/a2
        move.l  t1spsav,-(sp)   // restore return address
        rts

        .bss
        .even
t1spsav:
        ds.l    1       // Save space for _trap_1
t1regsav:
        ds.l    2

/*
 * _trap13 - perform trap 13 supervisor call on behalf of caller
 */

        .text
_trap13:
        move.l  (sp)+,biosav
        trap    #13
        move.l  biosav,-(sp)
        rts

        .bss
        .even
biosav:
        ds.l   1



/*
 * _trap14 - perform trap 14 supervisor call on behalf of caller
 */

        .text
_trap14:
        move.l  (sp)+,xbiosav
        trap    #14
        move.l  xbiosav,-(sp)
        rts

        .bss
        .even
xbiosav:
        .ds.l   1



/*
 *  GEMDOS Pexec() call. This version is reentrant unlike the trap1 function!
 */
        .text
_trap1_pexec:
        movem.l d2/a2,-(sp)
        move.l  22(sp),-(sp)    // Push parameter on the stack again for pexec
        move.l  22(sp),-(sp)
        move.l  22(sp),-(sp)
        move.w  24(sp),-(sp)
        move.w  #0x4b,-(sp)
        trap    #1              // Pexec
        lea     16(sp),sp
        movem.l (sp)+,d2/a2
        rts

