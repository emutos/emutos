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


        
