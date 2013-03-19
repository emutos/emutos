/*
 * keyb_cz.h - Czech keyboard layout definition
 *
 * Copyright (c) 2002 The EmuTOS development team
 *
 * Authors:
 *  xxx   xxx
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_cz_norm[] = {
       0, 0x1b, 0xf3, 0xec, 0xb9, 0xe8, 0xf8, 0xbe,
    0xfd, 0xe1, 0xed, 0xe9,  '=', '\'',    8, 0x09,
     'q',  'w',  'e',  'r',  't',  'z',  'u',  'i',
     'o',  'p', 0xfa,  ')', 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0xf9,
     '@',  '#',    0,  '*',  'y',  'x',  'c',  'v',
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

static const UBYTE keytbl_cz_shft[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '%',  '`',    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',
     'O',  'P',  '/',  '(', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', '\"',
     '!',  '^',    0,  '&',  'Y',  'X',  'C',  'V',
     'B',  'N',  'M',  '?',  ':',  '_',    0,    0,
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

static const UBYTE keytbl_cz_caps[] = {
       0, 0x1b, 0xd3, 0xcc, 0xa9, 0xc8, 0xd8, 0xae,
    0xdd, 0xc1, 0xcd, 0xc9,  '=', '\'',    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',
     'O',  'P', 0xda,  ')', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xd9,
     '@',  '#',    0,  '*',  'Y',  'X',  'C',  'V',
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


/* TODO - the tables below were not filled by the tool */

static const UBYTE keytbl_cz_altnorm[] = {
    0,
};

static const UBYTE keytbl_cz_altshft[] = {
    0,
};

static const UBYTE keytbl_cz_altcaps[] = {
    0,
};

static const struct keytbl keytbl_cz = {
    keytbl_cz_norm,
    keytbl_cz_shft,
    keytbl_cz_caps,
    keytbl_cz_altnorm,
    keytbl_cz_altshft,
    keytbl_cz_altcaps,
    NULL
};
