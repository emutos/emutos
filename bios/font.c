/*
 * font.c - bios part of font oading and initialization
 *
 * Copyright (c) 2004 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#define DBG_FONT 0

#include "font.h"
#include "country.h"
#include "string.h"
#include "lineavars.h"
#include "kprint.h"

/* RAM-copies of the ROM-fontheaders */
struct font_head *sysfonts[4];  // all three fonts and NULL
struct font_head fon8x16;
struct font_head fon8x8;
struct font_head fon6x6;



/*
 * font_init - set default font to linea, font ring initialization
 *
 * smallfont - on 320x200 resolution a small font is used.
 */

void font_init(BOOL smallfont)
{
    struct font_head *f6x6, *f8x8, *f8x16;
  
    /* ask country.c for the right fonts */

    get_fonts(&f6x6, &f8x8, &f8x16);
  
    /* copy the ROM-fontheaders of 3 system fonts to RAM */

    memmove(&fon6x6, f6x6, sizeof(struct font_head));
    memmove(&fon8x8, f8x8, sizeof(struct font_head));
    memmove(&fon8x16, f8x16, sizeof(struct font_head));

    /* now in RAM, chain the font headers to a linked list */

    fon6x6.next_font = &fon8x8;
    fon8x8.next_font = &fon8x16;
    fon8x16.next_font = 0;

    /* Initialize the system font array for linea */

    sysfonts[0] = &fon6x6;
    sysfonts[1] = &fon8x8;
    sysfonts[2] = &fon8x16;
    sysfonts[3] = NULL;

    font_count = 3;               // total number of fonts in fontring
}

/*
 * font_set_default - choose default font depending on screen height
 *
 * set linea variables according to choosen font configuration
 */

void font_set_default()
{
    struct font_head *font;

    if (v_vt_rez < 400)
	font = &fon8x8;
    else
	font = &fon8x16;

    v_cel_ht = font->form_height;
    v_cel_wr = v_lin_wr * font->form_height;
    v_cel_mx = (v_hz_rez / font->max_cell_width) - 1;
    v_cel_my = (v_vt_rez / font->form_height) - 1;

    v_fnt_wr = font->form_width;
    v_fnt_st = font->first_ade;
    v_fnt_nd = font->last_ade;
    v_fnt_ad = font->dat_table;
    v_off_ad = font->off_table;
}
