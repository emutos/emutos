/*
 * 
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



#include "portab.h"
#include "tosvars.h"
#include "lineavars.h"
#include "vdi_defs.h"
//#include "kprint.h"
#include "biosbind.h"
#include "asm.h"



Vwk virt_work;     /* attribute areas for workstations */

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
    1,                  /* 0  - type of alpha/graphic controllers */
    1,                  /* 1  - number of background colors  */
    0x1F,               /* 2  - text styles supported        */
    0,                  /* 3  - scale rasters = false        */
    1,                  /* 4  - number of planes         */
    0,                  /* 5  - video lookup table       */
    50,                 /* 6  - performance factor????       */
    1,                  /* 7  - contour fill capability      */
    1,                  /* 8  - character rotation capability    */
    4,                  /* 9  - number of writing modes      */
    2,                  /* 10 - highest input mode       */
    1,                  /* 11 - text alignment flag      */
    0,                  /* 12 - Inking capability        */
    0,                  /* 13 - rubber banding           */
    256,                /* 14 - maximum vertices (must agree with vdi_asm.s) */
    -1,                 /* 15 - maximum intin            */
    1,                  /* 16 - number of buttons on MOUSE   */
    0,                  /* 17 - styles for wide lines            */
    0,                  /* 18 - writing modes for wide lines     */
    0,                  /* 19 - filled in with clipping flag     */

    0,                  /* 20 - extended precision pixel size information */
    0,                  /* 21 - pixel width in 1/10, 1/100 or 1/1000 microns */
    0,                  /* 22 - pixel height in 1/10, 1/100 or 1/1000 microns */
    0,                  /* 23 - horizontal resolution in dpi */
    0,                  /* 24 - vertical resolution in dpi */
    0,                  /* 25 -  */
    0,                  /* 26 -  */
    0,                  /* 27 -  */
    0,                  /* 28 - bezier flag (bit 1) */
    0,                  /* 29 -  */
    0,                  /* 30 - raster flag (bit 0), does vro_cpyfm scaling? */
    0,                  /* 31 -  */
    0,                  /* 32 -  */
    0,                  /* 33 -  */
    0,                  /* 34 -  */
    0,                  /* 35 -  */
    0,                  /* 36 -  */
    0,                  /* 37 -  */
    0,                  /* 38 -  */
    0,                  /* 39 -  */
    0,                  /* 40 - not imprintable left border in pixels (printers/plotters) */
    0,                  /* 41 - not imprintable upper border in pixels (printers/plotters) */
    0,                  /* 42 - not imprintable right border in pixels (printers/plotters) */
    0,                  /* 43 - not imprintable lower border in pixels (printers/plotters) */
    0                   /* 44 - page size (printers etc.) */
};



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
    MX_FIL_PAT_INDEX,           /* 11   area patterns             */
    MX_FIL_HAT_INDEX,           /* 12   crosshatch patterns       */
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



Vwk * get_vwk_by_handle(WORD handle)
{
    Vwk * vwk = &virt_work;

    /* Find the attribute area which matches the handle */
    do {
        if (handle == vwk->handle)
            return vwk;
    } while ((vwk = vwk->next_work));

    return NULL;
}


/*
 * vs_color - set color index table
 */
void vs_color(Vwk * vwk)
{
    /* not implemented */
}



/*
 * vq_color - query color index table
 */
void vq_color(Vwk * vwk)
{
    /* not implemented */
}



/* Set Clip Region */
void s_clip(Vwk * vwk)
{
    vwk->clip = *INTIN;
    if (vwk->clip) {
        WORD rtemp;
        Rect * rect = (Rect*)PTSIN;
        arb_corner(rect);

        rtemp = rect->x1;
        vwk->xmn_clip = (rtemp < 0) ? 0 : rtemp;

        rtemp = rect->y1;
        vwk->ymn_clip = (rtemp < 0) ? 0 : rtemp;

        rtemp = rect->x2;
        vwk->xmx_clip = (rtemp > DEV_TAB[0]) ? DEV_TAB[0] : rtemp;

        rtemp = rect->y2;
        vwk->ymx_clip = (rtemp > DEV_TAB[1]) ? DEV_TAB[1] : rtemp;
    } else {
        vwk->xmn_clip = 0;
        vwk->ymn_clip = 0;
        vwk->xmx_clip = xres;
        vwk->ymx_clip = yres;
    }
}



/* SET_WRITING_MODE: */
void vswr_mode(Vwk * vwk)
{
    WORD wm;

    CONTRL[4] = 1;
    wm = INTIN[0] - 1;
    if ((wm > MAX_MODE) | (wm < 0))
        wm = 0;

    INTOUT[0] = (vwk->wrt_mode = wm) + 1;
}



static void init_wk(Vwk * vwk)
{
    WORD l;
    WORD *pointer, *src_ptr;

    pointer = INTIN;
    pointer++;

    l = *pointer++;             /* INTIN[1] */
    vwk->line_index = ((l > MX_LN_STYLE) || (l < 0)) ? 0 : l - 1;

    l = *pointer++;             /* INTIN[2] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    vwk->line_color = MAP_COL[l];

    l = *pointer++ - 1;         /* INTIN[3] */
    vwk->mark_index = ((l >= MAX_MARK_INDEX) || (l < 0)) ? 2 : l;

    l = *pointer++;             /* INTIN[4] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    vwk->mark_color = MAP_COL[l];

    /* You always get the default font */
    pointer++;                  /* INTIN[5] */

    l = *pointer++;             /* INTIN[6] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    vwk->text_color = MAP_COL[l];

    vwk->mark_height = DEF_MKHT;
    vwk->mark_scale = 1;

    l = *pointer++;             /* INTIN[7] */
    vwk->fill_style = ((l > MX_FIL_STYLE) || (l < 0)) ? 0 : l;

    l = *pointer++;             /* INTIN[8] */
    if (vwk->fill_style == 2)
        l = ((l > MX_FIL_PAT_INDEX) || (l < 1)) ? 1 : l;
    else
        l = ((l > MX_FIL_HAT_INDEX) || (l < 1)) ? 1 : l;
    vwk->fill_index = l;

    l = *pointer++;             /* INTIN[9] */
    if ((l >= DEV_TAB[13]) || (l < 0))
        l = 1;
    vwk->fill_color = MAP_COL[l];

    vwk->xfm_mode = *pointer;      /* INTIN[10] */

    st_fl_ptr(vwk);                /* set the fill pattern as requested */

    vwk->wrt_mode = 0;     /* default is replace mode */
    vwk->line_width = DEF_LWID;
    vwk->line_beg = 0;     /* default to squared ends */
    vwk->line_end = 0;

    vwk->fill_per = TRUE;

    vwk->xmn_clip = 0;
    vwk->ymn_clip = 0;
    vwk->xmx_clip = DEV_TAB[0];
    vwk->ymx_clip = DEV_TAB[1];
    vwk->clip = FALSE;

    text_init2(vwk);

    /* move default user defined pattern to RAM */
    pointer = &vwk->ud_patrn[0];
    src_ptr = ROM_UD_PATRN;
    for (l = 0; l < 16; l++)
        *pointer++ = *src_ptr++;

    vwk->multifill = 0;
    vwk->ud_ls = LINE_STYLE[0];

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

    /* setup initial bezier values */
    vwk->bez_qual = 7;
#if 0
    vwk->bezier.available = 1;
    vwk->bezier.depth_scale.min = 9;
    vwk->bezier.depth_scale.max = 0;
    vwk->bezier.depth.min = 2;
    vwk->bezier.depth.max = 7;
#endif

    flip_y = 1;
}



void d_opnvwk(Vwk * vwk)
{
    WORD handle;
    Vwk *temp, *work_ptr;

    /* Allocate the memory for a virtual workstation. */
    vwk = (Vwk *)trap1(X_MALLOC, (LONG) (sizeof(Vwk)));
    if (vwk == NULLPTR) {
        CONTRL[6] = 0;  /* No memory available, exit */
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

    /* Empty slot found, insert the workstation here */
    temp = work_ptr->next_work;   /* may be NULL */
    work_ptr->next_work = vwk;
    vwk->next_work = temp;

    vwk->handle = CONTRL[6] = handle;
    init_wk(vwk);
}

void d_clsvwk(Vwk * vwk)
{
    Vwk *work_ptr;
    WORD handle;

    /* vwk points to workstation to deallocate, find who points to me */
    handle = vwk->handle;
    if (handle == 1)            /* Can't close physical this way */
        return;

    for (work_ptr = &virt_work; handle != work_ptr->next_work->handle;
         work_ptr = work_ptr->next_work);

    work_ptr->next_work = vwk->next_work;
    trap1(X_MFREE, vwk);
}



/* OPEN_WORKSTATION: */
void v_opnwk(Vwk * vwk)
{
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

    vwk = &virt_work;
    CONTRL[6] = vwk->handle = 1;
    vwk->next_work = NULLPTR;

    line_cw = -1;               /* invalidate current line width */

    text_init(vwk);             /* initialize the SIZ_TAB info */

    init_wk(vwk);

    timer_init(vwk);
    vdimouse_init(vwk);         /* initialize mouse */
    esc_init(vwk);              /* enter graphics mode */
}



/* CLOSE_WORKSTATION: */
void v_clswk(Vwk * vwk)
{
    Vwk *next_work;

    if (virt_work.next_work != NULLPTR) {       /* Are there VWs to close */
        vwk = virt_work.next_work;
        do {
            next_work = vwk->next_work;
            trap1(X_MFREE, vwk);
        } while ((vwk = next_work));
    }

    timer_exit(vwk);
    vdimouse_exit(vwk);                 // deinitialize mouse
    esc_exit(vwk);                      // back to console mode
}



/*
 * v_clrwk - clear screen
 *
 * Screen is cleared between v_bas_ad and phystop.
 */

void v_clrwk(Vwk * vwk)
{
    UBYTE * addr;               /* pointer to screen longword */

    /* clear the screen */
    for (addr = v_bas_ad; addr < (UBYTE *)phystop; addr++) {
        *addr = 0;             /* clear the long word */
    }
}



/*
 * vq_extnd - Extended workstation inquire
 */

void vq_extnd(Vwk * vwk)
{
    WORD i;
    WORD *dst, *src;

    CONTRL[2] = 6;
    CONTRL[4] = 45;

    flip_y = 1;
    dst = PTSOUT;
    if (*(INTIN) == 0) {
        src = SIZ_TAB;
        for (i = 0; i < 12; i++)
            *dst++ = *src++;

        src = DEV_TAB;
    }
    else {
        /* copy the clipping ranges to PTSOUT */
        *dst++ = vwk->xmn_clip;       /* PTSOUT[0] */
        *dst++ = vwk->ymn_clip;       /* PTSOUT[1] */
        *dst++ = vwk->xmx_clip;       /* PTSOUT[2] */
        *dst++ = vwk->ymx_clip;       /* PTSOUT[3] */

        for (i = 4; i < 12; i++)
            *dst++ = 0;

        src = INQ_TAB;
        INQ_TAB[19] = vwk->clip;      /* now update INQTAB */
    }

    /* copy DEV_TAB or INQ_TAB to INTOUT */
    dst = INTOUT;
    for (i = 0; i < 45; i++)
        *dst++ = *src++;
}



/*
 * v_nop - dummy
 */
void v_nop(Vwk * vwk)
{
    /* never will be  implemented */
}
