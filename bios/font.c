/*
 * font.c - bios part of font loading and initialization
 *
 * Copyright (C) 2004-2014 by Authors (see below)
 * Copyright (C) 2016-2017 The EmuTOS development team
 *
 * Authors:
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/* #define ENABLE_KDEBUG */

#include "config.h"
#include "font.h"
#include "country.h"
#include "string.h"
#include "lineavars.h"
#include "kprint.h"

/* RAM-copies of the ROM-fontheaders */
Fonthead *sysfonts[4];  /* all three fonts and NULL */
Fonthead fon8x16;
Fonthead fon8x8;
Fonthead fon6x6;



/*
 * font_init - set default font to linea, font ring initialization
 */

void font_init(void)
{
    const Fonthead *f6x6, *f8x8, *f8x16;

    /* ask country.c for the right fonts */

    get_fonts(&f6x6, &f8x8, &f8x16);

    /* copy the ROM-fontheaders of 3 system fonts to RAM */

    fon6x6 = *f6x6;
    fon8x8 = *f8x8;
    fon8x16 = *f8x16;

    /* now in RAM, chain the font headers to a linked list */

    fon6x6.next_font = &fon8x8;
    fon8x8.next_font = &fon8x16;
    fon8x16.next_font = 0;

    /* Initialize the system font array for linea */

    sysfonts[0] = &fon6x6;
    sysfonts[1] = &fon8x8;
    sysfonts[2] = &fon8x16;
    sysfonts[3] = NULL;

    font_count = 3;               /* total number of fonts in fontring */
}

/*
 * font_set_default - choose default font
 *
 * if the 'cellheight' value is a known height, use corresponding font;
 * otherwise choose font according to screen height
 *
 * sets linea variables according to chosen font configuration
 */

void font_set_default(WORD cellheight)
{
    Fonthead *font;

    switch(cellheight)
    {
    case 8:
        font = &fon8x8;
        break;
    case 16:
        font = &fon8x16;
        break;
    default:
        if (V_REZ_VT < 400)
            font = &fon8x8;
        else
            font = &fon8x16;
    }

    v_cel_ht = font->form_height;
    v_cel_wr = v_lin_wr * font->form_height;
    v_cel_mx = (V_REZ_HZ / font->max_cell_width) - 1;
    v_cel_my = (V_REZ_VT / font->form_height) - 1;

    v_fnt_wr = font->form_width;
    v_fnt_st = font->first_ade;
    v_fnt_nd = font->last_ade;
    v_fnt_ad = font->dat_table;
    v_off_ad = font->off_table;
}
