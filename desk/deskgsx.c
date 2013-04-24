/*      DESKGSX.C       06/01/84 - 02/02/85     Lee Lorenzen    */
/*      merge source    5/27/87                 mdf             */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Desktop                                       Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1985                      Digital Research Inc.
*       -------------------------------------------------------------
*/

/*
*       Calls used in DESKTOP:
*
*       d_vsf_interior();
*       vr_recfl();
*       vst_height();
*       d_vsl_type();
*       d_vsl_udsty();
*       vsl_width();
*       d_v_pline();
*       vst_clip();
*       vr_cpyfm();
*       vq_extnd();
*       v_clsvwk( handle )
*       v_opnvwk( pwork_in, phandle, pwork_out )
*/


#include "config.h"
#include "portab.h"
#include "compat.h"
#include "gsxdefs.h"
#include "funcdef.h"
#include "obdefs.h"
#include "deskgraf.h"
#include "gsx2.h"
#include "deskgsx.h"


void gsx_ncode(WORD code, WORD n, WORD m)
{
        contrl[0] = code;
        contrl[1] = n;
        contrl[3] = m;
        contrl[6] = gl_handle;
        gsx2();
}


void gsx_vclose(void)
{
        gsx_ncode(CLOSE_VWORKSTATION, 0, 0);
}


void d_v_pline(WORD count, WORD *pxyarray)
{
        i_ptsin( pxyarray );
        gsx_ncode(POLYLINE, count, 0);
        i_ptsin( ptsin );
}


void gsx_1code(WORD  code, WORD  value)
{
        intin[0] = value;
        gsx_ncode(code, 0, 1);
}


void v_opnvwk(WORD *pwork_in, WORD *phandle, WORD *pwork_out )
{
        WORD            *ptsptr;

        i_intin( pwork_in );    /* set intin to point to callers data  */
        i_intout( pwork_out );  /* set intout to point to callers data */
        ptsptr = pwork_out + 45;
        i_ptsout( ptsptr );     /* set ptsout to work_out array */
        contrl[0] = 100;
        contrl[1] = 0;            /* no points to xform */
        contrl[3] = 11;           /* pass down xform mode also */
        contrl[6] = *phandle;
        gsx2();
        *phandle = contrl[6];
                                 /* reset pointers for next call to binding */
        i_intin( intin );
        i_intout( intout );
        i_ptsin( ptsin );
        i_ptsout( ptsout );
}


void gsx_vopen(void)
{
        WORD            i;

        for(i=0; i<10; i++)
          intin[i] = 1;
        intin[10] = 2;                  /* device coordinate space */

        v_opnvwk( intin, &gl_handle, (WORD *)&gl_ws);
}


WORD vst_clip(WORD clip_flag, WORD *pxyarray)
{
        WORD            value;

        value = ( clip_flag != 0 ) ? 2 : 0;
        i_ptsin( pxyarray );
        intin[0] = clip_flag;
        gsx_ncode(TEXT_CLIP, value, 1);
        i_ptsin(ptsin);
        return (0);
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


void d_vsl_width(WORD width )
{
        ptsin[0] = width;
        ptsin[1] = 0;
        gsx_ncode(S_LINE_WIDTH, 1, 0);
}


void vro_cpyfm(WORD wr_mode, WORD *pxyarray, FDB *psrcMFDB, FDB *pdesMFDB)
{
        intin[0] = wr_mode;
        i_ptr( psrcMFDB );
        i_ptr2( pdesMFDB );
        i_ptsin( pxyarray );
        gsx_ncode(COPY_RASTER_FORM, 4, 1);
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
        gsx_ncode(121, 4, 3);
        i_ptsin( ptsin );
}


void vrn_trnfm(FDB *psrcMFDB, FDB *pdesMFDB)
{
        i_ptr( psrcMFDB );
        i_ptr2( pdesMFDB );
        gsx_ncode(TRANSFORM_FORM, 0, 0);
}


void vr_recfl(WORD *pxyarray, FDB *pdesMFDB)
{
//gemdos(9,"Mysterious vr_recfl\r\n");
        i_ptr( pdesMFDB );
        i_ptsin( pxyarray );
        gsx_ncode(FILL_RECTANGLE, 2, 1);
        i_ptsin( ptsin );
//gemdos(9,"vr_recfl done\r\n");
}
