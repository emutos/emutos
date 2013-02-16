/*
 * gemgsxif.c - AES's interface to VDI (gsx)
 *
 * Copyright 2002-2011 The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*      GEMGSXIF.C      05/06/84 - 06/13/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */



#include "config.h"
#include "portab.h"
#include "compat.h"
#include "dos.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "funcdef.h"

#include "gemdos.h"
#include "gemrslib.h"
#include "gemgraf.h"
#include "geminput.h"
#include "gemdosif.h"
#include "gsx2.h"
#include "geminit.h"
#include "gemctrl.h"
#include "gemgsxif.h"
#include "kprint.h"

/*
 * Calls used in Crystal:
 *
 * g_vsf_interior();
 * vr_recfl();
 * vst_height();
 * g_vsl_type();
 * g_vsl_udsty();
 * g_vsl_width();
 * g_v_pline();
 * vst_clip();
 * vex_butv();
 * vex_motv();
 * vex_curv();
 * vex_timv();
 * vr_cpyfm();
 * g_v_opnwk();
 * v_clswk();
 * vq_extnd();
 * v_clsvwk( handle )
 * v_opnvwk( pwork_in, phandle, pwork_out )
 */

#define GRAFMEM         0xFFFFFFFFl

/*
 * calculate memory size to buffer a display area, given its
 * width (in words), height (in pixels), and the number of planes
 */
#define memsize(wdwidth,h,nplanes)  ((LONG)(wdwidth)*(h)*(nplanes)*2)

/*
 * the following specify the maximum dimensions of a form_alert, in
 * characters, derived by trial.  note that the actual maximum height
 * is 78 pixels (for 8-pixel chars) and 150 pixels (for 16-pixel chars),
 * so these are conservative numbers.
 */
#define MAX_ALERT_WIDTH  50     /* includes worst-case screen alignment */
#define MAX_ALERT_HEIGHT 10

GLOBAL WORD  intout[45];
GLOBAL WORD  ptsout[12];
GLOBAL WORD  gl_moff;                /* counting semaphore   */
                                     /*  == 0 implies ON     */
                                     /*  >  0 implies OFF    */

static FDB   gl_tmp;
static LONG  old_mcode;
static LONG  old_bcode;
static LONG  gl_mlen;
static WORD  gl_graphic;



/* Some local Prototypes: */
void  g_v_opnwk(WORD *pwork_in, WORD *phandle, WS *pwork_out );
ULONG  gsx_gbufflen(void);

static LONG form_alert_bufsize(void)
{
    int w = MAX_ALERT_WIDTH * gl_wchar;
    int h = MAX_ALERT_HEIGHT * gl_hchar;

    if (w > gl_width)       /* e.g. max size form alert in ST low */
        w = gl_width;

    return (LONG)h * w * gl_nplanes / 8;
}

/* this function calculates the size of the menu/alert screen buffer.
 * as in older versions of Atari TOS, it's a minimum of one-quarter
 * of the screen size.  this is increased if necessary to allow the
 * for the maximum-sized form alert.
 */
ULONG gsx_mcalc()
{
    LONG mem;

    gsx_fix(&gl_tmp, 0x0L, 0, 0);           /* store screen info    */
    gl_mlen = gsx_gbufflen();
    if (gl_mlen != 0x0l)
        gl_tmp.fd_addr = GRAFMEM;             /* buffer not in sys mem */
    else
        gl_mlen = memsize(gl_tmp.fd_wdwidth,gl_tmp.fd_h,gl_tmp.fd_nplanes) / 4;

    mem = form_alert_bufsize();
    if (gl_mlen < mem)
        gl_mlen = mem;

    return(gl_mlen);
}



void gsx_malloc()
{
    ULONG   mlen;

    mlen = gsx_mcalc();                     /* need side effects now     */
    if (gl_tmp.fd_addr != GRAFMEM)          /* buffer on graphics board? */
        gl_tmp.fd_addr = dos_alloc(mlen);     /*  no -- get from sys mem   */
}



void gsx_mfree()
{
    dos_free(gl_tmp.fd_addr);
}



void gsx_mret(LONG *pmaddr, LONG *pmlen)
{
    if (gl_tmp.fd_addr == GRAFMEM)
    {
        *pmaddr = 0x0l;
        *pmlen = 0x0l;
    }
    else
    {
        *pmaddr = gl_tmp.fd_addr;
        *pmlen = gl_mlen;
    }
}



void gsx_ncode(WORD code, WORD n, WORD m)
{
    contrl[0] = code;
    contrl[1] = n;
    contrl[3] = m;
    contrl[6] = gl_handle;
    gsx2();
}



void gsx_1code(WORD code, WORD value)
{
    intin[0] = value;
    gsx_ncode(code, 0, 1);
}



static void gsx_wsopen(void)
{
    WORD  i;

    for(i=0; i<10; i++)
        intin[i] = 1;
    intin[10] = 2;                  /* device coordinate space */
    g_v_opnwk( &intin[0], &gl_handle, &gl_ws );
    gl_graphic = TRUE;
}



void gsx_wsclose(void)
{
    gsx_ncode(CLOSE_WORKSTATION, 0, 0);
}



void gsx_wsclear(void)
{
    gsx_ncode(CLEAR_WORKSTATION, 0, 0);
}



void ratinit(void)
{
    gsx_1code(SHOW_CUR, 0);
    gl_moff = 0;
}


void ratexit(void)
{
    gsx_moff();
}


static void gsx_setmb(void *boff, void *moff, LONG *pdrwaddr)
{
    i_lptr1( boff );
    gsx_ncode(BUT_VECX, 0, 0);
    m_lptr2( &old_bcode );

    i_lptr1( moff );
    gsx_ncode(MOT_VECX, 0, 0);
    m_lptr2( &old_mcode );

/* not used in Atari GEM:
    i_lptr1( justretf );
    gsx_ncode(CUR_VECX, 0, 0);
    m_lptr2( pdrwaddr );
*/
}



static void gsx_resetmb(void)
{
    i_lptr1( (void*)old_bcode );
    gsx_ncode(BUT_VECX, 0, 0);

    i_lptr1( (void*)old_mcode );
    gsx_ncode(MOT_VECX, 0, 0);

/* not used in Atari GEM:
    i_lptr1( (void*)drwaddr );
    gsx_ncode(CUR_VECX, 0, 0);
*/
}



void gsx_init(void)
{
    void* old_wheelv; /* Ignored */

    gsx_wsopen();
    gsx_start();
    gsx_setmb(far_bcha, far_mcha, &drwaddr);
    gsx_ncode(MOUSE_ST, 0, 0);
    xrat = ptsout[0];
    yrat = ptsout[1];

    vex_wheelv(aes_wheel, &old_wheelv);
}



void gsx_graphic(WORD tographic)
{
    if (gl_graphic != tographic)
    {
        gl_graphic = tographic;
        if (gl_graphic)
        {
            contrl[5] = 2;
            gsx_ncode(5, 0, 0);
            gsx_setmb(far_bcha, far_mcha, &drwaddr);
        }
        else
        {
            contrl[5] = 3;
            gsx_ncode(5, 0, 0);
            gsx_resetmb();
        }
    }
}



static void bb_set(WORD sx, WORD sy, WORD sw, WORD sh, WORD *pts1, WORD *pts2,
                   FDB *pfd, FDB *psrc, FDB *pdst)
{
    WORD            oldsx;
    LONG            size;

    /* get on word boundary */
    oldsx = sx;
    sx = (sx / 16) * 16;
    sw = ( ((oldsx - sx) + (sw + 15)) / 16 ) * 16;
    size = memsize(sw/16,sh,gl_tmp.fd_nplanes);

    if (size > gl_mlen) {       /* buffer too small */
        /* adjust height to fit buffer: this will leave droppings! */
        sh = gl_mlen * sh / size;
        /* issue warning message for backup only, not for subsequent restore */
        if (pdst == &gl_tmp)
            kprintf("Menu/alert buffer too small: need at least %ld bytes\n",size);
    }

    gl_tmp.fd_stand = TRUE;
    gl_tmp.fd_wdwidth = sw / 16;
    gl_tmp.fd_w = sw;
    gl_tmp.fd_h = sh;

    gsx_moff();
    pts1[0] = sx;
    pts1[1] = sy;
    pts1[2] = sx + sw - 1;
    pts1[3] = sy + sh - 1;
    pts2[0] = 0;
    pts2[1] = 0;
    pts2[2] = sw - 1;
    pts2[3] = sh - 1 ;

    gsx_fix(pfd, 0L, 0, 0);
    vro_cpyfm( S_ONLY, &ptsin[0], psrc, pdst );
    gsx_mon();
}



void bb_save(GRECT *ps)
{
    bb_set(ps->g_x, ps->g_y, ps->g_w, ps->g_h, &ptsin[0], &ptsin[4],
           &gl_src, &gl_src, &gl_tmp);
}



void bb_restore(GRECT *pr)
{
    bb_set(pr->g_x, pr->g_y, pr->g_w, pr->g_h, &ptsin[4], &ptsin[0],
           &gl_dst, &gl_tmp, &gl_dst);
}



WORD gsx_tick(void *tcode, void *ptsave)
{
    i_lptr1( tcode );
    gsx_ncode(TIM_VECX, 0, 0);
    m_lptr2( ptsave );
    return(intout[0]);
}



void gsx_mfset(LONG pmfnew)
{
    gsx_moff();
    if (!gl_ctmown)
        LWCOPY(&gl_mouse[0], pmfnew, 37);
    LWCOPY(ad_intin, pmfnew, 37);
    gsx_ncode(ST_CUR_FORM, 0, 37);
    gsx_mon();
}



void gsx_mxmy(WORD *pmx, WORD *pmy)
{
    *pmx = xrat;
    *pmy = yrat;
}



WORD gsx_kstate()
{
    gsx_ncode(KEY_SHST, 0, 0);
    return(intout[0]);
}



void gsx_moff()
{
    if (!gl_moff)
        gsx_ncode(HIDE_CUR, 0, 0);

    gl_moff++;
}



void gsx_mon()
{
    gl_moff--;
    if (!gl_moff)
        gsx_1code(SHOW_CUR, 1);
}



WORD gsx_char()
{
    intin[0] = 4;
    intin[1] = 2;
    gsx_ncode(33, 0, 2);

    intin[0] = -1;
    intin[1] = FALSE;        /* no echo */
    gsx_ncode(31, FALSE, 2);
    if (contrl[4])
        return(intout[0]);
    else
        return(0);
}



/* this function seems bogus: EXTENDED_INQUIRE causes vq_extnd() to be
 * called, which copies INQ_TAB[] to intout[].  I have not been able to
 * find anywhere in EmuTOS where INQ_TAB[26] is set to a non-zero value,
 * so it seems that intout[26] will always contain zeros, and therefore
 * that this function will always return zero - Roger
 */
ULONG gsx_gbufflen()
{
    gsx_1code(EXTENDED_INQUIRE, 1);
    return(LLGET(ADDR(&intout[26])));
}



/* This function was formerly just called v_opnwk, but there was a
   conflict with the VDI then, so I renamed it to g_v_opnwk  - Thomas */
void g_v_opnwk(WORD *pwork_in, WORD *phandle, WS *pwork_out )
{
    WORD            *ptsptr;

    ptsptr = ((WORD *)pwork_out) + 45;
    i_ptsout( ptsptr );     /* set ptsout to work_out array */
    i_intin( pwork_in );    /* set intin to point to callers data  */
    i_intout( pwork_out );  /* set intout to point to callers data */
    gsx_ncode(OPEN_WORKSTATION, 0, 11);

    *phandle = contrl[6];
    i_intin( &intin );
    i_intout( &intout );
    i_ptsin( &ptsin );
    i_ptsout( &ptsout );
}



/* This function was formerly just called v_pline, but there was a
 conflict with the VDI then, so I renamed it to g_v_pline  - Thomas */
void g_v_pline(WORD  count, WORD *pxyarray )
{
    i_ptsin( pxyarray );
    gsx_ncode(POLYLINE, count, 0);
    i_ptsin( &ptsin );
}



void vst_clip(WORD clip_flag, WORD *pxyarray )
{
    WORD            value;

    value = ( clip_flag != 0 ) ? 2 : 0;
    i_ptsin( pxyarray );
    intin[0] = clip_flag;
    gsx_ncode(TEXT_CLIP, value, 1);
    i_ptsin(&ptsin);
}


void vst_height(WORD height, WORD *pchr_width, WORD *pchr_height,
                WORD *pcell_width, WORD *pcell_height)
{
    ptsin[0] = 0;
    ptsin[1] = height;
    gsx_ncode(CHAR_HEIGHT, 1, 0);
    *pchr_width = ptsout[0];
    *pchr_height = ptsout[1];
    *pcell_width = ptsout[2];
    *pcell_height = ptsout[3];
}



void vr_recfl(WORD *pxyarray, FDB *pdesMFDB)
{
    i_ptr( pdesMFDB );
    i_ptsin( pxyarray );
    gsx_ncode(FILL_RECTANGLE, 2, 1);
    i_ptsin( &ptsin );
}



void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB )
{
    intin[0] = wr_mode;
    i_ptr( psrcMFDB );
    i_ptr2( pdesMFDB );
    i_ptsin( pxyarray );
    gsx_ncode(COPY_RASTER_FORM, 4, 1);
    i_ptsin( &ptsin );
}



void vrt_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB,
               WORD fgcolor, WORD bgcolor)
{
    intin[0] = wr_mode;
    intin[1] = fgcolor;
    intin[2] = bgcolor;
    i_ptr( psrcMFDB );
    i_ptr2( pdesMFDB );
    i_ptsin( pxyarray );
    gsx_ncode(121, 4, 3);
    i_ptsin( &ptsin );
}



void vrn_trnfm(FDB *psrcMFDB, FDB *pdesMFDB)
{
    i_ptr( psrcMFDB );
    i_ptr2( pdesMFDB );

    gsx_ncode(TRANSFORM_FORM, 0, 0);
}



/*
 * This function was formerly just called vsl_width, but there was a
 * conflict with the VDI then, so I renamed it to g_vsl_width  - Thomas
 */
void g_vsl_width(WORD width)
{
    ptsin[0] = width;
    ptsin[1] = 0;
    gsx_ncode(S_LINE_WIDTH, 1, 0);
}



void vex_wheelv(void *new, void **old)
{
    i_ptr( new );
    gsx_ncode(WHEEL_VECX, 0, 0);
    *old = (void*)LLGET(ADDR(&contrl[9]));
}
