/*
 * uncompr.S - decompress the data compressed by compr.c
 *
 * Copyright (c) 02 by the EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */

/*
 * compression format:
 * the compressed data is a series of alternatingly 
 * - verbatim data to be written in memory, and 
 * - copy-back instructions telling to copy length bytes starting
 *   at offset bytes back.
 * it starts with (potentially null) verbatim data, and ends
 * when the copy-back offset is zero.
 * the address to which copy the uncompressed data is specified
 * outside of compressed data.
 *
 * <compressed-data> ::= <verbatim> ( <copy-back> <verbatim> ) * <end>
 * <end> ::= [0]
 * <couple> ::= <verbatim> <copy-back>
 * <verbatim> ::= <verbatim-num> <byte> *   (num bytes output)
 * <verbatim-num> ::= <num>
 * <copy-back> ::= <offset> <length>   
 * <offset> ::= <non-null-num>
 * <length> ::= <num>
 * <non-null-num> ::= <positive-byte-num> | <word-num>
 * <num> ::= <byte-num> | <word-num>
 * <byte-num> ::= [0 - 127]
 * <positive-byte-num> ::= [1 - 127]
 * <word-num> ::= [128 - 255] <byte>
 * <byte> ::= [0 - 255]
 * 
 * where notation [0] means byte 0, and [a - b] means any byte 
 * between a and b inclusive. 
 */

/*
 * This implementation is really stupid. It can be greatly improved
 * by performing the copy more efficiently if the data is known to be 
 * aligned.
 */
        

        .text

/*
 * void uncompr(void * dest, void * src)
 */
        
        .global _uncompr
_uncompr:
        move.l  4(sp),a1    // dest
        move.l  8(sp),a0    // src

        moveq.l #0,d0   // upper word of d0 will be zero from now on.

loop:
/* get the verbatim length */
        bsr     getnum

/* do the verbatim copy */
        bsr     copy
        
/* get the copy-back offset */
        bsr     getnum
        tst.w   d0
        beq     done
        
        move.l  a1,d1
        sub.l   d0,d1

/* get the copy-back length */
        bsr     getnum

/* do the copy-back copy */     
        exg     d1,a0
        bsr     copy
        exg     d1,a0

/* loop again */
        bra     loop

done:
        rts

/*
 * getnum: returns in d0
 */
getnum:
        clr.w   d0
        move.b  (a0)+,d0
        bge     next
        add.b   d0,d0
        lsl.w   #7,d0
        move.b  (a0)+,d0
next:
        rts

        
/*
 * copy: copy d0 bytes from a0 to a1
 * this needs, of course, to be improved for speed.
 */

copy:
        sub.w   #1,d0
        bmi     ret
copyloop:
        move.b  (a0)+,(a1)+
        dbra    d0,copyloop
ret:
        rts
        