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


/*==== Defines ============================================================*/

/*==== External declarations ==============================================*/

extern void con_state_init(void);       // from conout.S

extern struct font_head f8x16;
extern struct font_head f8x8;
extern struct font_head f6x6;

/*==== Prototypes =========================================================*/

void font_init(struct font_head *);
void resol_set(BYTE);
void clear_screen(void);



/*
 * Settings for the different video modes They are:
 * planes, lin_wr, hz_rez, vt_rez
 */

static const VIDEO_MODE video_mode[] = {
    {4, 160, 320, 200},         // 16 color mode
    {2, 160, 640, 200},         // 4 color mode
    {1,  80, 640, 400}          // monochrome mode
};


/* Clear screen with foreground color */

/* Can this be made with Laurents mem* functions? */

void clear_screen(void)
{
    UWORD color;
    UWORD *address;

    color=v_col_bg;                     // get actual foreground color


    for (address = (UWORD*)memtop; address < (UWORD*)phystop; address++)
        *address=color;                 // set screen to color
}



/*
 * font_init - font ring  initialization
 */

void font_init(struct font_head * font)
{
    v_cel_ht=font->form_height;		// init cell height.
    v_cel_wr=v_lin_wr*font->form_height;// init cell wrap

    v_cel_mx=(v_hz_rez/                 // init cell max x
              font->max_cell_width)-1;  
    v_cel_my=(v_vt_rez/
              font->form_height)-1;     // init cell max y

    v_fnt_wr=font->form_width;          // init font wrap
    v_fnt_st=font->first_ade;           // init font start ADE
    v_fnt_nd=font->last_ade;            // init font end ADE
    v_fnt_ad=font->dat_table;            // init font data ptr
    v_off_ad=font->off_table;            // init font offset ptr

    /* Initialize the font ring (is this right so???) */
    font_ring[0]=&f8x16;
    font_ring[1]=&f8x8;
    font_ring[2]=&f6x6;
    font_ring[3]=NULL;

    font_count=3;                       // total number of fonts in fontring
}



/*
 * resol_set - set screen parameters according to video mode
 */

void resol_set(BYTE vmode)
{
    WORD m;

    m=(WORD) vmode;

    v_planes=video_mode[m].planes;
    v_lin_wr=video_mode[m].lin_wr;
    v_hz_rez=video_mode[m].hz_rez;
    v_vt_rez=video_mode[m].vt_rez;
}



/*
 * cursconf - cursor configuration
 *
 * Arguments:
 *
 *   function =
 *   0 - switch off cursor
 *   1 - switch on cursor
 *   2 - blinking cursor
 *   3 - not blinking cursor
 *   4 - set cursor blink rate
 *   5 - get cursor blink rate
 *
 * Bits:
 *   M_CFLASH - cursor flash on
 *   M_CVIS   - cursor visibility on
 */

WORD cursconf(WORD function, WORD operand)
{
    switch (function) {
    case 0:
        cprintf("\ef");                 /* set cursor unvisible */
        break;
    case 1:
        cprintf("\ee");                 /* set cursor visible */
        break;
    case 2:
        v_stat_0 &= ~M_CFLASH;          /* unset cursor flash bit */
        break;
    case 3:
        v_stat_0 |= M_CFLASH;           /* set cursor flash bit */
        break;
    case 4:
        v_period = operand;             /* set cursor flash interval */
        break;
    case 5:
        return(v_period);               /* set cursor flash interval */
    }
    return(0);                          /* Hopefully never reached */
}



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

#if 0
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
#endif

    if (video_mode == 2)
    {
        font_init(&f8x16);
    }
    else
        font_init(&f8x8);

    /* Initial color settings */
    v_col_fg=0xffff;			// foreground color black
    v_col_bg=0x0000;			// background color white

    /* Initial cursor settings */
    v_cur_cx=0;				// cursor to column 0
    v_cur_cy=0;				// cursor to line 0
    v_cur_of=0;				// line offset is 0
    v_cur_ad=v_bas_ad;                 	// set cursor to begin of screen

    v_stat_0=M_CFLASH;	      		// cursor invisible, flash,
    					// nowrap, normal video.
    cursconf(4, 30);                    // .5 second blink rate (@60 Hz vblank).
    v_cur_tim=v_period;			// load initial value to blink timer
    disab_cnt=1;               		// cursor disabled 1 level deep.

    /* Clear screen with foreground color */
    clear_screen();

    /* Init conout state machine */
    con_state_init();                       // set initial state
}



