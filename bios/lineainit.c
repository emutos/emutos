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
#include "country.h"
#include "string.h"
#include "screen.h"
#include "machine.h"

#define DBG_LINEA 1

/*==== Global vars ========================================================*/

/* this will be set to 1 by linea_init();
 * kprintf will check this variable, to display panic messages on screen
 * also.
 */
int linea_inited;

/*==== External declarations ==============================================*/

extern void con_state_init(void);       // from conout.S


/*==== Font tables ========================================================*/

/* to add a set of fonts, please do the following:
 * - read doc/country.txt
 * - add a line in the charset names in bios/country.h
 * - add extern declaration referencing the fonts below
 * - add a line in the font_sets table below
 */
 
extern struct font_head f8x16;
extern struct font_head f8x8;
extern struct font_head f6x6;
extern struct font_head cz8x16;
extern struct font_head cz8x8;
extern struct font_head cz6x6;

struct charset_fonts {
    int charset;
    struct font_head *f6x6;
    struct font_head *f8x8;
    struct font_head *f8x16;
};

static struct charset_fonts font_sets[] = {
    { CHARSET_ST, &f6x6, &f8x8, &f8x16 },
    /* { CHARSET_CZ, &cz6x6, &cz8x8, &cz8x16 }, */
    /* { CHARSET_L2, &l26x6, &l28x8, &l28x16 }, */
};



/*==== Prototypes =========================================================*/

void font_init(void);
void init_fonts(WORD vmode);


/* Copies of the ROM-fontheaders */
struct font_head *sysfonts[4];  // all three fonts and NULL

struct font_head fon8x16;
struct font_head fon8x8;
struct font_head fon6x6;

/*
 * init_fonts - font ring initialization
 */
 
void init_fonts(WORD vmode)
{
    int i, j;
    int charset = get_charset();
    
    /* find the index of the required charset in our font table */
    for(i = j = 0 ; i < sizeof(font_sets)/sizeof(*font_sets) ; i++) {
        if( font_sets[i].charset == charset ) {
            j = i; 
            break;
        }
    }
    
    /* copy the ROM-fontheaders of 3 system fonts to RAM */
    memmove(&fon6x6, font_sets[j].f6x6, sizeof(struct font_head));
    memmove(&fon8x8, font_sets[j].f8x8, sizeof(struct font_head));
    memmove(&fon8x16, font_sets[j].f8x16, sizeof(struct font_head));

    /* now in RAM, chain the font headers to a linked list */
    fon6x6.next_font = &f8x8;
    fon8x8.next_font = &f8x16;
    fon8x16.next_font = 0;

    /* set current font depending on the video mode */
    if (vmode == 2) {
        cur_font = def_font = &fon8x16;
    } else {
        cur_font = def_font = &fon8x8;
    }
       
    font_count=3;                       // total number of fonts in fontring

    /* Initialize the system font array for linea */
    sysfonts[0]= &fon6x6;
    sysfonts[1]= &fon8x8;
    sysfonts[2]= &fon8x16;
    sysfonts[3]=NULL;

#if 0 /* No VDI functionality provided anymore - use fVDI instead */
    /* Initialize the VDI font_ring as an struct of now linked font lists */
    font_ring.first_list = &fon6x6;
    font_ring.second_list = &fon6x6;
    font_ring.gdos_list = &fon6x6;
    font_ring.null_list = 0;
#endif

}


/* Settings for the different video modes */
struct video_mode {
    UBYTE       planes;         // count of color planes (v_planes)
    UWORD       hz_rez;         // screen horizontal resolution (v_hz_rez)
    UWORD       vt_rez;         // screen vertical resolution (v_vt_rez)
    UBYTE       col_fg;         // color number of initial foreground color
};


static const struct video_mode video_mode[] = {
    {4, 320, 200, 15},        // 16 color mode
    {2, 640, 200, 3},         // 4 color mode
    {1, 640, 400, 1}          // monochrome mode
};



/*
 * font_init - configure LineA for the default font
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
    kprintf("bytes_lin: %d\n", v_bytes_lin);
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

    vmode = (sshiftmod & 3);      /* Get video mode from hardware */
#if DBG_LINEA
    kprintf("vmode : %d\n", vmode);
#endif

    /* set parameters for video mode */
    if (vmode >2){      /* Mode 3 == unvalid for ST (MAD) */
        kprintf("video mode was: %d !\n", vmode);
        vmode=2;                /* Falcon should be handled special? */
    }
    if (has_videl) {
        v_planes = get_videl_bpp();
        v_hz_rez = get_videl_width();
        v_vt_rez = get_videl_height();
    }
    else {
        v_planes = video_mode[vmode].planes;
        v_hz_rez = video_mode[vmode].hz_rez;
        v_vt_rez = video_mode[vmode].vt_rez;
    }
    v_lin_wr = v_hz_rez*v_planes/8;     /* bytes per line */
    /*v_pl_dspl = (long)v_hz_rez*(long)v_vt_rez/8;*/     /* bytes per plane */
    v_bytes_lin = v_lin_wr;       /* I think v_bytes_lin = v_lin_wr (joy) */

    v_col_fg = video_mode[vmode].col_fg;
    v_col_bg = 0;

#if DBG_LINEA
    kprintf("planes: %d\n", v_planes);
    kprintf("lin_wr: %d\n", v_lin_wr);
    kprintf("hz_rez: %d\n", v_hz_rez);
    kprintf("vt_rez: %d\n", v_vt_rez);
#endif

    init_fonts(vmode);                  // init the fonts and select current

    font_init();                        // init linea for current font

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
    con_state_init();                   // set initial state

    linea_inited = 1;
}
