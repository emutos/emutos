/*
 *  charblit.c - fast blitting of 8 bit width monospaced chars
 *
 * Copyright (c) 2001 Martin Doering
 *
 * Authors:
 *  MAD     Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*===========================================================================
 *	This module contains the mono-spaced text blt for displays with a
 * consecutive or interleaved video plane memory scheme. The only difference
 * is the number of bytes between the video planes: v_pl_dspl
 *
 * The font data is stored in a structure called a 'form'. You can imagine
 * a form as all characters written in one line from left to right. Each
 * character may be 8 bits wide and 16 bits high. So 256 of them may be:
 *
 * form_width  = 256 = 8 bit wide * 256 characters
 * form_height = 16  = 16 bit deep
 *=========================================================================*/



#include "portab.h"
#include "fontdef.h"
#include "kprint.h"


/*=========================================================================*/
typedef	void	(*FP_MODE)(UBYTE bits, UBYTE *a6);	/* function pointers */

extern UBYTE *v_bas_ad;
extern struct font_head f8x16;  /* hardcoded for now */


/*==== prototypes =========================================================*/

void mode_zer(UBYTE bits, UBYTE *a6);
void mode_rep(UBYTE bits, UBYTE *a6);
void mode_set0(UBYTE bits, UBYTE *a6);
void mode_set1(UBYTE bits, UBYTE *a6);
void mode_xor(UBYTE bits, UBYTE *a6);
void mode_not0(UBYTE bits, UBYTE *a6);
void mode_not1(UBYTE bits, UBYTE *a6);

/*==== Local constants ====================================================*/

#define v_pl_dspl 0x2	/* # of bytes between video planes */
#define v_lin_wr  80    /* # of bytes in a single scan line */
#define v_planes 1     /* # of video planes */



/*==== global variables ===================================================*/

#if 0
UWORD    v_lin_wr;  /* # of bytes in a single scan line */
UWORD    v_planes;  /* # of video planes */
#endif

UWORD    wrt_mode;  /* writing mode flag */
UWORD    fgcolor;   /* foreground color for text */


FP_MODE blitmode[8]=    /* jump table for writing modes */
{   
    mode_zer,       /* replace mode 0 */
    mode_set1,      /* transparent mode 0 */
    mode_xor,       /* exclusive or mode 0 */
    mode_not1,      /* reverse transparent mode 0 */

    mode_rep,       /* replace mode 1 */
    mode_set0,      /* transparent mode 1 */
    mode_xor,       /* exclusive or mode 1 */
    mode_not0      /* reverse transparent mode 1 */
};




/*==== gfx_char ===========================================================*/
/*	This routine performs text blt's on mono-spaced, 8-bit
 *	wide fonts.  They can be arbitrarily tall.  This blt
 * works with character coordinates only
 *
 *	Inputs:
 *	   charcode - the character to blit
 *	   wrt_mode  - writing mode
 *    destx     - character row
 *    desty     - character column
 *
 *	   fgcolor - text foreground color
 *	   v_bas_ad - base address of first video page
 *	   v_lin_wr - # of bytes in a single scan line
 *    v_planes - # of video planes
 */

void charblit(BYTE charcode, UWORD destx, UWORD desty)
{

    UBYTE *a2;       /* starting address within current plane */

    UBYTE *screenptr;         /* pointer to actual byte on screen */
    UBYTE *fontline;          /* font definition pointer to row */
    UBYTE *bits;              /* bits to place on screen */

    struct font_head *fnt_ptr;          /* Pointer to font def */
    UWORD form_h;
    UWORD form_w;
    
    ULONG lines;              /* lines counter */
    ULONG linebytes;          /* bytes / line */
    UWORD numplanes;          /* number of bitplanes to draw to */
    UWORD fgbits;             /* foreground color for text */

//    int i,j;                    /* for loop */

    /* set up values for character blitting routine */

    fgbits=fgcolor;     /* get bit plane flags for text fgnd color */
    linebytes=v_lin_wr; /* bytes in a scan line - offset to next scan */

       
    fnt_ptr=&f8x16;           /* pointer to font f8x16 */
    form_h=fnt_ptr->form_height;
    form_w=fnt_ptr->form_width;

    /* starting address within screen (charwidth = 8 bits) */
    a2=v_bas_ad+destx;

    a2+=(linebytes*form_h*desty); /*errornous ?!?  use loops instead */

    wrt_mode=4; /* for now just black on white */
    /*
     * Loop drawing all scans of every character to each video plane.
     * Each pass through the loop, set up the screen address and scan
     * loop counter and call the appropriate service routine.
     */

    for (numplanes=v_planes; numplanes>0; numplanes--)
    {
        screenptr=a2;          /* get starting address for this plane */
        fontline=(UBYTE*)fnt_ptr->dat_table;    /* get base address of font definition */


//        if (fgbits & 1)        /* if actual foreground color bit set */
//            wrt_mode+=4;        /* write it with 1's - use 2nd half of jump table */
//        fgbits=fgbits>>1;       /* get next foreground bit plane flag */


        /* loop until all scans drawn */
        for (lines=form_h; lines>0; lines--)
        {
            bits=(UBYTE*)fontline+charcode;        /* get scan byte from table */

            blitmode[wrt_mode](*bits, screenptr);       /* execute wrinting mode */
            screenptr+=linebytes;         /* bump screen pointer to next scan line */
            fontline+=form_w;       /* bump font definition pointer to next row */
        }

        a2+=v_pl_dspl;   /* get starting address in next plane */
    }
}



/*===========================================================================
 *
 *	Replace Mode Text BLT -- Foreground Flag = 0
 *
 *	This routine performs a mono-spaced text blt in replace
 *	mode for the case when the current plane should be written
 *	with 0's to acheive the current foreground color.  This is
 *	accomplished by clearing each target byte.
 */

void mode_zer(UBYTE bits, UBYTE *a6)
{
    *a6=0;      /* clear byte for current character */
}



/*===========================================================================
 *
 *	Replace Mode Text BLT -- Foreground Flag = 1
 *
 *	This routine performs a mono-spaced text blt in replace
 *	mode for the case when the current plane should be written
 *	with 1's to acheive the current foreground color.
 */

void mode_rep(UBYTE bits, UBYTE *a6)
{
    *a6=~bits;      /* invert scan byte to get reverse video */
}



/*===========================================================================
 *
 * 	Transparent Mode Text BLT -- Foreground Flag = 0
 *
 *		This routine performs a mono-spaced text blt in transparent
 *		mode for the case when the current plane should be written
 *		with 0's to acheive the current foreground color.
 *
 */

void mode_set0(UBYTE bits, UBYTE *a6)
{
    *a6&=~bits;     /* clear all the bits that get foreground color */
}


/*===========================================================================
 *
 * 	Transparent Mode Text BLT -- Foreground Flag = 1
 *
 *		This routine performs a mono-spaced text blt in transparent
 *		mode for the case when the current plane should be written
 *		with 1's to acheive the current foreground color.
 *
 */

void mode_set1(UBYTE bits, UBYTE *a6)
{
    *a6|=bits;      /* invert scan byte to get reverse video */
}



/*===========================================================================
 *
 * 	Reverse Transparent Mode Text BLT -- Foreground Flag = 0
 *
 *		This routine performs a mono-spaced text blt in reverse
 *		transparent mode for the case when the current plane should
 *		be written with 0's to acheive the current foreground color.
 *		In this case, the bits that would normally be given the
 *	background color are instead given the foreground color.
 */

void mode_not0(UBYTE bits, UBYTE *a6)
{
    *a6&=bits;     /* give the background bits the foreground color */
}



/*= ===========================================================================
 *
 * 	Reverse Transparent Mode Text BLT -- Foreground Flag = 1
 *
 *		This routine performs a mono-spaced text blt in reverse
 *		transparent mode for the case when the current plane should
 *		be written with 1's to acheive the current foreground color.
 *		In this case, the bits that would normally be given the
 *	background color are instead given the foreground color.
 */

void mode_not1(UBYTE bits, UBYTE *a6)
{
    *a6|=~bits;     /* give the background bits the foreground color */
}


/*===========================================================================
 *
 *  Exclusive Or Mode Text BLT -- Foreground Flag = 0 or 1
 *
 *	This routine performs a mono-spaced text blt in exclusive
 *	or mode.  The foreground color is not used in this opera-
 *	tion.  The data from the text font definition is xor'ed
 *	against the contents of each of the video planes.
 */
void mode_xor(UBYTE bits, UBYTE *a6)
{
    *a6^=bits;     /* invert all foreground bits in the plane */
}



