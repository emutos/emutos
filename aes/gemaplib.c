/*      GEMAPLIB.C      03/15/84 - 08/21/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
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
FPD      *gl_rbuf;


/*
*       Routine to initialize the application
*/
WORD ap_init(void)
{
        WORD    pid;
        char    scdir[32];

        pid = rlr->p_pid;

        strcpy(scdir, SCRAP_DIR_NAME);

        scdir[0] = gl_logdrv;                   /* set drive letter     */
        sc_write(scdir);

        return( pid );
}


/*
*       APplication READ or WRITE
*/
void ap_rdwr(WORD code, AESPD *p, WORD length, LONG pbuff)
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
          memcpy((void *)pbuff, (void *)p->p_qaddr, p->p_qindex);
          p->p_qindex = 0;
        }
        else
        {
          m.qpb_ppd = p;
          m.qpb_cnt = length;
          m.qpb_buf = pbuff;
          ev_block(code, (LONG)&m);
          }
}


/*
*       APplication FIND
*/
WORD ap_find(LONG pname)
{
        register AESPD  *p;
        BYTE            temp[9];

        strcpy(temp, (char *)pname);

        p = fpdnm(&temp[0], 0x0);
        return( ((p) ? (p->p_pid) : (-1)) );
}


/*
 *  APplication Tape PLAYer
 */
void ap_tplay(FPD *pbuff,WORD length,WORD scale)
{
    WORD   i;
    FPD    f;

    gl_play = TRUE;

    for (i = 0; i < length; i++) {
        /* get an event to play */
        memcpy(&f,pbuff,sizeof(FPD));
        pbuff++;

        /* convert to form suitable for forkq */
        switch((LONG)f.f_code) {
        case TCHNG:
            ev_timer((f.f_data*100L)/scale);
            f.f_code = NULL;
            break;
        case BCHNG:
            f.f_code = bchange;
            break;
        case MCHNG:
            f.f_code = mchange;
            break;
        case KCHNG:
            f.f_code = kchange;
            break;
        default:
            f.f_code = NULL;
        }

        if (f.f_code)   /* if valid, add to queue */
            forkq(f.f_code,f.f_data);

        dsptch();       /* let someone run */
    }

    gl_play = FALSE;
}

/*
 *  APplication Tape RECorDer
 */
WORD ap_trecd(FPD *pbuff,WORD length)
{
    WORD   i;
    LONG   code;
    FCODE  proutine;

    /* start recording in forker() [gemdisp.c] */
    disable_interrupts();
    gl_recd = TRUE;
    gl_rlen = length;
    gl_rbuf = pbuff;
    enable_interrupts();

    /* check every 0.1 seconds if recording is done */
    while(gl_recd)
        ev_timer(100L);

    /*
     * recording complete:
     * figure out actual length & reset globals for next time
     */
    disable_interrupts();
    length = (WORD)(gl_rbuf - pbuff);
    gl_rlen = 0;
    gl_rbuf = NULL;
    enable_interrupts();

    /* convert to standard format */
    for (i = 0; i < length; i++) {
        proutine = (FCODE)pbuff->f_code;
        if (proutine == tchange)
            code = TCHNG;
        else if (proutine == bchange)
            code = BCHNG;
        else if (proutine == mchange)
            code = MCHNG;
        else if (proutine == kchange)
            code = KCHNG;
        else code = -1;
        pbuff->f_code = (FCODE)code;
        pbuff++;
    }

    return length;
}

void ap_exit(void)
{
        wm_update(TRUE);
        mn_clsda();
        if (rlr->p_qindex)
          ap_rdwr(MU_MESAG, rlr, rlr->p_qindex, (LONG)D.g_valstr);
        gsx_mfset(ad_armice);
        wm_update(FALSE);
        all_run();
}
