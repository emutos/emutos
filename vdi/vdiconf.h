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

#define rbp             0       /* assemble for Atari ST (0=yes, 1=no) */
#define vme10           0       /* assemble for Motorola VME/10 (0=yes, 1=no) */


#define pattern         1       /* for selecting software which applies */
                                /* a pattern to the source */

#define handle 2                /* leave 2 for console input, change to 1 for auxin */

#define rev_vid         1       /* for selecting reverse video transform */

/* Conditionals just for textblit */
#define test0           0       /* if give program access to internal variables */
#define test1           0       /* if using very large fonts (else, 8x16) */
#define test2           0       /* if using initialized ram (.data) else, .bss */
#define bytswap         0       /* if font words are byte swapped! */


#endif                          /* _VDICONF_H */
