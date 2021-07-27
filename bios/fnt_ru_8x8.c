/*
 * fnt_ru_8x8.c - a font in standard format
 *
 * Copyright (C) 2010-2021 The EmuTOS development team
 *
 * Automatically generated by fntconv.c
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "bios.h"
#include "fonthdr.h"

static const UWORD dat_table[] =
{
    0x0018, 0x3c18, 0x183c, 0xffe7, 0x017e, 0x1818, 0xf0f0, 0x05a0,
    0x7c06, 0x7c7c, 0xc67c, 0x7c7c, 0x7c7c, 0x0078, 0x07f0, 0x1104,
    0x0018, 0x6600, 0x1800, 0x3818, 0x0c30, 0x0000, 0x0000, 0x0006,
    0x7c18, 0x3c3c, 0x1c7e, 0x1c7e, 0x3c3c, 0x0000, 0x0600, 0x603c,
    0x7c18, 0xfc3c, 0xf8fe, 0xfe3c, 0x663c, 0x1ee6, 0xf0c6, 0x663c,
    0xfc3c, 0xfc3c, 0x7e66, 0x66c6, 0xc666, 0xfe1e, 0xc078, 0x1000,
    0x6000, 0xe000, 0x1c00, 0x1c00, 0xe018, 0x06e0, 0x3800, 0x0000,
    0x0000, 0x0000, 0x1000, 0x0000, 0x0000, 0x000e, 0x0070, 0x7600,
    0x7800, 0x1c7e, 0xcce0, 0x3000, 0x7ecc, 0xe0cc, 0x7ce0, 0xc630,
    0x1c00, 0x3e78, 0x0000, 0x7800, 0x00c3, 0xcc18, 0x38cc, 0xf80e,
    0x1c38, 0x0000, 0x00fc, 0x3c38, 0x3000, 0x00c3, 0xc318, 0x0000,
    0x2200, 0xdb18, 0x1818, 0x3600, 0x0036, 0x3600, 0x367e, 0x7ef1,
    0x187e, 0xfc7e, 0x3cfe, 0xdb3c, 0x665c, 0xe61f, 0xc666, 0x3c7e,
    0xfc3c, 0x7e66, 0x18c6, 0x6666, 0xd6d6, 0xe0c3, 0x603c, 0xce3f,
    0x0002, 0x0000, 0x0000, 0x0000, 0x001c, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x003c, 0x241c, 0x3899, 0xffc3, 0x03c3, 0x3c1c, 0xc0c0, 0x05a0,
    0xc606, 0x0606, 0xc6c0, 0xc006, 0xc6c6, 0x0060, 0x0ff8, 0x0b28,
    0x003c, 0x666c, 0x3ec6, 0x6c18, 0x1818, 0x6618, 0x0000, 0x000c,
    0xc638, 0x6666, 0x3c60, 0x3066, 0x6666, 0x1818, 0x0c00, 0x3066,
    0xce3c, 0x6666, 0x6c62, 0x6266, 0x6618, 0x0c66, 0x60ee, 0x7666,
    0x6666, 0x6666, 0x5a66, 0x66c6, 0xc666, 0xc618, 0x6018, 0x3800,
    0x6000, 0x6000, 0x0c00, 0x3600, 0x6000, 0x0060, 0x1800, 0x0000,
    0x0000, 0x0000, 0x3000, 0x0000, 0x0000, 0x0018, 0x1818, 0xdc18,
    0xcccc, 0x00c3, 0x0000, 0x3000, 0xc300, 0x0000, 0xc600, 0x3830,
    0x0000, 0x6ccc, 0xcce0, 0xcce0, 0xcc18, 0x0018, 0x6ccc, 0xcc1b,
    0x0000, 0x1c1c, 0xf800, 0x6c6c, 0x0000, 0x00c6, 0xc618, 0x33cc,
    0x8800, 0x7718, 0x1818, 0x3600, 0x0036, 0x3600, 0x36c3, 0xc35b,
    0x3c60, 0x6662, 0x6c62, 0xdb66, 0x6666, 0x663e, 0xee66, 0x6666,
    0x6666, 0x5a66, 0x7ec6, 0x6666, 0xd6d6, 0x60c3, 0x6066, 0xdb66,
    0x003e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x1800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0066, 0x24f6, 0x6fc3, 0xfe99, 0x06d3, 0x3c16, 0xfedf, 0x05a0,
    0xc606, 0x0606, 0xc6c0, 0xc006, 0xc6c6, 0x3c78, 0x1fec, 0x0dd8,
    0x003c, 0x66fe, 0x60cc, 0x3830, 0x300c, 0x3c18, 0x0000, 0x0018,
    0xce18, 0x0606, 0x6c7c, 0x6006, 0x6666, 0x1818, 0x187e, 0x1806,
    0xde66, 0x6660, 0x6668, 0x6860, 0x6618, 0x0c6c, 0x60fe, 0x7e66,
    0x6666, 0x6660, 0x1866, 0x66c6, 0x6c66, 0x8c18, 0x3018, 0x6c00,
    0x3078, 0x603c, 0x0c3c, 0x3076, 0x6c38, 0x0666, 0x18cc, 0x7c3c,
    0xdc76, 0xdc3e, 0x7ccc, 0x66c6, 0xc666, 0x7e18, 0x1818, 0x0018,
    0xc000, 0x783c, 0x7878, 0x7878, 0x3c78, 0x7870, 0x3870, 0x6c00,
    0xfc7f, 0xcc00, 0x0000, 0x0000, 0x003c, 0xcc7e, 0x6478, 0xcc18,
    0x7870, 0x0000, 0x00cc, 0x6c6c, 0x3000, 0x00cc, 0xcc00, 0x6666,
    0x223e, 0xdb18, 0x18f8, 0x3600, 0xf8f6, 0x36fe, 0xf6bd, 0xbd5f,
    0x6660, 0x6660, 0x6c68, 0x7e06, 0x6e6e, 0x6c66, 0xfe66, 0x6666,
    0x6660, 0x1866, 0xdb6c, 0x6666, 0xd6d6, 0x60c3, 0x6006, 0xdb66,
    0x7860, 0x7c3e, 0x1e3c, 0xdb7c, 0xcccc, 0x661e, 0xcc66, 0x3c7e,
    0x7c3c, 0x7e66, 0x7ec6, 0x6666, 0xd6d6, 0xe0c6, 0x607c, 0xce3e,
    0x00c3, 0xe783, 0xc1e7, 0xfc3c, 0x8cd3, 0x3c10, 0xd8db, 0x0db0,
    0x0000, 0x7c7c, 0x7c7c, 0x7c00, 0x7c7c, 0x0660, 0x1804, 0x0628,
    0x0018, 0x006c, 0x3c18, 0x7600, 0x300c, 0xff7e, 0x007e, 0x0030,
    0xde18, 0x1c1c, 0xcc06, 0x7c0c, 0x3c3e, 0x0000, 0x3000, 0x0c0c,
    0xde66, 0x7c60, 0x6678, 0x7860, 0x7e18, 0x0c78, 0x60d6, 0x7e66,
    0x7c66, 0x7c3c, 0x1866, 0x66d6, 0x383c, 0x1818, 0x1818, 0xc600,
    0x000c, 0x7c66, 0x7c66, 0x78cc, 0x7618, 0x066c, 0x18fe, 0x6666,
    0x66cc, 0x7660, 0x30cc, 0x66d6, 0x6c66, 0x4c70, 0x180e, 0x0034,
    0xcccc, 0xcc06, 0x0c0c, 0x0cc0, 0x66cc, 0xcc30, 0x1830, 0xc678,
    0x660c, 0xfe78, 0x7878, 0xcccc, 0xcc66, 0xccc0, 0xf0fc, 0xfa3c,
    0x0c30, 0x78cc, 0xf8ec, 0x3e38, 0x60fc, 0xfcde, 0xdb18, 0xcc33,
    0x8866, 0xee18, 0x1818, 0x3600, 0x1806, 0x3606, 0x06b1, 0xa555,
    0x667c, 0x7c60, 0x6c78, 0x7e1c, 0x7e7e, 0x7866, 0xd67e, 0x6666,
    0x7c60, 0x183e, 0xdb38, 0x667e, 0xdada, 0x7cfb, 0x7c1e, 0xfb3e,
    0x0c7c, 0x6630, 0x3666, 0xdb06, 0xcccc, 0x6c36, 0xfe66, 0x6666,
    0x6666, 0x1866, 0xdb6c, 0x6666, 0xd6d6, 0x60c6, 0x6006, 0xdb66,
    0x00e7, 0xc383, 0xc1c3, 0xf999, 0xd8db, 0x7e10, 0xdeff, 0x0db0,
    0xc606, 0xc006, 0x0606, 0xc606, 0xc606, 0x7e7e, 0x1804, 0x07d0,
    0x0018, 0x006c, 0x0630, 0xdc00, 0x300c, 0x3c18, 0x0000, 0x0060,
    0xf618, 0x3006, 0xfe06, 0x6618, 0x6606, 0x0000, 0x1800, 0x1818,
    0xde7e, 0x6660, 0x6668, 0x686e, 0x6618, 0xcc6c, 0x62c6, 0x6e66,
    0x606e, 0x6c06, 0x1866, 0x66fe, 0x3818, 0x3218, 0x0c18, 0x0000,
    0x007c, 0x6660, 0xcc7e, 0x30cc, 0x6618, 0x0678, 0x18fe, 0x6666,
    0x66cc, 0x663c, 0x30cc, 0x66fe, 0x3866, 0x1818, 0x0018, 0x0034,
    0x78cc, 0xfc3e, 0x7c7c, 0x7c70, 0x7efc, 0xfc30, 0x1830, 0xfecc,
    0x787f, 0xcccc, 0xcccc, 0xcccc, 0xcc66, 0xccc0, 0x6030, 0xc618,
    0x7c30, 0xcccc, 0xccfc, 0x0000, 0xc0c0, 0x0c33, 0x3718, 0x6666,
    0x223e, 0xdb18, 0xf8f8, 0xf6fe, 0xf8f6, 0x36f6, 0xfeb1, 0xb951,
    0x7e66, 0x6660, 0x6c68, 0xdb06, 0x7676, 0x6c66, 0xc666, 0x6666,
    0x6060, 0x1806, 0x7e38, 0x6606, 0xdada, 0x66df, 0x6606, 0xdb36,
    0x7c66, 0x7c30, 0x367e, 0x7e1c, 0xcccc, 0x7836, 0xfe7e, 0x6666,
    0x6660, 0x1866, 0xdb38, 0x667e, 0xdada, 0x7cfa, 0x7c3e, 0xf33e,
    0x0024, 0x66f6, 0x6f99, 0xf3c3, 0x70c3, 0x1070, 0x181e, 0x1998,
    0xc606, 0xc006, 0x0606, 0xc606, 0xc606, 0x6618, 0x1004, 0x2e10,
    0x0000, 0x00fe, 0x7c66, 0xcc00, 0x1818, 0x6618, 0x3000, 0x18c0,
    0xe618, 0x6666, 0x0c66, 0x6618, 0x660c, 0x1818, 0x0c7e, 0x3000,
    0xc066, 0x6666, 0x6c62, 0x6066, 0x6618, 0xcc66, 0x66c6, 0x6666,
    0x603c, 0x6666, 0x1866, 0x3cee, 0x6c18, 0x6618, 0x0618, 0x0000,
    0x00cc, 0x6666, 0xcc60, 0x307c, 0x6618, 0x666c, 0x18d6, 0x6666,
    0x7c7c, 0x6006, 0x34cc, 0x3cfe, 0x6c3e, 0x3218, 0x1818, 0x0062,
    0x18cc, 0xc066, 0xcccc, 0xcc78, 0x60c0, 0xc030, 0x1830, 0xc6fc,
    0x60cc, 0xcccc, 0xcccc, 0xcccc, 0x7c3c, 0xcc78, 0xe6fc, 0xcf18,
    0xcc30, 0xcccc, 0xccdc, 0x7e7c, 0xccc0, 0x0c66, 0x6f18, 0x33cc,
    0x8836, 0x7718, 0x1818, 0x3636, 0x1836, 0x3636, 0x00bd, 0xad00,
    0x6666, 0x6660, 0x6c62, 0xdb66, 0x6666, 0x6666, 0xc666, 0x6666,
    0x6066, 0x1806, 0x186c, 0x6606, 0xdada, 0x66cf, 0x6666, 0xdb66,
    0xcc66, 0x6630, 0x3660, 0xdb06, 0xcccc, 0x6c36, 0xd666, 0x6666,
    0x6666, 0x183e, 0xdb6c, 0x6606, 0xdada, 0x66ce, 0x6606, 0xdb36,
    0x0024, 0x3c1c, 0x383c, 0xe7e7, 0x20c3, 0x38f0, 0x181b, 0x799e,
    0x7c06, 0x7c7c, 0x067c, 0x7c06, 0x7c7c, 0x3c1e, 0x1e3c, 0x39e0,
    0x0018, 0x006c, 0x18c6, 0x7600, 0x0c30, 0x0000, 0x3000, 0x1880,
    0x7c7e, 0x7e3c, 0x1e3c, 0x3c18, 0x3c38, 0x1818, 0x0600, 0x6018,
    0x7866, 0xfc3c, 0xf8fe, 0xf03e, 0x663c, 0x78e6, 0xfec6, 0x663c,
    0xf00e, 0xe63c, 0x3c7e, 0x18c6, 0xc63c, 0xfe1e, 0x0278, 0x00fe,
    0x0076, 0xdc3c, 0x763c, 0x780c, 0xe63c, 0x66e6, 0x3cc6, 0x663c,
    0x600c, 0xf07c, 0x1876, 0x186c, 0xc606, 0x7e0e, 0x1870, 0x007e,
    0x0c7e, 0x783f, 0x7e7e, 0x7e0c, 0x3c78, 0x7878, 0x3c78, 0xc6cc,
    0xfc7f, 0xce78, 0x7878, 0x7e7e, 0x0c18, 0x7818, 0xfc30, 0xc6d8,
    0x7e78, 0x787e, 0xcccc, 0x0000, 0x7800, 0x00cc, 0xcf18, 0x0000,
    0x2266, 0xdb18, 0x1818, 0x3636, 0x1836, 0x3636, 0x00c3, 0xc300,
    0x667c, 0xfc60, 0xfffe, 0xdb3c, 0x6666, 0xe6e6, 0xc666, 0x3c66,
    0xf03c, 0x3c7c, 0x3cc6, 0x7f06, 0xfeff, 0x7cfb, 0x7c3c, 0xce67,
    0x763c, 0x7c30, 0x7f3c, 0xdb7c, 0x7676, 0x6676, 0xc666, 0x3c66,
    0x7c3c, 0x1806, 0x7ec6, 0x7f06, 0xfeff, 0x7cfa, 0x7c7c, 0xce66,
    0x003c, 0x1818, 0x1800, 0x0000, 0x007e, 0x1060, 0x0000, 0x718e,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1754, 0x3800,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0030, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00f8, 0x0000, 0x3c00, 0x0000, 0x0000,
    0xf01e, 0x0000, 0x0000, 0x0000, 0x007c, 0x0000, 0x1800, 0x0000,
    0x7800, 0x0000, 0x0000, 0x0038, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xf800, 0x0018, 0x0030, 0xc770,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000f, 0x0300, 0x0000,
    0x8800, 0xee18, 0x1818, 0x3636, 0x1836, 0x3636, 0x007e, 0x7e00,
    0x0000, 0x0000, 0xc300, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0300, 0x0003, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x6300, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x6000, 0x007c, 0x1800, 0x0300, 0x0003, 0x0000, 0x0000, 0x0000,
};

const Fonthead fnt_ru_8x8 = {
    1,  /* font_id */
    9,  /* point */
    "8x8 Russian font",  /*   char name[32]     */
    0,  /* first_ade */
    255,  /* last_ade */
    6,  /* top */
    6,  /* ascent */
    4,  /* half */
    1,  /* descent */
    1,  /* bottom */
    8,  /* max_char_width */
    8,  /* max_cell_width */
    1,  /* left_offset */
    3,  /* right_offset */
    1,  /* thicken */
    1,  /* ul_size */
    0x5555, /* lighten */
    0x5555, /* skew */
    F_STDFORM | F_MONOSPACE | F_DEFAULT,  /* flags */
    0,                  /*   UBYTE *hor_table   */
    off_8x8_table,      /*   UWORD *off_table   */
    dat_table,          /*   UWORD *dat_table   */
    256,  /* form_width */
    8,  /* form_height */
    0,  /* Fonthead * next_font */
    0                   /*   reserved by Atari  */
};
