/*
 * lineavars.h - name of linea graphics related variables
 *
 * Copyright (C) 2001-2020 by Authors:
 *
 * Authors:
 *  MAD   Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Put in this file only the low-mem vars actually used by C code!
 */

#ifndef LINEAVARS_H
#define LINEAVARS_H

#include "biosdefs.h"
#include "fonthdr.h"

/* Screen related variables */

/*
 * mouse cursor save area
 *
 * NOTE: the lineA version of this only has the first 64 ULONGs,
 * to handle a maximum of 4 video planes.  Writing into area[64]
 * and above when referencing the lineA version will overwrite
 * other lineA variables with unpredictable results.
 */
typedef struct _mcs {
        WORD    len;            /* height of saved form */
        UWORD   *addr;          /* screen address of saved form */
        UBYTE    stat;          /* save status */
        char    reserved;
        ULONG   area[8*16];     /* handle up to 8 video planes */
} MCS;
/* defines for 'stat' above */
#define MCS_VALID   0x01        /* save area is valid */
#define MCS_LONGS   0x02        /* saved data is in longword format */

extern MCS mouse_cursor_save;       /* in linea variable area */
extern MCS ext_mouse_cursor_save;   /* use for v_planes > 4 */

#define line_a_vars (void *)&v_planes   /* start of linea variables */
extern UWORD v_planes;          /* count of color planes */
extern UWORD v_lin_wr;          /* line wrap : bytes per line */
extern UWORD v_cel_ht;          /* cell height (width is 8) */
extern UWORD v_cel_mx;          /* number of columns - 1 */
extern UWORD v_cel_my;          /* number of rows - 1 */
extern UWORD v_cel_wr;          /* length (in bytes) of a line of characters */
extern UWORD v_cur_cx;          /* current cursor column */
extern UWORD v_cur_cy;          /* current cursor row */
extern UWORD V_REZ_HZ;          /* screen horizontal resolution */
extern UWORD V_REZ_VT;          /* screen vertical resolution */
extern UWORD BYTES_LIN;         /* width of line in bytes */

extern WORD DEV_TAB[];          /* intout array for open workstation */

extern WORD MOUSE_BT;           /* mouse button state */

/* Line-drawing related variables */
extern WORD X1, Y1, X2, Y2;     /* coordinates for end points */
extern WORD WRT_MODE;           /* write mode */
extern WORD COLBIT0, COLBIT1, COLBIT2, COLBIT3; /* colour bit values for planes 0-3 */
extern WORD CLIP;               /* clipping flag */
extern WORD XMINCL, XMAXCL, YMINCL, YMAXCL; /* clipping rectangle */
extern UWORD *PATPTR;           /* fill pattern pointer */
extern UWORD PATMSK;            /* pattern mask */

/* text-blit related variables */
extern WORD XDDA;               /* accumulator for x DDA        */
extern UWORD DDAINC;            /* the fraction to be added to the DDA */
extern WORD SCALDIR;            /* 0 if scale down, 1 if enlarge */
extern WORD MONO;               /* True if current font monospaced */
extern WORD SOURCEX, SOURCEY;   /* upper left of character in font file */
extern WORD DESTX, DESTY;       /* upper left of destination on screen  */
extern UWORD DELX, DELY;        /* width and height of character    */
extern const UWORD *FBASE;      /* pointer to font data         */
extern WORD FWIDTH;             /* offset,segment and form width of font */
extern WORD STYLE;              /* Requested text special effects */
extern WORD LITEMASK, SKEWMASK; /* special effects          */
extern WORD WEIGHT;             /* special effects          */
extern WORD ROFF, LOFF;         /* skew above and below baseline    */
extern WORD SCALE;              /* True if current font scaled */
extern WORD CHUP;               /* Text baseline vector */
extern WORD TEXTFG;             /* text foreground colour */
extern WORD *SCRTCHP;           /* Pointer to text scratch buffer */
extern WORD SCRPT2;             /* Offset to large text buffer */

/* font-specific variables */
extern const Fonthead *CUR_FONT;/* most recently used font */
extern const UWORD *v_fnt_ad;   /* address of current monospace font */
extern const UWORD *v_off_ad;   /* address of font offset table */
extern UWORD v_fnt_nd;          /* ascii code of last cell in font */
extern UWORD v_fnt_st;          /* ascii code of first cell in font */
extern UWORD v_fnt_wr;          /* font cell wrap */
extern const Fonthead *def_font;/* default font of open workstation */

/*
 * font_ring is an array of four pointers, each of which points to
 * a linked list of font headers.  usage is as follows:
 *  font_ring[0]    system fonts that are available in all resolutions;
 *                  this is currently just the 6x6 font
 *  font_ring[1]    resolution-dependent system fonts; currently
 *                  the 8x8 and 8x16 fonts
 *  font_ring[2]    fonts loaded by GDOS; initially an empty list
 *  font_ring[3]    always NULL, marking the end of the list of lists
 */
extern const Fonthead *font_ring[4];/* all available fonts */
extern WORD font_count;             /* number of different font ids in font_ring[] */

/* timer-related vectors */
extern ETV_TIMER_T tim_addr;  /* timer interrupt vector */
extern ETV_TIMER_T tim_chain; /* timer interrupt vector save */

#endif /* LINEAVARS_H */
