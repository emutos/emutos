/*      GEMGRAF.C       04/11/84 - 09/17/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */ 
/*      fix gr_gicon null text                  11/18/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
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

#include "portab.h"
#include "machine.h"
#include "obdefs.h"
#include "gsxdefs.h"
#include "funcdef.h"

#include "gemgraf.h"
#include "gemgsxif.h"
#include "optimize.h"
#include "optimopt.h"
#include "gsx2.h"
#include "rectfunc.h"

#define ORGADDR 0x0L

                                                /* in GSXBIND.C         */
#define vsf_interior( x )       gsx_1code(S_FILL_STYLE, x)
#define vsl_type( x )           gsx_1code(S_LINE_TYPE, x)
#define vsf_style( x )          gsx_1code(S_FILL_INDEX, x)
#define vsf_color( x )          gsx_1code(S_FILL_COLOR, x)
#define vsl_udsty( x )          gsx_1code(ST_UD_LINE_STYLE, x)


GLOBAL WORD     gl_width;
GLOBAL WORD     gl_height;

GLOBAL WORD     gl_nrows;
GLOBAL WORD     gl_ncols;

GLOBAL WORD     gl_wchar;
GLOBAL WORD     gl_hchar;

GLOBAL WORD     gl_wschar;
GLOBAL WORD     gl_hschar;

GLOBAL WORD     gl_wptschar;
GLOBAL WORD     gl_hptschar;

GLOBAL WORD     gl_wsptschar;
GLOBAL WORD     gl_hsptschar;

GLOBAL WORD     gl_wbox;
GLOBAL WORD     gl_hbox;

GLOBAL WORD     gl_xclip;
GLOBAL WORD     gl_yclip;
GLOBAL WORD     gl_wclip;
GLOBAL WORD     gl_hclip;

GLOBAL WORD     gl_nplanes;
GLOBAL WORD     gl_handle;

GLOBAL FDB      gl_src;
GLOBAL FDB      gl_dst;

GLOBAL WS       gl_ws;
GLOBAL WORD     contrl[12];
GLOBAL WORD     intin[128];
GLOBAL WORD     ptsin[20];

GLOBAL LONG     ad_intin;

GLOBAL WORD     gl_mode;
GLOBAL WORD     gl_tcolor;
GLOBAL WORD     gl_lcolor;
GLOBAL WORD     gl_fis;
GLOBAL WORD     gl_patt;
GLOBAL WORD     gl_font;

GLOBAL GRECT    gl_rscreen;
GLOBAL GRECT    gl_rfull;
GLOBAL GRECT    gl_rzero;
GLOBAL GRECT    gl_rcenter;
GLOBAL GRECT    gl_rmenu;


/* Prototypes: */
void gsx_xline(WORD ptscount, WORD *ppoints );



/*
*       Routine to set the clip rectangle.  If the w,h of the clip
*       is 0, then no clip should be set.  Ohterwise, set the 
*       appropriate clip.
*/
void gsx_sclip(GRECT *pt)
{
        r_get(pt, &gl_xclip, &gl_yclip, &gl_wclip, &gl_hclip);

        if ( gl_wclip && gl_hclip )
        {
          ptsin[0] = gl_xclip;
          ptsin[1] = gl_yclip;
          ptsin[2] = gl_xclip + gl_wclip - 1;
          ptsin[3] = gl_yclip + gl_hclip - 1;
          vst_clip( TRUE, &ptsin[0]);
        }
        else
          vst_clip( FALSE, &ptsin[0]);
}


/*
*       Routine to get the current clip setting
*/
void gsx_gclip(GRECT *pt)
{
        r_set(pt, gl_xclip, gl_yclip, gl_wclip, gl_hclip);
}


/*
*       Routine to set if the passed in rectangle intersects the
*       current clip rectangle
*/
WORD gsx_chkclip(GRECT *pt)
{
                                                /* if clipping is on    */
        if (gl_wclip && gl_hclip)
        {
          if ((pt->g_y + pt->g_h) < gl_yclip)
            return(FALSE);                      /* all above    */
          if ((pt->g_x + pt->g_w) < gl_xclip) 
            return(FALSE);                      /* all left     */
          if ((gl_yclip + gl_hclip) <= pt->g_y)
            return(FALSE);                      /* all below    */
          if ((gl_xclip + gl_wclip) <= pt->g_x)
            return(FALSE);                      /* all right    */
        }
        return(TRUE);
}


/*
*       Routine to draw a certain number of points in a polyline
*       relative to a given x,y offset.
*/
/*
        VOID
gsx_pline(offx, offy, cnt, pts)
        WORD            offx, offy;
        WORD            cnt;
        WORD            *pts;
{
        WORD            i, j;

        for (i=0; i<cnt; i++)
        {
          j = i * 2;
          ptsin[j] = offx + pts[j];
          ptsin[j+1] = offy + pts[j+1];
        }

        gsx_xline( cnt, &ptsin[0]);
}
*/


/*
*       Routine to draw a clipped polyline, hiding the mouse as you
*       do it.
*/
void gsx_cline(UWORD x1, UWORD y1, UWORD x2, UWORD y2)
{

        gsx_moff();
        g_v_pline( 2, &x1 );
        gsx_mon();
}


/*
*       Routine to set the text, writing mode, and color attributes.
*/
void gsx_attr(UWORD text, UWORD mode, UWORD color)
{
        WORD            tmp;

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
            contrl[0] = S_TEXT_COLOR;
            gl_tcolor = color;
          }
        }       
        else
        {
          if (color != gl_lcolor)
          {
            contrl[0] = S_LINE_COLOR;
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
*       Routine to set up the points for drawing a box.
*/
void gsx_bxpts(GRECT *pt)
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
*       Routine to draw a box using the current attributes.
*/
void gsx_box(GRECT *pt)
{
        gsx_bxpts(pt);
        g_v_pline( 5, &ptsin[0] );
}


/*
*       Routine to draw a box that will look right on a dithered
*       surface.
*/
void gsx_xbox(GRECT *pt)
{
        gsx_bxpts(pt);
        gsx_xline( 5, &ptsin[0] );
}


/*
*       Routine to draw a portion of the corners of a box that will
*       look right on a dithered surface.
*/
void gsx_xcbox(GRECT *pt)
{
        WORD            wa, ha;

        wa = 2 * gl_wbox;
        ha = 2 * gl_hbox;
        ptsin[0] = pt->g_x;
        ptsin[1] = pt->g_y + ha;
        ptsin[2] = pt->g_x;
        ptsin[3] = pt->g_y;
        ptsin[4] = pt->g_x + wa;
        ptsin[5] = pt->g_y;
        gsx_xline( 3, &ptsin[0] );
        ptsin[0] = pt->g_x + pt->g_w - wa;
        ptsin[1] = pt->g_y;
        ptsin[2] = pt->g_x + pt->g_w - 1;
        ptsin[3] = pt->g_y;
        ptsin[4] = pt->g_x + pt->g_w - 1;
        ptsin[5] = pt->g_y + ha;
        gsx_xline( 3, &ptsin[0] );
        ptsin[0] = pt->g_x + pt->g_w - 1;
        ptsin[1] = pt->g_y + pt->g_h - ha;
        ptsin[2] = pt->g_x + pt->g_w - 1;
        ptsin[3] = pt->g_y + pt->g_h - 1;
        ptsin[4] = pt->g_x + pt->g_w - wa;
        ptsin[5] = pt->g_y + pt->g_h - 1;
        gsx_xline( 3, &ptsin[0] );
        ptsin[0] = pt->g_x + wa;
        ptsin[1] = pt->g_y + pt->g_h - 1;
        ptsin[2] = pt->g_x;
        ptsin[3] = pt->g_y + pt->g_h - 1;
        ptsin[4] = pt->g_x;
        ptsin[5] = pt->g_y + pt->g_h - ha;
        gsx_xline( 3, &ptsin[0] );
}


/*
*       Routine to fix up the MFDB of a particular raster form
*/
void gsx_fix(FDB *pfd, LONG theaddr, WORD wb, WORD h)
{
        if (theaddr == ORGADDR)
        {
          pfd->fd_w = gl_ws.ws_xres + 1;
          pfd->fd_wdwidth = pfd->fd_w / 16;
          pfd->fd_h = gl_ws.ws_yres + 1;
          pfd->fd_nplanes = gl_nplanes;
        }
        else
        {
          pfd->fd_wdwidth = wb / 2;
          pfd->fd_w = wb * 8;
          pfd->fd_h = h;
          pfd->fd_nplanes = 1;
        }
        pfd->fd_stand = FALSE;
        pfd->fd_addr = theaddr;
}


/*
*       Routine to blit, to and from a specific area
*/
void gsx_blt(LONG saddr, UWORD sx, UWORD sy, UWORD swb,
             LONG daddr, UWORD dx, UWORD dy, UWORD dwb, UWORD w, UWORD h,
             UWORD rule, WORD fgcolor, WORD bgcolor)
{
        gsx_fix(&gl_src, saddr, swb, h);
        gsx_fix(&gl_dst, daddr, dwb, h);

        gsx_moff();
        ptsin[0] = sx;
        ptsin[1] = sy;
        ptsin[2] = sx + w - 1;
        ptsin[3] = sy + h - 1;
        ptsin[4] = dx;
        ptsin[5] = dy;
        ptsin[6] = dx + w - 1;
        ptsin[7] = dy + h - 1 ;
        if (fgcolor == -1)
          vro_cpyfm( rule, &ptsin[0], &gl_src, &gl_dst);
        else
          vrt_cpyfm( rule, &ptsin[0], &gl_src, &gl_dst, fgcolor, bgcolor);
        gsx_mon();
}


/*
*       Routine to blit around something on the screen
*/
void bb_screen(WORD scrule, WORD scsx, WORD scsy, WORD scdx, WORD scdy,
               WORD scw, WORD sch)
{
        gsx_blt(0x0L, scsx, scsy, 0, 
                0x0L, scdx, scdy, 0,
                scw, sch, scrule, -1, -1);
}


/*
*       Routine to transform a standard form to device specific
*       form.
*/
void gsx_trans(LONG saddr, UWORD swb, LONG daddr, UWORD dwb, UWORD h)
{
        gsx_fix(&gl_src, saddr, swb, h);
        gl_src.fd_stand = TRUE;
        gl_src.fd_nplanes = 1;

        gsx_fix(&gl_dst, daddr, dwb, h);
        vrn_trnfm( &gl_src, &gl_dst );
}


/*
*       Routine to initialize all the global variables dealing
*       with a particular workstation open
*/
void gsx_start()
{
        WORD            char_height, nc;

        gl_xclip = 0;
        gl_yclip = 0;
        gl_width = gl_wclip = gl_ws.ws_xres + 1;
        gl_height = gl_hclip = gl_ws.ws_yres + 1;

        nc = gl_ws.ws_ncolors;
        gl_nplanes = 0;
        while (nc != 1)
        {
          nc >>= 1;
          gl_nplanes++;
        }
        char_height = gl_ws.ws_chminh;
        vst_height( char_height, &gl_wsptschar, &gl_hsptschar, 
                                &gl_wschar, &gl_hschar );
        char_height = gl_ws.ws_chmaxh;
        vst_height( char_height, &gl_wptschar, &gl_hptschar, 
                                &gl_wchar, &gl_hchar );
        gl_ncols = gl_width / gl_wchar;
        gl_nrows = gl_height / gl_hchar;
        gl_hbox = gl_hchar + 3;
        gl_wbox = (gl_hbox * gl_ws.ws_hpixel) / gl_ws.ws_wpixel;
        vsl_type( 7 );
        g_vsl_width( 1 );
        vsl_udsty( 0xffff );
        r_set(&gl_rscreen, 0, 0, gl_width, gl_height);
        r_set(&gl_rfull, 0, gl_hbox, gl_width, (gl_height - gl_hbox));
        r_set(&gl_rzero, 0, 0, 0, 0);
        r_set(&gl_rcenter, (gl_width-gl_wbox)/2, (gl_height-(2*gl_hbox))/2, 
                        gl_wbox, gl_hbox);
        r_set(&gl_rmenu, 0, 0, gl_width, gl_hbox);
        ad_intin = (LONG) ADDR(&intin[0]);
}

/*
*       Routine to do a filled bit blit, (a rectangle).
*/
void bb_fill(WORD mode, WORD fis, WORD patt, WORD hx, WORD hy, WORD hw, WORD hh)
{

        gsx_fix(&gl_dst, 0x0L, 0, 0);
        ptsin[0] = hx;
        ptsin[1] = hy;
        ptsin[2] = hx + hw - 1;
        ptsin[3] = hy + hh - 1;

        gsx_attr(TRUE, mode, gl_tcolor);
        if (fis != gl_fis)
        {
          vsf_interior( fis);
          gl_fis = fis;
        }
        if (patt != gl_patt)
        {
          vsf_style( patt );
          gl_patt = patt;
        }
        vr_recfl( &ptsin[0], &gl_dst );
}


UWORD ch_width(WORD fn)
{
        if (fn == IBM)
          return(gl_wchar);
        if (fn == SMALL)
          return(gl_wschar);
        return(0);
}



UWORD ch_height(WORD fn)
{
        if (fn == IBM)
          return(gl_hchar);
        if (fn == SMALL)
          return(gl_hschar);
        return(0);
}


void gsx_tcalc(WORD font, LONG ptext, WORD *ptextw, WORD *ptexth, WORD *pnumchs)
{
        WORD            wc, hc;

        wc = ch_width(font);
        hc = ch_height(font);
                                                /* figure out the       */
                                                /*   width of the text  */
                                                /*   string in pixels   */

        *pnumchs = LBWMOV((WORD *)ad_intin, (BYTE *)ptext);
        *ptextw = min(*ptextw, *pnumchs * wc );
                                                /* figure out the height*/
                                                /*   of the text        */
        *ptexth = min(*ptexth, hc );
        if (*ptexth / hc)
          *pnumchs = min(*pnumchs, *ptextw / wc );
        else
          *pnumchs = 0;
}


void gsx_tblt(WORD tb_f, WORD x, WORD y, WORD tb_nc)
{
        WORD            pts_height;

        if (tb_f == IBM)
        {
          if (tb_f != gl_font)
          {
            pts_height = gl_ws.ws_chmaxh;
            vst_height( pts_height, &gl_wptschar, &gl_hptschar, 
                                &gl_wchar, &gl_hchar );
            gl_font = tb_f;
          }
          y += gl_hptschar - 1;
        }

        if (tb_f == SMALL)
        {
          if (tb_f != gl_font)
          {
            pts_height = gl_ws.ws_chminh;
            vst_height( pts_height, &gl_wsptschar, &gl_hsptschar, 
                                &gl_wschar, &gl_hschar );
            gl_font = tb_f;
          }
          y += gl_hsptschar - 1;
        }

        contrl[0] = 8;          /* TEXT */
        contrl[1] = 1;
        contrl[6] = gl_handle;
        ptsin[0] = x;
        ptsin[1] = y;
        contrl[3] = tb_nc;
        gsx2();
}



void gsx_xline(WORD ptscount, WORD *ppoints )
{
        static  WORD    hztltbl[2] = { 0x5555, 0xaaaa };
        static  WORD    verttbl[4] = { 0x5555, 0xaaaa, 0xaaaa, 0x5555 };
        WORD            *linexy,i;
        WORD            st;

        for ( i = 1; i < ptscount; i++ )
        {
          if ( *ppoints == *(ppoints + 2) )
          {
            st = verttbl[( (( *ppoints) & 1) | ((*(ppoints + 1) & 1 ) << 1))];
          }     
          else
          {
            linexy = ( *ppoints < *( ppoints + 2 )) ? ppoints : ppoints + 2;
            st = hztltbl[( *(linexy + 1) & 1)];
          }
          vsl_udsty( st );
          g_v_pline( 2, ppoints );
          ppoints += 2;
        }
        vsl_udsty( 0xffff );
}       

/*
*       Routine to convert a rectangle to its inside dimensions.
*/
void gr_inside(GRECT *pt, WORD th)
{
        pt->g_x += th;
        pt->g_y += th;
        pt->g_w -= ( 2 * th );
        pt->g_h -= ( 2 * th );
}


/*
*       Routine to draw a colored, patterned, rectangle.
*/
void gr_rect(UWORD icolor, UWORD ipattern, GRECT *pt)
{
        WORD            fis;

        fis = FIS_PATTERN;
        if (ipattern == IP_HOLLOW)
          fis = FIS_HOLLOW;
        else if (ipattern == IP_SOLID)
          fis = FIS_SOLID;

        vsf_color(icolor);
        bb_fill(MD_REPLACE, fis, ipattern, 
                pt->g_x, pt->g_y, pt->g_w, pt->g_h);
}


/*
*       Routine to adjust the x,y starting point of a text string
*       to account for its justification.  The number of characters
*       in the string is also returned.
*/
WORD gr_just(WORD just, WORD font, LONG ptext, WORD w, WORD h, GRECT *pt)
{
        WORD            numchs, diff;
                                                /* figure out the       */
                                                /*   width of the text  */
                                                /*   string in pixels   */
        gsx_tcalc(font, ptext, &pt->g_w, &pt->g_h, &numchs);

        h -= pt->g_h;
        if ( h > 0 )                            /* check height         */
          pt->g_y += (h + 1) / 2;

        w -= pt->g_w;
        if ( w > 0 )
        {
                                                /* do justification     */
          if (just == TE_CNTR)
            pt->g_x += (w + 1) / 2;
          else if (just == TE_RIGHT)
            pt->g_x += w;
                                                /* try to byte align    */
          if ( (font == IBM) &&
               (w > 16) &&
               ((diff = (pt->g_x & 0x0007)) != 0))
          {
            if (just == TE_LEFT)
              pt->g_x += 8 - diff;
            else if (just == TE_CNTR)
            {
              if (diff > 3)
                diff -= 8;
              pt->g_x -= diff;
            }
            else if (just == TE_RIGHT)
              pt->g_x -= diff;
          }
        }

        return(numchs);
}


/*
*       Routine to draw a string of graphic text.
*/
void gr_gtext(WORD just, WORD font, LONG ptext, GRECT *pt)
{
        WORD            numchs;
        GRECT           t;
                                                /* figure out where &   */
                                                /*   how to put out     */
                                                /*   the text           */
        rc_copy(pt, &t);
        numchs = gr_just(just, font, ptext, t.g_w, t.g_h, &t);
        if (numchs > 0)
          gsx_tblt(font, t.g_x, t.g_y, numchs);
}



/*
*       Routine to crack out the border color, text color, inside pattern,
*       and inside color from a single color information word.
*/
void gr_crack(UWORD color, WORD *pbc, WORD *ptc, WORD *pip, WORD *pic, WORD *pmd)
{
                                                /* 4 bit encoded border */
                                                /*   color              */
        *pbc = (LHIBT(color) >> 4) & 0x0f;
                                                /* 4 bit encoded text   */
                                                /*   color              */
        *ptc = LHIBT(color) & 0x0f;
                                                /* 3 bit encoded pattern*/
        *pip = (LLOBT(color) >> 4) & 0x0f;
                                                /* 4th bit used to set  */
                                                /*   text writing mode  */
        if (*pip & 0x08)
        {
          *pip &= 0x07;
          *pmd = MD_REPLACE;
        }
        else
          *pmd = MD_TRANS;
                                                /* 4 bit encoded inside */
                                                /*   color              */
        *pic = LLOBT(color) & 0x0f;
}


void gr_gblt(LONG pimage, GRECT *pi, WORD col1, WORD col2)
{
        gsx_blt(pimage, 0, 0, pi->g_w/8, 0x0L, pi->g_x, pi->g_y, 
                        gl_width/8, pi->g_w, pi->g_h, MD_TRANS,
                        col1, col2); 
}


/*
*       Routine to draw an icon, which is a graphic image with a text
*       string underneath it.
*/
void gr_gicon(WORD state, LONG pmask, LONG pdata, LONG ptext, WORD ch,
              WORD chx, WORD chy, GRECT *pi, GRECT *pt)
{
        WORD            ifgcol, ibgcol;
        WORD            tfgcol, tbgcol, tmp;
                                                /* crack the color/char */
                                                /*   definition word    */
        tfgcol = ifgcol = (ch >> 12) & 0x000f;
        tbgcol = ibgcol = (ch >> 8) & 0x000f;
        ch &= 0x00ff;
                                                /* invert if selected   */
        if (state & SELECTED)
        {
          tmp = tfgcol;
          tfgcol = tbgcol;
          tbgcol = tmp;
          if ( !(state & DRAW3D) )
          {
            tmp = ifgcol;
            ifgcol = ibgcol;
            ibgcol = tmp;
          }
        }
                                                /* do mask unless its on*/
                                                /* a white background   */
        if ( !( (state & WHITEBAK) && (ibgcol == WHITE) ) )
          gr_gblt(pmask, pi, ibgcol, ifgcol);

        if ( !( (state & WHITEBAK) && (tbgcol == WHITE) ) )
        {
          if (pt->g_w)
            gr_rect(tbgcol, IP_SOLID, pt);
        }
                                                /* draw the data        */
        gr_gblt(pdata, pi, ifgcol, ibgcol);
        
        if ( (state & SELECTED) &&
             (state & DRAW3D) )
        {
          pi->g_x--;
          pi->g_y--;
          gr_gblt(pmask, pi, ifgcol, ibgcol);
          pi->g_x += 2;
          pi->g_y += 2;
          gr_gblt(pmask, pi, ifgcol, ibgcol);
          pi->g_x--;
          pi->g_y--;
        }
                                                /* draw the character   */
        gsx_attr(TRUE, MD_TRANS, ifgcol);
        if ( ch )
        {
          intin[0] = ch;
          gsx_tblt(SMALL, pi->g_x+chx, pi->g_y+chy, 1);
        }
                                                /* draw the label       */
        gsx_attr(TRUE, MD_TRANS, tfgcol);
        gr_gtext(TE_CNTR, SMALL, ptext, pt);
}


/*
*       Routine to draw a box of a certain thickness using the current
*       attribute settings
*/
void gr_box(WORD x, WORD y, WORD w, WORD h, WORD th)
{
        GRECT           t, n;

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


