/*
 *       Copyright 2002 The EmuTOS development team
 *
 *       This software is licenced under the GNU Public License.
 *       Please see LICENSE.TXT for further information.
 */

/*
 * This file contains the AES' RSC data
 */

#include <string.h>

#include "portab.h"
#include "obdefs.h"
#include "gembind.h"
#include "aesbind.h"


static char rs_s2[] = "--------------------";
static char rs_s34[] = "2";
static char rs_s36[] = "3";
static char rs_s38[] = "4";
static char rs_s0[] = "---------------------";
static char rs_s1[] = "---------------------";
static char rs_s3[] = "--------------------";
static char rs_s4[] = "OK";
static char rs_s10[] = "Cancel";
static char rs_s5[] = "OK";
static char rs_s6[] = "OK";
static char rs_s7[] = "OK";
static char rs_s8[] = "OK";
static char rs_s9[] = "OK";
static char rs_s11[] = "Cancel";
static char rs_s13[] = "Install";
static char rs_s15[] = "Remove";
static char rs_s12[] = "Cancel";
static char rs_s26[] = "Yes";
static char rs_s28[] = "No";
static char rs_s14[] = "Install";
static char rs_s16[] = "Remove";
static char rs_s17[] = "Cancel";
static char rs_s18[] = "OK";
static char rs_s19[] = "Cancel";
static char rs_s20[] = "OK";
static char rs_s21[] = "Cancel";
static char rs_s22[] = "OK";
static char rs_s23[] = "Cancel";
static char rs_s24[] = "OK";
static char rs_s25[] = "Cancel";
static char rs_s27[] = "Yes";
static char rs_s29[] = "No";
static char rs_s30[] = "Yes";
static char rs_s31[] = "No";
static char rs_s32[] = "Yes";
static char rs_s33[] = "No";
static char rs_s35[] = "2";
static char rs_s37[] = "3";
static char rs_s39[] = "4";
static char rs_s40[] = "OK";
static char rs_s41[] = "Cancel";
static char rs_s42[] = "Free";
static char rs_s43[] = "Free";
static char rs_s46[] = "@2345678901";
static char rs_s74[] = "Name:  ________.___";
static char rs_s48[] = "f";
static char rs_s44[] = "9";
static char rs_s45[] = "9";
static char rs_s56[] = "__:__ __";
static char rs_s58[] = "9999aa";
static char rs_s77[] = "@";
static char rs_s79[] = "A";
static char rs_s47[] = "@2345678901";
static char rs_s49[] = "f";
static char rs_s60[] = "@1234";
static char rs_s62[] = "Number of folders:     _____";
static char rs_s50[] = "9";
static char rs_s65[] = "Number of items:     _____";
static char rs_s51[] = "9";
static char rs_s68[] = "@7654321";
static char rs_s70[] = "Bytes used:  ________";
static char rs_s52[] = "9";
static char rs_s53[] = "9";
static char rs_s54[] = "f";
static char rs_s55[] = "9";
static char rs_s57[] = "__:__ __";
static char rs_s59[] = "9999aa";
static char rs_s61[] = "@1234";
static char rs_s63[] = "Number of folders:     _____";
static char rs_s64[] = "9";
static char rs_s66[] = "Number of items:     _____";
static char rs_s67[] = "9";
static char rs_s69[] = "@7654321";
static char rs_s71[] = "Bytes used:  ________";
static char rs_s72[] = "9";
static char rs_s73[] = "@2345678901";
static char rs_s75[] = "Name:  ________.___";
static char rs_s76[] = "f";
static char rs_s78[] = "@";
static char rs_s80[] = "A";
static char rs_s81[] = "F";
static char rs_s82[] = "F";
static char rs_s84[] = "___";
static char rs_s83[] = "F";
static char rs_s85[] = "___";
static char rs_s86[] = "F";
static char rs_s87[] = "___";
static char rs_s88[] = "F";
static char rs_s89[] = "___";
static char rs_s90[] = "F";
static char rs_s91[] = "___";
static char rs_s92[] = "F";
static char rs_s93[] = "___";
static char rs_s94[] = "F";
static char rs_s95[] = "___";
static char rs_s96[] = "F";
static char rs_s97[] = "___";
static char rs_s98[] = "F";
static char rs_s99[] = "F";
static char rs_s100[] = "9";
static char rs_s101[] = "9";
static char rs_s102[] = "9";
static char rs_s103[] = "9";
static char rs_s104[] = "f";
static char rs_s105[] = "f";
static char rs_s106[] = "Name:  ________.___";
static char rs_s107[] = "f";



#define RS_NTED 37

TEDINFO desk_rs_tedinfo[RS_NTED];

static TEDINFO desk_rs_tedinfo_rom[] = {
        (LONG)rs_s46,
        (LONG)rs_s74,
        (LONG)rs_s48,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20,

        (LONG)"@2345678",
        (LONG)"Size in bytes:  ________",
        (LONG)rs_s44,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 25,

        (LONG)"@54321",
        (LONG)"Last modified:  __/__/__",
        (LONG)rs_s45,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 7, 25,

        (LONG)"@12345",
        (LONG)rs_s56,
        (LONG)rs_s58,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 7, 9,

        (LONG)rs_s77,
        (LONG)"Drive identifier:  _:",
        (LONG)rs_s79,
        IBM, 0, TE_LEFT, 4352, 0, 0, 2, 22,

        (LONG)rs_s47,
        (LONG)"Disk label:  ___________",
        (LONG)rs_s49,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 25,

        (LONG)rs_s60,
        (LONG)rs_s62,
        (LONG)rs_s50,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29,

        (LONG)"@2345",
        (LONG)rs_s65,
        (LONG)rs_s51,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27,

        (LONG)rs_s68,
        (LONG)rs_s70,
        (LONG)rs_s52,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 22,

        (LONG)"@1010101",
        (LONG)"Bytes available:  ________",
        (LONG)rs_s53,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 27,

        (LONG)"@2345678999",
        (LONG)"Folder name:  ________.___",
        (LONG)rs_s54,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 27,

        (LONG)"@ddddd",
        (LONG)"Created:  __-__-__  ",
        (LONG)rs_s55,
        IBM, 0, TE_LEFT, 4352, 0, 0, 7, 21,

        (LONG)"@hhhhh",
        (LONG)rs_s57,
        (LONG)rs_s59,
        IBM, 0, TE_LEFT, 4352, 0, 0, 7, 9,

        (LONG)rs_s61,
        (LONG)rs_s63,
        (LONG)rs_s64,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 29,

        (LONG)"@8765",
        (LONG)rs_s66,
        (LONG)rs_s67,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 6, 27,

        (LONG)rs_s69,
        (LONG)rs_s71,
        (LONG)rs_s72,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 9, 22,

        (LONG)rs_s73,
        (LONG)rs_s75,
        (LONG)rs_s76,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20,

        (LONG)"@123456789012345678901234567890123456789012345678901",
        (LONG)"Parameters:  ____________________________________________________",
        (LONG)"X",
        IBM, 0, TE_LEFT, 4352, 0, 0, 53, 66,

        (LONG)rs_s78,
        (LONG)"Drive identifier:  _",
        (LONG)rs_s80,
        IBM, 0, TE_LEFT, 4352, 0, 0, 2, 21,

        (LONG)"@23456789012",
        (LONG)"Icon label:  ____________",
        (LONG)rs_s81,
        IBM, 0, TE_LEFT, 4352, 0, 0, 13, 26,

        (LONG)"@1234876512",
        (LONG)"Application name:  ________.___",
        (LONG)rs_s82,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 32,

        (LONG)"@01",
        (LONG)rs_s84,
        (LONG)rs_s83,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@02",
        (LONG)rs_s85,
        (LONG)rs_s86,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@03",
        (LONG)rs_s87,
        (LONG)rs_s88,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@04",
        (LONG)rs_s89,
        (LONG)rs_s90,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@05",
        (LONG)rs_s91,
        (LONG)rs_s92,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@06",
        (LONG)rs_s93,
        (LONG)rs_s94,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@07",
        (LONG)rs_s95,
        (LONG)rs_s96,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@08",
        (LONG)rs_s97,
        (LONG)rs_s98,
        IBM, 0, TE_LEFT, 4352, 0, 0, 4, 4,

        (LONG)"@HOLDSPACESFORICONNAMES",
        (LONG)"_______________________",
        (LONG)rs_s99,
        IBM, 1, TE_CNTR, 4480, 0, -1, 24, 24,

        (LONG)"@234",
        (LONG)"Folders to copy:  ____",
        (LONG)rs_s100,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 23,

        (LONG)"@432",
        (LONG)"Items to copy:  ____",
        (LONG)rs_s101,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 21,

        (LONG)"@980",
        (LONG)"Folders to delete:  ____",
        (LONG)rs_s102,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 25,

        (LONG)"@678",
        (LONG)"Items to delete:  ____",
        (LONG)rs_s103,
        IBM, 0, TE_RIGHT, 4352, 0, 0, 5, 23,

        (LONG)"@3456354890",
        (LONG)"Current name:  ________.___",
        (LONG)rs_s104,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 28,

        (LONG)"@5436354890",
        (LONG)"Copy's name:  ________.___",
        (LONG)rs_s105,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 27,

        (LONG)"@1726354890",
        (LONG)rs_s106,
        (LONG)rs_s107,
        IBM, 0, TE_LEFT, 4352, 0, 0, 12, 20
};


static int rs_b4img[] = {
        0x0780,0x0000,0x0000,0x0000,0x1860,0x0000,0x0000,0x0000,
        0x2010,0x0000,0x0000,0x0000,0x4708,0xe0fe,0x0000,0x0000,
        0x4489,0xc3ff,0x8000,0x0000,0x4709,0xc787,0x8000,0x0000,
        0x448b,0x8703,0xc000,0x0000,0x2013,0x8701,0xc000,0x0000,
        0x1867,0x0003,0xc000,0x0000,0x0787,0x0007,0x8000,0x0000,
        0x000e,0x003f,0x0000,0x0000,0x000e,0x003f,0x8000,0x0000,
        0x001c,0x0007,0xc000,0x0000,0x001c,0x0003,0xe000,0x0000,
        0x0038,0x0701,0xe000,0x0000,0x0038,0x0703,0xe000,0x0000,
        0x0070,0x0787,0xc000,0x0000,0x0070,0x03ff,0xc000,0x0000,
        0x00e0,0x01ff,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

static int rs_b3img[] = {
        0x3fff,0xfffc,0x7fff,0xfffe,0xe000,0x0007,0xc000,0x0003,
        0xc000,0x0003,0xc3f8,0x1fc3,0xc7fc,0x3fe3,0xc7fc,0x3063,
        0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,
        0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,
        0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,
        0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,0xc7fc,0x3063,
        0xc7fc,0x3063,0xc7fc,0x3fe3,0xc3f8,0x1fc3,0xc000,0x0003,
        0xc000,0x0003,0xe000,0x0007,0x7fff,0xfffe,0x3fff,0xfffc
};

static int rs_b2img[] = {
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x01f6,0x3000,0x0000,
        0x0000,0x0045,0x5000,0x0000,0xfc3f,0xe044,0x9000,0x0000,
        0xfe3f,0xf044,0x1000,0x0000,0x8f38,0x7800,0x0000,0x0000,
        0x0738,0x3800,0x0000,0x0000,0x0738,0x3800,0x0000,0x0000,
        0x0738,0x3800,0x0000,0x0000,0x0738,0x3800,0x0000,0x0000,
        0x8f38,0x7800,0x0000,0x0000,0xfe3f,0xf000,0x0000,0x0000,
        0xfc3f,0xe000,0x0000,0x0000,0x0038,0x0000,0x0000,0x0000,
        0x0038,0x0000,0x0000,0x0000,0x0038,0x0000,0x0000,0x0000,
        0x0038,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

static int rs_b1img[] = {
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0xffe0,0x0000,0x0000,0x0000,
        0xfff0,0x0000,0x00e0,0x0000,0x3878,0x0000,0x00e0,0x0000,
        0x3838,0x0000,0x00e0,0x0380,0x3838,0x0000,0x00e0,0x0380,
        0x3838,0x0000,0x00e0,0x0380,0x3838,0x7e03,0xf8e1,0xcfe1,
        0x3838,0xff07,0xf8e3,0x8fe3,0x3839,0xe38f,0x00e7,0x0387,
        0x3839,0xc1ce,0x00ee,0x0387,0x3839,0xffc7,0xf0fc,0x0387,
        0x3839,0xffc3,0xf8fe,0x0387,0x3839,0xc000,0x1ce7,0x0387,
        0x3879,0xe1c0,0x3ce3,0x8387,0xfff0,0xff87,0xf8e1,0xc383,
        0xffe0,0x7f07,0xf0e0,0xe381,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

static int rs_b0img[] = {
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x07fe,0x4fff,0xe7fc,0x00ff,
        0x1f07,0xc3e0,0xe1fc,0x01fc,0x3e01,0xc1e0,0x60fe,0x01f8,
        0x7c00,0xc1e0,0x20fe,0x03f8,0x7800,0xc1e0,0x00ef,0x0378,
        0x7800,0x01e2,0x00ef,0x0778,0x7800,0x01e6,0x00e7,0x8678,
        0x780f,0xf9fe,0x00e7,0x8e78,0x7801,0xe1e6,0x00e3,0xcc78,
        0x7801,0xe1e2,0x00e3,0xdc78,0x7801,0xe1e0,0x00e1,0xf878,
        0x7801,0xe1e0,0x00e1,0xf878,0x7801,0xe1e0,0x10e0,0xf078,
        0x3c03,0xe1e0,0x30e0,0xf078,0x1e07,0x83e0,0x71e0,0x607c,
        0x07fe,0x0fff,0xf7e0,0x607f,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};


#define RS_NBITBLK 5

static BITBLK desk_rs_bitblk[] = {
        { (LONG)rs_b0img, 8, 24, 0, 0, 1 },

        { (LONG)rs_b1img, 8, 24, 0, 0, 1 },

        { (LONG)rs_b2img, 8, 24, 0, 0, 1 },

        { (LONG)rs_b3img, 4, 32, 0, 0, 1 },

        { (LONG)rs_b4img, 8, 24, 0, 0, 1 }
};


#define RS_NOBS 211

OBJECT desk_rs_obj[RS_NOBS];

static OBJECT desk_rs_obj_rom[] = {
#define TR0 0
/* TREE 0 */
        -1, 1, 7, G_IBOX,                       /*** 0 ***/
        NONE,
        NORMAL,
        (long) 0L,
        0, 0, 80, 25,

        7, 2, 2, G_BOX,                 /*** 1 ***/
        NONE,
        NORMAL,
        (long) 4352L,
        0, 0, 80, 513,

        1, 3, 6, G_IBOX,                        /*** 2 ***/
        NONE,
        NORMAL,
        (long) 0L,
        2, 0, 30, 769,

        4, -1, -1, G_TITLE,                     /*** 3 ***/
        NONE,
        NORMAL,
        (long) " Desk ",
        0, 0, 6, 769,

        5, -1, -1, G_TITLE,                     /*** 4 ***/
        NONE,
        NORMAL,
        (long) " File ",
        6, 0, 6, 769,

        6, -1, -1, G_TITLE,                     /*** 5 ***/
        NONE,
        NORMAL,
        (long) " Options ",
        12, 0, 9, 769,

        2, -1, -1, G_TITLE,                     /*** 6 ***/
        NONE,
        NORMAL,
        (long) " Arrange ",
        21, 0, 9, 769,

        0, 8, 33, G_IBOX,                       /*** 7 ***/
        NONE,
        NORMAL,
        (long) 0L,
        0, 769, 80, 24,

        17, 9, 16, G_BOX,                       /*** 8 ***/
        NONE,
        NORMAL,
        (long) 16716032L,
        2, 0, 20, 8,

        10, -1, -1, G_STRING,                   /*** 9 ***/
        NONE,
        NORMAL,
        (long) "  Desktop info...   ",
        0, 0, 20, 1,

        11, -1, -1, G_STRING,                   /*** 10 ***/
        NONE,
        DISABLED,
        (long) rs_s2,
        0, 1, 20, 1,

        12, -1, -1, G_STRING,                   /*** 11 ***/
        NONE,
        NORMAL,
        (long) "1",
        0, 2, 20, 1,

        13, -1, -1, G_STRING,                   /*** 12 ***/
        NONE,
        NORMAL,
        (long) rs_s34,
        0, 3, 20, 1,

        14, -1, -1, G_STRING,                   /*** 13 ***/
        NONE,
        NORMAL,
        (long) rs_s36,
        0, 4, 20, 1,

        15, -1, -1, G_STRING,                   /*** 14 ***/
        NONE,
        NORMAL,
        (long) rs_s38,
        0, 5, 20, 1,

        16, -1, -1, G_STRING,                   /*** 15 ***/
        NONE,
        NORMAL,
        (long) "5",
        0, 6, 20, 1,

        8, -1, -1, G_STRING,                    /*** 16 ***/
        NONE,
        NORMAL,
        (long) "6",
        0, 7, 20, 1,

        26, 18, 25, G_BOX,                      /*** 17 ***/
        NONE,
        NORMAL,
        (long) 16716032L,
        8, 0, 21, 8,

        19, -1, -1, G_STRING,                   /*** 18 ***/
        NONE,
        NORMAL,
        (long) "  Open",
        0, 0, 21, 1,

        20, -1, -1, G_STRING,                   /*** 19 ***/
        NONE,
        NORMAL,
        (long) "  Info/Rename...  \007I",
        0, 1, 21, 1,

        21, -1, -1, G_STRING,                   /*** 20 ***/
        NONE,
        DISABLED,
        (long) rs_s0,
        0, 2, 21, 1,

        22, -1, -1, G_STRING,                   /*** 21 ***/
        NONE,
        NORMAL,
        (long) "  Delete...       \007D",
        0, 3, 21, 1,

        23, -1, -1, G_STRING,                   /*** 22 ***/
        NONE,
        NORMAL,
        (long) "  Format...",
        0, 4, 21, 1,

        24, -1, -1, G_STRING,                   /*** 23 ***/
        NONE,
        DISABLED,
        (long) rs_s1,
        0, 5, 21, 1,

        25, -1, -1, G_STRING,                   /*** 24 ***/
        NONE,
        NORMAL,
        (long) "  To Output       ^U",
        0, 6, 21, 1,

        17, -1, -1, G_STRING,                   /*** 25 ***/
        NONE,
        NORMAL,
        (long) "  Exit to DOS     ^Q",
        0, 7, 21, 1,

        33, 27, 32, G_BOX,                      /*** 26 ***/
        NONE,
        NORMAL,
        (long) 16716032L,
        14, 0, 31, 6,

        28, -1, -1, G_STRING,                   /*** 27 ***/
        NONE,
        NORMAL,
        (long) "  Install disk drive...",
        0, 0, 31, 1,

        29, -1, -1, G_STRING,                   /*** 28 ***/
        NONE,
        NORMAL,
        (long) "  Configure application...  \007A",
        0, 1, 31, 1,

        30, -1, -1, G_STRING,                   /*** 29 ***/
        NONE,
        DISABLED,
        (long) "-------------------------------",
        0, 2, 31, 1,

        31, -1, -1, G_STRING,                   /*** 30 ***/
        NONE,
        NORMAL,
        (long) "  Set preferences...",
        0, 3, 31, 1,

        32, -1, -1, G_STRING,                   /*** 31 ***/
        NONE,
        NORMAL,
        (long) "  Save desktop              \007V",
        0, 4, 31, 1,

        26, -1, -1, G_STRING,                   /*** 32 ***/
        NONE,
        NORMAL,
        (long) "  Enter DOS commands        \007C",
        0, 5, 31, 1,

        7, 34, 39, G_BOX,                       /*** 33 ***/
        NONE,
        NORMAL,
        (long) 16716032L,
        23, 0, 20, 6,

        35, -1, -1, G_STRING,                   /*** 34 ***/
        NONE,
        NORMAL,
        (long) "  xxxx xx xxxxx  xx",
        0, 0, 20, 1,

        36, -1, -1, G_STRING,                   /*** 35 ***/
        NONE,
        DISABLED,
        (long) rs_s3,
        0, 1, 20, 1,

        37, -1, -1, G_STRING,                   /*** 36 ***/
        NONE,
        NORMAL,
        (long) "  Sort by name   \007N",
        0, 2, 20, 1,

        38, -1, -1, G_STRING,                   /*** 37 ***/
        NONE,
        NORMAL,
        (long) "  Sort by type   \007P",
        0, 3, 20, 1,

        39, -1, -1, G_STRING,                   /*** 38 ***/
        NONE,
        NORMAL,
        (long) "  Sort by size   \007Z",
        0, 4, 20, 1,

        33, -1, -1, G_STRING,                   /*** 39 ***/
        LASTOB,
        NORMAL,
        (long) "  Sort by date   \007T",
        0, 5, 20, 1,

#define TR1 40
/* TREE 1 */
        -1, 1, 11, G_BOX,                       /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 45, 11,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "ITEM INFORMATION / RENAME",
        3, 1, 16, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[0],
        12, 3, 19, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[1],
        3, 4, 24, 1,

        5, -1, -1, G_FBOXTEXT,                  /*** 4 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[2],
        3, 5, 24, 1,

        6, -1, -1, G_FBOXTEXT,                  /*** 5 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[3],
        29, 5, 8, 1,

        7, -1, -1, G_STRING,                    /*** 6 ***/
        NONE,
        NORMAL,
        (long) "Attributes:",
        6, 7, 11, 1,

        10, 8, 9, G_IBOX,                       /*** 7 ***/
        NONE,
        NORMAL,
        (long) 0L,
        19, 7, 23, 1,

        9, -1, -1, G_BUTTON,                    /*** 8 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Read/Write",
        0, 0, 11, 1,

        7, -1, -1, G_BUTTON,                    /*** 9 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Read-Only",
        13, 0, 10, 1,

        11, -1, -1, G_BUTTON,                   /*** 10 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s4,
        24, 9, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 11 ***/
        SELECTABLE|EXIT|LASTOB,
        NORMAL,
        (long) rs_s10,
        34, 9, 8, 1,

#define TR2 52
/* TREE 2 */
        -1, 1, 8, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 37, 12,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "DISK INFORMATION",
        3, 1, 16, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[4],
        4, 3, 21, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[5],
        10, 4, 24, 1,

        5, -1, -1, G_FBOXTEXT,                  /*** 4 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[6],
        3, 5, 28, 1,

        6, -1, -1, G_FBOXTEXT,                  /*** 5 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[7],
        5, 6, 26, 1,

        7, -1, -1, G_FBOXTEXT,                  /*** 6 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[8],
        10, 7, 21, 1,

        8, -1, -1, G_FBOXTEXT,                  /*** 7 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[9],
        5, 8, 26, 1,

        0, -1, -1, G_BUTTON,                    /*** 8 ***/
        SELECTABLE|DEFAULT|EXIT|LASTOB,
        NORMAL,
        (long) rs_s5,
        26, 10, 8, 1,

#define TR3 61
/* TREE 3 */
        -1, 1, 8, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 46, 11,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "FOLDER INFORMATION",
        3, 1, 18, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[10],
        9, 3, 26, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[11],
        13, 4, 20, 1,

        5, -1, -1, G_FBOXTEXT,                  /*** 4 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[12],
        33, 4, 8, 1,

        6, -1, -1, G_FBOXTEXT,                  /*** 5 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[13],
        3, 5, 28, 1,

        7, -1, -1, G_FBOXTEXT,                  /*** 6 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[14],
        5, 6, 26, 1,

        8, -1, -1, G_FBOXTEXT,                  /*** 7 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[15],
        10, 7, 21, 1,

        0, -1, -1, G_BUTTON,                    /*** 8 ***/
        SELECTABLE|DEFAULT|EXIT|LASTOB,
        NORMAL,
        (long) rs_s6,
        31, 9, 8, 1,

#define TR4 70
/* TREE 4 */
        -1, 1, 15, G_BOX,                       /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 49, 19,

        2, -1, -1, G_IMAGE,                     /*** 1 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_bitblk[0],
        11, 1, 8, 3,

        3, -1, -1, G_IMAGE,                     /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_bitblk[1],
        25, 1, 8, 3,

        4, -1, -1, G_IMAGE,                     /*** 3 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_bitblk[2],
        33, 1, 8, 3,

        5, -1, -1, G_STRING,                    /*** 4 ***/
        NONE,
        NORMAL,
        (long) "Release 3.0",
        19, 4, 11, 1,

        6, -1, -1, G_STRING,                    /*** 5 ***/
        NONE,
        NORMAL,
        (long) "   December 7, 1987   ",
        13, 5, 23, 1,

        7, -1, -1, G_STRING,                    /*** 6 ***/
        NONE,
        NORMAL,
        (long) "AUTHORS",
        21, 10, 7, 1,

        8, -1, -1, G_IMAGE,                     /*** 7 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &desk_rs_bitblk[3],
        3, 12, 4, 4,

        9, -1, -1, G_STRING,                    /*** 8 ***/
        NONE,
        NORMAL,
        (long) "=====================",
        14, 11, 21, 1,

        10, -1, -1, G_STRING,                   /*** 9 ***/
        NONE,
        NORMAL,
        (long) "",
        16, 9, 0, 1,

        11, -1, -1, G_STRING,                   /*** 10 ***/
        NONE,
        NORMAL,
        (long) "Michael Franusich",
        16, 12, 17, 1,

        12, -1, -1, G_BUTTON,                   /*** 11 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s7,
        38, 13, 8, 1,

        13, -1, -1, G_STRING,                   /*** 12 ***/
        NONE,
        NORMAL,
        (long) "Lowell Webster",
        17, 13, 14, 1,

        14, -1, -1, G_STRING,                   /*** 13 ***/
        NONE,
        NORMAL,
        (long) "Copyright (c) 1987 Digital Research Inc.",
        4, 6, 41, 1,

        15, -1, -1, G_STRING,                   /*** 14 ***/
        NONE,
        NORMAL,
        (long) "All rights reserved.",
        15, 7, 20, 1,

        0, -1, -1, G_IMAGE,                     /*** 15 ***/
        LASTOB,
        NORMAL,
        (long) &desk_rs_bitblk[4],
        19, 1, 8, 3,

#define TR5 86
/* TREE 5 */
        -1, 1, 7, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 48, 11,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "NEW FOLDER INFORMATION",
        3, 1, 23, 1,

        3, -1, -1, G_STRING,                    /*** 2 ***/
        NONE,
        NORMAL,
        (long) "To create a new folder within the current",
        3, 3, 41, 1,

        4, -1, -1, G_STRING,                    /*** 3 ***/
        NONE,
        NORMAL,
        (long) "window, double-click on the New Folder",
        3, 4, 38, 1,

        5, -1, -1, G_STRING,                    /*** 4 ***/
        NONE,
        NORMAL,
        (long) "icon and complete the dialogue that appears",
        3, 5, 43, 1,

        6, -1, -1, G_STRING,                    /*** 5 ***/
        NONE,
        NORMAL,
        (long) "by entering the name of the folder you",
        3, 6, 38, 1,

        7, -1, -1, G_STRING,                    /*** 6 ***/
        NONE,
        NORMAL,
        (long) "want to create.",
        3, 7, 15, 1,

        0, -1, -1, G_BUTTON,                    /*** 7 ***/
        SELECTABLE|DEFAULT|EXIT|LASTOB,
        NORMAL,
        (long) rs_s8,
        37, 9, 8, 1,

#define TR6 94
/* TREE 6 */
        -1, 1, 8, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 71, 10,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "OPEN APPLICATION",
        3, 1, 16, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[16],
        9, 3, 19, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[17],
        3, 4, 65, 1,

        5, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s9,
        51, 8, 8, 1,

        6, -1, -1, G_BUTTON,                    /*** 5 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s11,
        61, 8, 8, 1,

        7, -1, -1, G_STRING,                    /*** 6 ***/
        NONE,
        NORMAL,
        (long) "Enter the name of the document you want",
        3, 6, 39, 1,

        8, -1, -1, G_STRING,                    /*** 7 ***/
        NONE,
        NORMAL,
        (long) "to load, or enter parameter values that",
        3, 7, 39, 1,

        0, -1, -1, G_STRING,                    /*** 8 ***/
        LASTOB,
        NORMAL,
        (long) "are acceptable to this application.",
        3, 8, 35, 1,

#define TR7 103
/* TREE 7 */
        -1, 1, 10, G_BOX,                       /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 59, 9,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "INSTALL DISK DRIVE",
        3, 1, 18, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[18],
        3, 3, 20, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[19],
        9, 4, 25, 1,

        5, -1, -1, G_STRING,                    /*** 4 ***/
        NONE,
        NORMAL,
        (long) "Disk type:",
        10, 6, 10, 1,

        8, 6, 7, G_IBOX,                        /*** 5 ***/
        NONE,
        NORMAL,
        (long) 0L,
        20, 6, 24, 1,

        7, -1, -1, G_BUTTON,                    /*** 6 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Floppy",
        2, 0, 8, 1,

        5, -1, -1, G_BUTTON,                    /*** 7 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Hard",
        12, 0, 8, 1,

        9, -1, -1, G_BUTTON,                    /*** 8 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s13,
        47, 2, 8, 1,

        10, -1, -1, G_BUTTON,                   /*** 9 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s15,
        47, 4, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 10 ***/
        SELECTABLE|DEFAULT|EXIT|LASTOB,
        NORMAL,
        (long) rs_s12,
        47, 6, 8, 1,

#define TR8 114
/* TREE 8 */
        -1, 1, 33, G_BOX,                       /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 64, 20,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "CONFIGURE APPLICATION",
        3, 1, 21, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[20],
        4, 3, 32, 1,

        4, -1, -1, G_STRING,                    /*** 3 ***/
        NONE,
        NORMAL,
        (long) "Document types:",
        6, 4, 15, 1,

        5, -1, -1, G_FBOXTEXT,                  /*** 4 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[21],
        23, 4, 3, 1,

        6, -1, -1, G_FBOXTEXT,                  /*** 5 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[22],
        28, 4, 3, 1,

        7, -1, -1, G_FBOXTEXT,                  /*** 6 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[23],
        33, 4, 3, 1,

        8, -1, -1, G_FBOXTEXT,                  /*** 7 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[24],
        38, 4, 3, 1,

        9, -1, -1, G_FBOXTEXT,                  /*** 8 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[25],
        43, 4, 3, 1,

        10, -1, -1, G_FBOXTEXT,                 /*** 9 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[26],
        48, 4, 3, 1,

        11, -1, -1, G_FBOXTEXT,                 /*** 10 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[27],
        53, 4, 3, 1,

        12, -1, -1, G_FBOXTEXT,                 /*** 11 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[28],
        58, 4, 3, 1,

        13, -1, -1, G_STRING,                   /*** 12 ***/
        NONE,
        NORMAL,
        (long) "Application type:",
        4, 6, 17, 1,

        17, 14, 16, G_IBOX,                     /*** 13 ***/
        NONE,
        NORMAL,
        (long) 4352L,
        23, 6, 39, 1,

        15, -1, -1, G_BUTTON,                   /*** 14 ***/
        SELECTABLE|RBUTTON|TOUCHEXIT,
        NORMAL,
        (long) "GEM",
        0, 0, 6, 1,

        16, -1, -1, G_BUTTON,                   /*** 15 ***/
        SELECTABLE|RBUTTON|TOUCHEXIT,
        NORMAL,
        (long) "DOS",
        8, 0, 6, 1,

        13, -1, -1, G_BUTTON,                   /*** 16 ***/
        SELECTABLE|RBUTTON|TOUCHEXIT,
        NORMAL,
        (long) "DOS-takes parameters",
        16, 0, 22, 1,

        18, -1, -1, G_STRING,                   /*** 17 ***/
        NONE,
        NORMAL,
        (long) "Needs full memory?",
        3, 8, 18, 1,

        21, 19, 20, G_IBOX,                     /*** 18 ***/
        NONE,
        NORMAL,
        (long) 4352L,
        23, 8, 15, 1,

        20, -1, -1, G_BUTTON,                   /*** 19 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s26,
        0, 0, 6, 1,

        18, -1, -1, G_BUTTON,                   /*** 20 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s28,
        8, 0, 6, 1,

        22, -1, -1, G_STRING,                   /*** 21 ***/
        NONE,
        NORMAL,
        (long) "Icon type:",
        11, 10, 10, 1,

        23, -1, -1, G_BOXTEXT,                  /*** 22 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[29],
        23, 10, 27, 1,

        26, 24, 25, G_BOX,                      /*** 23 ***/
        NONE,
        NORMAL,
        (long) 69888L,
        23, 11, 24, 6,

        25, -1, -1, G_BOX,                      /*** 24 ***/
        NONE,
        NORMAL,
        (long) 69889L,
        3, 1, 6, 4,

        23, -1, -1, G_BOX,                      /*** 25 ***/
        NONE,
        NORMAL,
        (long) 69889L,
        12, 1, 6, 4,

        31, 27, 29, G_BOX,                      /*** 26 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69888L,
        47, 11, 3, 6,

        28, -1, -1, G_BOXCHAR,                  /*** 27 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 201396480L,
        0, 0, 3, 2,

        29, -1, -1, G_BOXCHAR,                  /*** 28 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 218173696L,
        0, 4, 3, 2,

        26, 30, 30, G_BOX,                      /*** 29 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69905L,
        0, 2, 3, 2,

        29, -1, -1, G_BOX,                      /*** 30 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69889L,
        1024, 0, 258, 1,

        32, -1, -1, G_BUTTON,                   /*** 31 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s14,
        30, 18, 9, 1,

        33, -1, -1, G_BUTTON,                   /*** 32 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s16,
        42, 18, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 33 ***/
        SELECTABLE|EXIT|LASTOB,
        NORMAL,
        (long) rs_s17,
        53, 18, 8, 1,

#define TR9 148
/* TREE 9 */
        -1, 1, 5, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 34, 8,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "COPY FOLDERS / ITEMS",
        3, 1, 20, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[30],
        3, 3, 22, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[31],
        5, 4, 20, 1,

        5, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s18,
        13, 6, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 5 ***/
        SELECTABLE|EXIT|LASTOB,
        NORMAL,
        (long) rs_s19,
        23, 6, 8, 1,

#define TR10 154
/* TREE 10 */
        -1, 1, 5, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 30, 8,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "DELETE FOLDERS / ITEMS",
        3, 1, 23, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[32],
        3, 3, 24, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[33],
        5, 4, 22, 1,

        5, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s20,
        9, 6, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 5 ***/
        SELECTABLE|DEFAULT|EXIT|LASTOB,
        NORMAL,
        (long) rs_s21,
        19, 6, 8, 1,

#define TR11 160
/* TREE 11 */
        -1, 1, 6, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 34, 8,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "NAME CONFLICT DURING COPY",
        3, 1, 25, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        NONE,
        NORMAL,
        (long) &desk_rs_tedinfo[34],
        3, 3, 27, 1,

        4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[35],
        4, 4, 26, 1,

        5, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s22,
        3, 6, 8, 1,

        6, -1, -1, G_BUTTON,                    /*** 5 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) rs_s23,
        13, 6, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 6 ***/
        SELECTABLE|EXIT|LASTOB,
        NORMAL,
        (long) "Stop",
        23, 6, 8, 1,

#define TR12 167
/* TREE 12 */
        -1, 1, 4, G_BOX,                        /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 27, 7,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "NEW FOLDER",
        3, 1, 10, 1,

        3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        EDITABLE,
        NORMAL,
        (long) &desk_rs_tedinfo[36],
        3, 3, 19, 1,

        4, -1, -1, G_BUTTON,                    /*** 3 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s24,
        6, 5, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|EXIT|LASTOB,
        NORMAL,
        (long) rs_s25,
        16, 5, 8, 1,

#define TR13 172
/* TREE 13 */
        -1, 1, 38, G_BOX,                       /*** 0 ***/
        NONE,
        OUTLINED,
        (long) 135424L,
        0, 0, 59, 21,

        2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "SET PREFERENCES",
        3, 1, 15, 1,

        3, -1, -1, G_STRING,                    /*** 2 ***/
        NONE,
        NORMAL,
        (long) "Confirm deletes?",
        6, 3, 16, 1,

        6, 4, 5, G_IBOX,                        /*** 3 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 3, 12, 1,

        5, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s27,
        0, 0, 5, 1,

        3, -1, -1, G_BUTTON,                    /*** 5 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s29,
        7, 0, 5, 1,

        7, -1, -1, G_STRING,                    /*** 6 ***/
        NONE,
        NORMAL,
        (long) "Confirm copies?",
        7, 5, 15, 1,

        10, 8, 9, G_IBOX,                       /*** 7 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 5, 12, 1,

        9, -1, -1, G_BUTTON,                    /*** 8 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s30,
        0, 0, 5, 1,

        7, -1, -1, G_BUTTON,                    /*** 9 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s31,
        7, 0, 5, 1,

        11, -1, -1, G_STRING,                   /*** 10 ***/
        NONE,
        NORMAL,
        (long) "Confirm overwrites?",
        3, 7, 19, 1,

        14, 12, 13, G_IBOX,                     /*** 11 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 7, 12, 1,

        13, -1, -1, G_BUTTON,                   /*** 12 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s32,
        0, 0, 5, 1,

        11, -1, -1, G_BUTTON,                   /*** 13 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s33,
        7, 0, 5, 1,

        15, -1, -1, G_STRING,                   /*** 14 ***/
        NONE,
        NORMAL,
        (long) "Double-click speed:",
        3, 9, 19, 1,

        21, 16, 20, G_IBOX,                     /*** 15 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 9, 31, 1,

        17, -1, -1, G_BUTTON,                   /*** 16 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Slow",
        0, 0, 7, 1,

        18, -1, -1, G_BUTTON,                   /*** 17 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s35,
        9, 0, 3, 1,

        19, -1, -1, G_BUTTON,                   /*** 18 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s37,
        14, 0, 3, 1,

        20, -1, -1, G_BUTTON,                   /*** 19 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) rs_s39,
        19, 0, 3, 1,

        15, -1, -1, G_BUTTON,                   /*** 20 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Fast",
        24, 0, 7, 1,

        24, 22, 23, G_IBOX,                     /*** 21 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 11, 20, 1,

        23, -1, -1, G_BUTTON,                   /*** 22 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Click",
        0, 0, 8, 1,

        21, -1, -1, G_BUTTON,                   /*** 23 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "No click",
        10, 0, 10, 1,

        25, -1, -1, G_STRING,                   /*** 24 ***/
        NONE,
        NORMAL,
        (long) "To drop down menus:",
        3, 11, 19, 1,

        26, -1, -1, G_STRING,                   /*** 25 ***/
        NONE,
        NORMAL,
        (long) "Sound effects:",
        8, 13, 14, 1,

        29, 27, 28, G_IBOX,                     /*** 26 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 13, 12, 1,

        28, -1, -1, G_BUTTON,                   /*** 27 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "On",
        0, 0, 5, 1,

        26, -1, -1, G_BUTTON,                   /*** 28 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "Off",
        7, 0, 5, 1,

        30, -1, -1, G_STRING,                   /*** 29 ***/
        NONE,
        NORMAL,
        (long) "Time format:",
        10, 15, 12, 1,

        33, 31, 32, G_IBOX,                     /*** 30 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 15, 20, 1,

        32, -1, -1, G_BUTTON,                   /*** 31 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "12 Hour",
        0, 0, 9, 1,

        30, -1, -1, G_BUTTON,                   /*** 32 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "24 Hour",
        11, 0, 9, 1,

        34, -1, -1, G_STRING,                   /*** 33 ***/
        NONE,
        NORMAL,
        (long) "Date format:",
        10, 17, 12, 1,

        37, 35, 36, G_IBOX,                     /*** 34 ***/
        NONE,
        NORMAL,
        (long) 0L,
        24, 17, 24, 1,

        36, -1, -1, G_BUTTON,                   /*** 35 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "MM-DD-YY",
        0, 0, 11, 1,

        34, -1, -1, G_BUTTON,                   /*** 36 ***/
        SELECTABLE|RBUTTON,
        NORMAL,
        (long) "DD-MM-YY",
        13, 0, 11, 1,

        38, -1, -1, G_BUTTON,                   /*** 37 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) rs_s40,
        36, 19, 8, 1,

        0, -1, -1, G_BUTTON,                    /*** 38 ***/
        SELECTABLE|EXIT|LASTOB,
        NORMAL,
        (long) rs_s41,
        47, 19, 8, 1
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
        rs_s42,
        rs_s43,
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




void desk_rs_init()
{
        register int  i = 0;

        /* Copy data from ROM to RAM: */
        memcpy(desk_rs_obj, desk_rs_obj_rom, RS_NOBS*sizeof(OBJECT));
        memcpy(desk_rs_tedinfo, desk_rs_tedinfo_rom, RS_NTED*sizeof(TEDINFO));

        do
        {
                rsrc_obfix((LONG)desk_rs_obj, i);
        } while (++i<RS_NOBS);
}


/* Fake a rsrc_gaddr for the ROM desktop: */
WORD rsrc_gaddr(WORD rstype, WORD rsid, LONG *paddr)
{
        switch(rstype)
        {
            case R_TREE:   *paddr = (LONG)desk_rs_trees[rsid]; break;
            case R_BITBLK: *paddr = (LONG)&desk_rs_bitblk[rsid]; break;
            case R_STRING: *paddr = (LONG)desk_rs_fstr[rsid]; break;
            default:
                kcprintf("FIXME: unsupported (faked) rsrc_gaddr type!\n");
                return FALSE;
        }

        return TRUE;
}

