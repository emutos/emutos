/*
 * large.S
 *
 * Copyright 2002, The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "asmdefs.h"


.globl _LBWMOV

/*
 *       WORD LBWMOV(WORD *pdst, const UBYTE *psrc)
 *
 *       expand from unsigned bytes to words until you hit a null. 
 *       return the number of characters moved
 */

_LBWMOV:
            movea.l 0x08(SP),A0
            movea.l 0x04(SP),A1
            clr.l   D0
            clr.w   D1
lbwmov_loop:
            move.b  (A0)+,D1
            beq     lbwmov_exit
            move.w  D1,(A1)+
#ifdef __mcoldfire__
            addq.l  #1,D0
#else
            addq.w  #1,D0
#endif
            bra     lbwmov_loop
lbwmov_exit:
            rts
