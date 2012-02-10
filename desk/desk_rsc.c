/*
 *       Copyright 2002, 2007 The EmuTOS development team
 *
 *       This software is licenced under the GNU Public License.
 *       Please see LICENSE.TXT for further information.
 */

/*
 * This file contains the Desk's RSC data
 */

#include "config.h"
#include "string.h"

#include "portab.h"
#include "obdefs.h"
#include "desk_rsc.h"
#include "nls.h"

static const char rs_str_OK[] = N_("OK");
static const char rs_str_Cancel[] = N_("Cancel");
static const char rs_str_Install[] = N_("Install");
static const char rs_str_Remove[] = N_("Remove");
static const char rs_str_Yes[] = N_("Yes");
static const char rs_str_No[] = N_("No");


TEDINFO desk_rs_tedinfo[RS_NTED];

static const TEDINFO desk_rs_tedinfo_rom[] = {
    {0L,
     (LONG) N_("Name:  ________.___"),
     (LONG) "f",
     IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20},      /* 0 */

    {0L,
     (LONG) N_("Size in bytes:  __________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 11, 27},     /* 1 */

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
     IBM, 0, TE_CNTR, 4352, 0, 0, 2, 22},       /* 4 */

    {0L,
     (LONG) N_("Disk label:  ___________"),
     (LONG) "f",
     IBM, 0, TE_CNTR, 4352, 0, 0, 12, 25},      /* 5 */

    {0L,
     (LONG) N_("Number of folders:     _____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29},      /* 6 */

    {0L,
     (LONG) N_("Number of items:     _____"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27},      /* 7 */

    {0L,
     (LONG) N_("Bytes used:  __________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 11, 24},      /* 8 */

    {0L,
     (LONG) N_("Bytes available:  __________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 11, 29},      /* 9 */

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
     (LONG) N_("Bytes used:  __________"),
     (LONG) "9",
     IBM, 0, TE_RIGHT, 4352, 0, 0, 11, 24},      /* 15 */

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

/* EmuTOS logo as designed by Martin */
static const int rs_logo_img[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x000B, 0xD000, 
    0x000B, 0xD000, 0x001B, 0xD800, 
    0x001B, 0xD800, 0x002B, 0xD400, 
    0x003B, 0xDC00, 0x006B, 0xD600, 
    0x005B, 0xDA00, 0x00AB, 0xD500, 
    0x00DB, 0xDB00, 0x01AB, 0xD580, 
    0x015B, 0xDA80, 0x02AB, 0xD540, 
    0x0353, 0xCAC0, 0x06A3, 0xC560, 
    0x0543, 0xC2A0, 0x0A83, 0xC150, 
    0x0D03, 0xC0B0, 0x1A03, 0xC058, 
    0x1403, 0xC028, 0x2803, 0xC014, 
    0x3003, 0xC00C, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x0000, 
};


const BITBLK desk_rs_bitblk[] = {
    {(LONG) rs_logo_img, 4, 32, 0, 0, 1},
};


static char rs_str_iconOrText[32];      /* was: "  xxxx xx xxxxx  xx" */


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

   { 0, 8, 37, G_IBOX,                          /*** 7 ***/
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
     (long) N_("  Desktop info..."),
     0, 0, 20, 1},

   { 11, -1, -1, G_STRING,                      /*** 10 ***/
     NONE,
     DISABLED,
     (long) N_("--------------------"),
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

   { 30, 18, 29, G_BOX,                         /*** 17 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     8, 0, 21, 12},

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
     (long) N_("---------------------"),
     0, 2, 21, 1},

   { 22, -1, -1, G_STRING,                      /*** 21 ***/
     NONE,
     NORMAL,
     (long) N_("  New Folder..."),
     0, 3, 21, 1},

   { 23, -1, -1, G_STRING,                      /*** 22 ***/
     NONE,
     NORMAL,
     (long) N_("  Close"),
     0, 4, 21, 1},

   { 24, -1, -1, G_STRING,                      /*** 23 ***/
     NONE,
     NORMAL,
     (long) N_("  Close window"),
     0, 5, 21, 1},


   { 25, -1, -1, G_STRING,                      /*** 24 ***/
     NONE,
     DISABLED,
     (long) N_("---------------------"),
     0, 6, 21, 1},

   { 26, -1, -1, G_STRING,                      /*** 25 ***/
     NONE,
     NORMAL,
     (long) N_("  Delete...       \007D"),
     0, 7, 21, 1},

   { 27, -1, -1, G_STRING,                      /*** 26 ***/
     NONE,
     NORMAL,
     (long) N_("  Format..."),
     0, 8, 21, 1},

   { 28, -1, -1, G_STRING,                      /*** 27 ***/
     NONE,
     DISABLED,
     (long) N_("---------------------"),
     0, 9, 21, 1},

   { 29, -1, -1, G_STRING,                      /*** 28 ***/
     NONE,
     NORMAL,
     (long) N_("  Execute EmuCON  ^Z"),
     0, 10, 21, 1},

   { 17, -1, -1, G_STRING,                      /*** 29 ***/
     NONE,
     NORMAL,
     (long) N_("  Shutdown        ^Q"),
     0, 11, 21, 1},

   { 37, 31, 36, G_BOX,                         /*** 30 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     14, 0, 31, 6},

   { 32, -1, -1, G_STRING,                      /*** 31 ***/
     NONE,
     NORMAL,
     (long) N_("  Install disk drive..."),
     0, 0, 31, 1},

   { 33, -1, -1, G_STRING,                      /*** 32 ***/
     NONE,
     NORMAL,
     (long) N_("  Configure application...  \007A"),
     0, 1, 31, 1},

   { 34, -1, -1, G_STRING,                      /*** 33 ***/
     NONE,
     DISABLED,
     (long) N_("-------------------------------"),
     0, 2, 31, 1},

   { 35, -1, -1, G_STRING,                      /*** 34 ***/
     NONE,
     NORMAL,
     (long) N_("  Set preferences..."),
     0, 3, 31, 1},

   { 36, -1, -1, G_STRING,                      /*** 35 ***/
     NONE,
     NORMAL,
     (long) N_("  Save desktop              \007V"),
     0, 4, 31, 1},

   { 30, -1, -1, G_STRING,                      /*** 36 ***/
     NONE,
     NORMAL,
     (long) N_("  Change resolution         \007C"),
     0, 5, 31, 1},

   { 7, 38, 43, G_BOX,                          /*** 37 ***/
     NONE,
     NORMAL,
     (long) 16716032L,
     23, 0, 20, 6},

   { 39, -1, -1, G_STRING,                      /*** 38 ***/
     NONE,
     NORMAL,
     (long) rs_str_iconOrText,
     0, 0, 20, 1},

   { 40, -1, -1, G_STRING,                      /*** 39 ***/
     NONE,
     DISABLED,
     (long) N_("--------------------"),
     0, 1, 20, 1},

   { 41, -1, -1, G_STRING,                      /*** 40 ***/
     NONE,
     NORMAL,
     (long) N_("  Sort by name   \007N"),
     0, 2, 20, 1},

   { 42, -1, -1, G_STRING,                      /*** 41 ***/
     NONE,
     NORMAL,
     (long) N_("  Sort by type   \007P"),
     0, 3, 20, 1},

   { 43, -1, -1, G_STRING,                      /*** 42 ***/
     NONE,
     NORMAL,
     (long) N_("  Sort by size   \007Z"),
     0, 4, 20, 1},

   { 37, -1, -1, G_STRING,                      /*** 43 ***/
     LASTOB,
     NORMAL,
     (long) N_("  Sort by date   \007T"),
     0, 5, 20, 1},

#define TR1 44
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
     11, 3, 20, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[1],
     2, 4, 30, 1},

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
     1, 7, 11, 1},

   { 10, 8, 9, G_IBOX,                          /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     13, 7, 26, 1},

   { 9, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Read/Write"),
     0, 0, 11, 1},

   { 7, -1, -1, G_BUTTON,                       /*** 9 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Read-Only"),
     13, 0, 13, 1},

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

#define TR2 56
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
     2, 3, 32, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[5],
     2, 4, 32, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[6],
     2, 5, 32, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[7],
     2, 6, 32, 1},

   { 7, -1, -1, G_FBOXTEXT,                     /*** 6 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[8],
     2, 7, 32, 1},

   { 8, -1, -1, G_FBOXTEXT,                     /*** 7 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[9],
     2, 8, 32, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     26, 10, 8, 1},

#define TR3 65
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
     8, 3, 28, 1},

   { 4, -1, -1, G_FBOXTEXT,                     /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[11],
     7, 4, 20, 1},

   { 5, -1, -1, G_FBOXTEXT,                     /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[12],
     26, 4, 8, 1},

   { 6, -1, -1, G_FBOXTEXT,                     /*** 5 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[13],
     4, 5, 30, 1},

   { 7, -1, -1, G_FBOXTEXT,                     /*** 6 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[14],
     4, 6, 30, 1},

   { 8, -1, -1, G_FBOXTEXT,                     /*** 7 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[15],
     4, 7, 30, 1},

   { 0, -1, -1, G_BUTTON,                       /*** 8 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_OK,
     16, 9, 8, 1},

#define TR4 74
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
     (long) "0.0.0",
     22, 2, 4, 1 },

   { 5, -1, -1, G_IMAGE,                    /*** 4 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_bitblk[0],
     33, 1, 4, 4 },

   { 6, -1, -1, G_STRING,                   /*** 5 ***/
     NONE,
     NORMAL,
//     (long) __DATE__,
     (long) "http://emutos.sourceforge.net",
//     14|(4<<8), 4, 12, 1 },
     5, 7, 20, 1 },

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("Copyright (c) by"),
     12, 4, 18, 1 },

   { 8, -1, -1, G_IMAGE,                    /*** 7 ***/
     TOUCHEXIT,
     NORMAL,
     (long) &desk_rs_bitblk[0],
     3, 1, 4, 4 },

   { 9, -1, -1, G_STRING,                   /*** 8 ***/
     NONE,
     NORMAL,
     (long) N_("The EmuTOS development team"),
     6, 5, 27, 1 },

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
     2, 14, 36, 1 },

   { 0, -1, -1, G_STRING,                   /*** 15 ***/
     LASTOB,
     NORMAL,
     (long) N_("See doc/license.txt for details"),
     2, 15, 36, 1 },

#define TR5 90
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

#define TR6 98
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

#define TR7 107
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
     2, 3, 25, 1},

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[19],
     8, 4, 25, 1},

   { 5, -1, -1, G_STRING,                   /*** 4 ***/
     NONE,
     NORMAL,
     (long) N_("Disk type:"),
     2, 6, 14, 1},

   { 8, 6, 7, G_IBOX,                       /*** 5 ***/
     NONE,
     NORMAL,
     (long) 0L,
     17, 6, 23, 1},

   { 7, -1, -1, G_BUTTON,                   /*** 6 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Floppy"),
     1, 0, 10, 1},

   { 5, -1, -1, G_BUTTON,                   /*** 7 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Hard"),
     12, 0, 10, 1},

   { 9, -1, -1, G_BUTTON,                   /*** 8 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Install,
     2, 9, 12, 1},

   { 10, -1, -1, G_BUTTON,                  /*** 9 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Remove,
     16, 9, 10, 1},

   { 0, -1, -1, G_BUTTON,                   /*** 10 ***/
     SELECTABLE | DEFAULT | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     28, 9, 10, 1},

#define TR8 118
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
     2, 4, 15, 1 },

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
     24, 9, 15, 1 },

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
     2, 21, 12, 1 },

   { 33, -1, -1, G_BUTTON,                  /*** 32 ***/
     SELECTABLE | EXIT,
     NORMAL,
     (long) rs_str_Remove,
     16, 21, 10, 1 },

   { 0, -1, -1, G_BUTTON,                   /*** 33 ***/
     SELECTABLE | EXIT | LASTOB,
     NORMAL,
     (long) rs_str_Cancel,
     28, 21, 10, 1 },

#define TR9 152
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
     2, 3, 30, 1 },

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[31],
     2, 4, 30, 1 },

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

#define TR10 158
/* TREE 10 */

   { -1, 1, 5, G_BOX,                       /*** 0 ***/
     NONE,
     OUTLINED,
     (long) 135424L,
     0, 0, 34, 8 },

   { 2, -1, -1, G_STRING,                   /*** 1 ***/
     NONE,
     NORMAL,
     (long) N_("DELETE FOLDERS / ITEMS"),
     2, 1, 26, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[32],
     2, 3, 30, 1 },

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[33],
     2, 4, 30, 1 },

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

#define TR11 164
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
     2, 1, 25, 1 },

   { 3, -1, -1, G_FBOXTEXT,                 /*** 2 ***/
     NONE,
     NORMAL,
     (long) &desk_rs_tedinfo[34],
     3, 3, 28, 1 },

   { 4, -1, -1, G_FBOXTEXT,                 /*** 3 ***/
     EDITABLE,
     NORMAL,
     (long) &desk_rs_tedinfo[35],
     3, 4, 30, 1 },

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

#define TR12 171
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

#define TR13 176
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
     1, 3, 19, 1 },

   { 6, 4, 5, G_IBOX,                       /*** 3 ***/
     NONE,
     NORMAL,
     (long) 0L,
     26, 3, 12, 1 },

   { 5, -1, -1, G_BUTTON,                   /*** 4 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1 },

   { 3, -1, -1, G_BUTTON,                   /*** 5 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     6, 0, 5, 1 },

   { 7, -1, -1, G_STRING,                   /*** 6 ***/
     NONE,
     NORMAL,
     (long) N_("Confirm copies?"),
     1, 5, 19, 1 },

   { 10, 8, 9, G_IBOX,                      /*** 7 ***/
     NONE,
     NORMAL,
     (long) 0L,
     26, 5, 12, 1 },

   { 9, -1, -1, G_BUTTON,                   /*** 8 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1 },

   { 7, -1, -1, G_BUTTON,                   /*** 9 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     6, 0, 5, 1 },

   { 11, -1, -1, G_STRING,                  /*** 10 ***/
     NONE,
     NORMAL,
     (long) N_("Confirm overwrites?"),
     1, 7, 23, 1 },

   { 14, 12, 13, G_IBOX,                    /*** 11 ***/
     NONE,
     NORMAL,
     (long) 0L,
     26, 7, 12, 1 },

   { 13, -1, -1, G_BUTTON,                  /*** 12 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_Yes,
     0, 0, 5, 1 },

   { 11, -1, -1, G_BUTTON,                  /*** 13 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) rs_str_No,
     6, 0, 5, 1 },

   { 15, -1, -1, G_STRING,                  /*** 14 ***/
     NONE,
     NORMAL,
     (long) N_("Double-click speed:"),
     1, 9, 19, 1 },

   { 21, 16, 20, G_IBOX,                    /*** 15 ***/
     NONE,
     NORMAL,
     (long) 0L,
     23, 9, 16, 1 },

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
     1, 13, 14, 1 },

   { 29, 27, 28, G_IBOX,                    /*** 26 ***/
     NONE,
     NORMAL,
     (long) 0L,
     21, 13, 13, 1 },

   { 28, -1, -1, G_BUTTON,                  /*** 27 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("On"),
     0, 0, 6, 1 },

   { 26, -1, -1, G_BUTTON,                  /*** 28 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("Off"),
     7, 0, 5, 1 },

   { 30, -1, -1, G_STRING,                  /*** 29 ***/
     NONE,
     NORMAL,
     (long) N_("Time format:"),
     1, 15, 12, 1 },

   { 33, 31, 32, G_IBOX,                    /*** 30 ***/
     NONE,
     NORMAL,
     (long) 0L,
     18, 15, 20, 1 },

   { 32, -1, -1, G_BUTTON,                  /*** 31 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("12 Hour"),
     0, 0, 9, 1 },

   { 30, -1, -1, G_BUTTON,                  /*** 32 ***/
     SELECTABLE | RBUTTON,
     NORMAL,
     (long) N_("24 Hour"),
     10, 0, 9, 1 },

   { 34, -1, -1, G_STRING,                  /*** 33 ***/
     NONE,
     NORMAL,
     (long) N_("Date format:"),
     1, 17, 12, 1 },

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


OBJECT *desk_rs_trees[] = {
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


const char *desk_rs_fstr[] = {
    N_("%ld bytes used in %d items."),
    N_("  Show as icons  \007S"),
    N_("application"),
    N_("documents"),
    N_("[2][Switch resolution?][Yes|No]"),
    N_("  Show as text   \007S"),
    NULL,                       /* unused, was "DESKHI.ICN" */
    "A:\\EMUDESK.INF",
    NULL,                       /* unused, was "DESKLO.ICN" */
    NULL,                       /* unused, was "OUTPUT.APP" */
    NULL,                       /* unused, was "????????.BAT" */
    "#F FF 28 @ *.*@",
    "#D FF 02 @ *.*@",
    "#Y 08 FF *.GTP@ @",
    "#G 08 FF *.APP@ @",
    "#G 08 FF *.PRG@ @",
    "#P 08 FF *.TTP@ @",
    "#F 08 FF *.TOS@ @",
    N_("New Folder"),
    N_("Disk Drives:"),
    "am",
    "pm",
    NULL,                       /* unused, was "Free" */
    NULL,                       /* unused, was "Free" */
    N_("[1][The document type you selected is not|"
       "configured to work with a specific|"
       "application.  Use the \"Configure|"
       "application\" command to associate this|"
       "document type with an application.][  OK  ]"),
    N_("[1][The GEM Desktop has no more available|"
       "windows.  Before you open a disk, close|"
       "a window you're not using.][  OK  ]"),
    N_("[3][Cannot find FORMAT.PRG|"
       "or FORMAT.TTP.][  OK  ]"),
    N_("[3][Formatting will ERASE all|"
       "information on the disk in drive|"
       "%c:.  Click on OK only if you don't|"
       "mind losing the information on|"
       "this disk.][  OK  |Cancel]"),
    N_("[3][You cannot copy a parent folder|"
       "into one of its child folders.][  OK  ]"),
    N_("[3][If you are sure you want to|"
       "delete ALL the information on the|"
       "disk in drive %c:, click on OK.|"
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
#ifdef DESK1
    N_("[1][You cannot open the trash can icon|"
       "into a window. To delete a disk,|"
       "folder, document, or application,|"
       "drag it to the trash can.][  OK  ]"),
    N_("[1][The trash can is the destination to|"
        "which you drag the disks, folders,|"
        "documents, or applications that you|"
        "want to delete PERMANENTLY!][  OK  ]"),
    N_("[1][You cannot drag folders, documents, or|"
       "applications onto the Desktop.  However,|"
       "you can copy them to disks or drag them|"
       "to the trash.][  OK  ]"),
    N_("[1][You cannot drag the trash can|"
       "into a window.][  OK  ]"),
    N_("[1][You can drag the trash can to another|"
       "location on the GEM Desktop, but you|"
       "cannot place it on top of another icon.][  OK  ]"),
#endif
};


void desk_rs_init(void)
{
    /* Copy data from ROM to RAM: */
    memcpy(desk_rs_obj, desk_rs_obj_rom, RS_NOBS * sizeof(OBJECT));
    memcpy(desk_rs_tedinfo, desk_rs_tedinfo_rom,
           RS_NTED * sizeof(TEDINFO));
}

