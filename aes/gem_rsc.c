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
#include "gemrslib.h"
#include "gemdos.h"


static const char rs_str_fstmplt[] = "_ ________.___ ";
static const char rs_str_xF[] = "xF";


#define RS_NTED 13

TEDINFO rs_tedinfo[RS_NTED];

static const TEDINFO rs_tedinfo_rom[] = {
        { 0L,
        (LONG)"______________________________________",
        (LONG)"P",
        IBM, 1, TE_LEFT, 4352, 0, 0, 39, 39 },

        { 0L,
        (LONG)"Selection:  ________.___",
        (LONG)"f",
        IBM, 1, TE_LEFT, 4352, 0, 0, 12, 25 },

        { 0L,
        (LONG)"__________________",
        (LONG)"F",
        IBM, 6, TE_CNTR, 4352, 0, -1, 19, 19 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { 0L,
        (LONG)rs_str_fstmplt,
        (LONG)rs_str_xF,
        IBM, 1, TE_LEFT, 4352, 0, 0, 13, 16 },

        { (LONG)"\200",
        (LONG)"\200",
        (LONG)"\200",
        IBM, 1, TE_CNTR, 4352, 0, 1, 2, 2 }
};



static int rs_b10img[] = {
        0x0008,0x0008,0x0001,0x0000,0x0001,0x07e0,0x07e0,0x07e0,
        0x07e0,0x07e0,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
        0x07e0,0x07e0,0x07e0,0x07e0,0x07e0,0x0000,0x03c0,0x0240,
        0x0240,0x0240,0x0240,0x7e7e,0x4002,0x4002,0x7e7e,0x0240,
        0x0240,0x0240,0x0240,0x03c0,0x0000
};

static int rs_b9img[] = {
        0x0008,0x0008,0x0001,0x0000,0x0001,0x0000,0x07c0,0x07c0,
        0x07c0,0x07c0,0x07c0,0xffff,0xffff,0xffff,0xffff,0xffff,
        0x07c0,0x07c0,0x07c0,0x07c0,0x07c0,0x0000,0x0000,0x0380,
        0x0380,0x0380,0x0380,0x0380,0xffff,0xffff,0xffff,0x0380,
        0x0380,0x0380,0x0380,0x0380,0x0000
};

static int rs_b8img[] = {
        0x0007,0x0007,0x0001,0x0000,0x0001,0x0000,0x0380,0x0380,
        0x0380,0x0380,0x0380,0x7ffc,0x7ffc,0x7ffc,0x0380,0x0380,
        0x0380,0x0380,0x0380,0x0000,0x0000,0x0000,0x0000,0x0000,
        0x0100,0x0100,0x0100,0x0100,0x1ff0,0x0100,0x0100,0x0100,
        0x0100,0x0000,0x0000,0x0000,0x0000
};

static int rs_b7img[] = {
        0x0008,0x0008,0x0001,0x0000,0x0001,0x0000,0x0380,0x03f0,
        0x33fe,0x7bff,0x7bff,0x3fff,0xffff,0xffff,0x7fff,0x3fff,
        0x3fff,0x1ffe,0x07fc,0x07fc,0x0ffe,0x0000,0x0380,0x0270,
        0x324e,0x4a49,0x4a49,0x2649,0xf249,0x9801,0x4c01,0x2001,
        0x2001,0x1802,0x0404,0x0404,0x0ffe
};

static int rs_b6img[] = {
        0x0000,0x0000,0x0001,0x0000,0x0001,0xf000,0xf800,0x7c00,
        0x3e07,0x1f0f,0x0f9e,0x07de,0x07fe,0x1fff,0x3fff,0x7fff,
        0x7ffe,0x3ffe,0x1ffe,0x0fff,0x01ff,0xf000,0x8800,0x4400,
        0x2207,0x1109,0x0892,0x0452,0x0632,0x1913,0x2481,0x5241,
        0x4982,0x2602,0x1806,0x0e03,0x0180
};

static int rs_b5img[] = {
        0x0008,0x0008,0x0001,0x0000,0x0001,0xffff,0xffff,0xffff,
        0x7ffe,0x7ffe,0x3ffc,0x1ff8,0x0ef0,0x0ff0,0x1ff8,0x3ffc,
        0x7ffe,0x7ffe,0xffff,0xffff,0xffff,0x0000,0x7ffe,0x2004,
        0x1008,0x1448,0x0ab0,0x0560,0x02c0,0x0340,0x04a0,0x0910,
        0x1088,0x12a8,0x3554,0x7ffe,0x0000
};

static int rs_b4img[] = {
        0x0007,0x0007,0x0001,0x0000,0x0001,0xffff,0xffff,0x07e0,
        0x03c0,0x03c0,0x03c0,0x03c0,0x03c0,0x03c0,0x03c0,0x03c0,
        0x03c0,0x03c0,0x07e0,0xffff,0xffff,0x7c3e,0x0660,0x03c0,
        0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,
        0x0180,0x0180,0x03c0,0x0660,0x7c3e
};

static int rs_b3img[] = {
        0x0000,0x0000,0x0001,0x0000,0x0001,0xe000,0xf000,0xf800,
        0xfc00,0xfe00,0xff00,0xff80,0xffc0,0xfe00,0xfe00,0xef00,
        0x0f00,0x0780,0x0780,0x03c0,0x03c0,0x4000,0x6000,0x7000,
        0x7800,0x7c00,0x7e00,0x7f00,0x7f80,0x7c00,0x6c00,0x4600,
        0x0600,0x0300,0x0300,0x0180,0x0180
};

static int rs_b2img[] = {
        0x0000,0x0000,0x0001,0xc000,0x0073,0xe000,0x00fb,0xe700,
        0x00fb,0xef80,0x1cfb,0xef80,0x3efb,0xef80,0x3efb,0xef80,
        0x3efb,0xef80,0x3efb,0xef80,0x3efb,0xef80,0x3efb,0xef80,
        0x3efb,0xef9c,0x3efb,0xefbc,0x3efb,0xefbc,0x3fff,0xff7c,
        0x3fff,0xff7c,0x3fff,0xff7c,0x3fff,0xfefc,0x3fff,0xf7f8,
        0x3fff,0xdff8,0x3fff,0x7ff0,0x3fff,0xfff0,0x3ffd,0xffe0,
        0x1ffd,0xffc0,0x0fff,0xff80,0x07ff,0xff00,0x03ff,0xfe00,
        0x03ff,0xfe00,0x03ff,0xfe00,0x03ff,0xfe00,0x0000,0x0000
};

static int rs_b1img[] = {
        0x0000,0x0000,0x001f,0xfe00,0x007f,0xff80,0x00ff,0xffc0,
        0x01ff,0xffe0,0x01f8,0x07e0,0x01f0,0x03e0,0x01f0,0x03e0,
        0x00e0,0x03e0,0x0000,0x07c0,0x0000,0x0fc0,0x0000,0x1f80,
        0x0000,0x3f00,0x0000,0x7e00,0x0000,0xfc00,0x0001,0xf800,
        0x0003,0xf000,0x0003,0xe000,0x0007,0xc000,0x0007,0xc000,
        0x0007,0xc000,0x0007,0xc000,0x0007,0xc000,0x0003,0x8000,
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0007,0xc000,
        0x000f,0xe000,0x000f,0xe000,0x0007,0xc000,0x0000,0x0000
};

static int rs_b0img[] = {
        0x0000,0x0000,0x0000,0x0000,0x0000,0x0700,0x0000,0x0f80,
        0x0000,0x0f80,0x0000,0x0f80,0x0000,0x0f80,0x0000,0x0f80,
        0x0000,0x0f80,0x0000,0x0f80,0x0000,0x0f80,0x0000,0x0f80,
        0x0079,0xef9c,0x0efb,0xefbc,0x1efb,0xefbc,0x1efb,0xef7c,
        0x16aa,0xaf7c,0x1efb,0xef7c,0x0d75,0xdcbc,0x038e,0x37f8,
        0x1fff,0xdff8,0x1fff,0x7ff0,0x1fff,0xfff0,0x1ffd,0xffe0,
        0x1fff,0xffc0,0x0ffd,0xff80,0x07ff,0xfe00,0x01ff,0xf400,
        0x015f,0xfc00,0x01ff,0xfc00,0x01ff,0xfc00,0x0000,0x0000
};



char msg_str_1[40];         /* Strings for the alert box */
char msg_str_2[40];
char msg_str_3[40];
char msg_str_4[40];
char msg_str_5[40];
char msg_but_1[20];
char msg_but_2[20];
char msg_but_3[20];


#define RS_NOBS 37

OBJECT rs_obj[RS_NOBS];

static const OBJECT rs_obj_rom[] = {
#define TR0 0
/* TREE 0 */
        { -1, 1, 6, G_BOX,                        /*** 0 ***/
        NONE,
        SHADOWED,
        (long) 69888L,
        0, 0, 40+(4<<8), 20+(6<<8) },

        { 2, -1, -1, G_STRING,                    /*** 1 ***/
        NONE,
        NORMAL,
        (long) "ITEM SELECTOR",
        3, 1, 13, 1 },

        { 3, -1, -1, G_FBOXTEXT,                  /*** 2 ***/
        EDITABLE,
        NORMAL,
        (long) &rs_tedinfo[0],
        1, 3, 38, 1 },

        { 4, -1, -1, G_FBOXTEXT,                  /*** 3 ***/
        EDITABLE,
        NORMAL,
        (long) &rs_tedinfo[1],
        1, 5, 24, 1 },

        { 5, -1, -1, G_BUTTON,                    /*** 4 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) "OK",
        28, 15, 8, 1 },

        { 6, -1, -1, G_BUTTON,                    /*** 5 ***/
        SELECTABLE|EXIT,
        NORMAL,
        (long) "Cancel",
        28, 17, 8, 1 },

        { 0, 7, 14, G_IBOX,                       /*** 6 ***/
        NONE,
        NORMAL,
        (long) 4352L,
        2, 7, 22+(1<<8), 12+(1<<8) },

        { 8, -1, -1, G_BOXCHAR,                   /*** 7 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 0x05FF1101L,
        0, 0, 2, 1 },

        { 9, -1, -1, G_FBOXTEXT,                  /*** 8 ***/
        RBUTTON,
        NORMAL,
        (long) &rs_tedinfo[2],
        258, 0, 1555, 1 },

        { 14, 10, 12, G_BOX,                      /*** 9 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69888L,
        19, 1, 3, 11 },

        { 11, -1, -1, G_BOXCHAR,                  /*** 10 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 0x01011100L,
        0, 0, 3, 2 },

        { 12, -1, -1, G_BOXCHAR,                  /*** 11 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 0x02011100L,
        0, 9, 3, 2 },

        { 9, 13, 13, G_BOX,                       /*** 12 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69905L,
        0, 2, 3, 7 },

        { 12, -1, -1, G_BOX,                      /*** 13 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69889L,
        1024, 0, 2, 1 },

        { 6, 15, 23, G_BOX,                       /*** 14 ***/
        TOUCHEXIT,
        NORMAL,
        (long) 69888L,
        0, 1, 19, 11 },

        { 16, -1, -1, G_FBOXTEXT,                 /*** 15 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[3],
        2, 1, 15, 1 },

        { 17, -1, -1, G_FBOXTEXT,                 /*** 16 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[4],
        2, 2, 15, 1 },

        { 18, -1, -1, G_FBOXTEXT,                 /*** 17 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[5],
        2, 3, 15, 1 },

        { 19, -1, -1, G_FBOXTEXT,                 /*** 18 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[6],
        2, 4, 15, 1 },

        { 20, -1, -1, G_FBOXTEXT,                 /*** 19 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[7],
        2, 5, 15, 1 },

        { 21, -1, -1, G_FBOXTEXT,                 /*** 20 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[8],
        2, 6, 15, 1 },

        { 22, -1, -1, G_FBOXTEXT,                 /*** 21 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[9],
        2, 7, 15, 1 },

        { 23, -1, -1, G_FBOXTEXT,                 /*** 22 ***/
        TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[10],
        2, 8, 15, 1 },

        { 14, -1, -1, G_FBOXTEXT,                 /*** 23 ***/
        LASTOB|TOUCHEXIT,
        NORMAL,
        (long) &rs_tedinfo[11],
        2, 9, 15, 1 },

#define TR1 24
/* TREE 1 */
        { -1, 1, 9, G_BOX,                        /*** 0 ***/
        NONE,
        SHADOWED,
        (long) 135424L,
        0, 0, 78, 7 },

        { 2, -1, -1, G_BOX,                       /*** 1 ***/
        NONE,
        NORMAL,
        (long) 16716032L,
        3, 1, 4, 4 },

        { 3, -1, -1, G_STRING,                    /*** 2 ***/
        NONE,
        NORMAL,
        (long) msg_str_1,
        9, 1, 40, 1 },

        { 4, -1, -1, G_STRING,                    /*** 3 ***/
        NONE,
        NORMAL,
        (long) msg_str_2,
        9, 2, 50, 1 },

        { 5, -1, -1, G_STRING,                    /*** 4 ***/
        NONE,
        NORMAL,
        (long) msg_str_3,
        9, 3, 50, 1 },

        { 6, -1, -1, G_STRING,                    /*** 5 ***/
        NONE,
        NORMAL,
        (long) msg_str_4,
        9, 4, 50, 1 },

        { 7, -1, -1, G_STRING,                    /*** 6 ***/
        NONE,
        NORMAL,
        (long) msg_str_5,
        9, 5, 50, 1 },

        { 8, -1, -1, G_BUTTON,                    /*** 7 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) msg_but_1,
        61, 1, 16, 1 },

        { 9, -1, -1, G_BUTTON,                    /*** 8 ***/
        SELECTABLE|DEFAULT|EXIT,
        NORMAL,
        (long) msg_but_2,
        61, 3, 16, 1 },

        { 0, -1, -1, G_BUTTON,                    /*** 9 ***/
        SELECTABLE|DEFAULT|EXIT|LASTOB,
        NORMAL,
        (long) msg_but_3,
        61, 5, 16, 1 },

#define TR2 34
/* TREE 2 */
        { -1, 1, 2, G_BOX,                        /*** 0 ***/
        NONE,
        NORMAL,
        (long) 4420L,
        0, 0, 80, 25 },

        { 2, -1, -1, G_BOX,                       /*** 1 ***/
        NONE,
        NORMAL,
        (long) 16716032L,
        0, 0, 80, 513 },

        { 0, -1, -1, G_TEXT,                      /*** 2 ***/
        LASTOB,
        NORMAL,
        (long) &rs_tedinfo[12],
        0, 0, 80, 769 }
};


OBJECT *rs_tree[] = {
    &rs_obj[TR0],
    &rs_obj[TR1],
    &rs_obj[TR2]
};


#define RS_NFSTR 37

char *rs_fstr[] = {
        "PATH=",
        "DESKTOP.APP",
        ".APP",
        "*.ACC",
        "0..9",
        "a..zA..Z ",
        "a..zA..Z0..9 ",
        "a..zA..Z0..9 $#&@!%()-{}'`_^~\\?*:.,",
        "a..zA..Z0..9 $#&@!%()-{}'`_^~\\:",
        "a..zA..Z0..9 $#&@!%()-{}'`_^~:?*",
        "a..zA..Z0..9 $#&@!%()-{}'`_^~",
        "a..zA..Z ",
        "a..zA..Z0..9 ",
        "Insert your GEM STARTUP disk",
        "C:\\GEMAPPS\\GEMSYS;C:\\GEMAPPS;C:\\",
        "C:\\CLIPBRD",
        "GEM.RSC Release 3.0",
        "AVAILNUL",
        "SCRENMGR",
        "C:\\DESKTOP.INF",
        "\\SCRAP.",
        "[2][You cannot write to the disk in drive|%S: because it is physically write-|protected.  Before you Retry, remove|the write-protect tab or notch the|disk.][Cancel|Retry]",
        "[2][Drive %S: is not responding.  You must|use the right kind of disk, insert it|correctly, and close the door.  If the|problem is with a hard disk, check the|disk's connections.][Cancel|Retry]",
        "[2][Data on the disk in drive %S: may be|damaged.  You must use the right kind|of floppy disk; you must connect your|hard disk properly.][Cancel|Retry]",
        "[2][This application cannot read data on the|disk in drive %S:.  The disk must be|formatted, there must be power to the|disk drive, and the disk drive must be|physically connected to your computer.][Cancel|Retry]",
        "[2][Your output device is not receiving|data. Before you Retry, make sure the|device has power, is on-line, and is|loaded with paper or film.][Cancel|Retry]",
        "[3][An error has occurred with the|Graphics Environment Manager (GEM).|Contact Digital Research Technical|Support for assistance.][Cancel]",
        "[2][This application cannot find the|folder or document you are trying to|open. Check the name you have entered.][  OK  ]",
        "[3][Your computer doesn't have enough|memory to run the GEM Desktop.][ Sorry ]",
        "[1][This application doesn't have room to|open another document.  To make room,|close any documents you don't need.][  OK  ]",
        "[1][An item with this name already exists|in the folder, or the item is set to|Read-Only status. Use the \"Info/Rename\"|command to change the item's status.][  OK  ]",
        "[1][The disk drive you have indicated does|not exist. Check the drive identifier|letter you entered.][Cancel]",
        "[1][You cannot delete the folder in|which you are currently working.][  OK  ]",
        "[1][Your computer does not have enough|memory to run the application you|have selected.][  OK  ]",
        "[3][DOS error #%W.][Cancel]",
        "[3][Bad Function #][Cancel]",
        "[3][To run the GEM Desktop, insert your|GEM DESKTOP disk in drive A and click|on OK or press Enter. To return to DOS,|click on Cancel.][  OK  |Cancel]"
};

#define RS_NFIMG 11

BITBLK rs_fimg[] = {
        { (LONG)rs_b0img, 4, 32, 0, 0, 1 },

        { (LONG)rs_b1img, 4, 32, 0, 0, 1 },

        { (LONG)rs_b2img, 4, 32, 0, 0, 1 },

        { (LONG)rs_b3img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b4img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b5img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b6img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b7img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b8img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b9img, 2, 37, 0, 0, 3 },

        { (LONG)rs_b10img, 2, 37, 0, 0, 3 }
};




extern int count_chars(char *str, char c);    /* see desk_rsc.c */



void gem_rsc_init()
{
    long len;
    int i, j;
    char *tedinfptr;

    /* Copy data from ROM to RAM: */
    memcpy(rs_obj, rs_obj_rom, RS_NOBS*sizeof(OBJECT));
    memcpy(rs_tedinfo, rs_tedinfo_rom, RS_NTED*sizeof(TEDINFO));

    /* Fix TEDINFO strings: */
    len = 0;
    for (i = 0; i < RS_NTED; i++) {
        if (rs_tedinfo[i].te_ptext == 0) {
            /* Count number of '_' in strings ( +1 for \0 at the end ): */
            len += count_chars((char *)rs_tedinfo[i].te_ptmplt, '_') + 1;
        }
    }
    tedinfptr = (char *) dos_alloc(len);        /* Get memory */
    for (i = 0; i < RS_NTED; i++) {
        if (rs_tedinfo[i].te_ptext == 0) {
            rs_tedinfo[i].te_ptext = (LONG) tedinfptr;
            *tedinfptr++ = '@'; /* First character of uninitialized string */
            len = count_chars((char *)rs_tedinfo[i].te_ptmplt, '_');
            for (j = 0; j < len; j++) {
                *tedinfptr++ = '_';     /* Set other characters to '_' */
            }
            *tedinfptr++ = 0;   /* Final 0 */
        }
    }
    /* The first three TEDINFOs don't use a '@' as first character: */
    *(char *)rs_tedinfo[0].te_ptext = '_';
    *(char *)rs_tedinfo[1].te_ptext = '_';
    *(char *)rs_tedinfo[2].te_ptext = 0;

}


void gem_rsc_fixit()
{
        register int    i=0;

        do
        {
                rs_obfix((LONG)rs_obj, i);
        } while (++i<RS_NOBS);
}
