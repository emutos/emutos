/*
 * keyb_it.h - keyboard layout definition
 *
 * Copyright (C) 2011-2015 The EmuTOS development team
 *
 * Authors:
 *  L. Zanier
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_it_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', '\'', 0x8d, 0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p', 0x8a,  '+', 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0x95,
    0x85, 0x97,    0, '\\',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '-',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '<',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_it_shft[] = {
       0, 0x1b,  '!',  '"', 0x9c,  '$',  '%',  '&',
     '/',  '(',  ')',  '=',  '?',  '^', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', 0x82,  '*', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  '@',
     '#', 0xdd,    0,  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '>',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_it_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', '\'', 0x8d, 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', 0x8a,  '+', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0x95,
    0x85, 0x97,    0, '\\',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '-',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '<',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

/* Alt Tables iaw Keyboard.tbl format as defined in SpareMiNT Wiki */

static const UBYTE keytbl_it_altnorm[] = {
    0x1A, '[',
    0x1B, ']',
    0x2B,0xf8,
    0,
};

static const UBYTE keytbl_it_altshft[] = {
    0x1A, '{',
    0x1B, '}',
    0x2B,0x7e,
    0,
};

static const UBYTE keytbl_it_altcaps[] = {
    0x1A, '[',
    0x1B, ']',
    0x2B,0xf8,
    0,
};

static const struct keytbl keytbl_it = {
    keytbl_it_norm,
    keytbl_it_shft,
    keytbl_it_caps,
    keytbl_it_altnorm,
    keytbl_it_altshft,
    keytbl_it_altcaps,
    NULL,
    0
};
