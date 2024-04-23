/*
 * vdi_control.c - Workstation functions
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc.
 * Copyright 2002-2022 The EmuTOS development team.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "lineavars.h"
#include "vdi_defs.h"
#include "biosbind.h"
#include "xbiosbind.h"
#include "biosext.h"
#include "asm.h"
#include "string.h"
#include "has.h"
#include "intmath.h"
#include "bdosbind.h"
#include "tosvars.h"

#define FIRST_VDI_HANDLE    1
#define LAST_VDI_HANDLE     (FIRST_VDI_HANDLE+NUM_VDI_HANDLES-1)
#define VDI_PHYS_HANDLE     FIRST_VDI_HANDLE


/*
 * Mxalloc() mode used when allocating the virtual workstation.  This
 * is only significant when running under FreeMiNT, since EmuTOS ignores
 * these bits of the mode field.
 */
#define MX_SUPER            (3<<4)


/*
 * ptr to current mouse cursor save area, based on v_planes
 */
MCS *mcs_ptr;


/*
 * entry n in the following array points to the Vwk corresponding to
 * VDI handle n.  entry 0 is unused.
 */
static Vwk *vwk_ptr[NUM_VDI_HANDLES+1];
extern Vwk phys_work;       /* attribute area for physical workstation */
#if CONF_WITH_VDI_16BIT
static VwkExt phys_work_ext;
#endif

/*
 * template for SIZ_TAB - Returns text, line and marker sizes in device
 * coordinates. See lineavars.S for SIZE_TAB itself.
 */
static const WORD SIZ_TAB_rom[12] = {
    0,                          /* 0    min char width          */
    7,                          /* 1    min char height         */
    0,                          /* 2    max char width          */
    7,                          /* 3    max char height         */
    1,                          /* 4    min line width          */
    0,                          /* 5    reserved 0          */
    MAX_LINE_WIDTH,             /* 6    max line width          */
    0,                          /* 7    reserved 0          */
    15,                         /* 8    min marker width        */
    11,                         /* 9    min marker height       */
    120,                        /* 10   max marker width        */
    88,                         /* 11   max marker height       */
};



/* Here's the template INQ_TAB, see lineavars.S for the normal INQ_TAB */
static const WORD INQ_TAB_rom[45] = {
    4,                  /* 0  - type of alpha/graphic controllers */
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
    MAX_VERTICES,       /* 14 - maximum vertices         */
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
    0,                  /* 40 - unprintable left border in pixels (printers/plotters) */
    0,                  /* 41 - unprintable upper border in pixels (printers/plotters) */
    0,                  /* 42 - unprintable right border in pixels (printers/plotters) */
    0,                  /* 43 - unprintable lower border in pixels (printers/plotters) */
    0                   /* 44 - page size (printers etc.) */
};



/* Here's the template DEV_TAB, see lineavars.S for the normal DEV_TAB! */
static const WORD DEV_TAB_rom[45] = {
    639,                        /* 0    x resolution             */
    399,                        /* 1    y resolution             */
    0,                          /* 2    device precision 0=exact,1=not exact */
    372,                        /* 3    width of pixel           */
    372,                        /* 4    height of pixel          */
    3,                          /* 5    number of text font heights */
    MAX_LINE_STYLE,             /* 6    linestyles               */
    0,                          /* 7    linewidth                */
    6,                          /* 8    marker types             */
    8,                          /* 9    marker size              */
    1,                          /* 10   text font                */
    MAX_FILL_PATTERN,           /* 11   area patterns             */
    MAX_FILL_HATCH,             /* 12   crosshatch patterns       */
    2,                          /* 13   colors at one time       */
    10,                         /* 14   number of GDP's          */
    1,                          /* 15   GDP bar                  */
    2,                          /* 16   GDP arc                  */
    3,                          /* 17   GDP pie                  */
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
    2,                          /* 39   Palette size             */
    2,                          /* 40   # of locator devices 1 = mouse */
    1,                          /* 41   # of valuator devices    */
    1,                          /* 42   # of choice devices      */
    1,                          /* 43   # of string devices      */
    2                           /* 44   Workstation Type 2 = out/in */
};



Vwk * get_vwk_by_handle(WORD handle)
{
    if ((handle < FIRST_VDI_HANDLE) || (handle > LAST_VDI_HANDLE))
        return NULL;

    return vwk_ptr[handle];
}



/*
 * update resolution-dependent VDI/lineA variables
 *
 * this function assumes that v_planes, V_REZ_HZ, V_REZ_VT are already set
 */
void update_rez_dependent(void)
{
    BYTES_LIN = v_lin_wr = V_REZ_HZ / 8 * v_planes;

#if EXTENDED_PALETTE
    mcs_ptr = (v_planes <= 4) ? &mouse_cursor_save : &ext_mouse_cursor_save;
#else
    mcs_ptr = &mouse_cursor_save;
#endif

    DEV_TAB[0] = V_REZ_HZ - 1;
    DEV_TAB[1] = V_REZ_VT - 1;
    get_pixel_size(&DEV_TAB[3],&DEV_TAB[4]);
    DEV_TAB[13] = (v_planes<8) ? (1 << v_planes) : 256;
    DEV_TAB[35] = (v_planes==1) ? 0 : 1;
    DEV_TAB[39] = get_palette();    /* some versions of COLOR.CPX care about this */

    INQ_TAB[4] = v_planes;
    if ((v_planes == 16) || (get_monitor_type() == MON_MONO))
        INQ_TAB[5] = 0;
    else INQ_TAB[5] = 1;
}



/*
 * validate colour index
 *
 * checks the supplied colour index and, if valid, returns it;
 * otherwise returns 1 (which by default maps to black)
 */
WORD validate_color_index(WORD colnum)
{
    if ((colnum < 0) || (colnum >= numcolors))
        return 1;

    return colnum;
}



/* Set Clip Region */
void vdi_vs_clip(Vwk * vwk)
{
    vwk->clip = INTIN[0];
    if (vwk->clip) {
        Rect * rect = (Rect*)PTSIN;
        arb_corner(rect);
        vwk->xmn_clip = max(0, rect->x1);
        vwk->ymn_clip = max(0, rect->y1);
        vwk->xmx_clip = min(xres, rect->x2);
        vwk->ymx_clip = min(yres, rect->y2);
    } else {
        vwk->xmn_clip = 0;
        vwk->ymn_clip = 0;
        vwk->xmx_clip = xres;
        vwk->ymx_clip = yres;
    }
}



/* SET_WRITING_MODE: */
void vdi_vswr_mode(Vwk * vwk)
{
    WORD wm;

    CONTRL[4] = 1;
    wm = ((INTIN[0]<MIN_WRT_MODE) || (INTIN[0]>MAX_WRT_MODE)) ? DEF_WRT_MODE : INTIN[0];

    INTOUT[0] = wm;
    vwk->wrt_mode = wm - 1;
}



static void init_wk(Vwk * vwk)
{
    WORD l;
    WORD *pointer;
    const WORD *src_ptr;

    pointer = INTIN;
    pointer++;

    l = *pointer++;             /* INTIN[1] */
    if ((l > MAX_LINE_STYLE) || (l < MIN_LINE_STYLE))
        l = DEF_LINE_STYLE;
    vwk->line_index = l - 1;

    l = validate_color_index(*pointer++);   /* INTIN[2] */
    vwk->line_color = MAP_COL[l];

    l = *pointer++;             /* INTIN[3] */
    if ((l > MAX_MARK_STYLE) || (l < MIN_MARK_STYLE))
        l = DEF_MARK_STYLE;
    vwk->mark_index = l - 1;

    l = validate_color_index(*pointer++);   /* INTIN[4] */
    vwk->mark_color = MAP_COL[l];

    /* You always get the default font */
    pointer++;                  /* INTIN[5] */

    l = validate_color_index(*pointer++);   /* INTIN[6] */
    vwk->text_color = MAP_COL[l];

    vwk->mark_height = DEF_MKHT;
    vwk->mark_scale = 1;

    l = *pointer++;             /* INTIN[7] */
    vwk->fill_style = ((l > MAX_FILL_STYLE) || (l < MIN_FILL_STYLE)) ? DEF_FILL_STYLE : l;

    l = *pointer++;             /* INTIN[8] */
    if (vwk->fill_style == FIS_PATTERN)
        l = ((l > MAX_FILL_PATTERN) || (l < MIN_FILL_PATTERN)) ? DEF_FILL_PATTERN : l;
    else
        l = ((l > MAX_FILL_HATCH) || (l < MIN_FILL_HATCH)) ? DEF_FILL_HATCH : l;
    vwk->fill_index = l;

    l = validate_color_index(*pointer++);   /* INTIN[9] */
    vwk->fill_color = MAP_COL[l];

    vwk->xfm_mode = *pointer;      /* INTIN[10] */

    st_fl_ptr(vwk);                /* set the fill pattern as requested */

    vwk->wrt_mode = WM_REPLACE;    /* default is replace mode */
    vwk->line_width = DEF_LWID;
    vwk->line_beg = SQUARED;       /* default to squared ends */
    vwk->line_end = SQUARED;

    vwk->fill_per = TRUE;

    vwk->xmn_clip = 0;
    vwk->ymn_clip = 0;
    vwk->xmx_clip = xres;
    vwk->ymx_clip = yres;
    vwk->clip = FALSE;

    text_init2(vwk);

    /* move default user defined pattern to RAM */
    pointer = &vwk->ud_patrn[0];
    src_ptr = (const WORD *)ROM_UD_PATRN;
    for (l = 0; l < 16; l++)
        *pointer++ = *src_ptr++;

    vwk->multifill = 0;
    vwk->ud_ls = LINE_STYLE[0];

    pointer = INTOUT;
    src_ptr = DEV_TAB;
    for (l = 0; l < 45; l++)
        *pointer++ = *src_ptr++;

    pointer = PTSOUT;
    src_ptr = SIZ_TAB;
    for (l = 0; l < 12; l++)
        *pointer++ = *src_ptr++;

#if CONF_WITH_VDI_16BIT
    /* set up virtual palette stuff if we're not using a real one */
    if (TRUECOLOR_MODE) {
        memcpy(vwk->ext->req_col, REQ_COL, sizeof(REQ_COL));
        memcpy(vwk->ext->req_col+16, req_col2, sizeof(req_col2));
        /* convert requested colour values to pseudo-palette */
        for (l = 0; l < 255; l++)
            set_color16(vwk, l, vwk->ext->req_col[l]);
    }
#endif

#if HAVE_BEZIER
    /* setup initial bezier values */
    vwk->bez_qual = 7;
#if 0
    vwk->bezier.available = 1;
    vwk->bezier.depth_scale.min = 9;
    vwk->bezier.depth_scale.max = 0;
    vwk->bezier.depth.min = 2;
    vwk->bezier.depth.max = 7;
#endif
#endif

    vwk->next_work = NULL;  /* neatness */

    flip_y = 1;
}



/*
 * build a chain of Vwks
 *
 * this links all of the currently allocated Vwks together, as in
 * Atari TOS.  some programs may depend on this.
 */
static void build_vwk_chain(void)
{
    Vwk *prev, **vwk;
    WORD handle;

    prev = &phys_work;
    for (handle = VDI_PHYS_HANDLE+1, vwk = vwk_ptr+handle; handle <= LAST_VDI_HANDLE; handle++, vwk++) {
        if (*vwk) {
            prev->next_work = *vwk;
            prev = *vwk;
        }
    }
    prev->next_work = NULL;
}



void vdi_v_opnvwk(Vwk * vwk)
{
    WORD handle;
    LONG size;
    Vwk **p;

    /*
     * ensure that CUR_WORK always points to a valid workstation
     * even if v_opnvwk() exits early.
     */
    CUR_WORK = &phys_work;

    /* First find a free handle */
    for (handle = VDI_PHYS_HANDLE+1, p = vwk_ptr+handle; handle <= LAST_VDI_HANDLE; handle++, p++) {
        if (!*p) {
            break;
        }
    }
    if (handle > LAST_VDI_HANDLE) { /* No handle available, exit */
        CONTRL[6] = 0;
        return;
    }

    /*
     * Allocate the memory for a virtual workstation
     *
     * The virtual workstations for all programs are chained together by
     * build_vwk_chain(), because some programs (notably Warp9) expect this.
     * To avoid problems when running FreeMiNT with memory protection, we
     * must allocate the virtual workstations in supervisor-accessible memory.
     */
    size = sizeof(Vwk);
#if CONF_WITH_VDI_16BIT
    if (TRUECOLOR_MODE)
        size += sizeof(VwkExt); /* for simplicity, allocate them together */
#endif
    vwk = (Vwk *)Mxalloc(size, MX_SUPER);
    if (vwk == NULL) {
        CONTRL[6] = 0;  /* No memory available, exit */
        return;
    }

#if CONF_WITH_VDI_16BIT
    vwk->ext = NULL;
    if (TRUECOLOR_MODE)
        vwk->ext = (VwkExt *)(vwk + 1); /* immediately follows Vwk */
#endif

    vwk_ptr[handle] = vwk;
    vwk->handle = CONTRL[6] = handle;
    init_wk(vwk);
    build_vwk_chain();
    CUR_WORK = vwk;
}

void vdi_v_clsvwk(Vwk * vwk)
{
    WORD handle;

    /* vwk points to workstation to deallocate */
    handle = vwk->handle;
    if (handle == VDI_PHYS_HANDLE)  /* can't close physical this way */
        return;
    if (!vwk_ptr[handle])           /* workstation is already closed */
        return;

    vwk_ptr[handle] = NULL;         /* close it */

    build_vwk_chain();              /* rebuild chain */

    /*
     * When we close a virtual workstation, Atari TOS and previous versions
     * of EmuTOS update CUR_WORK (line-A's idea of the current workstation)
     * to point to the precursor of the closed workstation.  This is a bit
     * arbitrary, especially as the workstation being closed isn't necessarily
     * what line-A thinks is the current one.
     *
     * What we must do as a minimum is ensure that CUR_WORK points to a
     * valid open workstation.  The following does that by pointing it to
     * the physical workstation.  That's what NVDI appears to do too.
     */
    CUR_WORK = &phys_work;

    Mfree(vwk);
}



/* OPEN_WORKSTATION: */
void vdi_v_opnwk(Vwk * vwk)
{
    int i;
    Vwk **p;
    WORD newrez;

    /*
     * Programs can request a video mode switch by passing the desired
     * mode + 2 in INTIN[0].
     */
    newrez = INTIN[0] - 2;
    if (
        (newrez == ST_LOW) || (newrez == ST_MEDIUM) || (newrez == ST_HIGH)
#if CONF_WITH_TT_SHIFTER
        || (newrez == TT_LOW) || (newrez == TT_MEDIUM)
#endif
       ) {
        if (newrez != Getrez()) {
            Setscreen(-1L, -1L, newrez, 0);
        }
    }
#if CONF_WITH_VIDEL
    if (newrez == FALCON_REZ) {
        /* Atari TOS 4 uses INTOUT (sic!) to pass new Videl mode. */
        WORD newvidel = INTOUT[45];
        WORD curvidel = VsetMode(-1);
        if (curvidel != newvidel) {
            Setscreen(0L, 0L, newrez, newvidel);
        }
    }
#endif

    /* We need to copy some initial table data from the ROM */
    for (i = 0; i < 12; i++) {
        SIZ_TAB[i] = SIZ_TAB_rom[i];
    }

    for (i = 0; i < 45; i++) {
        DEV_TAB[i] = DEV_TAB_rom[i];
        INQ_TAB[i] = INQ_TAB_rom[i];
    }

    /* update resolution-dependent values */
    update_rez_dependent();

    /* initialize the vwk pointer array */
    vwk = &phys_work;
    vwk_ptr[VDI_PHYS_HANDLE] = vwk;
    CONTRL[6] = vwk->handle = VDI_PHYS_HANDLE;
    for (i = VDI_PHYS_HANDLE+1, p = vwk_ptr+i; i <= LAST_VDI_HANDLE; i++)
        *p++ = NULL;

    line_cw = -1;               /* invalidate current line width */

    init_colors();              /* Initialize palette etc. */

    text_init();                /* initialize the SIZ_TAB info */

#if CONF_WITH_VDI_16BIT
    vwk->ext = NULL;
    if (TRUECOLOR_MODE)
        vwk->ext = &phys_work_ext;  /* workstation extension */
#endif

    init_wk(vwk);

    timer_init();
    vdimouse_init();            /* initialize mouse */
    esc_init(vwk);              /* enter graphics mode */

    /* Just like TOS 2.06, make the physical workstation the current workstation for Line-A. */
    CUR_WORK = vwk;
}



/* CLOSE_WORKSTATION: */
void vdi_v_clswk(Vwk * vwk)
{
    WORD handle;
    Vwk **p;

    /* close all open virtual workstations */
    for (handle = VDI_PHYS_HANDLE+1, p = vwk_ptr+handle; handle <= LAST_VDI_HANDLE; handle++, p++) {
        if (*p) {
            Mfree(*p);
            *p = NULL;
        }
    }
    CUR_WORK = vwk_ptr[VDI_PHYS_HANDLE];

    timer_exit();
    vdimouse_exit();                    /* deinitialize mouse */
    esc_exit(vwk);                      /* back to console mode */
}



/*
 * vdi_v_clrwk - clear screen
 *
 * Screen is cleared from the base address v_bas_ad.
 */
void vdi_v_clrwk(Vwk * vwk)
{
    ULONG size;
    UBYTE fill;

    /* Calculate screen size */
    size = (ULONG)v_lin_wr * V_REZ_VT;

    /* clear the screen */
#if CONF_WITH_VDI_16BIT
    if (TRUECOLOR_MODE)
    {
        fill = 0xff;
    }
    else
#endif
    {
        fill = 0x00;
    }
    memset(v_bas_ad, fill, size);
}



/*
 * vdi_vq_extnd - Extended workstation inquire
 */
void vdi_vq_extnd(Vwk * vwk)
{
    WORD i;
    WORD *dst, *src;

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
