/*      OBDEFS.H        03/15/84 - 02/08/85     Gregg Morris            */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2011-2019 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/
#ifndef _OBDEFS_H
#define _OBDEFS_H

#define ROOT        0
#define NIL         -1

#define K_RSHIFT    0x0001      /* keybd states */
#define K_LSHIFT    0x0002
#define K_CTRL      0x0004
#define K_ALT       0x0008

#define MAX_LEN     81          /* max string length */

#define MAX_DEPTH   8           /* max depth of search or draw for objects */

#define IP_HOLLOW   0           /* inside patterns */
#define IP_1PATT    1
#define IP_2PATT    2
#define IP_3PATT    3
#define IP_4PATT    4
#define IP_5PATT    5
#define IP_6PATT    6
#define IP_SOLID    7

#define MD_REPLACE  1           /* gsx modes */
#define MD_TRANS    2
#define MD_XOR      3
#define MD_ERASE    4

#define FIS_HOLLOW  0           /* gsx styles */
#define FIS_SOLID   1
#define FIS_PATTERN 2
#define FIS_HATCH   3
#define FIS_USER    4

#define ALL_WHITE   0           /* bit blt rules */
#define S_AND_D     1
#define S_ONLY      3
#define NOTS_AND_D  4
#define S_XOR_D     6
#define S_OR_D      7
#define D_INVERT    10
#define NOTS_OR_D   13
#define ALL_BLACK   15

#define IBM         3           /* font types */
#define SMALL       5

/* Object Drawing Types */

#define G_BOX       20          /* Graphic types of obs */
#define G_TEXT      21
#define G_BOXTEXT   22
#define G_IMAGE     23
#define G_USERDEF   24
#define G_IBOX      25
#define G_BUTTON    26
#define G_BOXCHAR   27
#define G_STRING    28
#define G_FTEXT     29
#define G_FBOXTEXT  30
#define G_ICON      31
#define G_TITLE     32

#define NONE        0x0000      /* Object flags */
#define SELECTABLE  0x0001
#define DEFAULT     0x0002
#define EXIT        0x0004
#define EDITABLE    0x0008
#define RBUTTON     0x0010
#define LASTOB      0x0020
#define TOUCHEXIT   0x0040
#define HIDETREE    0x0080
#define INDIRECT    0x0100

#define NORMAL      0x0000      /* Object states */
#define SELECTED    0x0001
#define CROSSED     0x0002
#define CHECKED     0x0004
#define DISABLED    0x0008
#define OUTLINED    0x0010
#define SHADOWED    0x0020
#define WHITEBAK    0x0040
#define DRAW3D      0x0080

#define WHITE       0           /* Object colors */
#define BLACK       1
#define RED         2
#define GREEN       3
#define BLUE        4
#define CYAN        5
#define YELLOW      6
#define MAGENTA     7
#define LWHITE      8
#define LBLACK      9
#define LRED        10
#define LGREEN      11
#define LBLUE       12
#define LCYAN       13
#define LYELLOW     14
#define LMAGENTA    15

#define FILLPAT_MASK    0x00000070L /* obspec colour word masks */
#define FILLCOL_MASK    0x0000000fL

typedef struct
{
        WORD    ob_next;        /* -> object's next sibling     */
        WORD    ob_head;        /* -> head of object's children */
        WORD    ob_tail;        /* -> tail of object's children */
        UWORD   ob_type;        /* type of object- BOX, CHAR,...*/
        UWORD   ob_flags;       /* flags                        */
        UWORD   ob_state;       /* state- SELECTED, OPEN, ...   */
        LONG    ob_spec;        /* "out"- -> anything else      */
        WORD    ob_x;           /* upper left corner of object  */
        WORD    ob_y;           /* upper left corner of object  */
        WORD    ob_width;       /* width of obj                 */
        WORD    ob_height;      /* height of obj                */
} OBJECT;

typedef struct
{
        WORD    g_x;
        WORD    g_y;
        WORD    g_w;
        WORD    g_h;
} GRECT;

typedef struct _ORECT
{
        struct _ORECT *o_link;
        GRECT   o_gr;
} ORECT;

typedef struct
{
        char    *te_ptext;      /* ptr to text (must be 1st)    */
        char    *te_ptmplt;     /* ptr to template              */
        char    *te_pvalid;     /* ptr to validation chrs.      */
        WORD    te_font;        /* font                         */
        WORD    te_junk1;       /* junk word                    */
        WORD    te_just;        /* justification- left, right...*/
        WORD    te_color;       /* color information word       */
        WORD    te_junk2;       /* junk word                    */
        WORD    te_thickness;   /* border thickness             */
        WORD    te_txtlen;      /* length of text string        */
        WORD    te_tmplen;      /* length of template string    */
} TEDINFO;

typedef struct
{
        WORD    *ib_pmask;
        WORD    *ib_pdata;
        char    *ib_ptext;
        WORD    ib_char;
        WORD    ib_xchar;
        WORD    ib_ychar;
        WORD    ib_xicon;
        WORD    ib_yicon;
        WORD    ib_wicon;
        WORD    ib_hicon;
        WORD    ib_xtext;
        WORD    ib_ytext;
        WORD    ib_wtext;
        WORD    ib_htext;
} ICONBLK;

typedef struct
{
        void    *bi_pdata;      /* ptr to bit forms data        */
        WORD    bi_wb;          /* width of form in bytes       */
        WORD    bi_hl;          /* height in lines              */
        WORD    bi_x;           /* source x in bit form         */
        WORD    bi_y;           /* source y in bit form         */
        WORD    bi_color;       /* fg color of blt              */
} BITBLK;

struct _PARMBLK;

typedef struct
{
        WORD    (*ub_code)(struct _PARMBLK *parmblock);
        LONG    ub_parm;
} USERBLK;

typedef struct _PARMBLK
{
        OBJECT  *pb_tree;
        WORD    pb_obj;
        WORD    pb_prevstate;
        WORD    pb_currstate;
        WORD    pb_x, pb_y, pb_w, pb_h;
        WORD    pb_xc, pb_yc, pb_wc, pb_hc;
        LONG    pb_parm;
} PARMBLK;

#define EDSTART     0
#define EDINIT      1
#define EDCHAR      2
#define EDEND       3

#define TE_LEFT     0
#define TE_RIGHT    1
#define TE_CNTR     2

/* extract xywh values from GRECT */
static __inline__ void r_get(const GRECT *pxywh, WORD *px, WORD *py, WORD *pw, WORD *ph)
{
    *px = pxywh->g_x;
    *py = pxywh->g_y;
    *pw = pxywh->g_w;
    *ph = pxywh->g_h;
}

/* insert xywh values in GRECT */
static __inline__ void r_set(GRECT *pxywh, WORD x, WORD y, WORD w, WORD h)
{
    pxywh->g_x = x;
    pxywh->g_y = y;
    pxywh->g_w = w;
    pxywh->g_h = h;
}

#endif  /* _OBDEFS_H */
