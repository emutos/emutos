/*
 * vdimain.c - Many nongraphicle VDI functions
 *
 * Copyright (c) 1999 Caldera, Inc.
 *               2002 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "lineavars.h"
#include "vdidef.h"
#include "gsxextrn.h"
#include "styles.h"
#include "kprint.h"
#include "biosbind.h"
#include "asm.h"



/* Prototypes for this module */
/* As reference the TOS 1.0 start addresses are added */
void v_opnwk();          /* 1   - fcb53e */
void v_clswk();          /* 2   - fcb812 */
void v_clrwk();          /* 3   - fca4e8 */
void v_updwk();          /* 4   - fca4e6 */
void chk_esc();          /* 5   - fc412e */

void v_pline();          /* 6   - fcb85a */
void v_pmarker();        /* 7   - fcb8f4 */
void d_gtext();          /* 8   - fcd61c */
void v_fillarea();       /* 9   - fcba3a */
void v_cellarray();      /* 10  - fca4e6 */

void v_gdp();            /* 11  - fcba46 */
void dst_height();       /* 12  - fcde96 */
void dst_rotation();     /* 13  - fce308 */
void vs_color();         /* 14  - fd1a00 */
void vsl_type();         /* 15  - fcab20 */

void vsl_width();        /* 16  - fcab6a */
void vsl_color();        /* 17  - fcac26 */
void vsm_type();         /* 18  - fcad02 */
void vsm_height();       /* 19  - fcac76 */
void vsm_color();        /* 20  - fcad52 */

void dst_font();         /* 21  - fce342 */
void dst_color();        /* 22  - fce426 */
void vsf_interior();     /* 23  - fcada8 */
void vsf_style();        /* 24  - fcadf4 */
void vsf_color();        /* 25  - fcae5c */

void vq_color();         /* 26  - fd1ab2 */
void vq_cellarray();     /* 27  - fca4e6 */
void v_locator();        /* 28  - fcaeac */
void v_valuator();       /* 29  - fcb042 */
void v_choice();         /* 30  - fcb04a */

void v_string();         /* 31  - fcb0d4 */
void vswr_mode();        /* 32  - fcb1d8 */
void vsin_mode();        /* 33  - fcb232 */
void v_nop();            /* 34  - fca4e6 */
void vql_attr();         /* 35  - fcbbf8 */

void vqm_attr();         /* 36  - fcbc54 */
void vqf_attr();         /* 37  - fcbcb4 */
void dqt_attributes();   /* 38  - fce476 */
void dst_alignment();    /* 39  - fce2ac */


void d_opnvwk();         /* 100 - fcd4d8 */

void d_clsvwk();         /* 101 - fcd56a */
void vq_extnd();         /* 102 - fcb77a */
void d_contourfill();    /* 103 - fd1208 */
void vsf_perimeter();    /* 104 - fcb306 */
void v_get_pixel();      /* 105 - fd1906 */

void dst_style();        /* 106 - fce278 */
void dst_point();        /* 107 - fce132 */
void vsl_ends();         /* 108 - fcabca */
void dro_cpyfm();        /* 109 - fcb454 */
void vr_trnfm();         /* 110 - fd1960 */

void vdi_vro_cpyfm();    /* 111 - fd0770 */
void dsf_udpat();        /* 112 - fcd5c0 */
void vsl_udsty();        /* 113 - fcb34c */
void dr_recfl();         /* 114 - fcb4be */
void vqi_mode();         /* 115 - fcb2a0 */

void dqt_extent();       /* 116 - fce4f0 */
void dqt_width();        /* 117 - fce6b6 */
void vex_timv();         /* 118 - fca530 */
void dt_loadfont();      /* 119 - fcebcc */
void dt_unloadfont();    /* 120 - fcec60 */

void vdi_vrt_cpyfm();    /* 121 - fcb486 */
void v_show_c();         /* 122 - fcafca */
void v_hide_c();         /* 123 - fcaff2 */
void vq_mouse();         /* 124 - fcb000 */
void vex_butv();         /* 125 - fd040e */

void vex_motv();         /* 126 - fd0426 */
void vex_curv();         /* 127 - fd043e */
void vq_key_s();         /* 128 - fcb1b4 */
void s_clip();           /* 129 - fcb364 */
void dqt_name();         /* 130 - fce790 */

void dqt_fontinfo();     /* 131 - fce820 */



/* External declarations */
//extern struct attribute *trap();
//extern long trap13(int, ...);

extern void escfn2();
extern void escfn3();
extern void vdimouse_init();
extern void vdimouse_exit();

//#define tickcal() trap13(0x06)          /* ms between timer C calls */
//#define setexec(a,b) trap13(0x05, a,b)  /* change exception vector */

#define X_MALLOC 0x48
#define X_MFREE 0x49


extern WORD SIZ_TAB_rom[];
extern WORD DEV_TAB_rom[];
extern WORD INQ_TAB_rom[];


#if 0
/* Screen related variables */
extern UWORD v_planes;          // count of color planes
extern UWORD v_hz_rez;          // screen horizontal resolution
extern UWORD v_vt_rez;          // screen vertical resolution
extern UWORD v_bytes_lin;       // width of line in bytes
#endif


BYTE in_proc;                   /* flag, if we are still running */
WORD flip_y;                    /* True if magnitudes being returned */

struct attribute virt_work;     /* attribute areas for workstations */
WORD q_circle[MX_LN_WIDTH];     /* Holds the circle DDA */


/* GDP variables */
WORD angle, beg_ang, del_ang, deltay, end_ang;
WORD start, xc, xrad, y, yc, yrad;

/* Wide line attribute save areas */
WORD s_begsty, s_endsty, s_fil_col, s_fill_per, s_patmsk;
WORD *s_patptr;

struct font_head *cur_font;     /* Pointer to current font */



/* Some color tables */
WORD MAP_COL[MAX_COLOR] =
    { 0, 15, 1, 2, 4, 6, 3, 5, 7, 8, 9, 10, 12, 14, 11, 13 };

WORD REV_MAP_COL[MAX_COLOR] =
    { 0, 2, 3, 6, 4, 7, 5, 8, 9, 10, 11, 14, 12, 15, 13, 1 };

/* Here's the template DEV_TAB, see lineavars.S for the normal DEV_TAB! */

WORD DEV_TAB_rom[45] = {
    639,                        /* 0    x resolution             */
    399,                        /* 1    y resolution             */
    0,                          /* 2    device precision 0=exact,1=not exact */
    372,                        /* 3    width of pixel           */
    372,                        /* 4    heigth of pixel          */
    1,                          /* 5    character sizes          */
    MX_LN_STYLE,                /* 6    linestyles               */
    0,                          /* 7    linewidth                */
    6,                          /* 8    marker types             */
    8,                          /* 9    marker size              */
    1,                          /* 10   text font                */
    MX_FIL_PAT_INDEX,           /* 11  area patterns             */
    MX_FIL_HAT_INDEX,           /* 12  crosshatch patterns       */
    2,                          /* 13   colors at one time       */
    10,                         /* 14   number of GDP's          */
    1,                          /* 15   GDP bar                  */
    2,                          /* 16   GDP arc                  */
    3,                          /* 17   GDP pic                  */
    4,                          /* 18   GDP circle               */
    5,                          /* 19   GDP ellipse              */
    6,                          /* 20   GDP elliptical arc       */
    7,                          /* 21   GDP elliptical pie       */
    8,                          /* 22   GDP rounded rectangle    */
    9,                          /* 23   GDP filled rounded rectangle */
    10,                         /* 24   GDP #justified text      */
    3,                          /* 25   GDP #1                   */
    0,                          /* 26   GDP #2                   */
    3,                          /* 27   GDP #3                   */
    3,                          /* 28   GDP #4                   */
    3,                          /* 29   GDP #5                   */
    0,                          /* 30   GDP #6                   */
    3,                          /* 31   GDP #7                   */
    0,                          /* 32   GDP #8                   */
    3,                          /* 33   GDP #9                   */
    2,                          /* 34   GDP #10                  */
    0,                          /* 35   Color capability         */
    1,                          /* 36   Text Rotation            */
    1,                          /* 37   Polygonfill              */
    0,                          /* 38   Cell Array               */
    2,                          /* 39   Pallette size            */
    2,                          /* 40   # of locator devices 1 = mouse */
    1,                          /* 41   # of valuator devices    */
    1,                          /* 42   # of choice devices      */
    1,                          /* 43   # of string devices      */
    2                           /* 44   Workstation Type 2 = out/in */
};



/*
 * SIZ_TAB - Returns text, line and marker sizes in device coordinates
 */

WORD SIZ_TAB[12];
static WORD SIZ_TAB_rom[12] = {
    0,                          /* 0    min char width          */
    7,                          /* 1    min char height         */
    0,                          /* 2    max char width          */
    7,                          /* 3    max char height         */
    1,                          /* 4    min line width          */
    0,                          /* 5    reserved 0          */
    MX_LN_WIDTH,                /* 6    max line width          */
    0,                          /* 7    reserved 0          */
    15,                         /* 8    min marker width        */
    11,                         /* 9    min marker height       */
    120,                        /* 10   max marker width        */
    88,                         /* 11   max marker height       */
};


/* Here's the template INQ_TAB, see lineavars.S for the normal INQ_TAB */
static WORD INQ_TAB_rom[45] = {
    1,                          /* 0  type of alpha/graphic controllers */
    1,                          /* 1  number of background colors  */
    0x1F,                       /* 2  text styles supported        */
    0,                          /* 3  scale rasters = false        */
    1,                          /* 4  number of planes         */
    0,                          /* 5  video lookup table       */
    50,                         /* 6  performance factor????       */
    1,                          /* 7  contour fill capability      */
    1,                          /* 8  character rotation capability    */
    4,                          /* 9  number of writing modes      */
    2,                          /* 10 highest input mode       */
    1,                          /* 11 text alignment flag      */
    0,                          /* 12 Inking capability        */
    0,                          /* 13 rubber banding           */
    128,                        /* 14 maximum vertices - must agree with entry.s */
    -1,                         /* 15 maximum intin            */
    1,                          /* 16 number of buttons on MOUSE   */
    0,                          /* 17 styles for wide lines            */
    0,                          /* 18 writing modes for wide lines     */
    0,                          /* 19 filled in with clipping flag     */
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0
};



/*
 * do_nothing - doesn't do much  :-)
 */

static void do_nothing_int(int u)
{
    (void)u;
}


void d_opnvwk()
{
    REG WORD handle;
    REG struct attribute *new_work, *work_ptr, *temp;

    /* Allocate the memory for a virtual workstation.  If none available,
       exit */

    new_work = (struct attribute *)trap1(X_MALLOC, (LONG) (sizeof(struct attribute)));

    if (new_work == NULLPTR) {  /* No work available */
        CONTRL[6] = 0;
        return;
    }

    /* Now find a free handle */

    handle = 1;
    work_ptr = &virt_work;

    while (handle == work_ptr->handle) {
        handle++;
        if (work_ptr->next_work == NULLPTR)
            break;
        work_ptr = work_ptr->next_work;
    }

    /* Empty slot found, Insert the workstation here */

    if (work_ptr->next_work == NULLPTR) {       /* Add at end of chain */
        cur_work = work_ptr->next_work = new_work;
        new_work->next_work = NULLPTR;
    }

    else {                      /* Add in middle of chain */
        temp = work_ptr->next_work;
        cur_work = work_ptr->next_work = new_work;
        new_work->next_work = temp;
    }

    new_work->handle = CONTRL[6] = handle;
    init_wk();
}

void d_clsvwk()
{
    REG struct attribute *work_ptr;
    REG WORD handle;

    /* cur_work points to workstation to deallocate, find who points to me */

    handle = cur_work->handle;

    if (handle == 1)            /* Can't close physical this way */
        return;

    for (work_ptr = &virt_work; handle != work_ptr->next_work->handle;
         work_ptr = work_ptr->next_work);

    work_ptr->next_work = cur_work->next_work;
    trap1(X_MFREE, cur_work);
}



/*
 * tick_int -  VDI Timer interrupt routine
 *
 * The etv_timer does point to this routine
 */
 
void tick_int(int u)
{
    if (!in_proc) {
        in_proc = 1;                    // set flag, that we are running
        // MAD: evtl. registers to stack
        (*tim_addr)(u);                    // call the timer vector
        // and back from stack
    }
    in_proc = 0;                        // allow yet another trip through
    // MAD: evtl. registers to stack
    (*tim_chain)(u);                       // call the old timer vector too
    // and back from stack
}



/* OPEN_WORKSTATION: */
void v_opnwk()
{
    WORD old_sr;
    int i;

    /* We need to copy some initial table data from the ROM */
    for (i = 0; i < 12; i++) {
        SIZ_TAB[i] = SIZ_TAB_rom[i];
    }

    for (i = 0; i < 45; i++) {
        DEV_TAB[i] = DEV_TAB_rom[i];
        INQ_TAB[i] = INQ_TAB_rom[i];
    }

    /* Copy data from linea variables */
    DEV_TAB[0] = v_hz_rez-1;
    DEV_TAB[1] = v_vt_rez-1;
    INQ_TAB[4] = v_planes;

    /* Calculate colors allowed at one time */
    if (INQ_TAB[4] < 8)
        DEV_TAB[13] = 2<<(v_planes-1);
    else
        DEV_TAB[13] = 256;

    cur_work = &virt_work;
    CONTRL[6] = virt_work.handle = 1;
    virt_work.next_work = NULLPTR;

    line_cw = -1;               /* invalidate current line width */

    text_init();                /* initialize the SIZ_TAB info */

    init_wk();

    /* Input must be initialized here and not in init_wk */
    loc_mode = 0;               /* default is request mode  */
    val_mode = 0;               /* default is request mode  */
    chc_mode = 0;               /* default is request mode  */
    str_mode = 0;               /* default is request mode  */

    /* mouse settings */
    HIDE_CNT = 1;               /* mouse is initially hidden */

    GCURX = DEV_TAB[0] / 2;     /* initialize the mouse to center */
    GCURY = DEV_TAB[1] / 2;


    /* Now initialize the lower level things */
    in_proc = 0;                        // no vblanks in process
    tim_addr = do_nothing_int;          // tick points to rts

    old_sr = set_sr(0x2700);            // disable interrupts
    tim_chain = (void(*)(int))          // save old vector
        Setexc(0x100, (long)tick_int);  // set etv_timer to tick_int
    set_sr(old_sr);                     // enable interrupts

    vdimouse_init();                    // initialize mouse
    //cprintf("\033f");                 // FIXME: switch off cursor
    escfn2();                           // enter graphics mode
}



/* CLOSE_WORKSTATION: */
void v_clswk()
{
    WORD old_sr;
    struct attribute *next_work;

    if (virt_work.next_work != NULLPTR) {       /* Are there VWs to close */
        cur_work = virt_work.next_work;
        do {
            next_work = cur_work->next_work;
            trap1(X_MFREE, cur_work);
        } while ((cur_work = next_work));
    }


    /* Now de-initialize the lower level things */
    old_sr = set_sr(0x2700);            // disable interrupts
    Setexc(0x100, (long)tim_chain);     // set etv_timer to tick_int
    set_sr(old_sr);                     // enable interrupts

    vdimouse_exit();                    // initialize mouse
    escfn3();                           // back to console mode
}



/*
 * vq_extnd - Extended workstation inquire
 */

void vq_extnd()
{
    REG WORD i;
    REG WORD *dp, *sp;

    dp = CONTRL;
    *(dp + 2) = 6;
    *(dp + 4) = 45;

    flip_y = 1;

    dp = PTSOUT;

    if (*(INTIN) == 0) {
        sp = SIZ_TAB;
        for (i = 0; i < 12; i++)
            *dp++ = *sp++;

        sp = DEV_TAB;
    }

    else {
        *dp++ = XMN_CLIP;       /* PTSOUT[0] */
        *dp++ = YMN_CLIP;       /* PTSOUT[1] */
        *dp++ = XMX_CLIP;       /* PTSOUT[2] */
        *dp++ = YMX_CLIP;       /* PTSOUT[3] */

        for (i = 4; i < 12; i++)
            *dp++ = 0;

        sp = INQ_TAB;
    }

    dp = INTOUT;
    for (i = 0; i < 45; i++)
        *dp++ = *sp++;

}



void init_wk()
{
    REG WORD l;
    REG WORD *pointer, *src_ptr;
    REG struct attribute *work_ptr;

    pointer = INTIN;
    pointer++;
    work_ptr = cur_work;

    l = *pointer++;             /* INTIN[1] */
    work_ptr->line_index = ((l > MX_LN_STYLE) || (l < 0)) ? 0 : l - 1;

    l = *pointer++;             /* INTIN[2] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    work_ptr->line_color = MAP_COL[l];

    l = *pointer++ - 1;         /* INTIN[3] */
    work_ptr->mark_index = ((l >= MAX_MARK_INDEX) || (l < 0)) ? 2 : l;

    l = *pointer++;             /* INTIN[4] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    work_ptr->mark_color = MAP_COL[l];

    /* You always get the default font */

    pointer++;                  /* INTIN[5] */

    l = *pointer++;             /* INTIN[6] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    work_ptr->text_color = MAP_COL[l];

    work_ptr->mark_height = DEF_MKHT;
    work_ptr->mark_scale = 1;

    l = *pointer++;             /* INTIN[7] */
    work_ptr->fill_style = ((l > MX_FIL_STYLE) || (l < 0)) ? 0 : l;

    l = *pointer++;             /* INTIN[8] */
    if (work_ptr->fill_style == 2)
        l = ((l > MX_FIL_PAT_INDEX) || (l < 1)) ? 1 : l;
    else
        l = ((l > MX_FIL_HAT_INDEX) || (l < 1)) ? 1 : l;
    work_ptr->fill_index = l;

    l = *pointer++;             /* INTIN[9] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    work_ptr->fill_color = MAP_COL[l];

    work_ptr->xfm_mode = *pointer;      /* INTIN[10] */

    st_fl_ptr();                /* set the fill pattern as requested */

    work_ptr->wrt_mode = 0;     /* default is replace mode */
    work_ptr->line_width = DEF_LWID;
    work_ptr->line_beg = 0;     /* default to squared ends */
    work_ptr->line_end = 0;

    work_ptr->fill_per = TRUE;

    work_ptr->xmn_clip = 0;
    work_ptr->ymn_clip = 0;
    work_ptr->xmx_clip = DEV_TAB[0];
    work_ptr->ymx_clip = DEV_TAB[1];
    work_ptr->clip = 0;

    work_ptr->cur_font = def_font;

    work_ptr->loaded_fonts = NULLPTR;

    work_ptr->scrpt2 = scrtsiz;
    work_ptr->scrtchp = deftxbuf;

    work_ptr->num_fonts = font_count;

    work_ptr->style = 0;        /* reset special effects */
    work_ptr->scaled = FALSE;
    work_ptr->h_align = 0;
    work_ptr->v_align = 0;
    work_ptr->chup = 0;
    work_ptr->pts_mode = FALSE;

    /* move default user defined pattern to RAM */

    src_ptr = ROM_UD_PATRN;
    pointer = &work_ptr->ud_patrn[0];

    for (l = 0; l < 16; l++)
        *pointer++ = *src_ptr++;

    work_ptr->multifill = 0;

    work_ptr->ud_ls = LINE_STYLE[0];

    pointer = CONTRL;
    *(pointer + 2) = 6;
    *(pointer + 4) = 45;

    pointer = INTOUT;
    src_ptr = DEV_TAB;
    for (l = 0; l < 45; l++)
        *pointer++ = *src_ptr++;

    pointer = PTSOUT;
    src_ptr = SIZ_TAB;
    for (l = 0; l < 12; l++)
        *pointer++ = *src_ptr++;

    flip_y = 1;
}



/*
 * vex_butv
 *
 * This routine replaces the mouse button change vector with
 * the address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when there is a
 * change in the mouse button status.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 */

void vex_butv()
{
    LONG * pointer;

    pointer = (LONG*)&CONTRL[9];
    *pointer = (LONG)user_but;
    (LONG*)user_but = *--pointer;
}



/*
 * vex_motv
 *
 * This routine replaces the mouse coordinate change vector with the address
 * of a user-supplied routine.  The previous value is returned so that it
 * also may be called when there is a change in the mouse coordinates.
 *
 *  Inputs:
 *     contrl[7], contrl[8] - pointer to user routine
 *
 *  Outputs:
 *     contrl[9], contrl[10] - pointer to old routine
 */

void vex_motv()
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_mot;
    (LONG*)user_mot = *--pointer;
}



/*
 * vex_curv
 *
 * This routine replaces the mouse draw vector with the
 * address of a user-supplied routine.  The previous value
 * is returned so that it also may be called when the mouse
 * is to be drawn.
 *
 * Inputs:
 *    contrl[7], contrl[8] - pointer to user routine
 *
 * Outputs:
 *    contrl[9], contrl[10] - pointer to old routine
 *
 */

void vex_curv()
{
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];
    *pointer = (LONG) user_cur;
    (LONG*)user_cur = *--pointer;
}



/*
 * vex_timv - exchange timer interrupt vector
 * 
 * entry:          new vector in CONTRL[7-8]
 * exit:           old vector in CONTRL[9-10]
 */

void vex_timv()
{
    WORD old_sr;
    LONG * pointer;

    pointer = (LONG*) &CONTRL[9];

    old_sr = set_sr(0x2700);

    *pointer = (LONG) tim_addr;
    (LONG*)tim_addr = *--pointer;

    set_sr(old_sr);

    INTOUT[0] = (WORD)Tickcal();
}



/* Two main jumptables for VDI functions */
void (*jmptb1[])() = {
    v_opnwk,            /*   1 */
    v_clswk,            /*   2 */
    v_clrwk,            /*   3 */
    v_nop,              /*   4 - v_updwk not yet implemented */
    chk_esc,            /*   5 - each escape function has it's own call */
    v_pline,            /*   6 */
    v_pmarker,          /*   7 */
    d_gtext,            /*   8 */
    v_fillarea,         /*   9 */
    v_cellarray,        /*  10 */
    v_gdp,              /*  11 */
    dst_height,         /*  12 */
    dst_rotation,       /*  13 */
    vs_color,           /*  14 */
    vsl_type,           /*  15 */
    vsl_width,          /*  16 */
    vsl_color,          /*  17 */
    vsm_type,           /*  18 */
    vsm_height,         /*  19 */
    vsm_color,          /*  20 */
    dst_font,           /*  21 */
    dst_color,          /*  22 */
    vsf_interior,       /*  23 */
    vsf_style,          /*  24 */
    vsf_color,          /*  25 */
    vq_color,           /*  26 */
    vq_cellarray,       /*  27 */
    v_locator,          /*  28 */
    v_valuator,         /*  29 */
    v_choice,           /*  30 */
    v_string,           /*  31 */
    vswr_mode,          /*  32 */
    vsin_mode,          /*  33 */
    v_nop,              /*  34 */
    vql_attr,           /*  35 */
    vqm_attr,           /*  36 */
    vqf_attr,           /*  37 */
    dqt_attributes,     /*  38 */
    dst_alignment       /*  39 */
};

void(*jmptb2[])() = {
    d_opnvwk,           /* 100 */
    d_clsvwk,           /* 101 */
    vq_extnd,           /* 102 */
    d_contourfill,      /* 103 */
    vsf_perimeter,      /* 104 */
    v_get_pixel,        /* 105 */
    dst_style,          /* 106 */
    dst_point,          /* 107 */
    vsl_ends,           /* 108 */
    vdi_vro_cpyfm,      /* 109 */
    vr_trnfm,           /* 110 */
    xfm_crfm,           /* 111 */
    dsf_udpat,          /* 112 */
    vsl_udsty,          /* 113 */
    dr_recfl,           /* 114 */
    vqi_mode,           /* 115 */
    dqt_extent,         /* 116 */
    dqt_width,          /* 117 */
    vex_timv,           /* 118 */ /* in lisagem.S */
    dt_loadfont,        /* 119 */
    dt_unloadfont,      /* 120 */
    vdi_vrt_cpyfm,      /* 121 */
    v_show_c,           /* 122 */
    v_hide_c,           /* 123 */
    vq_mouse,           /* 124 */
    vex_butv,           /* 125 */ /* in vdimouse.S */
    vex_motv,           /* 126 */ /* in vdimouse.S */
    vex_curv,           /* 127 */ /* in vdimouse.S */
    vq_key_s,           /* 128 */
    s_clip,             /* 129 */
    dqt_name,           /* 130 */
    dqt_fontinfo        /* 131 */
};



/*
 * screen - Screen driver entry point
 */

void screen()
{
    REG WORD opcode, r, *control;
    REG struct attribute *work_ptr;
    BYTE found;

    control = CONTRL;
    r = *(control + 6);

    opcode = *control;

    //cprintf("SCREEN opcode=%d\n", opcode);

    /* no ints out & no pts out */

    *(control + 2) = 0;
    *(control + 4) = 0;

    flip_y = 0;

    if (opcode != 1 && opcode != 100) {

        /* Find the attribute area which matches the handle */
        work_ptr = &virt_work;

        found = 0;
        do {
            found = (r == work_ptr->handle);
        } while (!found && (work_ptr = work_ptr->next_work));

        /* handle is invalid if we fall through, so exit */
        if (!found)
            return;

        cur_work = work_ptr;
        INQ_TAB[19] = CLIP = work_ptr->clip;
        XMN_CLIP = work_ptr->xmn_clip;
        YMN_CLIP = work_ptr->ymn_clip;
        XMX_CLIP = work_ptr->xmx_clip;
        YMX_CLIP = work_ptr->ymx_clip;

        WRT_MODE = work_ptr->wrt_mode;

        patptr = work_ptr->patptr;
        patmsk = work_ptr->patmsk;

        if (work_ptr->fill_style == 4)
            multifill = work_ptr->multifill;
        else
            multifill = 0;

        font_ring[2] = work_ptr->loaded_fonts;

        DEV_TAB[10] = work_ptr->num_fonts;

        DDA_INC = work_ptr->dda_inc;
        T_SCLSTS = work_ptr->t_sclsts;
        DOUBLE = work_ptr->scaled;

        cur_font = work_ptr->cur_font;

        MONO_STATUS = F_MONOSPACE & cur_font->flags;
        scrpt2 = work_ptr->scrpt2;
        scrtchp = work_ptr->scrtchp;
        STYLE = work_ptr->style;
        h_align = work_ptr->h_align;
        v_align = work_ptr->v_align;
        CHUP = work_ptr->chup;

    }
    /* end if open work or vwork */
    if (opcode >= 1 && opcode <= 39) {
        (*jmptb1[opcode - 1]) ();
    }

    else if (opcode >= 100 && opcode <= 131) {
        (*jmptb2[opcode - 100]) ();
    }

}
