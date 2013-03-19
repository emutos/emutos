/*
 * keyb_fr.h - a keyboard layout definition
 *
 * Copyright (c) 2001 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const BYTE keytbl_fr_norm[] = {
       0, 0x1b,  '&', 0x82, '\"', '\'',  '(', 0xdd,
    0x8a,  '!', 0x87, 0x85,  ')',  '-',    8, 0x09,
     'a',  'z',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  DEAD(0) /*'^'*/ ,  '$', 0x0d,    0,  'q',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  'm',
    0x97,  '`',    0,  '#',  'w',  'x',  'c',  'v',
     'b',  'n',  ',',  ';',  ':',  '=',    0,    0,
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

static const BYTE keytbl_fr_shft[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', 0xf8,  '_',    8, 0x09,
     'A',  'Z',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', /*0xb9*/ DEAD(1),  '*', 0x0d,    0,  'Q',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  'M',
     '%', 0x9c,    0,  '|',  'W',  'X',  'C',  'V',
     'B',  'N',  '?',  '.',  '/',  '+',    0,    0,
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

static const BYTE keytbl_fr_caps[] = {
       0, 0x1b,  '&', 0x90, '\"', '\'',  '(', 0xdd,
    0x8a,  '!', 0x80, 0xb6,  ')',  '-',    8, 0x09,
     'A',  'Z',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  /*'^'*/ DEAD(0),  '$', 0x0d,    0,  'Q',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  'M',
    0x97,  '`',    0,  '#',  'W',  'X',  'C',  'V',
     'B',  'N',  ',',  ';',  ':',  '=',    0,    0,
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

static const BYTE keytbl_fr_altnorm[] = {
    0x1a, '[',
    0x1b, ']',
    0x28, '\\',
    0x2b, '@',
    0,
};

static const BYTE keytbl_fr_altshft[] = {
    0x1a, '{',
    0x1b, '}',
    0x2b, '~',
    0,
};

static const BYTE keytbl_fr_altcaps[] = {
    0x1a, '[',
    0x1b, ']',
    0x28, '\\',
    0x2b, '@',
    0,
};

static const BYTE keytbl_fr_dead0[] = {
    'a', 0x83, 'e', 0x88, 'i', 0x8c, 'o', 0x93, 'u', 0x96,
    ' ', '^', DEAD(0), '^', 0
};

static const BYTE keytbl_fr_dead1[] = {
    'A', 0x8e, 'O', 0x99, 'U', 0x9a,
    'a', 0x84, 'e', 0x89, 'i', 0x8b, 'o', 0x94, 'u', 0x81, 'y', 0x98,
    ' ', 0xb9, DEAD(1), 0xb9, 0
};

static const BYTE * const keytbl_fr_dead[] = {
    keytbl_fr_dead0,
    keytbl_fr_dead1,
};

static const struct keytbl keytbl_fr = {
    keytbl_fr_norm,
    keytbl_fr_shft,
    keytbl_fr_caps,
    keytbl_fr_altnorm,
    keytbl_fr_altshft,
    keytbl_fr_altcaps,
    keytbl_fr_dead
};
