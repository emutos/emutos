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

#include "nls.h"

static const char rs_str_OK[] = N_("OK");
static const char rs_str_Cancel[] = N_("Cancel");
static const char rs_str_Install[] = N_("Install");
static const char rs_str_Remove[] = N_("Remove");
static const char rs_str_Yes[] = N_("Yes");
static const char rs_str_No[] = N_("No");



#define RS_NTED 37



TEDINFO desk_rs_tedinfo[RS_NTED];

static const TEDINFO desk_rs_tedinfo_rom[] = {
    {0L,
     (LONG) N_("Name:  ________.___"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20},      /* 0 */

    {0L,
     (LONG) N_("Size in bytes:  ________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 25},      /* 1 */

    {0L,
     (LONG) N_("Last modified:  __/__/__"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 7, 25},      /* 2 */

    {0L,
     (LONG) "__:__ __",
     (LONG) "9999aa",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 7, 9},       /* 3 */

    {0L,
     (LONG) N_("Drive identifier:  _:"),
     (LONG) "A",
     IBM, 0, TE_LEFT, 4352, 0, 0, 2, 22},       /* 4 */

    {0L,
     (LONG) N_("Disk label:  ___________"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 25},      /* 5 */

    {0L,
     (LONG) N_("Number of folders:     _____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29},      /* 6 */

    {0L,
     (LONG) N_("Number of items:     _____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27},      /* 7 */

    {0L,
     (LONG) N_("Bytes used:  ________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 22},      /* 8 */

    {0L,
     (LONG) N_("Bytes available:  ________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 27},      /* 9 */

    {0L,
     (LONG) N_("Folder name:  ________.___"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 27},      /* 10 */

    {0L,
     (LONG) N_("Created:  __-__-__  "),
     (LONG) "9",
     IBM, 0, TE_LEFT, 4352, 0, 0, 7, 21},       /* 11 */

    {0L,
     (LONG) "__:__ __",
     (LONG) "9999aa",
     IBM, 0, TE_LEFT, 4352, 0, 0, 7, 9},        /* 12 */

    {0L,
     (LONG) N_("Number of folders:     _____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29},      /* 13 */

    {0L,
     (LONG) N_("Number of items:     _____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27},      /* 14 */

    {0L,
     (LONG) N_("Bytes used:  ________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 22},      /* 15 */

    {0L,
     (LONG) N_("Name:  ________.___"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20},      /* 16 */

    {0L,
     (LONG)  N_("Parameters:  ____________________________________________________"),
     (LONG) "X",
     IBM, 0, TE_LEFT, 4352, 0, 0, 53, 66},      /* 17 */

    {0L,
     (LONG) N_("Drive identifier:  _"),
     (LONG) "A",
     IBM, 0, TE_LEFT, 4352, 0, 0, 2, 21},       /* 18 */

    {0L,
     (LONG) N_("Icon label:  ____________"),
     (LONG) "F",
     IBM, 0, TE_LEFT, 4352, 0, 0, 13, 26},      /* 19 */

    {0L,
     (LONG) N_("Application name:  ________.___"),
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
     (LONG) N_("Folders to copy:  ____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 23},      /* 30 */

    {0L,
     (LONG) N_("Items to copy:  ____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 21},      /* 31 */

    {0L,
     (LONG) N_("Folders to delete:  ____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 25},      /* 32 */

    {0L,
     (LONG) N_("Items to delete:  ____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 23},      /* 33 */

    {0L,
     (LONG) N_("Current name:  ________.___"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 28},      /* 34 */

    {0L,
     (LONG) N_("Copy's name:  ________.___"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 27},      /* 35 */

    {0L,
     (LONG) N_("Name:  ________.___"),
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
     (long) N_(" Desk "),
     0, 0, 6, 769},

   { 5, -1, -1, G_TITLE,                        /*** 4 ***/
     NONE,
     NORMAL,
     (long) N_(" File "),
     6, 0, 6, 769},

   { 6, -1, -1, G_TITLE,                        /*** 5 ***/
     NONE,
     NORMAL,
     (long) N_(" Options "),
     12, 0, 9, 769},

   { 2, -1, -1, G_TITLE,                        /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_(" Arrange "),
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
     (long) N_("  Desktop info...   "),
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
     (long) N_("  Open"),
     0, 0, 21, 1},

   { 20, -1, -1, G_STRING,                      /*** 19 ***/
     NONE,
     NORMAL,
     (long) N_("  Info/Rename...  \007I"),
     0, 1, 21, 1},

   { 21, -1, -1, G_STRING,                      /*** 20 ***/
     NONE,
     DISABLED,
     (long) "---------------------",
     0, 2, 21, 1},

   { 22, -1, -1, G_STRING,                      /*** 21 ***/
     NONE,
     NORMAL,
     (long) N_("  Delete...       \007D"),
     0, 3, 21, 1},

   { 23, -1, -1, G_STRING,                      /*** 22 ***/
     NONE,
     NORMAL,
     (long) N_("  Format..."),
     0, 4, 21, 1},

   { 24, -1, -1, G_STRING,                      /*** 23 ***/
     NONE,
     DISABLED,
     (long) "---------------------",
     0, 5, 21, 1},

   { 25, -1, -1, G_STRING,                      /*** 24 ***/
     NONE,
     NORMAL,
     (long) N_("  To Output       ^U"),
     0, 6, 21, 1},

   { 17, -1, -1, G_STRING,                      /*** 25 ***/
     NONE,
     NORMAL,
     (long) N_("  Exit to DOS     ^Q"),
     0, 7, 21, 1},

   { 33, 27, 32, G_BOX,                         /*** 26 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     14, 0, 31, 6},

   { 28, -1, -1, G_STRING,                      /*** 27 ***/
     NONE,
     NORMAL,
     (long) N_("  Install disk drive..."),
     0, 0, 31, 1},

   { 29, -1, -1, G_STRING,                      /*** 28 ***/
     NONE,
     NORMAL,
     (long) N_("  Configure application...  \007A"),
     0, 1, 31, 1},

   { 30, -1, -1, G_STRING,                      /*** 29 ***/
     NONE,
     DISABLED,
     (long) "-------------------------------",
     0, 2, 31, 1},

   { 31, -1, -1, G_STRING,                      /*** 30 ***/
     NONE,
     NORMAL,
     (long) N_("  Set preferences..."),
     0, 3, 31, 1},

   { 32, -1, -1, G_STRING,                      /*** 31 ***/
     NONE,
     NORMAL,
     (long) N_("  Save desktop              \007V"),
     0, 4, 31, 1},

   { 26, -1, -1, G_STRING,                      /*** 32 ***/
     NONE,
     NORMAL,
     (long) N_("  Enter DOS commands        \007C"),
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
     (long) N_("  Sort by name   \007N"),
     0, 2, 20, 1},

   { 38, -1, -1, G_STRING,                      /*** 37 ***/
     NONE,
     NORMAL,
     (long) N_("  Sort by type   \007P"),
     0, 3, 20, 1},

   { 39, -1, -1, G_STRING,                      /*** 38 ***/
     NONE,
     NORMAL,
     (long) N_("  Sort by size   \007Z"),
     0, 4, 20, 1},

   { 33, -1, -1, G_STRING,                      /*** 39 ***/
     LASTOB,
     NORMAL,
     (long) N_("  Sort by date   \007T"),
     0, 5, 20, 1},

#define TR1 40
/* TREE 1 */
   { -1, 1, 11, G_BOX,                          /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 40, 11},

   { 2, -1, -1, G_STRING,                       /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("ITEM INFORMATION / RENAME"),
     2, 1, 16, 1},

   { 3, -1, -1, G_FBOXTEXT,                     /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[0],
     11, 3, 19, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[1],
     2, 4, 24, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[2],
     2, 5, 24, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[3],
     28, 5, 8, 1},

   { 7, -1, -1, G_STRING,                       /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("Attributes:"),
     2, 7, 11, 1},

   { 10, 8, 9, G_IBOX,                          /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     15, 7, 23, 1},

   { 9, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Read/Write"),
     0, 0, 11, 1},

   { 7, -1, -1, G_BUTTON,                       /*** 9 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Read-Only"),
     13, 0, 10, 1},

   { 11, -1, -1, G_BUTTON,                      /*** 10 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     10, 9, 8, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 11 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     22, 9, 8, 1},

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
     (long) N_("DISK INFORMATION"),
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
     0, 0, 40, 11},

   { 2, -1, -1, G_STRING,                       /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("FOLDER INFORMATION"),
     2, 1, 18, 1},

   { 3, -1, -1, G_FBOXTEXT,                     /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[10],
     8, 3, 26, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[11],
     12, 4, 20, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[12],
     31, 4, 8, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[13],
     2, 5, 28, 1},

   { 7, -1, -1, G_FBOXTEXT,                     /*** 6 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[14],
     4, 6, 26, 1},

   { 8, -1, -1, G_FBOXTEXT,                     /*** 7 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[15],
     9, 7, 21, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     16, 9, 8, 1},

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
     (long) N_("Version"),
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
     (long) __DATE__,
     14|(4<<8), 4, 12, 1 },

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("Copyright (c) 2002 by"),
     9, 6, 21, 1 },

   { 8, -1, -1, G_IMAGE,                    /*** 7 ***/
     TOUCHEXIT,
     NORMAL,
     (long) &desk_rs_bitblk[0],
     3, 1, 4, 4 },

   { 9, -1, -1, G_STRING,                   /*** 8 ***/
     NONE,
     NORMAL,
     (long) N_("The EmuTOS development team"),
     6, 7, 27, 1 },

   { 10, -1, -1, G_STRING,                  /*** 9 ***/
     NONE,
     NORMAL,
     (long) N_("Based on 'GPLed' sources"),
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
     (long) N_("EmuTOS is distributed under the GPL"),
     3, 14, 35, 1 },

   { 0, -1, -1, G_STRING,                   /*** 15 ***/
     LASTOB,
     NORMAL,
     (long) N_("See doc/license.txt for details"),
     4, 15, 21, 1 },

#define TR5 86
/* TREE 5 */
   { -1, 1, 7, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 40, 11},

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("NEW FOLDER INFORMATION"),
     3, 1, 23, 1},

   { 3, -1, -1, G_STRING,                   /*** 2 ***/
     NONE,
     NORMAL,
     (long) N_("To create a new folder within the cur-"),
     1, 3, 38, 1},

   { 4, -1, -1, G_STRING,                   /*** 3 ***/
     NONE,
     NORMAL,
     (long) N_("rent window, double-click on the New"),
     1, 4, 36, 1},

   { 5, -1, -1, G_STRING,                   /*** 4 ***/
     NONE,
     NORMAL,
     (long) N_("Folder icon and complete the dialogue"),
     1, 5, 37, 1},

   { 6, -1, -1, G_STRING,                   /*** 5 ***/
     NONE,
     NORMAL,
     (long) N_("that appears by entering the name of"),
     1, 6, 36, 1},

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("the folder you want to create."),
     1, 7, 30, 1},

   { 0, -1, -1, G_BUTTON,                   /*** 7 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     16, 9, 8, 1},

#define TR6 94
/* TREE 6 */
   { -1, 1, 8, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 71, 10},

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("OPEN APPLICATION"),
     3, 1, 16, 1},

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[16],
     9, 3, 19, 1},

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[17],
     3, 4, 65, 1},

   { 5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     51, 8, 8, 1},

   { 6, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Cancel,
     61, 8, 8, 1},

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("Enter the name of the document you want"),
     3, 6, 39, 1},

   { 8, -1, -1, G_STRING,                   /*** 7 ***/
     NONE,
     NORMAL,
     (long) N_("to load, or enter parameter values that"),
     3, 7, 39, 1},

   { 0, -1, -1, G_STRING,                   /*** 8 ***/
     LASTOB,
     NORMAL,
     (long) N_("are acceptable to this application."),
     3, 8, 35, 1},

#define TR7 103
/* TREE 7 */
   { -1, 1, 10, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 40, 11},

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("INSTALL DISK DRIVE"),
     11, 1, 18, 1},

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[18],
     2, 3, 20, 1},

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[19],
     8, 4, 25, 1},

   { 5, -1, -1, G_STRING,                   /*** 4 ***/
     NONE,
     NORMAL,
     (long) N_("Disk type:"),
     6, 6, 10, 1},

   { 8, 6, 7, G_IBOX,                       /*** 5 ***/
     NONE,
     NORMAL,
     (long) 0L,
     16, 6, 24, 1},

   { 7, -1, -1, G_BUTTON,                   /*** 6 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Floppy"),
     2, 0, 8, 1},

   { 5, -1, -1, G_BUTTON,                   /*** 7 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Hard"),
     12, 0, 8, 1},

   { 9, -1, -1, G_BUTTON,                   /*** 8 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Install,
     3, 9, 8, 1},

   { 10, -1, -1, G_BUTTON,                  /*** 9 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Remove,
     16, 9, 8, 1},

   { 0, -1, -1, G_BUTTON,                   /*** 10 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     29, 9, 8, 1},

#define TR8 114
/* TREE 8 */

   { -1, 1, 33, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 40, 23 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("CONFIGURE APPLICATION"),
     9, 1, 21, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[20],
     2, 3, 32, 1 },

   { 4, -1, -1, G_STRING,                   /*** 3 ***/
     NONE,
     NORMAL,
     (long) N_("Document types:"),
     3, 4, 15, 1 },

   { 5, -1, -1, G_FBOXTEXT,                 /*** 4 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[21],
     20, 4, 3, 1 },

   { 6, -1, -1, G_FBOXTEXT,                 /*** 5 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[22],
     25, 4, 3, 1 },

   { 7, -1, -1, G_FBOXTEXT,                 /*** 6 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[23],
     30, 4, 3, 1 },

   { 8, -1, -1, G_FBOXTEXT,                 /*** 7 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[24],
     35, 4, 3, 1 },

   { 9, -1, -1, G_FBOXTEXT,                 /*** 8 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[25],
     20, 5, 3, 1 },

   { 10, -1, -1, G_FBOXTEXT,                /*** 9 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[26],
     25, 5, 3, 1 },

   { 11, -1, -1, G_FBOXTEXT,                /*** 10 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[27],
     30, 5, 3, 1 },

   { 12, -1, -1, G_FBOXTEXT,                /*** 11 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[28],
     35, 5, 3, 1 },

   { 13, -1, -1, G_STRING,                  /*** 12 ***/
     NONE,
     NORMAL,
     (long) N_("Application type:"),
     2, 7, 17, 1 },

   { 17, 14, 16, G_IBOX,                    /*** 13 ***/
     NONE,
     NORMAL,
     (long) 4352L,
     21, 7, 22, 3 },

   { 15, -1, -1, G_BUTTON,                  /*** 14 ***/
     SELECTABLE | RBUTTON | TOUCHEXIT,
     NORMAL,
     (long) "GEM",
     0, 0, 5, 1 },

   { 16, -1, -1, G_BUTTON,                  /*** 15 ***/
     SELECTABLE | RBUTTON | TOUCHEXIT,
     NORMAL,
     (long) "TOS",
     6, 0, 5, 1 },

   { 13, -1, -1, G_BUTTON,                  /*** 16 ***/
     SELECTABLE | RBUTTON | TOUCHEXIT,
     NORMAL,
     (long) "TTP",
     12, 0, 5, 1 },

   { 18, -1, -1, G_STRING,                  /*** 17 ***/
     NONE,
     NORMAL,
     (long) N_("Needs full memory?"),
     2, 9, 18, 1 },

   { 21, 19, 20, G_IBOX,                    /*** 18 ***/
     NONE,
     NORMAL,
     (long) 4352L,
     22, 9, 15, 1 },

   { 20, -1, -1, G_BUTTON,                  /*** 19 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 6, 1 },

   { 18, -1, -1, G_BUTTON,                  /*** 20 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     8, 0, 6, 1 },

   { 22, -1, -1, G_STRING,                  /*** 21 ***/
     NONE,
     NORMAL,
     (long) N_("Icon type:"),
     2, 11, 10, 1 },

   { 23, -1, -1, G_BOXTEXT,                 /*** 22 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[29],
     7, 13, 27, 1 },

   { 26, 24, 25, G_BOX,                     /*** 23 ***/
     NONE,
     NORMAL,
     (long) 69888L,
     7, 14, 24, 6 },

   { 25, -1, -1, G_BOX,                     /*** 24 ***/
     NONE,
     NORMAL,
     (long) 69889L,
     3, 1, 6, 4 },

   { 23, -1, -1, G_BOX,                     /*** 25 ***/
     NONE,
     NORMAL,
     (long) 69889L,
     12, 1, 6, 4 },

   { 31, 27, 29, G_BOX,                     /*** 26 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 69888L,
     31, 14, 3, 6 },

   { 28, -1, -1, G_BOXCHAR,                 /*** 27 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 0x01011100L,
     0, 0, 3, 2 },

   { 29, -1, -1, G_BOXCHAR,                 /*** 28 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 0x02011100L,
     0, 4, 3, 2 },

   { 26, 30, 30, G_BOX,                     /*** 29 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 69905L,
     0, 2, 3, 2 },

   { 29, -1, -1, G_BOX,                     /*** 30 ***/
     TOUCHEXIT,
     NORMAL,
     (long) 69889L,
     0x400, 0, 0x102, 1 },

   { 32, -1, -1, G_BUTTON,                  /*** 31 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Install,
     3, 21, 9, 1 },

   { 33, -1, -1, G_BUTTON,                  /*** 32 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Remove,
     16, 21, 8, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 33 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     29, 21, 8, 1 },

#define TR9 148
/* TREE 9 */

   { -1, 1, 5, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 34, 8 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("COPY FOLDERS / ITEMS"),
     3, 1, 20, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[30],
     3, 3, 22, 1 },

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[31],
     5, 4, 20, 1 },

   { 5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     13, 6, 8, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     23, 6, 8, 1 },

#define TR10 154
/* TREE 10 */

   { -1, 1, 5, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 30, 8 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("DELETE FOLDERS / ITEMS"),
     3, 1, 23, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[32],
     3, 3, 24, 1 },

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[33],
     5, 4, 22, 1 },

   { 5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_OK,
     9, 6, 8, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     19, 6, 8, 1 },

#define TR11 160
/* TREE 11 */

   { -1, 1, 6, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 34, 8 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("NAME CONFLICT DURING COPY"),
     3, 1, 25, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[34],
     3, 3, 27, 1 },

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[35],
     4, 4, 26, 1 },

   { 5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     3, 6, 8, 1 },

   { 6, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Cancel,
     13, 6, 8, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 6 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) N_("Stop"),
     23, 6, 8, 1 },

#define TR12 167
/* TREE 12 */

   { -1, 1, 4, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 27, 7 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("NEW FOLDER"),
     3, 1, 10, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[36],
     3, 3, 19, 1 },

   { 4, -1, -1, G_BUTTON,                   /*** 3 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     6, 5, 8, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     16, 5, 8, 1 },

#define TR13 172
/* TREE 13 */

   { -1, 1, 38, G_BOX,                      /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 40, 21 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("SET PREFERENCES"),
     12, 1, 15, 1 },

   { 3, -1, -1, G_STRING,                   /*** 2 ***/
     NONE,
     NORMAL,
     (long) N_("Confirm deletes?"),
     4, 3, 16, 1 },

   { 6, 4, 5, G_IBOX,                       /*** 3 ***/
     NONE,
     NORMAL,
     (long) 0L,
     22, 3, 12, 1 },

   { 5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1 },

   { 3, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     7, 0, 5, 1 },

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("Confirm copies?"),
     5, 5, 15, 1 },

   { 10, 8, 9, G_IBOX,                      /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     22, 5, 12, 1 },

   { 9, -1, -1, G_BUTTON,                   /*** 8 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1 },

   { 7, -1, -1, G_BUTTON,                   /*** 9 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     7, 0, 5, 1 },

   { 11, -1, -1, G_STRING,                  /*** 10 ***/
     NONE,
     NORMAL,
     (long) N_("Confirm overwrites?"),
     1, 7, 19, 1 },

   { 14, 12, 13, G_IBOX,                    /*** 11 ***/
     NONE,
     NORMAL,
     (long) 0L,
     22, 7, 12, 1 },

   { 13, -1, -1, G_BUTTON,                  /*** 12 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1 },

   { 11, -1, -1, G_BUTTON,                  /*** 13 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     7, 0, 5, 1 },

   { 15, -1, -1, G_STRING,                  /*** 14 ***/
     NONE,
     NORMAL,
     (long) N_("Double-click speed:"),
     1, 9, 19, 1 },

   { 21, 16, 20, G_IBOX,                    /*** 15 ***/
     NONE,
     NORMAL,
     (long) 0L,
     22, 9, 16, 1 },

   { 17, -1, -1, G_BUTTON,                  /*** 16 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "1",
     0, 0, 2, 1 },

   { 18, -1, -1, G_BUTTON,                  /*** 17 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "2",
     3, 0, 2, 1 },

   { 19, -1, -1, G_BUTTON,                  /*** 18 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "3",
     6, 0, 2, 1 },

   { 20, -1, -1, G_BUTTON,                  /*** 19 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "4",
     9, 0, 2, 1 },

   { 15, -1, -1, G_BUTTON,                  /*** 20 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) "5",
     12, 0, 2, 1 },

   { 24, 22, 23, G_IBOX,                    /*** 21 ***/
     NONE,
     NORMAL,
     (long) 0L,
     21, 11, 19, 1 },

   { 23, -1, -1, G_BUTTON,                  /*** 22 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Click"),
     0, 0, 7, 1 },

   { 21, -1, -1, G_BUTTON,                  /*** 23 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("No click"),
     8, 0, 10, 1 },

   { 25, -1, -1, G_STRING,                  /*** 24 ***/
     NONE,
     NORMAL,
     (long) N_("To drop down menus:"),
     1, 11, 19, 1 },

   { 26, -1, -1, G_STRING,                  /*** 25 ***/
     NONE,
     NORMAL,
     (long) N_("Sound effects:"),
     4, 13, 14, 1 },

   { 29, 27, 28, G_IBOX,                    /*** 26 ***/
     NONE,
     NORMAL,
     (long) 0L,
     20, 13, 12, 1 },

   { 28, -1, -1, G_BUTTON,                  /*** 27 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("On"),
     0, 0, 5, 1 },

   { 26, -1, -1, G_BUTTON,                  /*** 28 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Off"),
     7, 0, 5, 1 },

   { 30, -1, -1, G_STRING,                  /*** 29 ***/
     NONE,
     NORMAL,
     (long) N_("Time format:"),
     3, 15, 12, 1 },

   { 33, 31, 32, G_IBOX,                    /*** 30 ***/
     NONE,
     NORMAL,
     (long) 0L,
     17, 15, 20, 1 },

   { 32, -1, -1, G_BUTTON,                  /*** 31 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("12 Hour"),
     0, 0, 9, 1 },

   { 30, -1, -1, G_BUTTON,                  /*** 32 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("24 Hour"),
     11, 0, 9, 1 },

   { 34, -1, -1, G_STRING,                  /*** 33 ***/
     NONE,
     NORMAL,
     (long) N_("Date format:"),
     2, 17, 12, 1 },

   { 37, 35, 36, G_IBOX,                    /*** 34 ***/
     NONE,
     NORMAL,
     (long) 0L,
     16, 17, 23, 1 },

   { 36, -1, -1, G_BUTTON,                  /*** 35 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("MM-DD-YY"),
     0, 0, 11, 1 },

   { 34, -1, -1, G_BUTTON,                  /*** 36 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("DD-MM-YY"),
     12, 0, 11, 1 },

   { 38, -1, -1, G_BUTTON,                  /*** 37 ***/
     SELECTABLE | DEFAULT | EXIT,
     NORMAL,
     (long) rs_str_OK,
     6, 19, 8, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 38 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     26, 19, 8, 1 }
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
    N_("%L bytes used in %W items."),
    N_("  Show as icons  \007S"),
    N_("application"),
    N_("documents"),
    "FORMAT.COM",
    N_("  Show as text   \007S"),
    "DESKHI.ICN",
    "EMUDESK.INF",
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
    N_("New Folder"),
    N_("Disk Drives:"),
    "am",
    "pm",
    N_("Free"),
    "Free",
    N_("[1][The document type you selected is not|"
       "configured to work with a specific|"
       "application.  Use the \"Configure|"
       "application\" command to associate this|"
       "document type with an application.][  OK  ]"),
    N_("[1][The GEM Desktop has no more available|"
       "windows.  Before you open a disk, close|"
       "a window you're not using.][  OK  ]"),
    N_("[1][Cannot find the FORMAT program.  If you|"
       "are using a dual-floppy system, you must|"
       "format disks from your DOS disk.  If you|"
       "are using a hard disk, copy FORMAT to |"
       "the root directory.][OK]"),
    N_("[3][Formatting will ERASE all|"
       "information on the disk in drive|"
       "%S:.  Click on OK only if you don't|"
       "mind losing the information on|"
       "this disk.][  OK  |Cancel]"),
    N_("[3][You cannot copy a parent folder|"
       "into one of its child folders.][  OK  ]"),
    N_("[3][If you are sure you want to|"
       "delete ALL the information on the|"
       "disk in drive %S:, click on OK.|"
       "Otherwise, click on Cancel.][  OK  |Cancel]"),
    N_("[1][There is not enough space available|"
       "to configure this application.  To free|"
       "up some space, you'll have to remove|"
       "one of your currently configured|applications.][  OK  ]"),
    N_("[2][A folder with that name already|"
       "exists or your disk is full.|"
       "Retry with a new name, or Cancel|"
       "and check the available disk space.][ Cancel | Retry ]"),
    N_("[1][This disk does not have enough room for|"
       "the information you are trying to copy.|"
       "Some items, however, may have been|"
       "copied to the disk.][  OK  ]"),
    N_("[3][The GEM Desktop cannot find the|"
       "documents DESKLO.ICN, DESKHI.ICN,|"
       "or DESKTOP.INF in the DOS search|"
       "path.  These documents are required|"
       "to run the GEM Desktop.][Cancel]"),
    N_("[3][To save your desktop, insert your|"
       "GEM DESKTOP disk into drive A:, close|"
       "the drive door, and click on OK.|"
       "Click on Cancel if you don't want to|"
       "save the desktop.][  OK  |Cancel]"),
    N_("[3][Sorry, but you cannot place any more|"
       "folders inside of your current one.|"
       "See your documentation for limits on|"
       "folders inside other folders.][Cancel]"),
    N_("[3][Sorry, but the Directory name you have|"
       "entered exceeds the maximum number of|"
       "characters.  See your documentation for|"
       "limits on the number of characters you|"
       "can enter.][   OK   ]"),
};




/* Counts the occurance of c in str */
static int count_chars(char *str, char c)
{
    int count;

    count = 0;
    while(*str) {
        if(*str++ == c) 
            count ++;
    }

    return count;
}

/* 
 * the xlate_ functions below are also used by the GEM rsc in aes/gem_rsc.c
 */

/* Translates the strings in an OBJECT array */
void xlate_obj_array(OBJECT *obj_array, int nobj)
{
    register OBJECT *obj;

    for(obj = obj_array; --nobj >= 0 ; obj++) {
        switch(obj->ob_type) {
        case G_TEXT:
        case G_BOXTEXT:
        case G_FTEXT:
        case G_FBOXTEXT:
            {
                LONG * str = & ((TEDINFO *)obj->ob_spec)->te_ptmplt;
                *str = (LONG) gettext((char *) *str);
            }
            break;
        case G_STRING:
        case G_BUTTON:
        case G_TITLE:
            obj->ob_spec = (LONG) gettext( (char *) obj->ob_spec);
            break;
        default:
            break;
        }
    }
}

/* Translates and fixes the TEDINFO strings */
void xlate_fix_tedinfo(TEDINFO *tedinfo, int nted)
{
    register int i = 0;
    long len;
    int j;
    char *tedinfptr;

    /* translate strings in TEDINFO */
    for (i = 0; i < nted; i++) {
        TEDINFO *ted = &tedinfo[i];
        ted->te_ptmplt = (LONG) gettext( (char *) ted->te_ptmplt);
    }

    /* Fix TEDINFO strings: */
    len = 0;
    for (i = 0; i < nted; i++) {
        if (tedinfo[i].te_ptext == 0) {
            /* Count number of '_' in strings ( +1 for \0 at the end ): */
            len += count_chars((char *) tedinfo[i].te_ptmplt, '_') + 1;
        }
    }
    tedinfptr = (char *) dos_alloc(len);        /* Get memory */
    for (i = 0; i < nted; i++) {
        if (tedinfo[i].te_ptext == 0) {
            tedinfo[i].te_ptext = (LONG) tedinfptr;
            *tedinfptr++ = '@'; /* First character of uninitialized string */
            len = count_chars((char *) tedinfo[i].te_ptmplt, '_');
            for (j = 0; j < len; j++) {
                *tedinfptr++ = '_';     /* Set other characters to '_' */
            }
            *tedinfptr++ = 0;   /* Final 0 */
        }
    }
}


void desk_rs_init(void)
{
    register int i;

    /* Copy data from ROM to RAM: */
    memcpy(desk_rs_obj, desk_rs_obj_rom, RS_NOBS * sizeof(OBJECT));
    memcpy(desk_rs_tedinfo, desk_rs_tedinfo_rom,
           RS_NTED * sizeof(TEDINFO));

    /* Fix objects coordinates: */
    for(i = 0 ; i < RS_NOBS ; i++) {
        rsrc_obfix((LONG) desk_rs_obj, i);
    }

    /* translate strings in objects */
    xlate_obj_array(desk_rs_obj, RS_NOBS);

    /* translate and fix TEDINFO strings */
    xlate_fix_tedinfo(desk_rs_tedinfo, RS_NTED);
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
        *paddr = (LONG) gettext( desk_rs_fstr[rsid] );
        break;
    default:
        kcprintf("FIXME: unsupported (faked) rsrc_gaddr type!\n");
        return FALSE;
    }

    return TRUE;
}

