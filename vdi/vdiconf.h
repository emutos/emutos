/*
 * vdiconf.h - Configuration for asm part of VDI
 *
 * Copyright (c) 2001 by Authors:
 *
 * MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#ifndef _VDICONF_H
#define _VDICONF_H

/*
 * Flags for Conditional Assembly of Code
 */

#define rbp             1        // assemble for Atari ST
//#define vme10           1        // assemble for Motorola VME/10


//#define pattern         1       // for selecting software which applies
                                // a pattern to the source

#define handle 2                // leave 2 for console input, change to 1 for auxin

#ifdef vme10
#define v_pl_dspl       0x10000 // # of bytes between VME10 video planes
#define screen_siz      8189
#define v_base          0xF8000
#endif

//#define rev_vid         1       // for selecting reverse video transform

/* Conditionals just for textblit */
#define test0           1       // if give program access to internal variables
#define test1           1       // if using very large fonts (else, 8x16)
#define test2           1       // if using initialized ram (.data) else, .bss
#define bytswap         1       // if font words are byte swapped!


#endif /* _VDICONF_H */