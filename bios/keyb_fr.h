/*
 * keyb_fr.h - a keyboard layout definition
 *
 * Copyright (C) 2001-2017 The EmuTOS development team
 *
 * Authors:
 *  LVL   Laurent Vogel
 *  VRI   Vincent Rivière
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_fr_norm[] = {
#ifdef MACHINE_AMIGA
       0, 0x1b,  '&', 0x82, '\"', '\'',  '(', 0xdd,
    0x8a,  '!', 0x87, 0x85,  ')',  '-',    8, 0x09,
     'a',  'z',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  DEAD(0) /*'^'*/ ,  '$', 0x0d,    0,  'q',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  'm',
    0x97,  '\\',   0, 0xe6,  'w',  'x',  'c',  'v',
     'b',  'n',  ',',  ';',  ':',  '=',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,  '`',    0,    0,    0,    0,
     '<',    0,    0,  '[',  ']',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
#else
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
#endif
};

static const UBYTE keytbl_fr_shft[] = {
#ifdef MACHINE_AMIGA
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0', 0xf8,  '_',    8, 0x09,
     'A',  'Z',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', /*0xb9*/ DEAD(1),  '*', 0x0d,    0,  'Q',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  'M',
     '%',  '|',    0, 0x9c,  'W',  'X',  'C',  'V',
     'B',  'N',  '?',  '.',  '/',  '+',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,  '~',    0,    0,    0,    0,
     '>',    0,    0,  '{',  '}',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
#else
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
#endif
};

static const UBYTE keytbl_fr_caps[] = {
#ifdef MACHINE_AMIGA
       0, 0x1b,  '&', 0x90, '\"', '\'',  '(', 0xdd,
    0x8a,  '!', 0x80, 0x85,  ')',  '-',    8, 0x09,
     'A',  'Z',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  DEAD(0) /*'^'*/ ,  '$', 0x0d,    0,  'Q',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  'M',
    0x97, '\\',    0, 0xe6,  'W',  'X',  'C',  'V',
     'B',  'N',  ',',  ';',  ':',  '=',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,  '`',    0,    0,    0,    0,
     '<',    0,    0,  '[',  ']',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
#else
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
#endif
};

static const UBYTE keytbl_fr_altnorm[] = {
#ifdef MACHINE_AMIGA
    0x03, 0xfd, /* superscript 2 */
    0x04, 0xfe, /* superscript 3 */
    0x05, 0x9b, /* cent sign */
    0x06, 0xac, /* fraction 1/4 */
    0x07, 0xab, /* fraction 1/2 */
    0x0a, 0xae, /* left double angle quote or guillemet */
    0x0b, 0xaf, /* right double angle quote or guillemet */
    0x10, 0x86, /* small a ring */
    0x11, 0xf8, /* degree sign */
    0x12, 0xbd, /* copyright sign */
    0x13, 0xbe, /* registered trademark sign */
    0x16, 0xe6, /* micro sign, mu */
    0x17, 0xad, /* inverted exclamation mark */
    0x18, 0xb3, /* small o slash */
    0x19, 0xbc, /* paragraph sign, pilcrow sign */
    0x1a, '[',
    0x1b, ']',
    0x1e, 0x91, /* small ae ligature */
    0x1f, 0x9e, /* small sharp s, sz ligature */
    0x26, 0x9c, /* pound sterling sign */
    0x2c, 0xf1, /* plus-or-minus sign */
    0x2e, 0x87, /* small c cedilla */
#else
    0x1a, '[',
    0x1b, ']',
    0x28, '\\',
    0x2b, '@',
#endif
    0,
};

static const UBYTE keytbl_fr_altshft[] = {
#ifdef MACHINE_AMIGA
    0x03, '@',
    0x04, '#',
    0x10, 0x8f, /* capital A ring */
    0x15, 0x9d, /* yen sign */
    0x18, 0xb2, /* capital O slash */
    0x1a, '{',
    0x1b, '}',
    0x1e, 0x92, /* capital AE ligature */
    0x1f, 0xdd, /* section sign */
    0x2c, 0xaa, /* logical not sign */
    0x2d, 0xf6, /* division sign */
    0x2e, 0x80, /* capital C cedilla */
    0x31, 0xff, /* spacing macron long accent */
#else
    0x1a, '{',
    0x1b, '}',
    0x2b, '~',
#endif
    0,
};

static const UBYTE keytbl_fr_altcaps[] = {
#ifdef MACHINE_AMIGA
    0x03, 0xfd, /* superscript 2 */
    0x04, 0xfe, /* superscript 3 */
    0x05, 0x9b, /* cent sign */
    0x06, 0xac, /* fraction 1/4 */
    0x07, 0xab, /* fraction 1/2 */
    0x0a, 0xae, /* left double angle quote or guillemet */
    0x0b, 0xaf, /* right double angle quote or guillemet */
    0x10, 0x86, /* small a ring */
    0x11, 0xf8, /* degree sign */
    0x12, 0xbd, /* copyright sign */
    0x13, 0xbe, /* registered trademark sign */
    0x16, 0xe6, /* micro sign, mu */
    0x17, 0xad, /* inverted exclamation mark */
    0x18, 0xb2, /* capital O slash */
    0x19, 0xbc, /* paragraph sign, pilcrow sign */
    0x1a, '[',
    0x1b, ']',
    0x1e, 0x92, /* capital AE ligature */
    0x1f, 0x9e, /* small sharp s, sz ligature */
    0x26, 0x9c, /* pound sterling sign */
    0x2c, 0xf1, /* plus-or-minus sign */
    0x2e, 0x80, /* capital C cedilla */
#else
    0x1a, '[',
    0x1b, ']',
    0x28, '\\',
    0x2b, '@',
#endif
    0,
};

static const UBYTE keytbl_fr_dead0[] = {
    'a', 0x83, 'e', 0x88, 'i', 0x8c, 'o', 0x93, 'u', 0x96,
    ' ', '^', DEAD(0), '^', 0
};

static const UBYTE keytbl_fr_dead1[] = {
    'A', 0x8e, 'O', 0x99, 'U', 0x9a,
    'a', 0x84, 'e', 0x89, 'i', 0x8b, 'o', 0x94, 'u', 0x81, 'y', 0x98,
    ' ', 0xb9, DEAD(1), 0xb9, 0
};

static const UBYTE * const keytbl_fr_dead[] = {
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
    keytbl_fr_dead,
    0
};
