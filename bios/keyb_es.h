 /*
 * keyb_es.h - spanish keyboard layout definition
 *
 * Copyright (C) 2011-2020 The EmuTOS development team
 *
 * Authors:
 *  L. Zanier
 *  Th. Otto
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_es_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=', 0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p', '\'',  '`', 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0xa4,
     ';', 0x87,    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.', 0xf8,    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '<',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static const UBYTE keytbl_es_shft[] = {
       0, 0x1b, 0xad, 0xa8, 0x9c,  '$',  '%',  '/',
     '&',  '*',  '(',  ')',  '_',  '+', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '"',  '^', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xa5,
     ':',  '~',    0,  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '?',  '!', 0xdd,    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '>',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

static const UBYTE keytbl_es_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', '\'',  '`', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xa5,
     ';', 0x87,    0, '\\',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.', 0xf8,    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '<',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0
};

/* Alt Tables format as defined in SpareMiNT Wiki */

static const UBYTE keytbl_es_altnorm[] = {
    0x1A, '[',
    0x1B, ']',
    0x2B,0x23,
    0x28,0x81,
    0x27,0x00,
    0,
};

static const UBYTE keytbl_es_altshft[] = {
    0x1A, '{',
    0x1B, '}',
    0x2B,0x40,
    0x28,0x00,
    0x27,0x00,
    0,
};

static const UBYTE keytbl_es_altcaps[] = {
    0x1A, '[',
    0x1B, ']',
    0x2B,0x23,
    0x28,0x81,
    0x27,0x00,
    0,
};

static const struct keytbl keytbl_es = {
    keytbl_es_norm,
    keytbl_es_shft,
    keytbl_es_caps,
    keytbl_es_altnorm,
    keytbl_es_altshft,
    keytbl_es_altcaps,
    NULL,
    0
};
