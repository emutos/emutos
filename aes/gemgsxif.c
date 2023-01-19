/*
 * gemgsxif.c - AES's interface to VDI (gsx)
 *
 * Copyright 2002-2023 The EmuTOS development team
 *           1999, Caldera Thin Clients, Inc.
 *           1987, Digital Research Inc.
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*      GEMGSXIF.C      05/06/84 - 06/13/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "string.h"
#include "obdefs.h"
#include "aesext.h"
#include "funcdef.h"

#include "gemdos.h"
#include "geminput.h"
#include "gemdosif.h"
#include "gsx2.h"
#include "geminit.h"
#include "gemctrl.h"
#include "gemgsxif.h"
#include "xbiosbind.h"
#include "has.h"        /* for blitter-related items */
#include "biosdefs.h"   /* for FALCON_REZ */
#include "biosext.h"
#include "asm.h"

#include "cookie.h"

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
#define MAX_ALERT_CHARS (MAX_ALERT_WIDTH*MAX_ALERT_HEIGHT)

/*
 * the following specify the maximum allowed dimensions of a drop-down
 * menu, in characters.  these are somewhat arbitrary, but were large
 * enough for EmuDesk itself at the time this was originally committed.
 * they were subsequently increased to handle the menus in the French
 * version of AtariWorks.
 */
#define MAX_MENU_WIDTH  32
#define MAX_MENU_HEIGHT 20
#define MAX_MENU_CHARS  (MAX_MENU_WIDTH*MAX_MENU_HEIGHT)

/*
 * the menu/alert buffer size, in characters
 */
#if (MAX_MENU_CHARS > MAX_ALERT_CHARS)
# define MENU_BUFFER_SIZE   MAX_MENU_CHARS
#else
# define MENU_BUFFER_SIZE   MAX_ALERT_CHARS
#endif

GLOBAL WORD  intout[45];
GLOBAL WORD  ptsout[12];
GLOBAL WORD  gl_moff;                /* counting semaphore   */
                                     /*  == 0 implies ON     */
                                     /*  >  0 implies OFF    */

static FDB   gl_tmp;
static PFVOID old_mcode;
static PFVOID old_bcode;
static LONG  gl_mlen;
static BOOL  gl_graphic;


/* Some local Prototypes: */
static void v_opnwk(WORD *pwork_in, WORD *phandle, WS *pwork_out);


/*
 * this function calculates the size of the menu/alert screen buffer.
 * because of the width of some translated menus, we can't use the
 * original Atari TOS method (1/4 of the screen size).
 *
 * note that gl_tmp, gl_mlen are initialised as a side effect
 */
static ULONG gsx_mcalc(void)
{
    gsx_fix_screen(&gl_tmp);            /* store screen info    */
    gl_mlen = (LONG)MENU_BUFFER_SIZE * gl_wchar * gl_hchar * gl_nplanes / 8;

    return(gl_mlen);
}



void gsx_malloc(void)
{
    ULONG   mlen;

    mlen = gsx_mcalc();                     /* need side effects now     */
    gl_tmp.fd_addr = dos_alloc_anyram(mlen);
}



void gsx_mfree(void)
{
    dos_free(gl_tmp.fd_addr);
}



void gsx_mret(LONG *pmaddr, LONG *pmlen)
{
     *pmaddr = (LONG)gl_tmp.fd_addr;
     *pmlen = gl_mlen;
}



static void gsx_ncode(WORD code, WORD n, WORD m)
{
    contrl[0] = code;
    contrl[1] = n;
    contrl[3] = m;
    contrl[6] = gl_handle;
    gsx2();
}



void gsx_0code(WORD code)
{
    gsx_ncode(code, 0, 0);
}



void gsx_1code(WORD code, WORD value)
{
    intin[0] = value;
    gsx_ncode(code, 0, 1);
}



static WORD screen_rez(void)
{
    if (HAS_VIDEL)
        return FALCON_REZ;

    return Getrez();
}



static void gsx_wsopen(void)
{
    WORD  i, *p = intin;

    /*
     * set up intin[]: the screen rez (+2) goes into intin[0]
     *
     * but there are (undocumented) complications for the Falcon:
     * we need to use the special pseudo-screen rez, and put the actual
     * videl mode into intout[45] (!).
     */
    *p++ = screen_rez() + 2;
    for (i = 1; i < 10; i++)
        *p++ = 1;
    *p = 2;                         /* device coordinate space */
    gl_ws.ws_pts0 = VsetMode(-1);   /* will be intout[45] ... harmless if not a Falcon */
    v_opnwk(intin, &gl_handle, &gl_ws);
    gl_graphic = TRUE;
}



void gsx_wsclose(void)
{
    gsx_0code(CLOSE_WORKSTATION);
}



void gsx_wsclear(void)
{
    gsx_0code(CLEAR_WORKSTATION);
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



static void gsx_setmb(PFVOID boff, PFVOID moff, PFVOID *pdrwaddr)
{
    i_ptr( boff );
    gsx_0code(BUT_VECX);
    m_lptr2( &old_bcode );

    i_ptr( moff );
    gsx_0code(MOT_VECX);
    m_lptr2( &old_mcode );

/* not used in Atari GEM:
    i_ptr( just_rts );
    gsx_ncode(CUR_VECX, 0, 0);
    m_lptr2( pdrwaddr );
*/
}



static void gsx_resetmb(void)
{
    i_ptr( (void*)old_bcode );
    gsx_0code(BUT_VECX);

    i_ptr( (void*)old_mcode );
    gsx_0code(MOT_VECX);

/* not used in Atari GEM:
    i_ptr( (void*)drwaddr );
    gsx_ncode(CUR_VECX, 0, 0);
*/
}

#if CONF_WITH_EXTENDED_MOUSE

/*
 * determine if trap #2 has been intercepted by someone else (e.g. NVDI)
 *
 * return 1 if intercepted, else 0
 */
static BOOL aestrap_intercepted(void)
{
    void *trap2_handler = (void *)ULONG_AT(0x88);
    return (!is_text_pointer(trap2_handler)) || nvdi_cookie_present();
}

#endif /* CONF_WITH_EXTENDED_MOUSE */

void gsx_init(void)
{
    gsx_wsopen();
    gsx_start();
    gsx_setmb(far_bcha, far_mcha, &drwaddr);
    gsx_0code(MOUSE_STATE);
    xrat = ptsout[0];
    yrat = ptsout[1];

#if CONF_WITH_EXTENDED_MOUSE
    /*
     * if NVDI3 has been installed, it will see the following VDI call.
     * since it doesn't understand vex_wheelv() (which is a Milan extension),
     * it will attempt to issue a warning form_alert().  this leads to a
     * crash due to a stack smash: the USP and SSP are both on the AES
     * process 0 stack at this time, and e.g. a VBL interrupt will save
     * registers which can overwrite dynamic variables.
     *
     * we circumvent this by avoiding calling vex_wheelv() if the trap #2
     * interrupt has been intercepted.
     */
    if (!aestrap_intercepted())
    {
        PFVOID old_wheelv; /* Ignored */
        vex_wheelv(aes_wheel, &old_wheelv);
    }
#endif
}



void gsx_graphic(BOOL tographic)
{
    if (gl_graphic != tographic)
    {
        gl_graphic = tographic;
        if (gl_graphic)
        {
            contrl[5] = 2;
            gsx_0code(ESCAPE_FUNCTION);
            gsx_setmb(far_bcha, far_mcha, &drwaddr);
        }
        else
        {
            contrl[5] = 3;
            gsx_0code(ESCAPE_FUNCTION);
            gsx_resetmb();
        }
    }
}



static void bb_set(BOOL save, GRECT *r)
{
    FDB *psrc, *pdst;
    WORD pxyarray[8], *pts1, *pts2;
    WORD sx, sy, sw, sh, oldsx;
    LONG            size;

    /* extract x/y/w/h from GRECT */
    r_get(r, &sx, &sy, &sw, &sh);

    /* get on word boundary */
    oldsx = sx;
    sx = (sx / 16) * 16;
    sw = ( ((oldsx - sx) + (sw + 15)) / 16 ) * 16;
    size = memsize(sw/16,sh,gl_tmp.fd_nplanes);

    if (size > gl_mlen) {       /* buffer too small */
        /* adjust height to fit buffer: this will leave droppings! */
        sh = (ULONG)gl_mlen * sh / size;

        /* issue warning message for backup only, not for subsequent restore */
        if (save)
            KINFO(("Menu/alert buffer too small: need at least %ld bytes\n",size));
    }

    gsx_fix_screen(&gl_src);
    /* gl_tmp.fd_addr was set by gsx_malloc() */
    gl_tmp.fd_stand = TRUE;
    gl_tmp.fd_wdwidth = sw / 16;
    gl_tmp.fd_w = sw;
    gl_tmp.fd_h = sh;

    if (save)
    {
        psrc = &gl_src;
        pdst = &gl_tmp;
        pts1 = pxyarray;
        pts2 = pxyarray + 4;
    }
    else
    {
        psrc = &gl_tmp;         /* invert FDBs & coordinates */
        pdst = &gl_src;
        pts1 = pxyarray + 4;
        pts2 = pxyarray;
    }

    gsx_moff();
    pts1[0] = sx;
    pts1[1] = sy;
    pts1[2] = sx + sw - 1;
    pts1[3] = sy + sh - 1;
    pts2[0] = 0;
    pts2[1] = 0;
    pts2[2] = sw - 1;
    pts2[3] = sh - 1 ;

    vro_cpyfm(S_ONLY, pxyarray, psrc, pdst);
    gsx_mon();
}



void bb_save(GRECT *ps)
{
    bb_set(TRUE, ps);
}



void bb_restore(GRECT *pr)
{
    bb_set(FALSE, pr);
}



WORD gsx_tick(void *tcode, void *ptsave)
{
    i_ptr( tcode );
    gsx_0code(TIM_VECX);
    m_lptr2( ptsave );
    return(intout[0]);
}



void gsx_mfset(const MFORM *pmfnew)
{
    gsx_moff();
    if (!gl_ctmown)
    {
#if CONF_WITH_GRAF_MOUSE_EXTENSION
        gl_prevmouse = gl_mouse;
#endif
        gl_mouse = *pmfnew;
    }
    memcpy(intin, (void *)pmfnew, sizeof(MFORM));
    gsx_ncode(SET_CUR_FORM, 0, sizeof(MFORM)/sizeof(WORD));
    gsx_mon();
}



void gsx_mxmy(WORD *pmx, WORD *pmy)
{
    *pmx = xrat;
    *pmy = yrat;
}



WORD gsx_kstate(void)
{
    gsx_0code(KEY_STATE);
    return(intout[0]);
}



void gsx_moff(void)
{
    if (!gl_moff)
        gsx_0code(HIDE_CUR);

    gl_moff++;
}



void gsx_mon(void)
{
    gl_moff--;
    if (!gl_moff)
        gsx_1code(SHOW_CUR, 1);
}



WORD gsx_char(void)
{
    intin[0] = 4;
    intin[1] = 2;
    gsx_ncode(SET_INPUT_MODE, 0, 2);

    intin[0] = -1;
    intin[1] = FALSE;        /* no echo */
    gsx_ncode(STRING_INPUT, FALSE, 2);
    if (contrl[4])
        return(intout[0]);
    else
        return(0);
}



/*
 * Set the VDI's mouse cursor coordinates
 *
 * This is used by the appl_tplay() code in AES to set the VDI mouse
 * coordinates to the same position that the playback code has set
 * the mouse.  This avoids sudden 'jumps' in mouse position at the end
 * of playback, or when the desktop restarts.
 */
void gsx_setmousexy(WORD x, WORD y)
{
    intin[0] = 1;
    intin[1] = 2;
    gsx_ncode(SET_INPUT_MODE, 0, 2);    /* sample mode */
    ptsin[0] = x;
    ptsin[1] = y;
    gsx_ncode(LOCATOR_INPUT, 1, 0);     /* vsm_locator() */
}



/* Get the number of planes (or bit depth) of the current screen */
WORD gsx_nplanes(void)
{
    gsx_1code(EXTENDED_INQUIRE, 1);
    return intout[4];
}


/* Get text size info */
void gsx_textsize(WORD *charw, WORD *charh, WORD *cellw, WORD *cellh)
{
    gsx_0code(INQ_TEXT_ATTRIBUTES);
    *charw = ptsout[0];
    *charh = ptsout[1];
    *cellw = ptsout[2];
    *cellh = ptsout[3];
}


/*
 *  Routine to set the FDB to correspond to the current screen
 */
void gsx_fix_screen(FDB *pfd)
{
    pfd->fd_addr = NULL;
    pfd->fd_w = gl_ws.ws_xres + 1;
    pfd->fd_h = gl_ws.ws_yres + 1;
    pfd->fd_wdwidth = pfd->fd_w / 16;
    pfd->fd_stand = FALSE;
    pfd->fd_nplanes = gl_nplanes;
}


/*
 *  Routine to fix up the MFDB of a particular raster form
 */
void gsx_fix(FDB *pfd, void *theaddr, WORD wb, WORD h)
{
    if (theaddr == NULL)
    {
        gsx_fix_screen(pfd);
        return;
    }

    pfd->fd_addr = theaddr;
    pfd->fd_w = wb * 8;
    pfd->fd_h = h;
    pfd->fd_wdwidth = wb / 2;
    pfd->fd_stand = FALSE;
    pfd->fd_nplanes = 1;
}


/*
 *  Routine to issue v_opnwk()
 */
static void v_opnwk(WORD *pwork_in, WORD *phandle, WS *pwork_out)
{
    WORD            *ptsptr;

    ptsptr = ((WORD *)pwork_out) + 45;
    i_ptsout( ptsptr );     /* set ptsout to work_out array */
    i_intin( pwork_in );    /* set intin to point to callers data  */
    i_intout( (WORD *)pwork_out ); /* set intout to point to callers data */
    gsx_ncode(OPEN_WORKSTATION, 0, 11);

    *phandle = contrl[6];
    i_intin( intin );
    i_intout( intout );
    i_ptsin( ptsin );
    i_ptsout( ptsout );
}


/*
 *  Routine to issue v_pline()
 */
void v_pline(WORD count, WORD *pxyarray)
{
    i_ptsin( pxyarray );
    gsx_ncode(POLYLINE, count, 0);
    i_ptsin( ptsin );
}


void vs_clip(WORD clip_flag, WORD *pxyarray )
{
    i_ptsin( pxyarray );
    intin[0] = clip_flag;
    gsx_ncode(TEXT_CLIP, 2, 1);
    i_ptsin(ptsin);
}


void vst_height(WORD height, WORD *pchr_width, WORD *pchr_height,
                WORD *pcell_width, WORD *pcell_height)
{
    ptsin[0] = 0;
    ptsin[1] = height;
    gsx_ncode(SET_CHAR_HEIGHT, 1, 0);
    *pchr_width = ptsout[0];
    *pchr_height = ptsout[1];
    *pcell_width = ptsout[2];
    *pcell_height = ptsout[3];
}



void vr_recfl(WORD *pxyarray)
{
    i_ptsin( pxyarray );
    gsx_ncode(FILL_RECTANGLE, 2, 0);
    i_ptsin( ptsin );
}



void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB )
{
    intin[0] = wr_mode;
    i_ptr( psrcMFDB );
    i_ptr2( pdesMFDB );
    i_ptsin( pxyarray );
    gsx_ncode(COPY_RASTER_OPAQUE, 4, 1);
    i_ptsin( ptsin );
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
    gsx_ncode(COPY_RASTER_TRANS, 4, 3);
    i_ptsin( ptsin );
}



void vrn_trnfm(FDB *psrcMFDB, FDB *pdesMFDB)
{
    i_ptr( psrcMFDB );
    i_ptr2( pdesMFDB );

    gsx_0code(TRANSFORM_FORM);
}



/*
 *  Routine to call vsl_width()
 */
void vsl_width(WORD width)
{
    ptsin[0] = width;
    ptsin[1] = 0;
    gsx_ncode(SET_LINE_WIDTH, 1, 0);
}



#if CONF_WITH_EXTENDED_MOUSE
void vex_wheelv(PFVOID new, PFVOID *old)
{
    i_ptr(new);
    gsx_0code(WHEEL_VECX);
    m_lptr2(old);
}
#endif
