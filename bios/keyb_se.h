/*
 * keyb_se.h - Swedish/Finnish keyboard layout definition
 *
 * Copyright (c) 2005 EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

BYTE keytbl_se_norm[];
BYTE keytbl_se_shft[];
BYTE keytbl_se_caps[];
BYTE keytbl_se_altnorm[];
BYTE keytbl_se_altshft[];
BYTE keytbl_se_altcaps[];

struct keytbl keytbl_se = {
    keytbl_se_norm, 
    keytbl_se_shft, 
    keytbl_se_caps, 
    keytbl_se_altnorm, 
    keytbl_se_altshft, 
    keytbl_se_altcaps, 
    NULL, 
};

BYTE keytbl_se_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6', 
     '7',  '8',  '9',  '0',  '+', 0x82,    8, 0x09, 
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i', 
     'o',  'p', 0x86, 0x81, 0x0d,    0,  'a',  's', 
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0x94, 
    0x84, '\'',    0, '\\',  'z',  'x',  'c',  'v', 
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

BYTE keytbl_se_shft[] = {
       0, 0x1b,  '!', '\"',  '#',  '$',  '%',  '&', 
     '/',  '(',  ')',  '=',  '?', 0x90,    8, 0x09, 
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I', 
     'O',  'P', 0x8f, 0x9a, 0x0d,    0,  'A',  'S', 
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0x99, 
    0x8e,  '*',    0,  '|',  'Z',  'X',  'C',  'V', 
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

BYTE keytbl_se_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6', 
     '7',  '8',  '9',  '0',  '+', 0x90,    8, 0x09, 
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I', 
     'O',  'P', 0x8f, 0x9a, 0x0d,    0,  'A',  'S', 
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0x99, 
    0x8e, '\'',    0,  '|',  'Z',  'X',  'C',  'V', 
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


/* TODO - the tables below were not filled by the tool.
 * they should contain couples of (scan code, char code), ended by zero.
 */

BYTE keytbl_se_altnorm[] = {
    0x1a, '[',
    0x1b, ']',
    0x28, '`',
    0x2b, '^',
    0,
};

BYTE keytbl_se_altshft[] = {
    0x1a, '{',
    0x1b, '}',
    0x28, '~',
    0x2b, '@',
    0,
};

BYTE keytbl_se_altcaps[] = {
    0x1a, '[',
    0x1b, ']',
    0x28, '`',
    0x2b, '^',
    0,
};

