/*
 * keyb_pl.h - a keyboard layout definition
 *
 * Copyright (C) 2020 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_pl_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',  0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']', 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_pl_shft[] = {
       0, 0x1b,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '{',  '}', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
     '"',  '~',    0,  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_pl_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '[',  ']', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',
    '\'',  '`',    0, '\\',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

/*
 * the ALT tables are taken from FreeMiNT (with Unicode info by RFB)
 */
static const UBYTE keytbl_pl_altnorm[] = {
    0x12, 0xea,     /* U+0119: Latin Small Letter e with ogonek */
    0x18, 0xf3,     /* U+00F3: Latin Small Letter o with acute */
    0x1e, 0xb1,     /* U+0105: Latin Small Letter a with ogonek */
    0x1f, 0xb6,     /* U+015B: Latin Small Letter s with acute */
    0x26, 0xb3,     /* U+0142: Latin Small Letter l with stroke */
    0x2c, 0xbf,     /* U+017C: Latin Small Letter z with dot above */
    0x2d, 0xbc,     /* U+017A: Latin Small Letter z with acute */
    0x2e, 0xe6,     /* U+0107: Latin Small Letter c with acute */
    0x31, 0xf1,     /* U+0144: Latin Small Letter n with acute */
    0,
};

static const UBYTE keytbl_pl_altshft[] = {
    0x12, 0xca,     /* U+0118: Latin Capital Letter E with ogonek */
    0x18, 0xd3,     /* U+00D3: Latin Capital Letter O with acute */
    0x1e, 0xa1,     /* U+0104: Latin Capital Letter A with ogonek */
    0x1f, 0xa6,     /* U+015A: Latin Capital Letter S with acute */
    0x26, 0xa3,     /* U+0141: Latin Capital Letter L with stroke */
    0x2c, 0xaf,     /* U+017B: Latin Capital Letter Z with dot above */
    0x2d, 0xac,     /* U+0179: Latin Capital Letter Z with acute */
    0x2e, 0xc6,     /* U+0106: Latin Capital Letter C with acute */
    0x31, 0xd1,     /* U+0143: Latin Capital Letter N with acute */
    0,
};

/* the following table is currently exactly the same as the altshft[] table */
static const UBYTE keytbl_pl_altcaps[] = {
    0x12, 0xca,
    0x18, 0xd3,
    0x1e, 0xa1,
    0x1f, 0xa6,
    0x26, 0xa3,
    0x2c, 0xaf,
    0x2d, 0xac,
    0x2e, 0xc6,
    0x31, 0xd1,
    0,
};

static const struct keytbl keytbl_pl = {
    keytbl_pl_norm,
    keytbl_pl_shft,
    keytbl_pl_caps,
    keytbl_pl_altnorm,
    keytbl_pl_altshft,
    keytbl_pl_altcaps,
    NULL,
    0
};
