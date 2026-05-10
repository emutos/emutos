/*
 * keyb_cz.h - Czech keyboard layout definition
 *
 * Copyright (C) 2002-2015 The EmuTOS development team
 *
 * Authors:
 *  Jan Krupka
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_cz_norm[] = {
       0, 0x1b,  '+', 0x88, 0xa8, 0x87, 0xa9, 0x91,
    0x98, 0xa0, 0xa1, 0x82,  '=', DEAD(0), 0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'z',  'u',  'i',
     'o',  'p', 0xa3, 0x83, 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0x96,
    0x9f,  '#',    0,  '~',  'y',  'x',  'c',  'v',
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
     '7',  '8',  '9',  '0',  '?',  DEAD(1), 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',
     'O',  'P', 0x97, 0x85, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xa6,
    0x86,  '^',    0,  '|',  'Y',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '_',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '_',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '>',    0,    0,  '{',  '}',  '/',  '*', 0x53,
     '[',  ']',  '$',  '%',  '&',  '!',  '"',  '#',
     '=',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_cz_caps[] = {
       0, 0x1b,  '!', 0x89, 0x9b, 0x80, 0x9e, 0x92,
    0x9d, 0x8f, 0x8b, 0x90,  '=', 0x27, 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Z',  'U',  'I',
     'O',  'P', 0x97, 0x85, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xa6,
    0x86,  '#',    0,  '~',  'Y',  'X',  'C',  'V',
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
    0x1a, '@',
    0x27, '[',
    0x28, ']',
    0,
};

static const UBYTE keytbl_cz_altshft[] = {
    0x1a, '\\',
    0x27, '{',
    0x28, '}',
    0,
};

static const UBYTE keytbl_cz_altcaps[] = {
    0x1a, '@',
    0x27, '[',
    0x28, ']',
    0,
};


static const UBYTE keytbl_cz_dead0[] = {
     'a', 0xa0,
     'e', 0x82,
     'i', 0xa1,
     'o', 0xa2,
     'u', 0xa3,
     'l', 0x8c,
     'r', 0xa9,
     'u', 0xa3,
     'y', 0x98,

     'A', 0x8f,
     'E', 0x90,
     'I', 0x8b,
     'O', 0x95,
     'U', 0x97,
     'L', 0x8a,
     'R', 0x9e,
     'Y', 0x9d,

     ' ', 0x27,
     DEAD(0), 0x27, 
     0
};

static const UBYTE keytbl_cz_dead1[] = {
    'a', 0x84,
    'c', 0x87,
    'd', 0x83, 
    'e', 0x88,
    'l', 0x8c, 
    'n', 0xa4,
    'o', 0x93,
    'r', 0xa9, 
    's', 0xa8, 
    't', 0x9f,
    'u', 0x96, 
    'z', 0x91,

    'A', 0x8f,
    'C', 0x80,
    'D', 0x85, 
    'E', 0x89,
    'L', 0x9c, 
    'N', 0xa5,
    'O', 0xa7,
    'R', 0x9e, 
    'S', 0x9b, 
    'T', 0x86,
    'U', 0xa6, 
    'Z', 0x92,

    ' ', 0x60, 
    DEAD(1), 0x60,
    0
};


static const UBYTE * const keytbl_cz_dead[] = {
    keytbl_cz_dead0,
    keytbl_cz_dead1,
};

static const struct keytbl keytbl_cz = {
    keytbl_cz_norm,
    keytbl_cz_shft,
    keytbl_cz_caps,
    keytbl_cz_altnorm,
    keytbl_cz_altshft,
    keytbl_cz_altcaps,
    keytbl_cz_dead,
    0
};

