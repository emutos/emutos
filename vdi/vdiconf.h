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


/*
 * The first, VME10, should always be equal to zero.  This causes the code
 * generated to work for the contiguous plane memory layout.  The code
 * switched out by these switches is for other memory layouts, and full
 * source code is not provided to build a driver not intended for a
 * contiguous plane layout.
 */
#define vme10           0       /* assemble for Motorola VME/10 (1=yes, 0=no) */

#define pattern         1       /* for selecting software which applies */
                                /* a pattern to the source */
/*
 * The rev_vid switch controls how the drawing algorithms treat the colors
 * white and black.  The lisa has reverse video hardware, a 0 in memory is
 * white on the screen a 1 in memory is black on the screen.  If the hardware
 * you are using is not reverse video, set this switch to zero in all *.S
 * modules.  This will cause the drawing code to emulate reverse video in
 * software by clearing bits rather than setting them to draw black.
 *
 * In order to use reverse video, the procedure _v_clrwk, which is a Lisa
 * dependent routine, must be modified to set all video memory to 1s rather
 * than clear it to 0s.
 */
#define rev_vid         1       /* for selecting reverse video transform */

/* Conditionals just for textblit */
#define test0           0       /* if give program access to internal variables */
#define test1           0       /* if using very large fonts (else, 8x16) */
#define bytswap         0       /* if font words are byte swapped! */



/* Frame parameters - just for copyfrm.S and bitblt.S */

#define FRAME_LEN   76

#define B_WD       -76  // +00 width of block in pixels
#define B_HT       -74  // +02 height of block in pixels

#define PLANE_CT   -72  // +04 number of consequitive planes to blt

#define FG_COL     -70  // +06 foreground color (logic op table index:hi bit)
#define BG_COL     -68  // +08 background color (logic op table index:lo bit)
#define OP_TAB     -66  // +10 logic ops for all fore and background combos
#define S_XMIN     -62  // +14 minimum X: source
#define S_YMIN     -60  // +16 minimum Y: source
#define S_FORM     -58  // +18 source form base address

#define S_NXWD     -54  // +22 offset to next word in line  (in bytes)
#define S_NXLN     -52  // +24 offset to next line in plane (in bytes)
#define S_NXPL     -50  // +26 offset to next plane from start of current plane

#define D_XMIN     -48  // +28 minimum X: destination
#define D_YMIN     -46  // +30 minimum Y: destination
#define D_FORM     -44  // +32 destination form base address

#define D_NXWD     -40  // +36 offset to next word in line  (in bytes)
#define D_NXLN     -38  // +38 offset to next line in plane (in bytes)
#define D_NXPL     -36  // +40 offset to next plane from start of current plane

#define P_ADDR     -34  // +42 address of pattern buffer   (0:no pattern)
#define P_NXLN     -30  // +46 offset to next line in pattern  (in bytes)
#define P_NXPL     -28  // +48 offset to next plane in pattern (in bytes)
#define P_MASK     -26  // +50 pattern index mask

/* these frame parameters are internally set */

#define P_INDX     -24  // +52 initial pattern index

#define S_ADDR     -22  // +54 initial source address
#define S_XMAX     -18  // +58 maximum X: source
#define S_YMAX     -16  // +60 maximum Y: source

#define D_ADDR     -14  // +62 initial destination address
#define D_XMAX     -10  // +66 maximum X: destination
#define D_YMAX      -8  // +68 maximum Y: destination

#define INNER_CT    -6  // +70 blt inner loop initial count
#define DST_WR      -4  // +72 destination form wrap (in bytes)
#define SRC_WR      -2  // +74 source form wrap (in bytes)



#endif                          /* _VDICONF_H */
