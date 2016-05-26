/*
 * keyb_no.h - Norwegian keyboard layout definition
 *
 * Copyright (C) 2016 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_no_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '+', 0x82,    8, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p', 0x86, 0x81, 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0xb3,
    0x91, '\'',    0, '\\',  'z',  'x',  'c',  'v',
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

static const UBYTE keytbl_no_shft[] = {
       0, 0x1b,  '!', '\"',  '#',  '$',  '%',  '&',
     '/',  '(',  ')',  '=',  '?', 0x90,    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', 0x8f, 0x9a, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xb2,
    0x92,  '*',    0,  '|',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '>',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_no_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '+', 0x90,    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', 0x8f, 0x9a, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xb2,
    0x92, '\'',    0,  '|',  'Z',  'X',  'C',  'V',
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

static const UBYTE keytbl_no_altnorm[] = {
    0x1a, '[',
    0x1b, ']',
    0x28, '`',
    0x2b, '^',
    0,
};

static const UBYTE keytbl_no_altshft[] = {
    0x1a, '{',
    0x1b, '}',
    0x28, '~',
    0x2b, '@',
    0,
};

static const UBYTE keytbl_no_altcaps[] = {
    0x1a, '[',
    0x1b, ']',
    0x28, '`',
    0x2b, '^',
    0,
};

static const struct keytbl keytbl_no = {
    keytbl_no_norm,
    keytbl_no_shft,
    keytbl_no_caps,
    keytbl_no_altnorm,
    keytbl_no_altshft,
    keytbl_no_altcaps,
    NULL,
    0
};
