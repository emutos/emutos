/*
 * keyb_hu.h - a keyboard layout definition
 *
 * Copyright (C) 2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_hu_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', 0xf6, 0xfc, 0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'z',  'u',  'i',
     'o',  'p', 0xf5, 0xfa, 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0xe9,
    0xe1, 0xf3,    0, 0xfb,  'y',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '-',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    0xed,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  ',', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static const UBYTE keytbl_hu_shft[] = {
       0, 0x1b, 0xba,  '"',  '+',  '!',  '%',  '/',
     '=',  '(',  ')', 0xa7, 0xd6, 0xdc, 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',
     'O',  'P', 0xd5, 0xda, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xc9,
    0xc1, 0xd3,    0, 0xdb,  'Y',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    0xcd,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  ',', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static const UBYTE keytbl_hu_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', 0xd6, 0xfc, 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',
     'O',  'P', 0xd5, 0xda, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xc9,
    0xc1, 0xd3,    0, 0xdb,  'Y',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '-',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    0xcd,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  ',', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static const UBYTE keytbl_hu_altnorm[] = {
    0x02, '~',     /* U+007E: Ascii tilde */
    0x03, 0xb7,    /* U+02C7: Caron */
    0x04, '^',     /* U+005E: Circumflex accent */
    0x05, 0xa2,    /* U+02D8: Breve */
    0x06, 0xb0,    /* U+00B0: Degree sign */
    0x07, 0xb2,    /* U+02DB: Ogonek */
    0x08, '`',     /* U+0060: Grave accent */
    0x09, 0xff,    /* U+02D9: Dot above */
    0x0a, 0xb4,    /* U+00B4: Acute accent */
    0x0c, 0xbd,    /* U+02DD: Double acute accent */
    0x0d, 0xa8,    /* U+00A8: Diaresis */
    0x10, 0x5c,    /* U+005C: Reverse solidus */
    0x11, '|',     /* U+007C: Vertical line */
    0x12, 0xc4,    /* U+00C4: Latin capital letter A with diaresis */
    0x16, 0x80,    /* U+20AC: Euro sign */
    0x17, 0xcd,    /* U+00CD: Latin capital letter I with acute */
    0x1a, 0xf7,    /* U+00F7: Division sign */
    0x1b, 0xd7,    /* U+00D7: Multiplication sign */
    0x1e, 0xe4,    /* U+00E4: Latin small letter a with diaresis */
    0x1f, 0xf0,    /* U+0111: Latin small letter d with stroke */
    0x20, 0xd0,    /* U+0110: Latin capital letter D with stroke */
    0x21, '[',     /* U+005B: Left square bracket */
    0x22, ']',     /* U+005D: Right square bracket */
    0x24, 0xed,    /* U+00ED: Latin small letter i with acute */
    0x25, 0xb3,    /* U+0142: Latin small letter l with stroke */
    0x26, 0xa3,    /* U+0141: Latin capital letter L with stroke */
    0x27, '$',     /* U+0024: Dollar sign */
    0x28, 0xdf,    /* U+00DF: Latin small letter sharp s */
    0x29, 0xb8,    /* U+00B8: Cedilla */
    0x2b, 0xa4,    /* U+00A4: Currency sign */
    0x2c, '>',     /* U+003E: Greater than sign */
    0x2d, '#',     /* U+0023: Number sign */
    0x2e, '&',     /* U+0026: Ampersand */
    0x2f, '@',     /* U+0040: Commercial at */
    0x30, '{',     /* U+007B: Left curly bracket */
    0x31, '}',     /* U+007D: Right curly bracket */
    0x32, '<',     /* U+003C: Less than sign */
    0x33, ';',     /* U+003B: Semicolon */
    0x34, '>',     /* U+003E: Greater than sign */
    0x35, '*',     /* U+002A: Asterisk */
    0x60, '<',     /* U+003C: Less than sign */
    0
};

static const UBYTE keytbl_hu_altshft[] = {
    0
};

static const UBYTE keytbl_hu_altcaps[] = {
    0
};


static const struct keytbl keytbl_hu = {
    keytbl_hu_norm,
    keytbl_hu_shft,
    keytbl_hu_caps,
    keytbl_hu_altnorm,
    keytbl_hu_altshft,
    keytbl_hu_altcaps,
    NULL,
    0
};
