/*
 * keyb_it.h - a keyboard layout definition
 *
 * Copyright (c) 2001 EmuTOS development team
 *
 * Authors:
 *  TODO: This is currently a stub and need real Italian tables
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */


BYTE keytbl_it_norm[];
BYTE keytbl_it_shft[];
BYTE keytbl_it_caps[];
BYTE keytbl_it_altnorm[];
BYTE keytbl_it_altshft[];
BYTE keytbl_it_altcaps[];

struct keytbl keytbl_it = {
    keytbl_it_norm, 
    keytbl_it_shft, 
    keytbl_it_caps, 
    keytbl_it_altnorm, 
    keytbl_it_altshft, 
    keytbl_it_altcaps, 
    NULL
};

BYTE keytbl_it_norm[] = {
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

BYTE keytbl_it_shft[] = {
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

BYTE keytbl_it_caps[] = {
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

BYTE keytbl_it_altnorm[] = {
    0,
};
BYTE keytbl_it_altshft[] = {
    0,
};
BYTE keytbl_it_altcaps[] = {
    0,
};


