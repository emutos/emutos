/*
 *       Copyright 2002 The EmuTOS development team
 *
 *       This software is licenced under the GNU Public License.
 *       Please see LICENSE.TXT for further information.
 */

/*
 * This file contains the Desk's RSC data
 */

#include <string.h>
#include <version.h>

#include "portab.h"
#include "obdefs.h"
#include "gembind.h"
#include "aesbind.h"
#include "gemdos.h"
#include "kprint.h"
#include "desk_rsc.h"


static const char rs_str_OK[] = "OK";
static const char rs_str_Cancel[] = "Cancel";
static const char rs_str_Install[] = "Install";
static const char rs_str_Remove[] = "Remove";
static const char rs_str_Yes[] = "Yes";
static const char rs_str_No[] = "No";



#define RS_NTED 37



TEDINFO desk_rs_tedinfo[RS_NTED];

static const TEDINFO desk_rs_tedinfo_rom[] = {
    {0L,
     (LONG) "Name:  ________.___",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20},      /* 0 */

    {0L,
     (LONG) "Size in bytes:  ________",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 25},      /* 1 */

    {0L,
     (LONG) "Last modified:  __/__/__",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 7, 25},      /* 2 */

    {0L,
     (LONG) "__:__ __",
     (LONG) "9999aa",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 7, 9},       /* 3 */

    {0L,
     (LONG) "Drive identifier:  _:",
     (LONG) "A",
     IBM, 0, TE_LEFT, 4352, 0, 0, 2, 22},       /* 4 */

    {0L,
     (LONG) "Disk label:  ___________",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 25},      /* 5 */

    {0L,
     (LONG) "Number of folders:     _____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29},      /* 6 */

    {0L,
     (LONG) "Number of items:     _____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27},      /* 7 */

    {0L,
     (LONG) "Bytes used:  ________",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 22},      /* 8 */

    {0L,
     (LONG) "Bytes available:  ________",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 27},      /* 9 */

    {0L,
     (LONG) "Folder name:  ________.___",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 27},      /* 10 */

    {0L,
     (LONG) "Created:  __-__-__  ",
     (LONG) "9",
     IBM, 0, TE_LEFT, 4352, 0, 0, 7, 21},       /* 11 */

    {0L,
     (LONG) "__:__ __",
     (LONG) "9999aa",
     IBM, 0, TE_LEFT, 4352, 0, 0, 7, 9},        /* 12 */

    {0L,
     (LONG) "Number of folders:     _____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29},      /* 13 */

    {0L,
     (LONG) "Number of items:     _____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27},      /* 14 */

    {0L,
     (LONG) "Bytes used:  ________",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 22},      /* 15 */

    {0L,
     (LONG) "Name:  ________.___",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20},      /* 16 */

    {0L,
     
     (LONG)
     "Parameters:  ____________________________________________________",
     (LONG) "X",
     IBM, 0, TE_LEFT, 4352, 0, 0, 53, 66},      /* 17 */

    {0L,
     (LONG) "Drive identifier:  _",
     (LONG) "A",
     IBM, 0, TE_LEFT, 4352, 0, 0, 2, 21},       /* 18 */

    {0L,
     (LONG) "Icon label:  ____________",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 13, 26},      /* 19 */

    {0L,
     (LONG) "Application name:  ________.___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 32},      /* 20 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 21 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 22 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 23 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 24 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 25 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 26 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 27 */

    {0L,
     (LONG) "___",
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4},        /* 28 */

    {0L,
     (LONG) "_______________________",
     (LONG) "F",
     IBM, 1, TE_CNTR, 4480, 0, -1, 24, 24},     /* 29 */

    {0L,
     (LONG) "Folders to copy:  ____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 23},      /* 30 */

    {0L,
     (LONG) "Items to copy:  ____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 21},      /* 31 */

    {0L,
     (LONG) "Folders to delete:  ____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 25},      /* 32 */

    {0L,
     (LONG) "Items to delete:  ____",
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 23},      /* 33 */

    {0L,
     (LONG) "Current name:  ________.___",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 28},      /* 34 */

    {0L,
     (LONG) "Copy's name:  ________.___",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 27},      /* 35 */

    {0L,
     (LONG) "Name:  ________.___",
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20}       /* 36 */
};




static int rs_logo_img[] = {
    0x3fff, 0xfffc, 0x7fff, 0xfffe, 0xe000, 0x0007, 0xc000, 0x0003,
    0xc000, 0x0003, 0xc3f8, 0x1fc3, 0xc7fc, 0x3fe3, 0xc7fc, 0x3063,
    0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063,
    0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063,
    0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063,
    0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063, 0xc7fc, 0x3063,
    0xc7fc, 0x3063, 0xc7fc, 0x3fe3, 0xc3f8, 0x1fc3, 0xc000, 0x0003,
    0xc000, 0x0003, 0xe000, 0x0007, 0x7fff, 0xfffe, 0x3fff, 0xfffc
};


static BITBLK desk_rs_bitblk[] = {
    {(LONG) rs_logo_img, 4, 32, 0, 0, 1},
};




static char rs_str_iconOrText[20];      /* was: "  xxxx xx xxxxx  xx" */


#define RS_NOBS 211

OBJECT desk_rs_obj[RS_NOBS];

static const OBJECT desk_rs_obj_rom[] = {
#define TR0 0
    /* TREE 0 */
   { -1, 1, 7, G_IBOX,                          /*** 0 ***/
     NONE,
     NORMAL,
     (long) 0L,
     0, 0, 80, 25 },

   { 7, 2, 2, G_BOX,                            /*** 1 ***/
     NONE,
     NORMAL,
     (long) 4352L,
     0, 0, 80, 513 },

   { 1, 3, 6, G_IBOX,                           /*** 2 ***/
     NONE,
     NORMAL,
     (long) 0L,
     2, 0, 30, 769},

   { 4, -1, -1, G_TITLE,                        /*** 3 ***/
     NONE,
     NORMAL,
     (long) " Desk ",
     0, 0, 6, 769},

   { 5, -1, -1, G_TITLE,                        /*** 4 ***/
     NONE,
     NORMAL,
     (long) " File ",
     6, 0, 6, 769},

   { 6, -1, -1, G_TITLE,                        /*** 5 ***/
     NONE,
     NORMAL,
     (long) " Options ",
     12, 0, 9, 769},

   { 2, -1, -1, G_TITLE,                        /*** 6 ***/
     NONE,
     NORMAL,
     (long) " Arrange ",
     21, 0, 9, 769},

   { 0, 8, 33, G_IBOX,                          /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     0, 769, 80, 24},

   { 17, 9, 16, G_BOX,                          /*** 8 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     2, 0, 20, 8},

   { 10, -1, -1, G_STRING,                      /*** 9 ***/
     NONE,
     NORMAL,
     (long) "  Desktop info...   ",
     0, 0, 20, 1},

   { 11, -1, -1, G_STRING,                      /*** 10 ***/
     NONE,
     DISABLED,
     (long) "--------------------",
     0, 1, 20, 1},

   { 12, -1, -1, G_STRING,                      /*** 11 ***/
     NONE,
     NORMAL,
     (long) "1",
     0, 2, 20, 1},

   { 13, -1, -1, G_STRING,                      /*** 12 ***/
     NONE,
     NORMAL,
     (long) "2",
     0, 3, 20, 1},

   { 14, -1, -1, G_STRING,                      /*** 13 ***/
     NONE,
     NORMAL,
     (long) "3",
     0, 4, 20, 1},

   { 15, -1, -1, G_STRING,                      /*** 14 ***/
     NONE,
     NORMAL,
     (long) "4",
     0, 5, 20, 1},

   { 16, -1, -1, G_STRING,                      /*** 15 ***/
     NONE,
     NORMAL,
     (long) "5",
     0, 6, 20, 1},

   { 8, -1, -1, G_STRING,                       /*** 16 ***/
     NONE,
     NORMAL,
     (long) "6",
     0, 7, 20, 1},

   { 26, 18, 25, G_BOX,                         /*** 17 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     8, 0, 21, 8},

   { 19, -1, -1, G_STRING,                      /*** 18 ***/
     NONE,
     NORMAL,
     (long) "  Open",
     0, 0, 21, 1},

   { 20, -1, -1, G_STRING,                      /*** 19 ***/
     NONE,
     NORMAL,
     (long) "  Info/Rename...  \007I",
     0, 1, 21, 1},

   { 21, -1, -1, G_STRING,                      /*** 20 ***/
     NONE,
     DISABLED,
     (long) "---------------------",
     0, 2, 21, 1},

   { 22, -1, -1, G_STRING,                      /*** 21 ***/
     NONE,
     NORMAL,
     (long) "  Delete...       \007D",
     0, 3, 21, 1},

   { 23, -1, -1, G_STRING,                      /*** 22 ***/
     NONE,
     NORMAL,
     (long) "  Format...",
     0, 4, 21, 1},

   { 24, -1, -1, G_STRING,                      /*** 23 ***/
     NONE,
     DISABLED,
     (long) "---------------------",
     0, 5, 21, 1},

   { 25, -1, -1, G_STRING,                      /*** 24 ***/
     NONE,
     NORMAL,
     (long) "  To Output       ^U",
     0, 6, 21, 1},

   { 17, -1, -1, G_STRING,                      /*** 25 ***/
     NONE,
     NORMAL,
     (long) "  Exit to DOS     ^Q",
     0, 7, 21, 1},

   { 33, 27, 32, G_BOX,                         /*** 26 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     14, 0, 31, 6},

   { 28, -1, -1, G_STRING,                      /*** 27 ***/
     NONE,
     NORMAL,
     (long) "  Install disk drive...",
     0, 0, 31, 1},

   { 29, -1, -1, G_STRING,                      /*** 28 ***/
     NONE,
     NORMAL,
     (long) "  Configure application...  \007A",
     0, 1, 31, 1},

   { 30, -1, -1, G_STRING,                      /*** 29 ***/
     NONE,
     DISABLED,
     (long) "-------------------------------",
     0, 2, 31, 1},

   { 31, -1, -1, G_STRING,                      /*** 30 ***/
     NONE,
     NORMAL,
     (long) "  Set preferences...",
     0, 3, 31, 1},

   { 32, -1, -1, G_STRING,                      /*** 31 ***/
     NONE,
     NORMAL,
     (long) "  Save desktop              \007V",
     0, 4, 31, 1},

   { 26, -1, -1, G_STRING,                      /*** 32 ***/
     NONE,
     NORMAL,
     (long) "  Enter DOS commands        \007C",
     0, 5, 31, 1},

   { 7, 34, 39, G_BOX,                          /*** 33 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     23, 0, 20, 6},

   { 35, -1, -1, G_STRING,                      /*** 34 ***/
     NONE,
     NORMAL,
     (long) rs_str_iconOrText,
     0, 0, 20, 1},

   { 36, -1, -1, G_STRING,                      /*** 35 ***/
     NONE,
     DISABLED,
     (long) "--------------------",
     0, 1, 20, 1},

   { 37, -1, -1, G_STRING,                      /*** 36 ***/
     NONE,
     NORMAL,
     (long) "  Sort by name   \007N",
     0, 2, 20, 1},

   { 38, -1, -1, G_STRING,                      /*** 37 ***/
     NONE,
     NORMAL,
     (long) "  Sort by type   \007P",
     0, 3, 20, 1},

   { 39, -1, -1, G_STRING,                      /*** 38 ***/
     NONE,
     NORMAL,
     (long) "  Sort by size   \007Z",
     0, 4, 20, 1},

   { 33, -1, -1, G_STRING,                      /*** 39 ***/
     LASTOB,
     NORMAL,
     (long) "  Sort by date   \007T",
     0, 5, 20, 1},

#define TR1 40
/* TREE 1 */
   { -1, 1, 11, G_BOX,                          /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 45, 11},

   { 2, -1, -1, G_STRING,                       /*** 1 ***/
     NONE,
     NORMAL,
     (long) "ITEM INFORMATION / RENAME",
     3, 1, 16, 1},

   { 3, -1, -1, G_FBOXTEXT,                     /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[0],
     12, 3, 19, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[1],
     3, 4, 24, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[2],
     3, 5, 24, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[3],
     29, 5, 8, 1},

   { 7, -1, -1, G_STRING,                       /*** 6 ***/
     NONE,
     NORMAL,
     (long) "Attributes:",
     6, 7, 11, 1},

   { 10, 8, 9, G_IBOX,                          /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     19, 7, 23, 1},

   { 9, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Read/Write",
     0, 0, 11, 1},

   { 7, -1, -1, G_BUTTON,                       /*** 9 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Read-Only",
     13, 0, 10, 1},

   { 11, -1, -1, G_BUTTON,                      /*** 10 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     24, 9, 8, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 11 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     34, 9, 8, 1},

#define TR2 52
/* TREE 2 */
   { -1, 1, 8, G_BOX,                           /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 37, 12},

   { 2, -1, -1, G_STRING,                       /*** 1 ***/
     NONE,
     NORMAL,
     (long) "DISK INFORMATION",
     3, 1, 16, 1},

   { 3, -1, -1, G_FBOXTEXT,                     /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[4],
     4, 3, 21, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[5],
     10, 4, 24, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[6],
     3, 5, 28, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[7],
     5, 6, 26, 1},

   { 7, -1, -1, G_FBOXTEXT,                     /*** 6 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[8],
     10, 7, 21, 1},

   { 8, -1, -1, G_FBOXTEXT,                     /*** 7 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[9],
     5, 8, 26, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     26, 10, 8, 1},

#define TR3 61
/* TREE 3 */
   { -1, 1, 8, G_BOX,                           /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 46, 11},

   { 2, -1, -1, G_STRING,                       /*** 1 ***/
     NONE,
     NORMAL,
     (long) "FOLDER INFORMATION",
     3, 1, 18, 1},

   { 3, -1, -1, G_FBOXTEXT,                     /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[10],
     9, 3, 26, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[11],
     13, 4, 20, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[12],
     33, 4, 8, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[13],
     3, 5, 28, 1},

   { 7, -1, -1, G_FBOXTEXT,                     /*** 6 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[14],
     5, 6, 26, 1},

   { 8, -1, -1, G_FBOXTEXT,                     /*** 7 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[15],
     10, 7, 21, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     31, 9, 8, 1},

#define TR4 70
/* TREE 4 */
   { -1, 1, 15, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 40, 19 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "- EmuTOS -",
     15, 1, 10, 1 },

   { 3, -1, -1, G_STRING,                   /*** 2 ***/
     NONE,
     NORMAL,
     (long) "Version",
     14, 2, 7, 1 },

   { 4, -1, -1, G_STRING,                   /*** 3 ***/
     NONE,
     NORMAL,
     (long) EMUTOS_VERSION,
     22, 2, 4, 1 },

   { 5, -1, -1, G_IMAGE,                    /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_bitblk[0],
     33, 1, 4, 4 },

   { 6, -1, -1, G_STRING,                   /*** 5 ***/
     NONE,
     NORMAL,
     (long) "April 23, 2002",
     13, 4, 24, 1 },

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) "Copyright (c) 2002 by",
     9, 6, 21, 1 },

   { 8, -1, -1, G_IMAGE,                    /*** 7 ***/
     TOUCHEXIT,
     NORMAL,
     (long) &desk_rs_bitblk[0],
     3, 1, 4, 4 },

   { 9, -1, -1, G_STRING,                   /*** 8 ***/
     NONE,
     NORMAL,
     (long) "The EmuTOS development team",
     6, 7, 27, 1 },

   { 10, -1, -1, G_STRING,                  /*** 9 ***/
     NONE,
     NORMAL,
     (long) "Based on 'GPLed' sources",
     8, 9, 24, 1 },

   { 11, -1, -1, G_STRING,                  /*** 10 ***/
     NONE,
     NORMAL,
     (long) "(c) 1987 by Digital Research Inc.",
     3, 10, 33, 1 },

   { 12, -1, -1, G_BUTTON,                  /*** 11 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     16, 17, 8, 1 },

   { 13, -1, -1, G_STRING,                  /*** 12 ***/
     NONE,
     NORMAL,
     (long) "(c) 1999 Caldera Thin Clients, Inc.",
     3, 11, 35, 1 },

   { 14, -1, -1, G_STRING,                  /*** 13 ***/
     NONE,
     NORMAL,
     (long) "(c) 2001 Lineo, Inc.",
     3, 12, 20, 1 },

   { 15, -1, -1, G_STRING,                  /*** 14 ***/
     NONE,
     NORMAL,
     (long) "EmuTOS is distributed under the GPL",
     3, 14, 35, 1 },

   { 0, -1, -1, G_STRING,                   /*** 15 ***/
     LASTOB,
     NORMAL,
     (long) "See doc/license.txt for details",
     4, 15, 21, 1 },

#define TR5 86
/* TREE 5 */
   { -1, 1, 7, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 48, 11},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "NEW FOLDER INFORMATION",
     3, 1, 23, 1},
    {
     3, -1, -1, G_STRING,                   /*** 2 ***/
     NONE,
     NORMAL,
     (long) "To create a new folder within the current",
     3, 3, 41, 1},
    {
     4, -1, -1, G_STRING,                   /*** 3 ***/
     NONE,
     NORMAL,
     (long) "window, double-click on the New Folder",
     3, 4, 38, 1},
    {
     5, -1, -1, G_STRING,                   /*** 4 ***/
     NONE,
     NORMAL,
     (long) "icon and complete the dialogue that appears",
     3, 5, 43, 1},
    {
     6, -1, -1, G_STRING,                   /*** 5 ***/
     NONE,
     NORMAL,
     (long) "by entering the name of the folder you",
     3, 6, 38, 1},
    {
     7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) "want to create.",
     3, 7, 15, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 7 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     37, 9, 8, 1},
    {
#define TR6 94
/* TREE 6 */
     -1, 1, 8, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 71, 10},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "OPEN APPLICATION",
     3, 1, 16, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[16],
     9, 3, 19, 1},
    {
     4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[17],
     3, 4, 65, 1},
    {
     5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     51, 8, 8, 1},
    {
     6, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Cancel,
     61, 8, 8, 1},
    {
     7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) "Enter the name of the document you want",
     3, 6, 39, 1},
    {
     8, -1, -1, G_STRING,                   /*** 7 ***/
     NONE,
     NORMAL,
     (long) "to load, or enter parameter values that",
     3, 7, 39, 1},
    {
     0, -1, -1, G_STRING,                   /*** 8 ***/
     LASTOB,
     NORMAL,
     (long) "are acceptable to this application.",
     3, 8, 35, 1},
    {
#define TR7 103
/* TREE 7 */
     -1, 1, 10, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 59, 9},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "INSTALL DISK DRIVE",
     3, 1, 18, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[18],
     3, 3, 20, 1},
    {
     4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[19],
     9, 4, 25, 1},
    {
     5, -1, -1, G_STRING,                   /*** 4 ***/
     NONE,
     NORMAL,
     (long) "Disk type:",
     10, 6, 10, 1},
    {
     8, 6, 7, G_IBOX,                       /*** 5 ***/
     NONE,
     NORMAL,
     (long) 0L,
     20, 6, 24, 1},
    {
     7, -1, -1, G_BUTTON,                   /*** 6 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Floppy",
     2, 0, 8, 1},
    {
     5, -1, -1, G_BUTTON,                   /*** 7 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Hard",
     12, 0, 8, 1},
    {
     9, -1, -1, G_BUTTON,                   /*** 8 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Install,
     47, 2, 8, 1},
    {
     10, -1, -1, G_BUTTON,                  /*** 9 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Remove,
     47, 4, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 10 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     47, 6, 8, 1},
    {
#define TR8 114
/* TREE 8 */
     -1, 1, 33, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 64, 20},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "CONFIGURE APPLICATION",
     3, 1, 21, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[20],
     4, 3, 32, 1},
    {
     4, -1, -1, G_STRING,                   /*** 3 ***/
     NONE,
     NORMAL,
     (long) "Document types:",
     6, 4, 15, 1},
    {
     5, -1, -1, G_FBOXTEXT,                 /*** 4 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[21],
     23, 4, 3, 1},
    {
     6, -1, -1, G_FBOXTEXT,                 /*** 5 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[22],
     28, 4, 3, 1},
    {
     7, -1, -1, G_FBOXTEXT,                 /*** 6 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[23],
     33, 4, 3, 1},
    {
     8, -1, -1, G_FBOXTEXT,                 /*** 7 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[24],
     38, 4, 3, 1},
    {
     9, -1, -1, G_FBOXTEXT,                 /*** 8 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[25],
     43, 4, 3, 1},
    {
     10, -1, -1, G_FBOXTEXT,                /*** 9 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[26],
     48, 4, 3, 1},
    {
     11, -1, -1, G_FBOXTEXT,                /*** 10 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[27],
     53, 4, 3, 1},
    {
     12, -1, -1, G_FBOXTEXT,                /*** 11 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[28],
     58, 4, 3, 1},
    {
     13, -1, -1, G_STRING,                  /*** 12 ***/
     NONE,
     NORMAL,
     (long) "Application type:",
     4, 6, 17, 1},
    {
     17, 14, 16, G_IBOX,                    /*** 13 ***/
     NONE,
     NORMAL,
     (long) 4352L,
     23, 6, 39, 1},
    {
     15, -1, -1, G_BUTTON,                  /*** 14 ***/
     SELECTABLE | RBUTTON | TOUCHEXIT,
     NORMAL,
     (long) "GEM",
     0, 0, 6, 1},
    {
     16, -1, -1, G_BUTTON,                  /*** 15 ***/
     SELECTABLE | RBUTTON | TOUCHEXIT,
     NORMAL,
     (long) "DOS",
     8, 0, 6, 1},
    {
     13, -1, -1, G_BUTTON,                  /*** 16 ***/
     SELECTABLE | RBUTTON | TOUCHEXIT,
     NORMAL,
     (long) "DOS-takes parameters",
     16, 0, 22, 1},
    {
     18, -1, -1, G_STRING,                  /*** 17 ***/
     NONE,
     NORMAL,
     (long) "Needs full memory?",
     3, 8, 18, 1},
    {
     21, 19, 20, G_IBOX,                    /*** 18 ***/
     NONE,
     NORMAL,
     (long) 4352L,
     23, 8, 15, 1},
    {
     20, -1, -1, G_BUTTON,                  /*** 19 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 6, 1},
    {
     18, -1, -1, G_BUTTON,                  /*** 20 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     8, 0, 6, 1},
    {
     22, -1, -1, G_STRING,                  /*** 21 ***/
     NONE,
     NORMAL,
     (long) "Icon type:",
     11, 10, 10, 1},
    {
     23, -1, -1, G_BOXTEXT,                 /*** 22 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[29],
     23, 10, 27, 1},
    {
     26, 24, 25, G_BOX,                     /*** 23 ***/
     NONE,
     NORMAL,
     (long) 69888L,
     23, 11, 24, 6,
     },
    {
     25, -1, -1, G_BOX,                     /*** 24 ***/
     NONE,
     NORMAL,
     (long) 69889L,
     3, 1, 6, 4,
     },
    {
     23, -1, -1, G_BOX,                     /*** 25 ***/
     NONE,
     NORMAL,
     (long) 69889L,
     12, 1, 6, 4,
     },
    {
     31, 27, 29, G_BOX,                     /*** 26 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 69888L,
     47, 11, 3, 6,
     },
    {
     28, -1, -1, G_BOXCHAR,                 /*** 27 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 201396480L,
     0, 0, 3, 2,
     },
    {
     29, -1, -1, G_BOXCHAR,                 /*** 28 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 218173696L,
     0, 4, 3, 2,
     },
    {
     26, 30, 30, G_BOX,                     /*** 29 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 69905L,
     0, 2, 3, 2,
     },
    {
     29, -1, -1, G_BOX,                     /*** 30 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 69889L,
     1024, 0, 258, 1},
    {
     32, -1, -1, G_BUTTON,                  /*** 31 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Install,
     30, 18, 9, 1},
    {
     33, -1, -1, G_BUTTON,                  /*** 32 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Remove,
     42, 18, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 33 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     53, 18, 8, 1},
    {
#define TR9 148
/* TREE 9 */
     -1, 1, 5, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 34, 8},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "COPY FOLDERS / ITEMS",
     3, 1, 20, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[30],
     3, 3, 22, 1},
    {
     4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[31],
     5, 4, 20, 1},
    {
     5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     13, 6, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     23, 6, 8, 1},
    {
#define TR10 154
/* TREE 10 */
     -1, 1, 5, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 30, 8},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "DELETE FOLDERS / ITEMS",
     3, 1, 23, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[32],
     3, 3, 24, 1},
    {
     4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[33],
     5, 4, 22, 1},
    {
     5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_OK,
     9, 6, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     19, 6, 8, 1},
    {
#define TR11 160
/* TREE 11 */
     -1, 1, 6, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 34, 8},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "NAME CONFLICT DURING COPY",
     3, 1, 25, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[34],
     3, 3, 27, 1},
    {
     4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[35],
     4, 4, 26, 1},
    {
     5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     3, 6, 8, 1},
    {
     6, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Cancel,
     13, 6, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 6 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) "Stop",
     23, 6, 8, 1},
    {
#define TR12 167
/* TREE 12 */
     -1, 1, 4, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 27, 7},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "NEW FOLDER",
     3, 1, 10, 1},
    {
     3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[36],
     3, 3, 19, 1},
    {
     4, -1, -1, G_BUTTON,                   /*** 3 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     6, 5, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     16, 5, 8, 1},
    {
#define TR13 172
/* TREE 13 */
     -1, 1, 38, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 59, 21},
    {
     2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) "SET PREFERENCES",
     3, 1, 15, 1},
    {
     3, -1, -1, G_STRING,                   /*** 2 ***/
     NONE,
     NORMAL,
     (long) "Confirm deletes?",
     6, 3, 16, 1},
    {
     6, 4, 5, G_IBOX,                       /*** 3 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 3, 12, 1},
    {
     5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1},
    {
     3, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     7, 0, 5, 1},
    {
     7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) "Confirm copies?",
     7, 5, 15, 1},
    {
     10, 8, 9, G_IBOX,                      /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 5, 12, 1},
    {
     9, -1, -1, G_BUTTON,                   /*** 8 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1},
    {
     7, -1, -1, G_BUTTON,                   /*** 9 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     7, 0, 5, 1},
    {
     11, -1, -1, G_STRING,                  /*** 10 ***/
     NONE,
     NORMAL,
     (long) "Confirm overwrites?",
     3, 7, 19, 1},
    {
     14, 12, 13, G_IBOX,                    /*** 11 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 7, 12, 1},
    {
     13, -1, -1, G_BUTTON,                  /*** 12 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1},
    {
     11, -1, -1, G_BUTTON,                  /*** 13 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     7, 0, 5, 1},
    {
     15, -1, -1, G_STRING,                  /*** 14 ***/
     NONE,
     NORMAL,
     (long) "Double-click speed:",
     3, 9, 19, 1},
    {
     21, 16, 20, G_IBOX,                    /*** 15 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 9, 31, 1},
    {
     17, -1, -1, G_BUTTON,                  /*** 16 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Slow",
     0, 0, 7, 1},
    {
     18, -1, -1, G_BUTTON,                  /*** 17 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "2",
     9, 0, 3, 1},
    {
     19, -1, -1, G_BUTTON,                  /*** 18 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "3",
     14, 0, 3, 1},
    {
     20, -1, -1, G_BUTTON,                  /*** 19 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "4",
     19, 0, 3, 1},
    {
     15, -1, -1, G_BUTTON,                  /*** 20 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Fast",
     24, 0, 7, 1},
    {
     24, 22, 23, G_IBOX,                    /*** 21 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 11, 20, 1},
    {
     23, -1, -1, G_BUTTON,                  /*** 22 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Click",
     0, 0, 8, 1},
    {
     21, -1, -1, G_BUTTON,                  /*** 23 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "No click",
     10, 0, 10, 1},
    {
     25, -1, -1, G_STRING,                  /*** 24 ***/
     NONE,
     NORMAL,
     (long) "To drop down menus:",
     3, 11, 19, 1},
    {
     26, -1, -1, G_STRING,                  /*** 25 ***/
     NONE,
     NORMAL,
     (long) "Sound effects:",
     8, 13, 14, 1},
    {
     29, 27, 28, G_IBOX,                    /*** 26 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 13, 12, 1},
    {
     28, -1, -1, G_BUTTON,                  /*** 27 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "On",
     0, 0, 5, 1},
    {
     26, -1, -1, G_BUTTON,                  /*** 28 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "Off",
     7, 0, 5, 1},
    {
     30, -1, -1, G_STRING,                  /*** 29 ***/
     NONE,
     NORMAL,
     (long) "Time format:",
     10, 15, 12, 1},
    {
     33, 31, 32, G_IBOX,                    /*** 30 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 15, 20, 1},
    {
     32, -1, -1, G_BUTTON,                  /*** 31 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "12 Hour",
     0, 0, 9, 1},
    {
     30, -1, -1, G_BUTTON,                  /*** 32 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "24 Hour",
     11, 0, 9, 1},
    {
     34, -1, -1, G_STRING,                  /*** 33 ***/
     NONE,
     NORMAL,
     (long) "Date format:",
     10, 17, 12, 1},
    {
     37, 35, 36, G_IBOX,                    /*** 34 ***/
     NONE,
     NORMAL,
     (long) 0L,
     24, 17, 24, 1},
    {
     36, -1, -1, G_BUTTON,                  /*** 35 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "MM-DD-YY",
     0, 0, 11, 1},
    {
     34, -1, -1, G_BUTTON,                  /*** 36 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "DD-MM-YY",
     13, 0, 11, 1},
    {
     38, -1, -1, G_BUTTON,                  /*** 37 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     36, 19, 8, 1},
    {
     0, -1, -1, G_BUTTON,                   /*** 38 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     47, 19, 8, 1}
};


static OBJECT *desk_rs_trees[] = {
    &desk_rs_obj[TR0],
    &desk_rs_obj[TR1],
    &desk_rs_obj[TR2],
    &desk_rs_obj[TR3],
    &desk_rs_obj[TR4],
    &desk_rs_obj[TR5],
    &desk_rs_obj[TR6],
    &desk_rs_obj[TR7],
    &desk_rs_obj[TR8],
    &desk_rs_obj[TR9],
    &desk_rs_obj[TR10],
    &desk_rs_obj[TR11],
    &desk_rs_obj[TR12],
    &desk_rs_obj[TR13]
};


#define RS_NFSTR 37

char *desk_rs_fstr[] = {
    "%L bytes used in %W items.",
    "  Show as icons  \007S",
    "application",
    "documents",
    "FORMAT.COM",
    "  Show as text   \007S",
    "DESKHI.ICN",
    "DESKTOP.INF",
    "DESKLO.ICN",
    "OUTPUT.APP",
    "????????.BAT",
    "#FFF28 @ *.*@",
    "#DFF02 @ *.*@",
    "#G08FF *.APP@ @",
    "#P08FF *.EXE@ @",
    "#P08FF *.COM@ @",
    "#P08FF *.BAT@ @",
    "FORMAT.EXE",
    "New Folder",
    "Disk Drives:",
    "am",
    "pm",
    "Free",
    "Free",
    "[1][The document type you selected is not|configured to work with a specific|application.  Use the \"Configure|application\" command to associate this|document type with an application.][  OK  ]",
    "[1][The GEM Desktop has no more available|windows.  Before you open a disk, close|a window you're not using.][  OK  ]",
    "[1][Cannot find the FORMAT program.  If you|are using a dual-floppy system, you must|format disks from your DOS disk.  If you|are using a hard disk, copy FORMAT to |the root directory.][OK]",
    "[3][Formatting will ERASE all|information on the disk in drive|%S:.  Click on OK only if you don't|mind losing the information on|this disk.][  OK  |Cancel]",
    "[3][You cannot copy a parent folder|into one of its child folders.][  OK  ]",
    "[3][If you are sure you want to|delete ALL the information on the|disk in drive %S:, click on OK.|Otherwise, click on Cancel.][  OK  |Cancel]",
    "[1][There is not enough space available|to configure this application.  To free|up some space, you'll have to remove|one of your currently configured|applications.][  OK  ]",
    "[2][A folder with that name already|exists or your disk is full.|Retry with a new name, or Cancel|and check the available disk space.][ Cancel | Retry ]",
    "[1][This disk does not have enough room for|the information you are trying to copy.|Some items, however, may have been|copied to the disk.][  OK  ]",
    "[3][The GEM Desktop cannot find the|documents DESKLO.ICN, DESKHI.ICN,|or DESKTOP.INF in the DOS search|path.  These documents are required|to run the GEM Desktop.][Cancel]",
    "[3][To save your desktop, insert your|GEM DESKTOP disk into drive A:, close|the drive door, and click on OK.|Click on Cancel if you don't want to|save the desktop.][  OK  |Cancel]",
    "[3][Sorry, but you cannot place any more|folders inside of your current one.|See your documentation for limits on|folders inside other folders.][Cancel]",
    "[3][Sorry, but the Directory name you have|entered exceeds the maximum number of|characters.  See your documentation for|limits on the number of characters you|can enter.][   OK   ]"
};




/* Counts the occurance of c in str */
int count_chars(char *str, char c)
{
    int j;
    int len;

    len = 0;
    for (j = 0; j < strlen(str); j++) {
        if (str[j] == c)
            len += 1;
    }

    return len;
}


void desk_rs_init(void)
{
    register int i = 0;
    long len;
    int j;
    char *tedinfptr;


    /* Copy data from ROM to RAM: */
    memcpy(desk_rs_obj, desk_rs_obj_rom, RS_NOBS * sizeof(OBJECT));
    memcpy(desk_rs_tedinfo, desk_rs_tedinfo_rom,
           RS_NTED * sizeof(TEDINFO));

    /* Fix objects coordinates: */
    do {
        rsrc_obfix((LONG) desk_rs_obj, i);
    }
    while (++i < RS_NOBS);

    /* Fix TEDINFO strings: */
    len = 0;
    for (i = 0; i < RS_NTED; i++) {
        if (desk_rs_tedinfo[i].te_ptext == 0) {
            /* Count number of '_' in strings ( +1 for \0 at the end ): */
            len +=
                count_chars((char *) desk_rs_tedinfo[i].te_ptmplt,
                            '_') + 1;
        }
    }
    tedinfptr = (char *) dos_alloc(len);        /* Get memory */
    for (i = 0; i < RS_NTED; i++) {
        if (desk_rs_tedinfo[i].te_ptext == 0) {
            desk_rs_tedinfo[i].te_ptext = (LONG) tedinfptr;
            *tedinfptr++ = '@'; /* First character of uninitialized string */
            len = count_chars((char *) desk_rs_tedinfo[i].te_ptmplt, '_');
            for (j = 0; j < len; j++) {
                *tedinfptr++ = '_';     /* Set other characters to '_' */
            }
            *tedinfptr++ = 0;   /* Final 0 */
        }
    }

}


/* Fake a rsrc_gaddr for the ROM desktop: */
WORD rsrc_gaddr(WORD rstype, WORD rsid, LONG * paddr)
{
    switch (rstype) {
    case R_TREE:
        *paddr = (LONG) desk_rs_trees[rsid];
        break;
    case R_BITBLK:
        *paddr = (LONG) & desk_rs_bitblk[rsid];
        break;
    case R_STRING:
        *paddr = (LONG) desk_rs_fstr[rsid];
        break;
    default:
        kcprintf("FIXME: unsupported (faked) rsrc_gaddr type!\n");
        return FALSE;
    }

    return TRUE;
}
