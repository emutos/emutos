/*
 * text.c - uses text_blt to move data from a font table to screen
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "vdi_defs.h"


extern Fonthead *def_font;      /* Default font of open workstation */
extern Fonthead *font_ring[];   /* Ring of available fonts */


/* linea-variables used for text_blt in assembler */
extern WORD CLIP, XMN_CLIP, XMX_CLIP, YMN_CLIP, YMX_CLIP;
extern WORD DDA_INC;            /* the fraction to be added to the DDA */
extern WORD T_SCLSTS;           /* 0 if scale down, 1 if enlarge */
extern WORD MONO_STATUS;        /* True if current font monospaced */
extern WORD STYLE;              /* Requested text special effects */
extern WORD DOUBLE;             /* True if current font scaled */
extern WORD CHUP;               /* Text baseline vector */
extern WORD WRT_MODE;

extern WORD XACC_DDA;           /* accumulator for x DDA        */
extern WORD SOURCEX, SOURCEY;   /* upper left of character in font file */
extern WORD DESTX, DESTY;       /* upper left of destination on screen  */
extern WORD DELX, DELY;         /* width and height of character    */
extern WORD *FBASE;             /* pointer to font data         */
extern WORD FWIDTH;             /* offset,segment and form with of font */
extern WORD LITEMASK, SKEWMASK; /* special effects          */
extern WORD WEIGHT;             /* special effects          */
extern WORD R_OFF, L_OFF;       /* skew above and below baseline    */
extern WORD TEXT_FG;


/* font-header definitions */

/* fh_flags   */
#define F_DEFAULT 1             /* this is the default font (face and size) */
#define F_HORZ_OFF  2           /* there are left and right offset tables */
#define F_STDFORM  4            /* is the font in standard format */
#define F_MONOSPACE 8           /* is the font monospaced */

/* style bits */
#define F_THICKEN 1
#define F_LIGHT 2
#define F_SKEW  4
#define F_UNDER 8
#define F_OUTLINE 16
#define F_SHADOW        32

extern WORD font_count;         /* Number of fonts in driver */
extern WORD deftxbuf[];         /* Default text scratch buffer */
extern WORD scrtsiz;            /* Default offset to large text buffer */
extern WORD scrpt2;             /* Offset to large text buffer */
extern WORD *scrtchp;           /* Pointer to text scratch buffer */

extern Fonthead fon6x6;         /* See bios/fntxxx.c */
extern Fonthead fon8x8;         /* See bios/fntxxx.c */
extern Fonthead fon8x16;        /* See bios/fntxxx.c */

/* Global variables */
WORD width, height;      /* extent of string set in dqt_extent   */
WORD wordx, wordy;       /* add this to each space for interword */
WORD rmword;             /* the number of pixels left over   */
WORD rmwordx, rmwordy;   /* add this to use up remainder     */
WORD charx, chary;       /* add this to each char for interchar  */
WORD rmchar;             /* number of pixels left over       */
WORD rmcharx, rmchary;   /* add this to use up remainder     */


/* Prototypes for this module */
void make_header(Vwk * vwk);
WORD clc_dda(Vwk * vwk, WORD act, WORD req);



void d_gtext(Vwk * vwk)
{
    WORD count;
    WORD i, j;
    WORD startx=0, starty=0;
    WORD xfact=0, yfact=0;
    WORD tx1, tx2, ty1, ty2;
    WORD delh=0, delv=0;
    WORD d1, d2;
    WORD extent[8];
    WORD *old_ptr;
    WORD justified;

    WORD temp;
    Fonthead *fnt_ptr = NULL;
    Point * point = NULL;

    /* some data copying for the assembler part */
    DDA_INC = vwk->dda_inc;
    T_SCLSTS = vwk->t_sclsts;
    DOUBLE = vwk->scaled;;
    MONO_STATUS = F_MONOSPACE & vwk->cur_font->flags;
    WRT_MODE = vwk->wrt_mode;

    CLIP = vwk->clip;
    XMN_CLIP = vwk->xmn_clip;
    YMN_CLIP = vwk->ymn_clip;
    XMX_CLIP = vwk->xmx_clip;
    YMX_CLIP = vwk->ymx_clip;
    STYLE = vwk->style;
    CHUP = vwk->chup;
    scrpt2 = vwk->scrpt2;
    scrtchp = vwk->scrtchp;

    count = CONTRL[3];
    if (count > 0) {

        fnt_ptr = vwk->cur_font;     /* Get current font pointer in register */

        justified = (CONTRL[0] == 11);

        if (vwk->style & F_THICKEN)
            WEIGHT = fnt_ptr->thicken;

        if (vwk->style & F_LIGHT)
            LITEMASK = fnt_ptr->lighten;

        if (vwk->style & F_SKEW) {
            L_OFF = fnt_ptr->left_offset;
            R_OFF = fnt_ptr->right_offset;
            SKEWMASK = fnt_ptr->skew;
        } else {
            L_OFF = 0;
            R_OFF = 0;
        }

        FBASE = fnt_ptr->dat_table;
        FWIDTH = fnt_ptr->form_width;

        switch (vwk->h_align) {
        case 0:
            delh = 0;
            break;
        case 1:
            if (!justified) {   /* width set if GDP */
                old_ptr = PTSOUT;
                PTSOUT = extent;
                dqt_extent(vwk);
                PTSOUT = old_ptr;
                CONTRL[2] = 0;
            }
            delh = width / 2;
            break;
        case 2:
            if (!justified) {   /* width set if GDP */
                old_ptr = PTSOUT;
                PTSOUT = extent;
                dqt_extent(vwk);
                PTSOUT = old_ptr;
                CONTRL[2] = 0;
            }
            delh = width;
            break;
        }

        if (vwk->style & F_SKEW) {
            d1 = fnt_ptr->left_offset;
            d2 = fnt_ptr->right_offset;
        } else {
            d1 = 0;
            d2 = 0;
        }

        switch (vwk->v_align) {
        case 0:
            delv = fnt_ptr->top;
            delh += d1;
            break;
        case 1:
            delv = fnt_ptr->top - fnt_ptr->half;
            delh += (fnt_ptr->half * d2) / fnt_ptr->top;
            break;
        case 2:
            delv = fnt_ptr->top - fnt_ptr->ascent;
            delh += (fnt_ptr->ascent * d2) / fnt_ptr->top;
            break;
        case 3:
            delv = fnt_ptr->top + fnt_ptr->bottom;
            break;
        case 4:
            delv = fnt_ptr->top + fnt_ptr->descent;
            delh += (fnt_ptr->descent * d1) / fnt_ptr->bottom;
            break;
        case 5:
            delv = 0;
            delh += d1 + d2;
            break;
        }

        point = (Point*)PTSIN;
        switch (vwk->chup) {
        case 0:
            DESTX = point->x - delh;
            DESTY = point->y - delv;
            startx = DESTX;
            starty = DESTY + fnt_ptr->top + fnt_ptr->ul_size + 1;
            xfact = 0;
            yfact = 1;
            break;
        case 900:
            DESTX = point->x - delv;
            DESTY = point->y + delh;
            startx = DESTX + fnt_ptr->top + fnt_ptr->ul_size + 1;
            starty = DESTY;
            xfact = 1;
            yfact = 0;
            break;
        case 1800:
            DESTX = point->x + delh;
            DESTY = point->y - ((fnt_ptr->top + fnt_ptr->bottom) - delv);
            startx = DESTX;
            starty = (DESTY + fnt_ptr->bottom) - (fnt_ptr->ul_size + 1);
            xfact = 0;
            yfact = -1;
            break;
        case 2700:
            DESTX = point->x - ((fnt_ptr->top + fnt_ptr->bottom) - delv);
            DESTY = point->y - delh;
            starty = DESTY;
            startx = (DESTX + fnt_ptr->bottom) - (fnt_ptr->ul_size + 1);
            xfact = -1;
            yfact = 0;
            break;
        }

        TEXT_FG = vwk->text_color;
        DELY = fnt_ptr->form_height;
        XACC_DDA = 32767;   /* init the horizontal dda */

        for (j = 0; j < count; j++) {

            temp = INTIN[j];

            /* If the character is out of range for this font make it a ? */
            if ((temp < fnt_ptr->first_ade) || (temp > fnt_ptr->last_ade))
                temp = 63;
            temp -= fnt_ptr->first_ade;

            SOURCEX = fnt_ptr->off_table[temp];
            DELX = fnt_ptr->off_table[temp + 1] - SOURCEX;

            SOURCEY = 0;
            DELY = fnt_ptr->form_height;

            text_blt();

            fnt_ptr = vwk->cur_font;     /* restore reg var */

            if (justified) {
                DESTX += charx;
                DESTY += chary;
                if (rmchar) {
                    DESTX += rmcharx;
                    DESTY += rmchary;
                    rmchar--;
                }
                if (INTIN[j] == 32) {
                    DESTX += wordx;
                    DESTY += wordy;
                    if (rmword) {
                        DESTX += rmwordx;
                        DESTY += rmwordy;
                        rmword--;
                    }
                }
            }
            /* end if justified */
            if (fnt_ptr->flags & F_HORZ_OFF)
                DESTX += fnt_ptr->hor_table[temp];

        }                   /* for j */

        if (vwk->style & F_UNDER) {
            Line * line = (Line*)PTSIN;
            line->x1 = startx;
            line->y1 = starty;

            if (vwk->chup % 1800 == 0) {
                line->x2 = DESTX;
                line->y2 = line->y1;
            } else {
                line->x2 = line->x1;
                line->y2 = DESTY;
            }
            if (vwk->style & F_LIGHT)
                LN_MASK = vwk->cur_font->lighten;
            else
                LN_MASK = 0xffff;

            count = vwk->cur_font->ul_size;
            for (i = 0; i < count; i++) {
                if (vwk->clip) {
                    tx1 = line->x1;
                    tx2 = line->x2;
                    ty1 = line->y1;
                    ty2 = line->y2;

                    if (clip_line(vwk, line))
                        abline(vwk, line);

                    line->x1 = tx1;
                    line->x2 = tx2;
                    line->y1 = ty1;
                    line->y2 = ty2;
                } else
                    abline(vwk, line);

                line->x1 += xfact;
                line->x2 += xfact;
                line->y1 += yfact;
                line->y2 += yfact;

                if (LN_MASK & 1)
                    LN_MASK = (LN_MASK >> 1) | 0x8000;
                else
                    LN_MASK = LN_MASK >> 1;
            }
        } 
    }
}



/*
 * trnsfont - converts a font to standard form
 *
 * The routine does just byte swapping.
 *
 * input:
 *     FWIDTH = width of font data in bytes.
 *     DELY   = number of scan lines in font.
 *     FBASE  = starting address of the font data.
 */

void trnsfont()
{
    WORD cnt, i;
    UWORD *addr;

    addr = FBASE;
    cnt = (FWIDTH * DELY) << 1;

    //for (i = 1; i < cnt; i++) {
    for (i = cnt - 1; i >= 0; i--) {             /* dbra optimized loop */
        *addr=((*addr) << 8 | (*addr) >> 8);    /* swap bytes */
    }
}



void text_init2(Vwk * vwk)
{
    vwk->cur_font = def_font;
    vwk->loaded_fonts = NULLPTR;
    vwk->scrpt2 = scrtsiz;
    vwk->scrtchp = deftxbuf;
    vwk->num_fonts = font_count;

    vwk->style = 0;        /* reset special effects */
    vwk->scaled = FALSE;
    vwk->h_align = 0;
    vwk->v_align = 0;
    vwk->chup = 0;
    vwk->pts_mode = FALSE;

    font_ring[2] = vwk->loaded_fonts;
    DEV_TAB[10] = vwk->num_fonts;
}

void text_init(Vwk * vwk)
{
    WORD i, j;
    WORD id_save;
    Fonthead *fnt_ptr, **chain_ptr;

    SIZ_TAB[0] = 32767;         // minimal char width
    SIZ_TAB[1] = 32767;         // minimal char height
    SIZ_TAB[2] = 0;             // maximal char width
    SIZ_TAB[3] = 0;             // maximal char heigh

    /* Initialize the font ring. */
    font_ring[0] = &fon6x6;
    font_ring[1] = &fon8x8;
    font_ring[2] = NULLPTR;
    font_ring[3] = NULLPTR;

    id_save = fon6x6.font_id;

    chain_ptr = font_ring;
    i = 0;
    j = 0;
    while ((fnt_ptr = *chain_ptr++)) {
        do {
            if (fnt_ptr->flags & F_DEFAULT)     /* If default save pointer */
                def_font = fnt_ptr;

            if (fnt_ptr->font_id != id_save) {  /* If new font count */
                j++;
                id_save = fnt_ptr->font_id;
            }

            if (fnt_ptr->font_id == 1) {        /* Update SIZ_TAB if system font */
                if (SIZ_TAB[0] > fnt_ptr->max_char_width)
                    SIZ_TAB[0] = fnt_ptr->max_char_width;

                if (SIZ_TAB[1] > fnt_ptr->top)
                    SIZ_TAB[1] = fnt_ptr->top;

                if (SIZ_TAB[2] < fnt_ptr->max_char_width)
                    SIZ_TAB[2] = fnt_ptr->max_char_width;

                if (SIZ_TAB[3] < fnt_ptr->top)
                    SIZ_TAB[3] = fnt_ptr->top;
                i++;            /* Increment count of heights */
            }
            /* end if system font */
            if (!(fnt_ptr->flags & F_STDFORM)) {
                FBASE = fnt_ptr->dat_table;
                FWIDTH = fnt_ptr->form_width;
                DELY = fnt_ptr->form_height;
                trnsfont();
            }

        } while ((fnt_ptr = fnt_ptr->next_font));
    }
    DEV_TAB[5] = i;                     /* number of sizes */
    font_count = DEV_TAB[10] = ++j;     /* number of faces */
}

void dst_height(Vwk * vwk)
{
    Fonthead **chain_ptr;
    Fonthead *test_font, *single_font;
    WORD *pointer, font_id, test_height;
    BYTE found;

    font_id = vwk->cur_font->font_id;
    vwk->pts_mode = FALSE;

    /* Find the smallest font in the requested face */
    chain_ptr = font_ring;

    found = 0;
    while (!found && (test_font = *chain_ptr++)) {
        do {
            found = (test_font->font_id == font_id);
        } while (!found && (test_font = test_font->next_font));
    }

    single_font = test_font;
    test_height = PTSIN[1];
    if (vwk->xfm_mode == 0)     /* If NDC transformation, swap y coordinate */
        test_height = DEV_TAB[1] + 1 - test_height;

    /* Traverse the chains and find the font closest to the size requested. */
    do {
        while ((test_font->top <= test_height)
               && (test_font->font_id == font_id)) {
            single_font = test_font;
            if (!(test_font = test_font->next_font))
                break;
        }
    } while ((test_font = *chain_ptr++));

    /* Set up environment for this font in the non-scaled case */
    vwk->cur_font = single_font;
    vwk->scaled = FALSE;

    if (single_font->top != test_height) {
        vwk->dda_inc = clc_dda(vwk, single_font->top, test_height);
        make_header(vwk);
        single_font = vwk->cur_font;
    }

    CONTRL[2] = 2;

    pointer = PTSOUT;
    *pointer++ = single_font->max_char_width;
    *pointer++ = test_height = single_font->top;
    *pointer++ = single_font->max_cell_width;
    *pointer++ = test_height + single_font->bottom + 1;
    flip_y = 1;
}



/*
 * act_siz - Actual sizer routine
 *
 * entry:
 *   top     - size to scale (DELY)
 *
 * used variables:
 *   DDA_INC - (WORD) DDA increment passed externally
 *   T_SCLST - (WORD) 0 if scale down, 1 if enlarge
 *
 * exit:
 *   actual size
 */
WORD act_siz(Vwk * vwk, WORD top)
{
    UWORD size;
    UWORD accu;
    UWORD retval;
    UWORD i;

    if (vwk->dda_inc == 0xffff) {
        /* double size */
        return ((WORD)(top<<1));
    }
    size = (UWORD)(top - 1);
    accu = 0x7fff;
    retval = 0;

    if (vwk->t_sclsts) {
        /* enlarge */
        for (i = size; i >= 0; --i) {
            accu += vwk->dda_inc;
            if (accu < vwk->dda_inc) {
                // Not sz_sm_1 stuff here
                retval++;
            }
            retval++;
        }
    } else {
        /* scale down */
        for (i = size; i >= 0; --i) {
            accu += vwk->dda_inc;
            if (accu < vwk->dda_inc) {
                // Not sz_sm_1 stuff here
                retval++;
            }
        }
        /* Make return value at least 1 */
        if (!retval)
            retval = 1;
    }
    return ((WORD)retval);
}



void copy_name(BYTE * source, BYTE * dest)
{
    WORD i;
    BYTE *sptr, *dptr;

    sptr = source;
    dptr = dest;

    for (i = 0; i < 32; i++)
        *dptr++ = *sptr++;
}


void make_header(Vwk * vwk)
{
    Fonthead *source_font, *dest_font;

    source_font = vwk->cur_font;
    dest_font = &vwk->scratch_head;

    dest_font->font_id = source_font->font_id;
    dest_font->point = source_font->point * 2;

    copy_name(&source_font->name[0], &dest_font->name[0]);

    dest_font->first_ade = source_font->first_ade;
    dest_font->last_ade = source_font->last_ade;

    if (vwk->dda_inc == 0xFFFF) {
        dest_font->top = source_font->top * 2 + 1;
        dest_font->ascent = source_font->ascent * 2 + 1;
        dest_font->half = source_font->half * 2 + 1;
        dest_font->descent = source_font->descent * 2;
        dest_font->bottom = source_font->bottom * 2;
        dest_font->max_char_width = source_font->max_char_width * 2;
        dest_font->max_cell_width = source_font->max_cell_width * 2;
        dest_font->left_offset = source_font->left_offset * 2;
        dest_font->right_offset = source_font->right_offset * 2;
        dest_font->thicken = source_font->thicken * 2;
        dest_font->ul_size = source_font->ul_size * 2;
    } else {
        dest_font->top = act_siz(vwk, source_font->top);
        dest_font->ascent = act_siz(vwk, source_font->ascent);
        dest_font->half = act_siz(vwk, source_font->half);
        dest_font->descent = act_siz(vwk, source_font->descent);
        dest_font->bottom = act_siz(vwk, source_font->bottom);
        dest_font->max_char_width = act_siz(vwk, source_font->max_char_width);
        dest_font->max_cell_width = act_siz(vwk, source_font->max_cell_width);
        dest_font->left_offset = act_siz(vwk, source_font->left_offset);
        dest_font->right_offset = act_siz(vwk, source_font->right_offset);
        dest_font->thicken = act_siz(vwk, source_font->thicken);
        dest_font->ul_size = act_siz(vwk, source_font->ul_size);
    }

    dest_font->lighten = source_font->lighten;
    dest_font->skew = source_font->skew;
    dest_font->flags = source_font->flags;

    dest_font->hor_table = source_font->hor_table;
    dest_font->off_table = source_font->off_table;
    dest_font->dat_table = source_font->dat_table;

    dest_font->form_width = source_font->form_width;
    dest_font->form_height = source_font->form_height;

    vwk->scaled = TRUE;
    vwk->cur_font = dest_font;
}


void dst_point(Vwk * vwk)
{
    WORD font_id;
    Fonthead **chain_ptr, *double_font;
    Fonthead *test_font, *single_font;
    WORD *pointer, test_height, height;
    BYTE found;

    font_id = vwk->cur_font->font_id;
    vwk->pts_mode = TRUE;

    /* Find the smallest font in the requested face */
    chain_ptr = font_ring;
    found = 0;
    while (!found && (test_font = *chain_ptr++)) {
        do {
            found = (test_font->font_id == font_id);
        } while (!found && (test_font = test_font->next_font));
    }

    double_font = single_font = test_font;
    test_height = INTIN[0];

    /* Traverse the chains and find the font closest to the size requested */
    /* and closest to half the size requested.                 */
    do {
        while (((height = test_font->point) <= test_height)
               && (test_font->font_id == font_id)) {
            single_font = test_font;
            if (height * 2 <= test_height)
                double_font = test_font;

            if (!(test_font = test_font->next_font))
                break;
        }
    } while ((test_font = *chain_ptr++));

    /* Set up environment for this font in the non-scaled case */
    vwk->cur_font = single_font;
    vwk->scaled = FALSE;

    if (single_font->point != test_height) {
        height = double_font->point * 2;

        if ((height > single_font->point) && (height <= test_height)) {
            vwk->dda_inc = 0xFFFF;
            vwk->t_sclsts = 1;
            vwk->cur_font = double_font;
            make_header(vwk);
            single_font = vwk->cur_font;
        }
    }

    CONTRL[4] = 1;
    CONTRL[2] = 2;

    INTOUT[0] = single_font->point;

    pointer = PTSOUT;
    *pointer++ = single_font->max_char_width;
    *pointer++ = test_height = single_font->top;
    *pointer++ = single_font->max_cell_width;
    *pointer++ = test_height + single_font->bottom + 1;
    flip_y = 1;
}


void dst_style(Vwk * vwk)
{
    INTOUT[0] = vwk->style = INTIN[0] & INQ_TAB[2];
    CONTRL[4] = 1;
}


void dst_alignment(Vwk * vwk)
{
    WORD a, *int_out, *int_in;

    int_in = INTIN;
    int_out = INTOUT;
    a = *int_in++;
    if (a < 0 || a > 2)
        a = 0;
    vwk->h_align = *int_out++ = a;

    a = *int_in;
    if (a < 0 || a > 5)
        a = 0;
    vwk->v_align = *int_out = a;

    CONTRL[4] = 2;
}


void dst_rotation(Vwk * vwk)
{
    INTOUT[0] = vwk->chup = ((INTIN[0] + 450) / 900) * 900;
    CONTRL[4] = 1;
}


void dst_font(Vwk * vwk)
{
    WORD *old_intin, point, *old_ptsout, dummy[4], *old_ptsin;
    WORD face;
    Fonthead *test_font, **chain_ptr;
    BYTE found;

    test_font = vwk->cur_font;
    point = test_font->point;
    dummy[1] = test_font->top;
    face = INTIN[0];

    chain_ptr = font_ring;

    found = 0;
    while (!found && (test_font = *chain_ptr++)) {
        do {
            found = (test_font->font_id == face);
        } while (!found && (test_font = test_font->next_font));
    }

    /* If we fell through the loop, we could not find the face. */
    /* Default to the system font.                  */
    if (!found)
        test_font = &fon6x6;

    /* Call down to the set text height routine to get the proper size */
    vwk->cur_font = test_font;

    old_intin = INTIN;
    old_ptsin = PTSIN;
    old_ptsout = PTSOUT;
    INTIN = &point;
    PTSIN = PTSOUT = dummy;

    if (vwk->pts_mode)
        dst_point(vwk);
    else
        dst_height(vwk);

    INTIN = old_intin;
    PTSIN = old_ptsin;
    PTSOUT = old_ptsout;

    CONTRL[2] = 0;
    CONTRL[4] = 1;
    INTOUT[0] = vwk->cur_font->font_id;
}


void dst_color(Vwk * vwk)
{
    WORD r;

    r = INTIN[0];
    if ((r >= DEV_TAB[13]) || (r < 0))
        r = 1;
    CONTRL[4] = 1;
    INTOUT[0] = r;
    vwk->text_color = MAP_COL[r];
}


void dqt_attributes(Vwk * vwk)
{
    WORD *pointer, temp;
    Fonthead *fnt_ptr;

    pointer = INTOUT;
    fnt_ptr = vwk->cur_font;

    *pointer++ = fnt_ptr->font_id;      /* INTOUT[0] */
    *pointer++ = REV_MAP_COL[vwk->text_color];     /* INTOUT[1] */
    *pointer++ = vwk->chup;        /* INTOUT[2] */
    *pointer++ = vwk->h_align;     /* INTOUT[3] */
    *pointer++ = vwk->v_align;     /* INTOUT[4] */
    *pointer = vwk->wrt_mode;      /* INTOUT[5] */

    pointer = PTSOUT;
    *pointer++ = fnt_ptr->max_char_width;
    *pointer++ = temp = fnt_ptr->top;
    *pointer++ = fnt_ptr->max_cell_width;
    *pointer = temp + fnt_ptr->bottom + 1;

    CONTRL[2] = 2;
    CONTRL[4] = 6;
    flip_y = 1;
}


void dqt_extent(Vwk * vwk)
{
    WORD i, chr, table_start;
    WORD *pointer;
    Fonthead *fnt_ptr;

    WORD cnt;

    fnt_ptr = vwk->cur_font;
    pointer = INTIN;

    width = 0;
    table_start = fnt_ptr->first_ade;
    cnt = CONTRL[3];

    for (i = 0; i < cnt; i++) {
        chr = *pointer++ - table_start;
        width += fnt_ptr->off_table[chr + 1] - fnt_ptr->off_table[chr];
    }

    if (vwk->scaled) {
        if (vwk->dda_inc == 0xFFFF)
            width *= 2;
        else
            width = act_siz(vwk, width);
    }

    if ((vwk->style & F_THICKEN) && !(fnt_ptr->flags & F_MONOSPACE))
        width += cnt * fnt_ptr->thicken;

    if (vwk->style & F_SKEW)
        width += fnt_ptr->left_offset + fnt_ptr->right_offset;

    height = fnt_ptr->top + fnt_ptr->bottom + 1;

    CONTRL[2] = 4;

    pointer = PTSOUT;
    switch (vwk->chup) {
    case 0:
        *pointer++ = 0;
        *pointer++ = 0;
        *pointer++ = width;
        *pointer++ = 0;
        *pointer++ = width;
        *pointer++ = height;
        *pointer++ = 0;
        *pointer = height;
        break;
    case 900:
        *pointer++ = height;
        *pointer++ = 0;
        *pointer++ = height;
        *pointer++ = width;
        *pointer++ = 0;
        *pointer++ = width;
        *pointer++ = 0;
        *pointer = 0;
        break;
    case 1800:
        *pointer++ = width;
        *pointer++ = height;
        *pointer++ = 0;
        *pointer++ = height;
        *pointer++ = 0;
        *pointer++ = 0;
        *pointer++ = width;
        *pointer = 0;
        break;
    case 2700:
        *pointer++ = 0;
        *pointer++ = height;
        *pointer++ = 0;
        *pointer++ = 0;
        *pointer++ = height;
        *pointer++ = 0;
        *pointer++ = width;
        *pointer = height;
        break;
    }
    flip_y = 1;
}


void dqt_width(Vwk * vwk)
{
    WORD k;
    WORD *pointer;
    Fonthead *fnt_ptr;

    fnt_ptr = vwk->cur_font;
    pointer = PTSOUT;

    /* Set that there is no horizontal offset */
    *(pointer + 2) = 0;
    *(pointer + 4) = 0;

    k = INTIN[0];
    if ((k < fnt_ptr->first_ade) || (k > fnt_ptr->last_ade))
        INTOUT[0] = -1;
    else {
        INTOUT[0] = k;
        k -= fnt_ptr->first_ade;
        *(pointer) = fnt_ptr->off_table[k + 1] - fnt_ptr->off_table[k];
        if (vwk->scaled) {
            if (vwk->dda_inc == 0xFFFF)
                *pointer *= 2;
            else
                *pointer = act_siz(vwk, *pointer);
        }

        if (fnt_ptr->flags & F_HORZ_OFF) {
            *(pointer + 2) = fnt_ptr->hor_table[k * 2];
            *(pointer + 4) = fnt_ptr->hor_table[(k * 2) + 1];
        }
    }

    CONTRL[2] = 3;
    CONTRL[4] = 1;
    flip_y = 1;
}



void dqt_name(Vwk * vwk)
{
    WORD i, element;
    BYTE *name;
    WORD *int_out;
    Fonthead *tmp_font;
    BYTE found;

    WORD font_id;
    Fonthead **chain_ptr;

    element = INTIN[0];
    chain_ptr = font_ring;
    i = 0;
    font_id = -1;

    found = 0;
    while (!found && (tmp_font = *chain_ptr++)) {
        do {
            font_id = tmp_font->font_id;
            if ((++i) == element)
                found = 1;
        } while (!found && (tmp_font = tmp_font->next_font));
    }

    /* The element is out of bounds - default to the system font */
    if (!found)
        tmp_font = &fon6x6;

    int_out = INTOUT;
    *int_out++ = tmp_font->font_id;
    for (i = 1, name = tmp_font->name; (*int_out++ = *name++); i++);
    while (i < 33) {
        *int_out++ = 0;
        i++;
    }
    CONTRL[4] = 33;

}


void dqt_fontinfo(Vwk * vwk)
{
    WORD *pointer;
    Fonthead *fnt_ptr;

    fnt_ptr = vwk->cur_font;

    pointer = INTOUT;
    *pointer++ = fnt_ptr->first_ade;
    *pointer = fnt_ptr->last_ade;

    pointer = PTSOUT;
    *pointer++ = fnt_ptr->max_cell_width;
    *pointer++ = fnt_ptr->bottom;

    if (vwk->style & F_THICKEN)
        *pointer++ = fnt_ptr->thicken;
    else
        *pointer++ = 0;

    *pointer++ = fnt_ptr->descent;

    if (vwk->style & F_SKEW) {
        *pointer++ = fnt_ptr->left_offset;
        *pointer++ = fnt_ptr->half;
        *pointer++ = fnt_ptr->right_offset;
    } else {
        *pointer++ = 0;
        *pointer++ = fnt_ptr->half;
        *pointer++ = 0;
    }

    *pointer++ = fnt_ptr->ascent;
    *pointer++ = 0;
    *pointer = fnt_ptr->top;

    CONTRL[2] = 5;
    CONTRL[4] = 2;
    flip_y = 1;
}


void d_justified(Vwk * vwk)
{
    WORD spaces;
    WORD expand, sav_cnt;
    WORD interword, interchar;
    WORD cnt, *old_intin, *old_ptsout, extent[8], max_x;
    WORD i, direction, delword, delchar;
    WORD *pointer;

    pointer = (CONTRL + 3);
    cnt = *pointer = (sav_cnt = *pointer) - 2;

    pointer = INTIN;
    interword = *pointer++;
    interchar = *pointer++;

    old_intin = INTIN;
    INTIN = pointer;
    old_ptsout = PTSOUT;
    PTSOUT = extent;

    for (i = 0, spaces = 0; i < cnt; i++)
        if (*(pointer++) == 32)
            spaces++;

    dqt_extent(vwk);
    CONTRL[2] = 0;

    max_x = PTSIN[2];

    if (interword && spaces) {
        delword = (max_x - width) / spaces;
        rmword = (max_x - width) % spaces;

        if (rmword < 0) {
            direction = -1;
            rmword = 0 - rmword;
        } else
            direction = 1;

        if (interchar) {
            expand = vwk->cur_font->max_cell_width / 2;
            if (delword > expand) {
                delword = expand;
                rmword = 0;
            }
            if (delword < (0 - expand)) {
                delword = 0 - expand;
                rmword = 0;
            }
            width += (delword * spaces) + (rmword * direction);
        }

        switch (vwk->chup) {
        case 0:
            wordx = delword;
            wordy = 0;
            rmwordx = direction;
            rmwordy = 0;
            break;
        case 900:
            wordx = 0;
            wordy = 0 - delword;
            rmwordx = 0;
            rmwordy = 0 - direction;
            break;
        case 1800:
            wordx = 0 - delword;
            wordy = 0;
            rmwordx = 0 - direction;
            rmwordy = 0;
            break;
        case 2700:
            wordx = 0;
            wordy = delword;
            rmwordx = 0;
            rmwordy = direction;
            break;
        }
    } else {
        wordx = 0;
        wordy = 0;
        rmword = 0;
    }

    if (interchar && cnt > 1) {
        delchar = (max_x - width) / (cnt - 1);
        rmchar = (max_x - width) % (cnt - 1);

        if (rmchar < 0) {
            direction = -1;
            rmchar = 0 - rmchar;
        } else
            direction = 1;

        switch (vwk->chup) {
        case 0:
            charx = delchar;
            chary = 0;
            rmcharx = direction;
            rmchary = 0;
            break;
        case 900:
            charx = 0;
            chary = 0 - delchar;
            rmcharx = 0;
            rmchary = 0 - direction;
            break;
        case 1800:
            charx = 0 - delchar;
            chary = 0;
            rmcharx = 0 - direction;
            rmchary = 0;
            break;
        case 2700:
            charx = 0;
            chary = delchar;
            rmcharx = 0;
            rmchary = direction;
            break;
        }
    } else {
        charx = 0;
        chary = 0;
        rmchar = 0;
    }

    width = max_x;

    d_gtext(vwk);

    CONTRL[2] = sav_cnt;
    PTSOUT = old_ptsout;
    INTIN = old_intin;
}


void dt_loadfont(Vwk * vwk)
{
    WORD id, count, *control;

    Fonthead *first_font;

    /* Init some common variables */
    control = CONTRL;
    *(control + 4) = 1;

    /* You only get one chance to load fonts.  If fonts are linked in, exit  */
    if (vwk->loaded_fonts) {
        INTOUT[0] = 0;
        return;
    }

    /* The inputs to this routine are :         */
    /* CONTRL[7-8]   = Pointer to scratch buffer    */
    /* CONTRL[9]     = Offset to buffer 2       */
    /* CONTRL[10-11] = Pointer to first font    */

    /* Init the global structures           */
    vwk->scrpt2 = *(control + 9);
    vwk->scrtchp = (WORD *) *((LONG *) (control + 7));

    first_font = (Fonthead *) *((LONG *) (control + 10));
    vwk->loaded_fonts = first_font;

    /* Find out how many distinct font id numbers have just been linked in. */
    id = -1;
    count = 0;

    do {
        /* Update the count of font id numbers, if necessary. */
        if (first_font->font_id != id) {
            id = first_font->font_id;
            count++;
        }

        /* Make sure the font is in device specific format. */
        if (!(first_font->flags & F_STDFORM)) {
            FBASE = first_font->dat_table;
            FWIDTH = first_font->form_width;
            DELY = first_font->form_height;
            trnsfont();
            first_font->flags ^= F_STDFORM;
        }
        first_font = first_font->next_font;
    } while (first_font);

    font_ring[2] = vwk->loaded_fonts;

    /* Update the device table count of faces. */
    vwk->num_fonts += count;
    DEV_TAB[10] = vwk->num_fonts;
    INTOUT[0] = count;
}


void dt_unloadfont(Vwk * vwk)
{
    /* Since we always unload all fonts, this is easy. */
    vwk->loaded_fonts = NULLPTR;   /* No fonts installed */
    font_ring[2] = vwk->loaded_fonts;
    vwk->scrpt2 = scrtsiz; /* Reset pointers to default buffers */
    vwk->scrtchp = deftxbuf;
    vwk->num_fonts = font_count;   /* Reset font count to default */
    DEV_TAB[10] = vwk->num_fonts;
}



/*
 * input:
 *   req - d0, get requested size
 *   act - d1, get actual size
 *
 * output:
 *   T_SCLSTS is the text scaling flag (means: scale up or down)
 *   returns the quotient * 256
 */

WORD clc_dda(Vwk * vwk, WORD act, WORD req)
{
    /* if actual =< requested */
    if ( act <= req ) {
        vwk->t_sclsts = 0;           /* we do scale down */
        /* check requested size */
        if ( req <= 0 ) {
            req = 1;            /* if 0 then make it 1 (minimum value) */
        }
    }
    else {
        vwk->t_sclsts = 1;           /* we do scale up */
        req -= act;
        /* if larger than 2x? */
        if ( req >= act )
            return -1;          /* put 0xFFFF in d0 (max value, 2x) */
    }

    /* requested/actual: quotient = bits 15-0 */
    return (WORD)(((ULONG)req << 16) / act);
}
