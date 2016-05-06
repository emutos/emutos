/*
 * keyb_gr.h - Greek keyboard layout definition
 *
 * Copyright (c) 2002-2015 The EmuTOS development team
 *
 * Authors:
 *  GGN   ggn@atari.org
 *
 * This file may not be distributed at all, or I'll have yer head off!
 *
 * Only kidding! Actually the keymaps took a bit of time to rip!
 * Yes, they were ripped from an Accessory written by D. Gizis and
 * M.Statharas of ELKAT, Greece (which was Atari Greece if you wonder).
 *
 * Note: this file was not automatically generated, although I took this
 * header from an automatically generated header, just for the looks!
 *
 * Rewritten by RFB to allow for DUAL_KEYBOARD support: the 'normal'
 * tables are copies of the US ones, the 'alternate' tables remain
 * essentially the same (with a number of fixes), and dead key tables
 * have been added to support accented characters.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_gr_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8, 0x09,
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

static const UBYTE keytbl_gr_shft[] = {
       0, 0x1b,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',    8, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '{',  '}', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
    '\"',  '~',    0,  '|',  'Z',  'X',  'C',  'V',
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

static const UBYTE keytbl_gr_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8, 0x09,
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

static const UBYTE keytbl_gr_altnorm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8, 0x09,
     ';', 0xaa, 0x9c, 0xa8, 0xab, 0xac, 0x9f, 0xa0,
    0xa6, 0xa7,  '[',  ']', 0x0d,    0, 0x98, 0xa9,
    0x9b, 0xad, 0x9a, 0x9e, 0xa5, 0xa1, 0xa2,  DEAD(0),
    '\'',  '`',    0, '\\', 0x9d, 0xae, 0xaf, 0xe0,
    0x99, 0xa4, 0xa3,  ',',  '.',  '/',    0,    0,
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

static const UBYTE keytbl_gr_altshft[] = {
       0, 0x1b,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+',    8, 0x09,
     ':', 0xaa, 0x84, 0x90, 0x92, 0x93, 0x87, 0x88,
    0x8e, 0x8f,  '{',  '}', 0x0d,    0, 0x80, 0x91,
    0x83, 0x94, 0x82, 0x86, 0x8d, 0x89, 0x8a, DEAD(1),
    '\"',  '~',    0,  '|', 0x85, 0x95, 0x96, 0x97,
    0x81, 0x8c, 0x8b,  '<',  '>',  '?',    0,    0,
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

static const UBYTE keytbl_gr_altcaps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',    8, 0x09,
     ';', 0xaa, 0x84, 0x90, 0x92, 0x93, 0x87, 0x88,
    0x8e, 0x8f,  '[',  ']', 0x0d,    0, 0x80, 0x91,
    0x83, 0x94, 0x82, 0x86, 0x8d, 0x89, 0x8a, DEAD(0),
    '\'',  '`',    0, '\\', 0x85, 0x95, 0x96, 0x97,
    0x81, 0x8c, 0x8b,  ',',  '.',  '/',    0,    0,
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

static const UBYTE keytbl_gr_dead0[] = {
    /* maps the greek keyboard equivalents of a,e,h,i,o,y,v to their ' accented versions */
    0x98, 0xe1,  0x9c, 0xe2,  0x9e, 0xe3,  0xa0, 0xe5,  0xa6, 0xe6,  0xac, 0xe7,  0xe0, 0xe9,
    /* and the same for uppercase */
    0x80, 0xea,  0x84, 0xeb,  0x86, 0xec,  0x88, 0xed,  0x8e, 0xee,  0x93, 0xef,  0x97, 0xf0,
    DEAD(0), ';', 0
};

static const UBYTE keytbl_gr_dead1[] = {
    /* maps the greek keyboard equivalents of i,y to their .. accented versions */
    0xa0, 0xe4,  0xac, 0xe8,
    /* and the same for uppercase */
    0x88, 0xf4,  0x93, 0xf5,
    DEAD(1), ':', 0
};

static const UBYTE * const keytbl_gr_dead[] = {
    keytbl_gr_dead0,
    keytbl_gr_dead1,
};

static const struct keytbl keytbl_gr = {
    keytbl_gr_norm,
    keytbl_gr_shft,
    keytbl_gr_caps,
    keytbl_gr_altnorm,
    keytbl_gr_altshft,
    keytbl_gr_altcaps,
    keytbl_gr_dead,
    DUAL_KEYBOARD
};
