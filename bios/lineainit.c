/*
 * lineainit.c - linea graphics initialization
 *
 * Copyright (c) 2001 by Authors:
 *
 *  MAD  Martin Doering
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 *
 */


#include "tosvars.h"
#include "lineavars.h"
#include "fontdef.h"
#include "kprint.h"



/*==== External declarations ==============================================*/

/* Just used, if functions are assembler coded */
extern void font_init(struct font_head *);
extern void con_state_init(void);
extern void resol_set(BYTE);
extern void clear_screen(void);

extern struct font_head f8x16;
extern struct font_head f8x8;

/*==== Prototypes =========================================================*/



/*==== Defines ============================================================*/


/* font header structure equates. */

#define FIRST	36
#define LAST	38
#define CEL_WD  52
#define POFF    72
#define PDAT    76
#define FRM_WD  80
#define FRM_HT  82



/* Clear screen with foreground color */

/* Can this be made with Laurents mem* functions? */

#if IMPLEMENTED
void clear_screen(void)
{
    WORD color;
    WORD *address;

    color=v_col_bg;                     // get actual foreground color


    for (address = (LONG *)memtop; address < phystop; address++)
        *address=color;                 // set screen to color
}



/*
 * font_init - font ring  initialization
 */

void font_init(struct font_head)
{
}
#endif



/*
 * linea_init - escape initialization
 */

void linea_init(void)
{
    BYTE video_mode;

    video_mode=(sshiftmod & 3);       /* Get video mode from hardware */

    if (video_mode == 3)                /* Mode 3 == unvalid? */
        video_mode=2;

    resol_set(video_mode);


    if (video_mode == 2)
    {
        font_init(&f8x16);
    }
    else
        font_init(&f8x8);

    /* Initial color settings */
    v_col_fg=0xffff;			// foreground color
    v_col_bg=0x0000;			// background color

    /* Initial cursor settings */
    v_cur_cx=0;				// cursor to column 0
    v_cur_cy=0;				// cursor to line 0
    v_cur_of=0;				// line offset is 0
    v_cur_ad=v_bas_ad;                 	// set cursor to begin of screen

    v_stat_0=1;				// cursor invisible, flash,
    					// nowrap, normal video.
    v_cur_tim=30;			// .5 second blink counter (@60 Hz vblank).
    v_period=30;			// .5 second blink rate (@60 Hz vblank).
    disab_cnt=1;               		// cursor disabled 1 level deep.

    /* Clear screen with foreground color */
    clear_screen();

    /* Init conout state machine */
    con_state_init();                       // set initial state

}

