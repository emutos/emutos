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

#define DBG_LINEA 1
/*==== Defines ============================================================*/

/*==== External declarations ==============================================*/

extern void con_state_init(void);       // from conout.S

extern struct font_head f8x16;
extern struct font_head f8x8;
extern struct font_head f6x6;

/*==== Prototypes =========================================================*/

void font_init(void);



/*
 * Settings for the different video modes They are:
 * planes, lin_wr, hz_rez, vt_rez
 */

static const VIDEO_MODE video_mode[] = {
    {4, 160, 320, 200},         // 16 color mode
    {2, 160, 640, 200},         // 4 color mode
    {1,  80, 640, 400}          // monochrome mode
};



/*
 * font_init - font ring  initialization
 */

void font_init(void)
{
    struct font_head * font;

    /* Set font */
    font=def_font;                      /* get actual system font */

    v_cel_ht=font->form_height;         // init cell height.
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

#if DBG_LINEA
    kprintf("================\n");
    kprintf("fontad: %ld\n", (LONG)def_font);
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
    kprintf("v_cel_my: %d\n", v_cel_my);
    kprintf("v_cel_wr: %d\n", v_cel_wr);
    kprintf("\n");
#endif
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
 * linea_init - init linea variables
 */

void linea_init(void)
{
    WORD vmode;                 /* video mode */

    vmode=(sshiftmod & 3);      /* Get video mode from hardware */
#if DBG_LINEA
    kprintf("vmode : %d\n", vmode);
#endif

    /* set parameters for video mode */
    if (vmode >2){      /* Mode 3 == unvalid for ST (MAD) */
        kprintf("video mode was: %d !\n", vmode);
        vmode=2;                /* Falcon should be handeled special? */
    }
    v_planes=video_mode[vmode].planes;
    v_lin_wr=video_mode[vmode].lin_wr;
    v_hz_rez=video_mode[vmode].hz_rez;
    v_vt_rez=video_mode[vmode].vt_rez;

#if DBG_LINEA
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
#endif

    /* set font dependend from video mode */
    if (vmode == 2) {
        cur_font=&f8x16;
        def_font=&f8x16;
    }
    else {
        cur_font=&f8x8;
        def_font=&f8x8;
    }
    font_init();                        // init linea for actual font

    /* Initialize the font ring (is this right so???) */
    font_ring[0]=&f6x6;
    font_ring[1]=&f8x8;
    font_ring[2]=&f8x16;
    font_ring[3]=NULL;
    font_count=3;                       // total number of fonts in fontring

    /* Initial cursor settings */
    v_cur_cx=0;                         // cursor to column 0
    v_cur_cy=0;                         // cursor to line 0
    v_cur_of=0;                         // line offset is 0
    v_cur_ad=v_bas_ad;                  // set cursor to begin of screen

    v_stat_0=M_CFLASH;                  // cursor invisible, flash,
                                        // nowrap, normal video.
    cursconf(4, 30);                    // .5 second blink rate (@60 Hz vblank).
    v_cur_tim=v_period;                 // load initial value to blink timer
    disab_cnt=1;                        // cursor disabled 1 level deep.

    /* Init conout state machine */
    con_state_init();                       // set initial state
}
