/*
 * fnt_ru_6x6.c - 6x6 font for Russian language
 *
 * Copyright (C) 2010-2024 The EmuTOS development team
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
    0x0082, 0x0421, 0xcfb6, 0x0de3, 0x04e3, 0x8150, 0xf987, 0xbcc3,
    0xcc3e, 0x73e0, 0x381f, 0x8442, 0x00cd, 0x947b, 0x260c, 0x3184,
    0x8800, 0x0006, 0x704f, 0x3c33, 0xc73e, 0x71c3, 0x0c18, 0x061c,
    0x71cf, 0x1ef3, 0xef9e, 0x89c0, 0x9242, 0x289c, 0xf1cf, 0x1efa,
    0x28a2, 0x8a2f, 0x9ec1, 0xe200, 0x6008, 0x0008, 0x0180, 0x8001,
    0x2060, 0x0000, 0x0000, 0x0020, 0x0000, 0x0000, 0x0e31, 0xc400,
    0x7941, 0x0851, 0x0200, 0x2144, 0x1421, 0x0508, 0x2007, 0x8851,
    0x0210, 0x5145, 0x041a, 0x2f06, 0x1041, 0x04f1, 0xe71c, 0x6000,
    0x30c0, 0xc36c, 0x6803, 0x4201, 0xe41a, 0x6941, 0x0869, 0xe7bd,
    0x73ef, 0x3e3b, 0xeb7c, 0x8aa4, 0x8e8a, 0x273e, 0xf1ef, 0xa2fa,
    0x28a2, 0x8a2c, 0x2241, 0xc99e, 0x00ce, 0x0070, 0x001c, 0x01c0,
    0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000,
    0x01c2, 0x0662, 0xaf2a, 0x1a17, 0x8682, 0x0150, 0xc880, 0x84c2,
    0x0c02, 0x5367, 0x203f, 0x42f4, 0x00cd, 0xbea3, 0x4d0c, 0x60c3,
    0x0800, 0x000c, 0x98c0, 0x8252, 0x0802, 0x8a23, 0x0c31, 0xe326,
    0x8a28, 0xa08a, 0x0820, 0x8880, 0x9443, 0x6ca2, 0x8a28, 0xa022,
    0x28a2, 0x5221, 0x1860, 0x6700, 0x61cf, 0x1c79, 0xc21e, 0xb181,
    0x2421, 0x4f1c, 0xf1e7, 0x0e72, 0x28a2, 0x4a27, 0x8c30, 0xce88,
    0x8002, 0x1400, 0x801e, 0x5002, 0x0050, 0x8000, 0xfbca, 0x1400,
    0x8508, 0x0000, 0x0e23, 0x6d88, 0x2082, 0x0800, 0x00a2, 0x0000,
    0x30c0, 0x06f6, 0xb1e4, 0x8c72, 0xc22c, 0xb002, 0x1ceb, 0x38d7,
    0x8a08, 0xa04a, 0x0fc2, 0x9a65, 0x12da, 0x28a2, 0x8a02, 0x22a9,
    0x48a2, 0x8a24, 0x2240, 0x2a72, 0x730a, 0x3e09, 0xcb42, 0x8809,
    0x0e52, 0x273c, 0xf1cf, 0xa472, 0x4912, 0x8a2e, 0x2241, 0xc99e,
    0x0362, 0x3bdf, 0x6e1c, 0xb297, 0x84de, 0xe150, 0xc88f, 0xbec3,
    0xef8e, 0x7320, 0xb760, 0x6294, 0x00c9, 0x1470, 0x8618, 0x60c7,
    0xbe01, 0xe018, 0xa847, 0x1c93, 0xcf04, 0x71e0, 0x0060, 0x018c,
    0xbbef, 0x208b, 0xcf26, 0xf880, 0x9842, 0xaaa2, 0xf22f, 0x1c22,
    0x28aa, 0x2142, 0x1830, 0x6d80, 0x3028, 0xa08b, 0xe7a2, 0xc881,
    0x3823, 0xe8a2, 0x8a24, 0x9822, 0x28aa, 0x3221, 0x1830, 0x6b9c,
    0x8227, 0x1c71, 0xc720, 0x71c7, 0x1821, 0x871c, 0x80ef, 0x9c71,
    0xc8a2, 0x89c8, 0x9871, 0xcf1e, 0x7187, 0x22f1, 0x2fa2, 0x61e7,
    0xb6cc, 0xcd9b, 0x7325, 0x96ba, 0xe71c, 0x7000, 0x08ea, 0xdb55,
    0xfbcf, 0x204b, 0xc79c, 0xaaa6, 0x12ab, 0xe8a2, 0xf202, 0x1ea8,
    0x889e, 0xaaa7, 0xb279, 0xee5e, 0x8bcf, 0x207b, 0xe78c, 0x8a2e,
    0x12fa, 0x28a2, 0x8a02, 0x24a9, 0x8912, 0xaaa3, 0xb270, 0xae72,
    0x008d, 0x8662, 0xacaa, 0xe2df, 0xdc93, 0xa358, 0xd9cc, 0x06d8,
    0x698c, 0xdbef, 0xa440, 0x2168, 0x00c0, 0x3e29, 0x6e80, 0x60c3,
    0x0830, 0x0330, 0xc848, 0x02f8, 0x2888, 0x8823, 0x0c31, 0xe30c,
    0xb228, 0xa08a, 0x0822, 0x8888, 0x9442, 0x29a2, 0x822a, 0x0222,
    0x2536, 0x5084, 0x1818, 0x6000, 0x03e8, 0xa08a, 0x021e, 0x8881,
    0x2422, 0xa8a2, 0x8a24, 0x0622, 0x252a, 0x31e2, 0x0c30, 0xc132,
    0x822f, 0x8208, 0x20a0, 0xfbef, 0x8820, 0x88a2, 0xf38a, 0x228a,
    0x28a2, 0x7a28, 0x8e20, 0x8d88, 0x0888, 0xa289, 0xa79c, 0x6100,
    0x8b14, 0xc6f6, 0x09e6, 0x9aa2, 0xc8a2, 0x8800, 0x086a, 0xf8c0,
    0x8a28, 0xa04a, 0x0b42, 0xcb25, 0x128a, 0x28a2, 0x8202, 0x02f9,
    0x4882, 0xaaa4, 0x6a44, 0x2a4a, 0xfa29, 0x208a, 0x0fc2, 0x8a29,
    0x12ab, 0xe8a2, 0x8a02, 0x1ca9, 0x890e, 0xaaa2, 0x6a48, 0x6e5e,
    0x0087, 0x0421, 0xc9b6, 0x4210, 0x3c18, 0xe75c, 0xd9cc, 0x06f8,
    0x6d8c, 0xd867, 0x3c71, 0xeef0, 0x0000, 0x14f2, 0x6d00, 0x3184,
    0x8830, 0x0320, 0x704f, 0xbc13, 0xc708, 0x71c3, 0x0418, 0x0600,
    0x822f, 0x1ef3, 0xe81e, 0x89c7, 0x127a, 0x289c, 0x81c9, 0xbc21,
    0xe222, 0x888f, 0x9e09, 0xe000, 0x01ef, 0x1c79, 0xc202, 0x89c1,
    0x2272, 0x289c, 0xf1e4, 0x1c11, 0xe236, 0x4827, 0x8e31, 0xc03e,
    0x7a28, 0x3efb, 0xef9e, 0x8208, 0x0820, 0x8fbe, 0x81eb, 0xa28a,
    0x28a2, 0x0a28, 0x8479, 0xcf08, 0xf888, 0xa289, 0x6000, 0xc900,
    0x863c, 0xc36c, 0xfb24, 0x8c79, 0xefbe, 0x8800, 0x082b, 0x1a40,
    0x8bcf, 0x20ff, 0xeb7c, 0x8a24, 0xb28a, 0x2722, 0x81e2, 0x1c22,
    0x2fc2, 0xfbf7, 0xb279, 0xc9b2, 0x89cf, 0x2071, 0xcb5c, 0x75d8,
    0xb28a, 0x2722, 0xf1c2, 0x0472, 0x4782, 0xfbf3, 0xb271, 0xc9b2,
    0x0082, 0x0000, 0x0000, 0x01e3, 0x1810, 0xb64c, 0xf9cf, 0xbe1b,
    0xef8c, 0xf860, 0x0758, 0xac00, 0x00c0, 0x0020, 0x0680, 0x0000,
    0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0800, 0x000c,
    0x7800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0060, 0x0000,
    0x0000, 0x0000, 0x0000, 0x003e, 0x0000, 0x0000, 0x003c, 0x000e,
    0x0000, 0x0000, 0x8020, 0x0000, 0x0000, 0x03c0, 0x0030, 0x0000,
    0xc1e7, 0x1e79, 0xe7b8, 0x71c7, 0x1c71, 0xc8a2, 0xf800, 0x1c71,
    0xc79e, 0xf1c7, 0x8000, 0x8c30, 0x79c7, 0x1e89, 0x2fbe, 0x7000,
    0x0f04, 0xc000, 0x780b, 0x1000, 0x08a2, 0x7000, 0x0029, 0xe780,
    0x0000, 0x0084, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0040, 0x0010, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x8000, 0x3820, 0x0080, 0x0010, 0x0000, 0x0000,
};

const Fonthead fnt_ru_6x6 = {
    1,  /* font_id */
    8,  /* point */
    "6x6 Russian font",  /*   char name[32]     */
    0,  /* first_ade */
    255,  /* last_ade */
    4,  /* top */
    4,  /* ascent */
    3,  /* half */
    1,  /* descent */
    1,  /* bottom */
    6,  /* max_char_width */
    6,  /* max_cell_width */
    0,  /* left_offset */
    3,  /* right_offset */
    1,  /* thicken */
    1,  /* ul_size */
    0x5555, /* lighten */
    0xaaaa, /* skew */
    F_STDFORM | F_MONOSPACE,  /* flags */
    0,                  /*   UBYTE *hor_table   */
    off_6x6_table,      /*   UWORD *off_table   */
    dat_table,          /*   UWORD *dat_table   */
    192,  /* form_width */
    6,  /* form_height */
    0,  /* Fonthead * next_font */
    0                   /*   reserved by Atari  */
};
