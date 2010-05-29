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

        .global _trap1          // call GEMDOS
        .global _trap13         // call BIOS
        .global _trap14         // call XBIOS
        .global _trap1_pexec    // Reentrant Pexec call


/*
 * trap1 - trap 1 (GEMDOS) entry point
 */
        .text
_trap1:
        move.l  (sp)+,t1spsav   // save return address
#ifdef __mcoldfire__
        move.l  d2,t1regsav
        move.l  a2,t1regsav+4
        trap    #1              // call bdos call
        move.l  t1regsav,d2
        move.l  t1regsav+4,a2
#else
        movem.l d2/a2,t1regsav
        trap    #1              // call bdos call
        movem.l t1regsav,d2/a2
#endif
        move.l  t1spsav,-(sp)   // restore return address
        rts

        .bss
        .even
t1spsav:
        ds.l    1       // Save space for _trap_1
t1regsav:
        ds.l    2



/*
 *  GEMDOS Pexec() call. This version is reentrant unlike the trap1 function!
 */
        .text
_trap1_pexec:
#ifdef __mcoldfire__
        move.l  a2,-(sp)
        move.l  d2,-(sp)
#else
        movem.l d2/a2,-(sp)
#endif
        move.l  22(sp),-(sp)    // Push parameter on the stack again for pexec
        move.l  22(sp),-(sp)
        move.l  22(sp),-(sp)
        move.w  24(sp),-(sp)
        move.w  #0x4b,-(sp)
        trap    #1              // Pexec
        lea     16(sp),sp
#ifdef __mcoldfire__
        move.l  (sp)+,d2
        move.l  (sp)+,a2
#else
        movem.l (sp)+,d2/a2
#endif
        rts

