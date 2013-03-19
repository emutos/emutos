/*
 * keyb_uk.h - a keyboard layout definition
 *
 * Copyright (c) 2012 David Savinkoff
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  DS    David Savinkoff
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_uk_norm[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8,    9,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']',   13,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0,  '#',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,  127,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    '\\',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.',   13,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_uk_shft[] = {
       0,   27,  '!', '\"',  156,  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',    8,    9,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '{',  '}',   13,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
     '@',  255,    0,  '~',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 127,     0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '|',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.',   13,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_uk_caps[] = {
       0,   27,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8,    9,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '[',  ']',   13,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',
    '\'',  '`',    0,  '#',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0,  127,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    '\\',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.',   13,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_uk_altnorm[] = {
    0,
};

static const UBYTE keytbl_uk_altshft[] = {
    0,
};

static const UBYTE keytbl_uk_altcaps[] = {
    0,
};

static const struct keytbl keytbl_uk = {
    keytbl_uk_norm,
    keytbl_uk_shft,
    keytbl_uk_caps,
    keytbl_uk_altnorm,
    keytbl_uk_altshft,
    keytbl_uk_altcaps,
    NULL
};
