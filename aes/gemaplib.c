/*      GEMAPLIB.C      03/15/84 - 08/21/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */ 

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

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "geminit.h"
#include "gempd.h"
#include "geminput.h"
#include "gemflag.h"
#include "gemevlib.h"
#include "gemgsxif.h"
#include "gemwmlib.h"
#include "gemmnlib.h"
#include "gemdosif.h"
#include "gemasm.h"
#include "gemdisp.h"
#include "gemsclib.h"
#include "gemrslib.h"
#include "gemaplib.h"

#include "string.h"

#define TCHNG 0
#define BCHNG 1
#define MCHNG 2
#define KCHNG 3


/* Global variables: */
WORD     gl_play;
WORD     gl_recd;
WORD     gl_rlen;
LONG     gl_rbuf;


/*
*       Routine to initialize the application
*/
WORD ap_init(void)
{
        WORD    pid;
        char    scdir[32];

        pid = rlr->p_pid;

        strcpy(scdir, rs_fstr[STSCDIR]);

        LBSET(scdir, gl_logdrv);                /* set drive letter     */
        sc_write(scdir);

        return( pid );
}


/*
*       APplication READ or WRITE
*/
void ap_rdwr(WORD code, PD *p, WORD length, LONG pbuff)
{
        QPB             m;
                                                /* do quick version if  */
                                                /*   it is standard 16  */
                                                /*   byte read and the  */
                                                /*   pipe has only 16   */
                                                /*   bytes inside it    */
        if ( (code == MU_MESAG) &&
             (p->p_qindex == length) &&
             (length == 16) )
        {
          LBCOPY(pbuff, p->p_qaddr, p->p_qindex);
          p->p_qindex = 0;
        }
        else
        {
          m.qpb_ppd = p;
          m.qpb_cnt = length;
          m.qpb_buf = pbuff;
          ev_block(code, ADDR(&m));
          }
}


/*
*       APplication FIND
*/
WORD ap_find(LONG pname)
{
        register PD     *p;
        BYTE            temp[9];

        strcpy(temp, (char *)pname);
 
        p = fpdnm(&temp[0], 0x0);
        return( ((p) ? (p->p_pid) : (-1)) );
}


/*
*       APplication Tape PLAYer
*/
void ap_tplay(LONG pbuff, WORD length, WORD scale)
{
        register WORD   i;
        FPD             f;
        LONG            ad_f;

        ad_f = (LONG) ADDR(&f);

        gl_play = TRUE;
        for(i=0; i<length; i++)
        {
                                                /* get an event to play */
          LBCOPY(ad_f, pbuff, sizeof(FPD));
          pbuff += sizeof(FPD);
                                                /* convert it to machine*/
                                                /*   specific form      */
          switch( ((LONG)(f.f_code)) )
          {
            case TCHNG:
                ev_timer( (f.f_data*100L) / scale );
                f.f_code = 0;
                break;
            case MCHNG:
                f.f_code = (void(*)())mchange;
                break;
            case BCHNG:
                f.f_code = (void(*)())bchange;
                break;
            case KCHNG:
                f.f_code = (void(*)())kchange;
                break;
          }
                                                /* play it              */
          if (f.f_code)      /* FIXME: Hope the endianess is okay here: */
            forkq(f.f_code, LLOWD(f.f_data), LHIWD(f.f_data));
                                                /* let someone else     */
                                                /*   hear it and respond*/
          dsptch();
        }
        gl_play = FALSE;
} /* ap_tplay */


/*
*       APplication Tape RECorDer
*/
WORD ap_trecd(LONG pbuff, WORD length)
{
        register WORD   i;
        register WORD   code;
        WORD            (*proutine)(void);

        code = -1;
                                                /* start recording in   */
                                                /*   forker()           */
        cli();
        gl_recd = TRUE;
        gl_rlen = length;
        gl_rbuf = pbuff;
        sti();
                                                /* 1/10 of a second     */
                                                /*   sample rate        */
        while( gl_recd )
          ev_timer( 100L );
                                                /* done recording so    */
                                                /*   figure out length  */
        cli();
        gl_recd = FALSE;
        gl_rlen = 0;
        length = ((WORD)(gl_rbuf - pbuff)) / sizeof(FPD);
        gl_rbuf = 0x0L;
        sti();
                                                /* convert to machine   */
                                                /*   independent        */
                                                /*   recording          */
        for(i=0; i<length; i++)
        {
          proutine = (WORD (*)(void))LLGET(pbuff);
          if((LONG)proutine == (LONG)tchange)
          {
            code = TCHNG;
            LLSET(pbuff+sizeof(WORD *), LLGET(pbuff+sizeof(WORD *)) * 
                        gl_ticktime);
          }
          if((LONG)proutine == (LONG)mchange)
            code = MCHNG;
          if((LONG)proutine == (LONG)kchange)
            code = KCHNG;
          if((LONG)proutine == (LONG)bchange)
            code = BCHNG;
          LWSET(pbuff, code);
          pbuff += sizeof(FPD);
        }
        return(length);
} /* ap_trecd */


void ap_exit(void)
{
        wm_update(TRUE);
        mn_clsda();
        if (rlr->p_qindex)
          ap_rdwr(MU_MESAG, rlr, rlr->p_qindex, ad_valstr);
        gsx_mfset(ad_armice);
        wm_update(FALSE);
        all_run();
}

