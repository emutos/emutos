/*      GEMGRAF.C       04/11/84 - 09/17/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */
/*      fix gr_gicon null text                  11/18/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2025 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "obdefs.h"
#include "aesdefs.h"
#include "aesext.h"
#include "intmath.h"
#include "funcdef.h"
#include "gemdos.h"

#include "gemgraf.h"
#include "gemgsxif.h"
#include "optimopt.h"
#include "gsx2.h"
#include "rectfunc.h"


GLOBAL WORD     gl_width;       /* screen width */
GLOBAL WORD     gl_height;      /* screen height */

GLOBAL WORD     gl_wchar;       /* width of character cell (normal font) */
GLOBAL WORD     gl_hchar;       /* height of character cell (normal font) */

GLOBAL WORD     gl_wschar;      /* width of character cell (small font) */
GLOBAL WORD     gl_hschar;      /* height of character cell (small font) */

GLOBAL WORD     gl_wbox;        /* box width */
GLOBAL WORD     gl_hbox;        /* box height */

GLOBAL GRECT    gl_clip;        /* global clipping rectangle */

GLOBAL WORD     gl_nplanes;     /* number of bit planes */
GLOBAL WORD     gl_handle;      /* physical workstation handle */

GLOBAL FDB      gl_src;
GLOBAL FDB      gl_dst;

GLOBAL WS       gl_ws;          /* work_out[] for physical workstation */
GLOBAL WORD     contrl[12];
GLOBAL WORD     intin[128];
GLOBAL WORD     ptsin[20];

GLOBAL GRECT    gl_rscreen;     /* the entire screen */
GLOBAL GRECT    gl_rfull;       /* the screen except the menu bar */
GLOBAL GRECT    gl_rzero;       /* 0,0,0,0 */
GLOBAL GRECT    gl_rcenter;     /* a box centered in the 'gl_rfull' area */
GLOBAL GRECT    gl_rmenu;       /* the menu bar */


/*
 * the following are used to save the currently-set values for
 * various VDI attributes, in order to save unnecessary VDI calls
 */
static WORD     gl_mode;        /* text writing mode (vswr_mode) */
static WORD     gl_tcolor;      /* text colour (vst_color) */
static WORD     gl_lcolor;      /* line colour (vsl_color) */
static WORD     gl_fis;         /* interior type (vsf_interior) */
static WORD     gl_patt;        /* style of fill pattern (vsf_style) */
static WORD     gl_font;        /* font type (IBM/SMALL) for v_gtext */

static WORD     gl_wptschar;    /* width of character (normal font) */
static WORD     gl_hptschar;    /* height of character (normal font) */

static WORD     gl_wsptschar;   /* width of character (small font) */
static WORD     gl_hsptschar;   /* height of character (small font) */


/*
 *  Routine to set the clip rectangle.  If the w,h of the clip is 0,
 *  then no clip should be set.  Otherwise, set the appropriate clip.
 */
void gsx_sclip(const GRECT *pt)
{
    gl_clip = *pt;

    if (gl_clip.g_w && gl_clip.g_h)
    {
        ptsin[0] = gl_clip.g_x;
        ptsin[1] = gl_clip.g_y;
        ptsin[2] = gl_clip.g_x + gl_clip.g_w - 1;
        ptsin[3] = gl_clip.g_y + gl_clip.g_h - 1;
        vs_clip( TRUE, ptsin);
    }
    else
        vs_clip( FALSE, ptsin);
}


/*
 *  Routine to get the current clip setting
 */
void gsx_gclip(GRECT *pt)
{
    *pt = gl_clip;
}


/*
 *  Routine to return TRUE iff the specified rectangle intersects the
 *  current clip rectangle ... or clipping is off (?)
 */
WORD gsx_chkclip(GRECT *pt)
{
    /* if clipping is on */
    if (gl_clip.g_w && gl_clip.g_h)
    {
        if ((pt->g_y + pt->g_h) < gl_clip.g_y)
            return FALSE;                   /* all above    */
        if ((pt->g_x + pt->g_w) < gl_clip.g_x)
            return FALSE;                   /* all left     */
        if ((gl_clip.g_y + gl_clip.g_h) <= pt->g_y)
            return FALSE;                   /* all below    */
        if ((gl_clip.g_x + gl_clip.g_w) <= pt->g_x)
            return FALSE;                   /* all right    */
    }

    return TRUE;
}


static void gsx_xline(WORD ptscount, WORD *ppoints)
{
    static const WORD hztltbl[2] = { 0x5555, 0xaaaa };
    static const WORD verttbl[4] = { 0x5555, 0xaaaa, 0xaaaa, 0x5555 };
    WORD    *linexy, i;
    WORD    st;

    for (i = 1; i < ptscount; i++)
    {
        if (*ppoints == *(ppoints + 2))
        {
            st = verttbl[((*ppoints & 1) | ((*(ppoints+1) & 1) << 1))];
        }
        else
        {
            linexy = (*ppoints < *(ppoints+2)) ? ppoints : ppoints + 2;
            st = hztltbl[*(linexy+1) & 1];
        }
        vsl_udsty(st);
        v_pline(2, ppoints);
        ppoints += 2;
    }
    vsl_udsty(0xffff);
}


/*
 *  Routine to draw a certain number of points in a polyline relative
 *  to a given x,y offset
 */
void gsx_pline(WORD offx, WORD offy, WORD cnt, const Point *pts)
{
    WORD    i, j;

    for (i = 0; i < cnt; i++)
    {
        j = i * 2;
        ptsin[j] = offx + pts[i].x;
        ptsin[j+1] = offy + pts[i].y;
    }

    gsx_xline(cnt, ptsin);
}


/*
 *  Routine to draw a clipped polyline, hiding the mouse as you do it
 */
void gsx_cline(UWORD x1, UWORD y1, UWORD x2, UWORD y2)
{
    WORD pxy[4] = { x1, y1, x2, y2 };

    gsx_moff();
    v_pline(2, pxy);
    gsx_mon();
}


/*
 *  Routine to set the text, writing mode, and color attributes
 */
void gsx_attr(UWORD text, UWORD mode, UWORD color)
{
    WORD    tmp;

    tmp = intin[0];
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = gl_handle;
    if (mode != gl_mode)
    {
        contrl[0] = SET_WRITING_MODE;
        intin[0] = gl_mode = mode;
        gsx2();
    }

    contrl[0] = FALSE;
    if (text)
    {
        if (color != gl_tcolor)
        {
            contrl[0] = SET_TEXT_COLOR;
            gl_tcolor = color;
        }
    }
    else
    {
        if (color != gl_lcolor)
        {
            contrl[0] = SET_LINE_COLOR;
            gl_lcolor = color;
        }
    }

    if (contrl[0])
    {
        intin[0] = color;
        gsx2();
    }
    intin[0] = tmp;
}


/*
 *  Routine to set up the points for drawing a box
 */
static void gsx_bxpts(GRECT *pt)
{
    ptsin[0] = pt->g_x;
    ptsin[1] = pt->g_y;
    ptsin[2] = pt->g_x + pt->g_w - 1;
    ptsin[3] = pt->g_y;
    ptsin[4] = pt->g_x + pt->g_w - 1;
    ptsin[5] = pt->g_y + pt->g_h - 1;
    ptsin[6] = pt->g_x;
    ptsin[7] = pt->g_y + pt->g_h - 1;
    ptsin[8] = pt->g_x;
    ptsin[9] = pt->g_y;
}


/*
 *  Routine to draw a box using the current attributes
 */
static void gsx_box(GRECT *pt)
{
    gsx_bxpts(pt);
    v_pline(5, ptsin);
}


/*
 *  Routine to draw a box that will look right on a dithered surface
 */
void gsx_xbox(GRECT *pt)
{
    gsx_bxpts(pt);
    gsx_xline(5, ptsin);
}


/*
 *  Routine to draw a portion of the corners of a box that will look
 *  right on a dithered surface
 */
void gsx_xcbox(GRECT *pt)
{
    WORD    wa, ha;

    wa = 2 * gl_wbox;
    ha = 2 * gl_hbox;
    ptsin[0] = pt->g_x;
    ptsin[1] = pt->g_y + ha;
    ptsin[2] = pt->g_x;
    ptsin[3] = pt->g_y;
    ptsin[4] = pt->g_x + wa;
    ptsin[5] = pt->g_y;
    gsx_xline(3, ptsin);

    ptsin[0] = pt->g_x + pt->g_w - wa;
    ptsin[1] = pt->g_y;
    ptsin[2] = pt->g_x + pt->g_w - 1;
    ptsin[3] = pt->g_y;
    ptsin[4] = pt->g_x + pt->g_w - 1;
    ptsin[5] = pt->g_y + ha;
    gsx_xline(3, ptsin);

    ptsin[0] = pt->g_x + pt->g_w - 1;
    ptsin[1] = pt->g_y + pt->g_h - ha;
    ptsin[2] = pt->g_x + pt->g_w - 1;
    ptsin[3] = pt->g_y + pt->g_h - 1;
    ptsin[4] = pt->g_x + pt->g_w - wa;
    ptsin[5] = pt->g_y + pt->g_h - 1;
    gsx_xline(3, ptsin);

    ptsin[0] = pt->g_x + wa;
    ptsin[1] = pt->g_y + pt->g_h - 1;
    ptsin[2] = pt->g_x;
    ptsin[3] = pt->g_y + pt->g_h - 1;
    ptsin[4] = pt->g_x;
    ptsin[5] = pt->g_y + pt->g_h - ha;
    gsx_xline(3, ptsin);
}


/*
 *  Routine to blit to a screen area
 */
void gsx_blt(void *saddr, WORD sx, WORD sy,
             WORD dx, WORD dy, WORD w, WORD h,
             WORD rule, WORD fgcolor, WORD bgcolor)
{
    WORD pxyarray[8];

    gsx_fix(&gl_src, (void *)saddr, w/8, h);
    gsx_fix_screen(&gl_dst);

    gsx_moff();
    pxyarray[0] = sx;
    pxyarray[1] = sy;
    pxyarray[2] = sx + w - 1;
    pxyarray[3] = sy + h - 1;
    pxyarray[4] = dx;
    pxyarray[5] = dy;
    pxyarray[6] = dx + w - 1;
    pxyarray[7] = dy + h - 1 ;
    if (fgcolor == -1)
        vro_cpyfm(rule, pxyarray, &gl_src, &gl_dst);
    else
        vrt_cpyfm(rule, pxyarray, &gl_src, &gl_dst, fgcolor, bgcolor);
    gsx_mon();
}


/*
 *  Routine to blit around something on the screen
 */
void bb_screen(WORD scsx, WORD scsy, WORD scdx, WORD scdy,
               WORD scw, WORD sch)
{
    gsx_blt(NULL, scsx, scsy,
            scdx, scdy,
            scw, sch, S_ONLY, -1, -1);
}


/*
 *  Routine to transform a mono standard form to device specific form, in place
 */
void gsx_trans(void *addr, UWORD wb, UWORD h)
{
    gsx_fix(&gl_src, addr, wb, h);
    gl_src.fd_stand = TRUE;

    gsx_fix(&gl_dst, addr, wb, h);
    vrn_trnfm(&gl_src, &gl_dst);
}


/*
 *  Routine to initialize all the global variables dealing with
 *  a particular workstation open
 */
void gsx_start(void)
{
    WORD dummy;

    /* reset variables to force initial VDI calls */
    gl_mode = gl_tcolor = gl_lcolor = -1;
    gl_fis = gl_patt = gl_font = -1;

    gl_clip.g_x = 0;
    gl_clip.g_y = 0;
    gl_width = gl_clip.g_w = gl_ws.ws_xres + 1;
    gl_height = gl_clip.g_h = gl_ws.ws_yres + 1;
    gl_nplanes = gsx_nplanes();

    KINFO(("VDI video mode = %dx%d %d-%s\n", gl_width, gl_height, gl_nplanes,
        (gl_nplanes < 16 ? "plane" : "bit")));

    /*
     * replacement VDIs (e.g. NVDI) may support more than two font sizes.
     * we use the current font height for the big (IBM) desktop font,
     * and the minimum font height for the small (SMALL) desktop font.
     */
    gsx_textsize(&gl_wptschar, &gl_hptschar, &gl_wchar, &gl_hchar);
    gl_ws.ws_chmaxh = gl_hptschar;  /* save the IBM font size here */
    vst_height(gl_ws.ws_chminh, &gl_wsptschar, &gl_hsptschar, &gl_wschar, &gl_hschar);

    /* must restore the current font height */
    vst_height(gl_ws.ws_chmaxh, &dummy, &dummy, &dummy, &dummy);

    gl_hbox = gl_hchar + 3;
    gl_wbox = (gl_hbox * gl_ws.ws_hpixel) / gl_ws.ws_wpixel;
    if (gl_wbox < gl_wchar + 4)
        gl_wbox = gl_wchar + 4;

    KDEBUG(("gsx_start(): gl_wchar=%d, gl_hchar=%d, gl_wbox=%d, gl_hbox=%d\n",
            gl_wchar, gl_hchar, gl_wbox, gl_hbox));

    vsl_type(7);
    vsl_width(1);
    vsl_udsty(0xffff);
    r_set(&gl_rscreen, 0, 0, gl_width, gl_height);
    r_set(&gl_rfull, 0, gl_hbox, gl_width, (gl_height - gl_hbox));
    r_set(&gl_rzero, 0, 0, 0, 0);
    r_set(&gl_rcenter, (gl_width-gl_wbox)/2, (gl_height-(2*gl_hbox))/2, gl_wbox, gl_hbox);
    r_set(&gl_rmenu, 0, 0, gl_width, gl_hbox);
}


/*
 *  Routine to do a filled bit blit (a rectangle)
 */
void bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh)
{
    gsx_attr(TRUE, mode, gl_tcolor);
    if (fis != gl_fis)
    {
        vsf_interior(fis);
        gl_fis = fis;
    }
    if (patt != gl_patt)
    {
        vsf_style(patt);
        gl_patt = patt;
    }

    ptsin[0] = hx;
    ptsin[1] = hy;
    ptsin[2] = hx + hw - 1;
    ptsin[3] = hy + hh - 1;
    vr_recfl(ptsin);
}


static UWORD ch_width(WORD fn)
{
    if (fn == IBM)
        return gl_wchar;

    if (fn == SMALL)
        return gl_wschar;

    return 0;
}


static UWORD ch_height(WORD fn)
{
    if (fn == IBM)
        return gl_hchar;

    if (fn == SMALL)
        return gl_hschar;

    return 0;
}


static void gsx_tcalc(WORD font, char *ptext, WORD *ptextw, WORD *ptexth, WORD *pnumchs)
{
    WORD    wc, hc;

    wc = ch_width(font);
    hc = ch_height(font);

    /* figure out the width of the text string in pixels */
    *pnumchs = expand_string(intin, ptext);
    *ptextw = min(*ptextw, *pnumchs * wc);

    /* figure out the height of the text */
    *ptexth = min(*ptexth, hc);
    if (*ptexth / hc)
        *pnumchs = min(*pnumchs, *ptextw / wc);
    else
        *pnumchs = 0;
}


void gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc)
{
    if (tb_f == IBM)
    {
        if (tb_f != gl_font)
        {
            vst_height(gl_ws.ws_chmaxh, &gl_wptschar, &gl_hptschar, &gl_wchar, &gl_hchar);
            gl_font = tb_f;
        }
        y += gl_hptschar;
    }

    if (tb_f == SMALL)
    {
        if (tb_f != gl_font)
        {
            vst_height(gl_ws.ws_chminh, &gl_wsptschar, &gl_hsptschar, &gl_wschar, &gl_hschar);
            gl_font = tb_f;
        }
        y += gl_hsptschar;
    }

    contrl[0] = TEXT;
    contrl[1] = 1;
    contrl[6] = gl_handle;
    ptsin[0] = x;
    ptsin[1] = y;
    contrl[3] = tb_nc;
    gsx2();
}


/*
 *  Routine to convert a rectangle to its inside dimensions
 */
void gr_inside(GRECT *pt, WORD th)
{
    pt->g_x += th;
    pt->g_y += th;
    pt->g_w -= ( 2 * th );
    pt->g_h -= ( 2 * th );
}


/*
 *  Routine to draw a colored, patterned, rectangle
 */
void gr_rect(UWORD icolor, UWORD ipattern, GRECT *pt)
{
    WORD    fis;

    fis = FIS_PATTERN;
    if (ipattern == IP_HOLLOW)
        fis = FIS_HOLLOW;
    else if (ipattern == IP_SOLID)
        fis = FIS_SOLID;

    vsf_color(icolor);
    bb_fill(MD_REPLACE, fis, ipattern, pt->g_x, pt->g_y, pt->g_w, pt->g_h);
}


/*
 *  Routine to adjust the x,y starting point of a text string to
 *  account for its justification.  The number of characters in
 *  the string is also returned.
 */
WORD gr_just(WORD just, WORD font, char *ptext, WORD w, WORD h, GRECT *pt)
{
    WORD    numchs;

    /* figure out the width of the text string in pixels */
    gsx_tcalc(font, ptext, &pt->g_w, &pt->g_h, &numchs);

    h -= pt->g_h;
    if (h > 0)              /* check height */
        pt->g_y += (h + 1) / 2;

    w -= pt->g_w;
    if (w > 0)
    {
        /* do justification */
        if (just == TE_CNTR)
            pt->g_x += (w + 1) / 2;
        else if (just == TE_RIGHT)
            pt->g_x += w;
    }

    return numchs;
}


/*
 *  Routine to draw a string of graphic text
 */
void gr_gtext(WORD just, WORD font, char *ptext, GRECT *pt)
{
    WORD    numchs;
    GRECT   t;

    /* figure out where & how to put out the text */
    rc_copy(pt, &t);
    numchs = gr_just(just, font, ptext, t.g_w, t.g_h, &t);
    if (numchs > 0)
        gsx_tblt(font, t.g_x, t.g_y, numchs);
}


/*
 *  Routine to crack out the border color, text color, inside pattern,
 *  and inside color from a single color information word
 */
void gr_crack(UWORD color, WORD *pbc, WORD *ptc, WORD *pip, WORD *pic, WORD *pmd)
{
    /* 4 bit encoded border color */
    *pbc = (color >> 12) & 0x000f;

    /* 4 bit encoded text color */
    *ptc = (color >> 8) & 0x000f;

    /* 1 bit used to set text writing mode */
    *pmd = (color & 0x80) ? MD_REPLACE : MD_TRANS;

    /* 3 bit encoded pattern */
    *pip = (color >> 4) & 0x0007;

    /* 4 bit encoded inside color */
    *pic = color & 0x000f;
}


static void gr_gblt(WORD *pimage, GRECT *pi, WORD col1, WORD col2)
{
    gsx_blt(pimage, 0, 0, pi->g_x, pi->g_y,
            pi->g_w, pi->g_h, MD_TRANS, col1, col2);
}


#if CONF_WITH_COLOUR_ICONS
/*
 *  Blit a previously-transformed colour icon from memory to screen
 */
static void gr_colourblit(WORD *pdata, GRECT *pi, WORD num_planes)
{
    WORD pxyarray[8];
    WORD mode;

    gsx_fix(&gl_src, pdata, pi->g_w/8, pi->g_h);
    gl_src.fd_nplanes = num_planes;

    gsx_fix_screen(&gl_dst);
    gl_dst.fd_nplanes = num_planes;

    gsx_moff();
    pxyarray[0] = 0;
    pxyarray[1] = 0;
    pxyarray[2] = pi->g_w - 1;
    pxyarray[3] = pi->g_h - 1;
    pxyarray[4] = pi->g_x;
    pxyarray[5] = pi->g_y;
    pxyarray[6] = pi->g_x + pi->g_w - 1;
    pxyarray[7] = pi->g_y + pi->g_h - 1;

    mode = S_OR_D;

#if CONF_WITH_VDI_16BIT
    if (num_planes > 8)     /* Truecolor mode */
        mode = S_AND_D;
#endif

    vro_cpyfm(mode, pxyarray, &gl_src, &gl_dst);
    gsx_mon();
}


/*
 *  Create a dithered copy of the specified mask and return its address
 *
 *  returns NULL if memory can't be allocated
 */
static WORD *dither_mask(WORD *pdata, WORD w, WORD h)
{
    WORD width = w / 16;    /* in WORDs */
    WORD i, j, dither;
    WORD *mask, *p, *q;

    mask = dos_alloc_anyram(width * sizeof(WORD) * h);
    if (!mask)
        return NULL;

    for (i = 0, p = pdata, q = mask; i < h; i++)
    {
        dither = (i & 1) ? 0xaaaa : 0x5555;
        for (j = 0; j < width; j++)
        {
            *q++ = *p++ & dither;
        }
    }

    return mask;
}
#endif


/*
 *  Routine to draw a mono or colour icon
 *
 *  Iff colour icons are enabled, and 'cicon' is not NULL, this will
 *  draw a colour icon
 */
void gr_gicon(WORD state, ICONBLK *ib, CICON *cicon)
{
    WORD    *pdata, *pmask;
    WORD    fgcol, bgcol, tmp;
    WORD    ch;
    GRECT   *pi, *pt;

    ch = ib->ib_char;
    pi = (GRECT *)&ib->ib_xicon;
    pt = (GRECT *)&ib->ib_xtext;

    /* crack the color/char definition word */
    fgcol = (ch >> 12) & 0x000f;
    bgcol = (ch >> 8) & 0x000f;
    ch &= 0x00ff;

#if CONF_WITH_COLOUR_ICONS
    /* set up the data & mask ptrs */
    if (cicon)
    {
        /* use the 'selected' data/mask if available */
        if ((state & SELECTED) && cicon->sel_data)
        {
            pdata = cicon->sel_data;
            pmask = cicon->sel_mask;
        }
        else
        {
            pdata = cicon->col_data;
            pmask = cicon->col_mask;
        }
    }
    else
#endif
    {
        if (state & SELECTED)       /* invert mono icon colours if selected   */
        {
            tmp = fgcol;
            fgcol = bgcol;
            bgcol = tmp;
        }
        pdata = ib->ib_pdata;
        pmask = ib->ib_pmask;
    }

    /* do mask & text background unless WHITEBAK is set & the icon's background is white */
    if ( !((state & WHITEBAK) && (bgcol == WHITE)) )
    {
        /* NOTE: this may need to be modified for 16-bit colour */
        gr_gblt(pmask, pi, bgcol, fgcol);
        /* do not draw background rectangle for icon text if the string is empty */
        if (ib->ib_ptext[0])
        {
            if (cicon && (state & SELECTED))
                gr_rect(fgcol, IP_SOLID, pt);
            else
                gr_rect(bgcol, IP_SOLID, pt);
        }
    }

    /* draw the image */
#if CONF_WITH_COLOUR_ICONS
    if (cicon)
    {
        gr_colourblit(pdata, pi, cicon->num_planes);
        if (state & SELECTED)
        {
            tmp = fgcol;
            fgcol = bgcol;
            bgcol = tmp;
            if (!cicon->sel_data)       /* check if we need to darken */
            {
                WORD *mask = dither_mask(pmask, pi->g_w, pi->g_h);
                if (mask)               /* malloc ok */
                {
                    gr_gblt(mask, pi, BLACK, WHITE);
                    dos_free(mask);
                }
            }
        }
    }
    else
#endif
    {
        gr_gblt(pdata, pi, fgcol, bgcol);
    }

    /* draw the character */
    gsx_attr(TRUE, MD_TRANS, fgcol);
    if (ch)
    {
        intin[0] = ch;
        gsx_tblt(SMALL, ib->ib_xicon+ib->ib_xchar, ib->ib_yicon+ib->ib_ychar, 1);
    }

    /* draw the label */
    gr_gtext(TE_CNTR, SMALL, ib->ib_ptext, pt);
}


/*
 * Routine to draw a box of a certain thickness using the current attribute settings
 */
void gr_box(WORD x, WORD y, WORD w, WORD h, WORD th)
{
    GRECT   t, n;

    r_set(&t, x, y, w, h);
    if (th != 0)
    {
        if (th < 0)
            th--;
        gsx_moff();
        do
        {
            th += (th > 0) ? -1 : 1;
            rc_copy(&t, &n);
            gr_inside(&n, th);
            gsx_box(&n);
        } while (th != 0);
        gsx_mon();
    }
}
