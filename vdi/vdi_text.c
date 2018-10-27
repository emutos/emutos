/*
 * text.c - uses text_blt to move data from a font table to screen
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2017 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "config.h"
#include "portab.h"
#include "asm.h"
#include "string.h"
#include "vdi_defs.h"
#include "../bios/lineavars.h"


extern const Fonthead *def_font;    /* Default font of open workstation */
extern const Fonthead *font_ring[]; /* Ring of available fonts */

extern WORD font_count;         /* Number of fonts in driver */
extern WORD deftxbuf[];         /* Default text scratch buffer */
extern const WORD scrtsiz;      /* Default offset to large text buffer */

extern Fonthead fon6x6;         /* See bios/fntxxx.c */
extern Fonthead fon8x8;         /* See bios/fntxxx.c */
extern Fonthead fon8x16;        /* See bios/fntxxx.c */

/*
 * Local structure for passing justification info
 *
 * note 1: in each of the following pairs of variables, only one variable
 * has a non-zero value:
 *  (wordx,wordy), (rmwordx,rmwordy), (charx,chary), (rmcharx,rmchary)
 *
 * note 2: if rmword is zero, there are no 'remainder pixels' to use up
 * between words; otherwise, rmword = max(rmwordx/rmwordy).  similarly
 * for rmchar/rmcharx/rmchary.
 */
typedef struct
{
                        /* word-spacing variables */
    WORD wordx, wordy;          /* #pixels to add to each space */
    WORD rmword;                /* remaining #pixels to add over all spaces */
    WORD rmwordx, rmwordy;      /* add this to use up remainder */
                        /* character-spacing variables */
    WORD charx, chary;          /* #pixels to add to each character */
    WORD rmchar;                /* remaining #pixels to add over all characters */
    WORD rmcharx, rmchary;      /* add this to use up remainder */
} JUSTINFO;


/* Prototypes for this module */
static void make_header(Vwk * vwk);
static UWORD clc_dda(Vwk * vwk, UWORD act, UWORD req);
static UWORD act_siz(Vwk * vwk, UWORD top);

/*
 * calculates height of text string
 */
static WORD calc_height(Vwk *vwk)
{
    const Fonthead *fnt_ptr = vwk->cur_font;
    WORD height;

    height = fnt_ptr->top + fnt_ptr->bottom + 1;    /* handles scaled fonts */

    if (vwk->style & F_OUTLINE)
        height += 2;        /* outlining adds 1 pixel all around */

    return height;
}

/*
 * calculates width of text string
 */
static WORD calc_width(Vwk *vwk, WORD cnt, WORD *str)
{
    const Fonthead *fnt_ptr = vwk->cur_font;
    WORD table_start = fnt_ptr->first_ade;
    WORD i, chr, width;

    if (fnt_ptr->flags & F_MONOSPACE)
    {
        width = cnt * (fnt_ptr->off_table[1]-fnt_ptr->off_table[0]);
    }
    else
    {
        for (i = 0, width = 0; i < cnt; i++) {
            chr = *str++ - table_start;
            width += fnt_ptr->off_table[chr + 1] - fnt_ptr->off_table[chr];
        }
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

    if (vwk->style & F_OUTLINE)
        width += cnt * 2;       /* outlining adds 1 pixel all around */

    return width;
}

/*
 * output specified text string
 *
 * 'width' is the pre-calculated width of the text on the screen;
 * if negative, it has not yet been calculated
 */
static void output_text(Vwk *vwk, WORD count, WORD *str, WORD width, JUSTINFO *justified)
{
    WORD i, j;
    WORD startx, starty;
    WORD xfact, yfact;
    WORD tx1, tx2, ty1, ty2;
    WORD delh, delv;
    WORD d1, d2;

    WORD temp;
    const Fonthead *fnt_ptr;
    Point * point;

    CONTRL[2] = 0;      /* # points in PTSOUT */

    if (count <= 0)     /* quick out for unlikely occurrence */
        return;

    /* some data copying for the assembler part */
    DDAINC = vwk->dda_inc;
    SCALDIR = vwk->t_sclsts;
    SCALE = vwk->scaled;
    MONO = F_MONOSPACE & vwk->cur_font->flags;
    WRT_MODE = vwk->wrt_mode;

    CLIP = vwk->clip;
    XMINCL = vwk->xmn_clip;
    YMINCL = vwk->ymn_clip;
    XMAXCL = vwk->xmx_clip;
    YMAXCL = vwk->ymx_clip;
    STYLE = vwk->style;
    CHUP = vwk->chup;
    SCRPT2 = vwk->scrpt2;
    SCRTCHP = vwk->scrtchp;

    fnt_ptr = vwk->cur_font;     /* Get current font pointer in register */

    if (vwk->style & F_THICKEN)
        WEIGHT = fnt_ptr->thicken;

    if (vwk->style & F_LIGHT)
        LITEMASK = fnt_ptr->lighten;

    if (vwk->style & F_SKEW) {
        d1 = fnt_ptr->left_offset;  /* used in vertical alignment calcs */
        d2 = fnt_ptr->right_offset;
        SKEWMASK = fnt_ptr->skew;
    } else {
        d1 = 0;
        d2 = 0;
    }
    LOFF = d1;
    ROFF = d2;

    FBASE = fnt_ptr->dat_table;
    FWIDTH = fnt_ptr->form_width;

    switch(vwk->h_align) {
    default:                /* normally case 0: left justified */
        delh = 0;
        break;
    case 1:
        if (width < 0)      /* called from vdi_v_gtext() */
            width = calc_width(vwk, count, str);
        delh = width / 2;
        break;
    case 2:
        if (width < 0)      /* called from vdi_v_gtext() */
            width = calc_width(vwk, count, str);
        delh = width;
        break;
    }

    switch(vwk->v_align) {
    default:                /* normally case 0: baseline */
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
    switch(vwk->chup) {
    default:                /* normally case 0: no rotation */
        DESTX = point->x - delh;
        DESTY = point->y - delv;
        startx = DESTX;
        starty = DESTY + fnt_ptr->top + fnt_ptr->ul_size + 1;
        xfact = 0;
        yfact = 1;
        break;
    case 900:
        DESTX = point->x - delv;
        DESTY = point->y + delh + 1;
        startx = DESTX + fnt_ptr->top + fnt_ptr->ul_size + 1;
        starty = DESTY;
        xfact = 1;
        yfact = 0;
        break;
    case 1800:
        DESTX = point->x + delh + 1;
        DESTY = point->y - ((fnt_ptr->top + fnt_ptr->bottom) - delv);
        startx = DESTX;
        starty = (DESTY + fnt_ptr->bottom) - (fnt_ptr->ul_size + 1);
        xfact = 0;
        yfact = -1;
        break;
    case 2700:
        DESTX = point->x - ((fnt_ptr->top + fnt_ptr->bottom) - delv);
        DESTY = point->y - delh;
        startx = (DESTX + fnt_ptr->bottom) - (fnt_ptr->ul_size + 1);
        starty = DESTY;
        xfact = -1;
        yfact = 0;
        break;
    }

    TEXTFG = vwk->text_color;
    DELY = fnt_ptr->form_height;
    XDDA = 32767;       /* init the horizontal dda */

    for (j = 0; j < count; j++) {

        temp = str[j];

        /* If the character is out of range for this font make it a ? */
        if ((temp < fnt_ptr->first_ade) || (temp > fnt_ptr->last_ade))
            temp = '?';
        temp -= fnt_ptr->first_ade;

        SOURCEX = fnt_ptr->off_table[temp];
        DELX = fnt_ptr->off_table[temp + 1] - SOURCEX;

        SOURCEY = 0;
        DELY = fnt_ptr->form_height;

        text_blt();

        if (justified) {
            DESTX += justified->charx;
            DESTY += justified->chary;
            if (justified->rmchar) {
                DESTX += justified->rmcharx;
                DESTY += justified->rmchary;
                justified->rmchar--;
            }
            if (str[j] == ' ') {
                DESTX += justified->wordx;
                DESTY += justified->wordy;
                if (justified->rmword) {
                    DESTX += justified->rmwordx;
                    DESTY += justified->rmwordy;
                    justified->rmword--;
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
                    abline(line, vwk->wrt_mode, vwk->text_color);

                line->x1 = tx1;
                line->x2 = tx2;
                line->y1 = ty1;
                line->y2 = ty2;
            } else
                abline(line, vwk->wrt_mode, vwk->text_color);

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

void vdi_v_gtext(Vwk * vwk)
{
    output_text(vwk, CONTRL[3], INTIN, -1, NULL);
}

void text_init2(Vwk * vwk)
{
    vwk->cur_font = def_font;
    vwk->loaded_fonts = NULL;
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

void text_init(void)
{
    WORD i, j;
    WORD id_save, cell_height;
    const Fonthead *fnt_ptr, **chain_ptr;

    SIZ_TAB[0] = 32767;         /* minimal char width */
    SIZ_TAB[1] = 32767;         /* minimal char height */
    SIZ_TAB[2] = 0;             /* maximal char width */
    SIZ_TAB[3] = 0;             /* maximal char height */

    /* Initialize the font ring. */
    font_ring[0] = &fon6x6;
    font_ring[1] = &fon8x8;
    font_ring[2] = NULL;
    font_ring[3] = NULL;

    id_save = fon6x6.font_id;

    def_font = NULL;
    cell_height = (V_REZ_VT >= 400) ? 16 : 8;   /* to select among default fonts */

    chain_ptr = font_ring;
    i = 0;
    j = 0;
    while ((fnt_ptr = *chain_ptr++)) {
        do {
            if (fnt_ptr->flags & F_DEFAULT) {   /* If default, save font pointer */
                if (!def_font)                  /* ... for sure if we don't have one yet */
                    def_font = fnt_ptr;
                else if (def_font->form_height != cell_height)
                    def_font = fnt_ptr;         /* ... also if previously-saved has wrong height */
            }

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
        } while ((fnt_ptr = fnt_ptr->next_font));
    }
    DEV_TAB[5] = i;                     /* number of sizes */
    font_count = DEV_TAB[10] = ++j;     /* number of faces */
}

/*
 * set up width/height values returned by vst_height()/vst_point()
 */
static void setup_width_height(const Fonthead *font)
{
    WORD *p;
    UWORD top;

    CONTRL[2] = 2;      /* # points in PTSOUT */

    p = PTSOUT;
    *p++ = font->max_char_width;
    *p++ = top = font->top;
    *p++ = font->max_cell_width;
    *p = top + font->bottom + 1;
    flip_y = 1;
}

void vdi_vst_height(Vwk * vwk)
{
    const Fonthead **chain_ptr;
    const Fonthead *test_font, *single_font;
    WORD font_id;
    UWORD test_height;
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

    setup_width_height(single_font);    /* set up return values */
}


/*
 * act_siz - Actual sizer routine
 *
 * entry:
 *   top     - size to scale (DELY etc)
 *
 * used variables:
 *   vwk->dda_inc (UWORD) - DDA increment passed externally
 *   vwk->t_sclsts (WORD) - 0 if scale down, 1 if enlarge
 *
 * exit:
 *   actual size
 */
static UWORD act_siz(Vwk * vwk, UWORD top)
{
    UWORD accu;
    UWORD retval;
    UWORD i;

    if (vwk->dda_inc == 0xffff) {
        /* double size */
        return (top<<1);
    }
    accu = 0x7fff;
    retval = vwk->t_sclsts ? top : 0;

    for (i = 0; i < top; i++) {
        accu += vwk->dda_inc;
        if (accu < vwk->dda_inc)
            retval++;
    }

    /* if input is non-zero, make return value at least 1 */
    if (top && !retval)
        retval = 1;

    return retval;
}


static void make_header(Vwk * vwk)
{
    const Fonthead *source_font;
    Fonthead *dest_font;

    source_font = vwk->cur_font;
    dest_font = &vwk->scratch_head;

    dest_font->font_id = source_font->font_id;
    dest_font->point = source_font->point * 2;

    memcpy(dest_font->name, source_font->name, FONT_NAME_LEN);

    dest_font->first_ade = source_font->first_ade;
    dest_font->last_ade = source_font->last_ade;

    if (vwk->dda_inc == 0xFFFF) {
        dest_font->top = source_font->top * 2 + 1;
        dest_font->ascent = source_font->ascent * 2 + 1;
        dest_font->half = source_font->half * 2 + 1;
    } else {
        dest_font->top = act_siz(vwk, source_font->top);
        dest_font->ascent = act_siz(vwk, source_font->ascent);
        dest_font->half = act_siz(vwk, source_font->half);
    }
    dest_font->descent = act_siz(vwk, source_font->descent);
    dest_font->bottom = act_siz(vwk, source_font->bottom);
    dest_font->max_char_width = act_siz(vwk, source_font->max_char_width);
    dest_font->max_cell_width = act_siz(vwk, source_font->max_cell_width);
    dest_font->left_offset = act_siz(vwk, source_font->left_offset);
    dest_font->right_offset = act_siz(vwk, source_font->right_offset);
    dest_font->thicken = act_siz(vwk, source_font->thicken);
    dest_font->ul_size = act_siz(vwk, source_font->ul_size);

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


void vdi_vst_point(Vwk * vwk)
{
    WORD font_id;
    const Fonthead **chain_ptr, *double_font;
    const Fonthead *test_font, *single_font;
    WORD test_height, h;
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
        while (((h = test_font->point) <= test_height)
               && (test_font->font_id == font_id)) {
            single_font = test_font;
            if (h * 2 <= test_height)
                double_font = test_font;

            if (!(test_font = test_font->next_font))
                break;
        }
    } while ((test_font = *chain_ptr++));

    /* Set up environment for this font in the non-scaled case */
    vwk->cur_font = single_font;
    vwk->scaled = FALSE;

    if (single_font->point != test_height) {
        h = double_font->point * 2;

        if ((h > single_font->point) && (h <= test_height)) {
            vwk->dda_inc = 0xFFFF;
            vwk->t_sclsts = 1;
            vwk->cur_font = double_font;
            make_header(vwk);
            single_font = vwk->cur_font;
        }
    }

    setup_width_height(single_font);    /* set up return values */

    CONTRL[4] = 1;          /* also return point size actually set */
    INTOUT[0] = single_font->point;
}


void vdi_vst_effects(Vwk * vwk)
{
    INTOUT[0] = vwk->style = INTIN[0] & INQ_TAB[2];
    CONTRL[4] = 1;
}


void vdi_vst_alignment(Vwk * vwk)
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


/*
 * the following implementation mimics TOS3/TOS4 behaviour
 *
 * the rotation angle in tenths of a degree may be supplied as any
 * positive or negative WORD value, but it is first normalised to
 * a value between 0 & 3599 by adding or subtracting multiples of
 * 3600, and then rounded to the nearest multiple of 900 between
 * 0 and 3600 inclusive before storing.
 *
 * trivia: a value of 3600 is handled by other VDI code in the same
 * way as a value of 0, but if you really want to store a value of
 * 3600 rather than 0, you can pass a value between 3150 and 3599
 * inclusive ...
 *
 * TOS1 difference: TOS1 does not normalise before rounding, so
 * stored values range from -32400 to +32400; values outside the
 * range of 0 to 3600 inclusive cause vqt_extent() to not return
 * values ...
 */
void vdi_vst_rotation(Vwk * vwk)
{
    WORD angle = INTIN[0];

    while(angle < 0)        /* ensure positive angle */
        angle += 3600;

    while(angle >= 3600)    /* between 0 and 3599 inclusive */
        angle -= 3600;

    /* this sets a value of 0, 900, 1800, 2700 or 3600, just like TOS3/TOS4 */
    INTOUT[0] = vwk->chup = ((angle + 450) / 900) * 900;
    CONTRL[4] = 1;
}


void vdi_vst_font(Vwk * vwk)
{
    WORD *old_intin, point, *old_ptsout, dummy[4], *old_ptsin;
    WORD face;
    const Fonthead *test_font, **chain_ptr;
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
        vdi_vst_point(vwk);
    else
        vdi_vst_height(vwk);

    INTIN = old_intin;
    PTSIN = old_ptsin;
    PTSOUT = old_ptsout;

    CONTRL[2] = 0;
    CONTRL[4] = 1;
    INTOUT[0] = vwk->cur_font->font_id;
}


void vdi_vst_color(Vwk * vwk)
{
    WORD r;

    r = validate_color_index(INTIN[0]);

    CONTRL[4] = 1;
    INTOUT[0] = r;
    vwk->text_color = MAP_COL[r];
}


void vdi_vqt_attributes(Vwk * vwk)
{
    WORD *pointer;
    const Fonthead *fnt_ptr;

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
    *pointer++ = fnt_ptr->top;
    *pointer++ = fnt_ptr->max_cell_width;
    *pointer = fnt_ptr->top + fnt_ptr->bottom + 1;  /* handles scaled fonts */

    CONTRL[2] = 2;
    CONTRL[4] = 6;
    flip_y = 1;
}


void vdi_vqt_extent(Vwk * vwk)
{
    WORD height, width;

    height = calc_height(vwk);
    width = calc_width(vwk, CONTRL[3], INTIN);

    CONTRL[2] = 4;

    memset(PTSOUT,0,8*sizeof(WORD));
    switch (vwk->chup) {
    default:                    /* 0 or 3600 ... see vdi_vst_rotation() */
        PTSOUT[2] = width;
        PTSOUT[4] = width;
        PTSOUT[5] = height;
        PTSOUT[7] = height;
        break;
    case 900:
        PTSOUT[0] = height;
        PTSOUT[2] = height;
        PTSOUT[3] = width;
        PTSOUT[5] = width;
        break;
    case 1800:
        PTSOUT[0] = width;
        PTSOUT[1] = height;
        PTSOUT[3] = height;
        PTSOUT[6] = width;
        break;
    case 2700:
        PTSOUT[1] = width;
        PTSOUT[4] = height;
        PTSOUT[6] = height;
        PTSOUT[7] = width;
        break;
    }
    flip_y = 1;
}


void vdi_vqt_width(Vwk * vwk)
{
    WORD k;
    WORD *pointer;
    const Fonthead *fnt_ptr;

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



void vdi_vqt_name(Vwk * vwk)
{
    WORD i, element;
    const BYTE *name;
    WORD *int_out;
    const Fonthead *tmp_font;
    BYTE found;

    const Fonthead **chain_ptr;

    element = INTIN[0];
    chain_ptr = font_ring;
    i = 0;

    found = 0;
    while (!found && (tmp_font = *chain_ptr++)) {
        do {
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


void vdi_vqt_fontinfo(Vwk * vwk)
{
    WORD *pointer;
    const Fonthead *fnt_ptr;

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


void gdp_justified(Vwk * vwk)
{
    WORD spaces;
    WORD expand;
    WORD interword, interchar;
    WORD cnt, width, max_x;
    WORD i, direction, delword, delchar;
    WORD *pointer, *str;
    JUSTINFO just;

    cnt = CONTRL[3] - 2;

    pointer = INTIN;
    interword = *pointer++;
    interchar = *pointer++;
    str = pointer;

    /*
     * if interword adjustment required, count spaces
     */
    if (interword) {
        for (i = 0, spaces = 0; i < cnt; i++)
            if (*(pointer++) == ' ')
                spaces++;
    }

    width = calc_width(vwk, cnt, str);

    max_x = PTSIN[2];

    if (interword && spaces) {
        delword = (max_x - width) / spaces;
        just.rmword = (max_x - width) % spaces;

        if (just.rmword < 0) {
            direction = -1;
            just.rmword = 0 - just.rmword;
        } else
            direction = 1;

        if (interchar) {
            expand = vwk->cur_font->max_cell_width / 2;
            if (delword > expand) {
                delword = expand;
                just.rmword = 0;
            }
            if (delword < (0 - expand)) {
                delword = 0 - expand;
                just.rmword = 0;
            }
            width += (delword * spaces) + (just.rmword * direction);
        }

        switch (vwk->chup) {
        default:                /* normally case 0: no rotation */
            just.wordx = delword;
            just.wordy = 0;
            just.rmwordx = direction;
            just.rmwordy = 0;
            break;
        case 900:
            just.wordx = 0;
            just.wordy = 0 - delword;
            just.rmwordx = 0;
            just.rmwordy = 0 - direction;
            break;
        case 1800:
            just.wordx = 0 - delword;
            just.wordy = 0;
            just.rmwordx = 0 - direction;
            just.rmwordy = 0;
            break;
        case 2700:
            just.wordx = 0;
            just.wordy = delword;
            just.rmwordx = 0;
            just.rmwordy = direction;
            break;
        }
    } else {
        just.wordx = 0;
        just.wordy = 0;
        just.rmword = 0;
    }

    if (interchar && cnt > 1) {
        delchar = (max_x - width) / (cnt - 1);
        just.rmchar = (max_x - width) % (cnt - 1);

        if (just.rmchar < 0) {
            direction = -1;
            just.rmchar = 0 - just.rmchar;
        } else
            direction = 1;

        switch (vwk->chup) {
        default:                /* normally case 0: no rotation */
            just.charx = delchar;
            just.chary = 0;
            just.rmcharx = direction;
            just.rmchary = 0;
            break;
        case 900:
            just.charx = 0;
            just.chary = 0 - delchar;
            just.rmcharx = 0;
            just.rmchary = 0 - direction;
            break;
        case 1800:
            just.charx = 0 - delchar;
            just.chary = 0;
            just.rmcharx = 0 - direction;
            just.rmchary = 0;
            break;
        case 2700:
            just.charx = 0;
            just.chary = delchar;
            just.rmcharx = 0;
            just.rmchary = direction;
            break;
        }
    } else {
        just.charx = 0;
        just.chary = 0;
        just.rmchar = 0;
    }

    output_text(vwk, cnt, str, max_x, &just);
}


void vdi_vst_load_fonts(Vwk * vwk)
{
    CONTRL[4] = 1;
    INTOUT[0] = 0;      /* we loaded no new fonts */
}


void vdi_vst_unload_fonts(Vwk * vwk)
{
    /* nothing to do */
}



/*
 * input:
 *   req - d0, get requested size
 *   act - d1, get actual size
 *
 * output:
 *   vwk->t_sclsts is the text scaling flag (means: scale up or down)
 */

static UWORD clc_dda(Vwk * vwk, UWORD act, UWORD req)
{
    if ( req < act ) {
        vwk->t_sclsts = 0;           /* we do scale down */
        if ( req == 0 ) {
            req = 1;            /* if 0 then make it 1 (minimum value) */
        }
    }
    else {
        vwk->t_sclsts = 1;           /* we do scale up */
        req -= act;
        /* if larger than 2x? */
        if ( req >= act )
            return 0xFFFF;          /* indicates 2x scale up */
    }

    return (UWORD)((((ULONG)req)<<16)/act);
}
