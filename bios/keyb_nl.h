/*
 * keyb_nl.h - a keyboard layout definition
 *
 * Copyright (C) 2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_nl_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']', 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0,  '#',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    '\\',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_nl_shft[] = {
       0, 0x1b,  '!', '\"', 0x9c,  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '{',  '}', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
     '@', 0xff,    0,  '~',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '|',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_nl_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '[',  ']', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',
    '\'',  '`',    0,  '#',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    '\\',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_nl_altnorm[] = {
    0,
};

static const UBYTE keytbl_nl_altshft[] = {
    0,
};

static const UBYTE keytbl_nl_altcaps[] = {
    0,
};

static const struct keytbl keytbl_nl = {
    keytbl_nl_norm,
    keytbl_nl_shft,
    keytbl_nl_caps,
    keytbl_nl_altnorm,
    keytbl_nl_altshft,
    keytbl_nl_altcaps,
    NULL,
};
