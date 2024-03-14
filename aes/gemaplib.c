/*      GEMAPLIB.C      03/15/84 - 08/21/85     Lee Lorenzen            */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2024 The EmuTOS development team
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

#include "emutos.h"
#include "struct.h"
#include "aesdefs.h"
#include "aesvars.h"
#include "obdefs.h"
#include "gemlib.h"

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
#include "gemaplib.h"
#include "gsx2.h"
#include "funcdef.h"
#include "intmath.h"
#include "string.h"
#include "asm.h"

/* Global variables: */
BOOL     gl_play;
BOOL     gl_recd;
WORD     gl_rlen;
FPD      *gl_rbuf;


/*
 *  Routine to initialize the application
 */
WORD ap_init(void)
{
    WORD    pid;

    pid = rlr->p_pid;

    rlr->p_flags |= AP_OPEN;        /* appl_init() done */

    return pid;
}


/*
 *  APplication READ or WRITE
 */
WORD ap_rdwr(WORD code, AESPD *p, WORD length, WORD *pbuff)
{
    QPB     m;

    /*
     * do quick version if it is standard 16-byte read and the
     * pipe has only 16 bytes inside it
     */
    if ((code == MU_MESAG) && (p->p_qindex == length) && (length == 16))
    {
        memcpy(pbuff, p->p_qaddr, p->p_qindex);
        p->p_qindex = 0;
        return 1;       /* non-zero means it worked */
    }

    m.qpb_ppd = p;
    m.qpb_cnt = length;
    m.qpb_buf = (LONG)pbuff;

    return ev_block(code, (LONG)&m);
}


/*
 *  APplication FIND
 */
WORD ap_find(char *pname)
{
    AESPD  *p;

    /*
     * explicitly disallow a NULL filename pointer, since this has
     * a special meaning for fpdnm()
     */
    if (!pname)
        return -1;

    p = fpdnm(pname, 0);
    return p ? p->p_pid : -1;
}


/*
 *  APplication Tape PLAYer
 *
 *  this is relatively straightforward, except if we have to playback
 *  mouse movement.  in this case, we need to:
 *      a) disconnect the cursor from the VDI (done here), and
 *      b) draw it ourselves (done in mchange() in geminput.c).
 */
void ap_tplay(const EVNTREC *pbuff,WORD length,WORD scale)
{
    WORD   i;
    FPD    f;
    PFVOID mot_vecx_save = NULL;

    gl_play = TRUE;

    for (i = 0; i < length; i++, pbuff++) {
        /* set up FPD for forkq */
        f.f_code = NULL;            /* by default we don't call forkq() */
        f.f_data = pbuff->ap_value;

        /* convert to form suitable for forkq */
        switch(pbuff->ap_event) {
        case TCHNG:
            ev_timer(divu(f.f_data*100L, scale));
            break;
        case BCHNG:
            f.f_code = bchange;
            break;
        case MCHNG:
            if (!mot_vecx_save)     /* i.e. first time for MCHNG */
            {
                /*
                 * disconnect cursor drawing & movement routines
                 */
                i_ptr(just_rts);
                gsx_0code(CUR_VECX);
                m_lptr2(&drwaddr);  /* old address will be used by drawrat() */
                i_ptr(just_rts);
                gsx_0code(MOT_VECX);
                m_lptr2(&mot_vecx_save);
            }
            f.f_code = mchange;
            break;
        case KCHNG:
            f.f_code = kchange;
            break;
        }

        if (f.f_code)   /* if valid, add to queue */
        {
            disable_interrupts();
            forkq(f.f_code,f.f_data);
            enable_interrupts();
        }

        dsptch();       /* let someone run */
    }

    /*
     *  if we disconnected above, reconnect the old routines
     */
    if (mot_vecx_save)
    {
        drawrat(xrat, yrat);
        gsx_setmousexy(xrat, yrat);     /* no jumping cursors, please */
        i_ptr(drwaddr);                 /* restore vectors */
        gsx_0code(CUR_VECX);
        i_ptr(mot_vecx_save);
        gsx_0code(MOT_VECX);
    }

    gl_play = FALSE;
}

/*
 *  APplication Tape RECorDer
 */
WORD ap_trecd(EVNTREC *pbuff,WORD length)
{
    WORD   i;
    LONG   code;
    FCODE  proutine;

    /*
     * start recording in forker() [gemdisp.c]
     *
     * the events are recorded in the buffer in FPD format, and converted
     * to EVNTREC format below, after recording is complete.  This assumes
     * that sizeof(FPD) = sizeof(EVNTREC).
     *
     * if the size of the FPD structure changes (unlikely), you'll get
     * an error from _Static_assert(), and you'll probably need to do
     * the conversion to EVNTREC on the fly in gemdisp.c.
     */
    _Static_assert(sizeof(EVNTREC)==sizeof(FPD),
                    "EVNTREC & FPD structures are not the same size!");

    disable_interrupts();
    gl_recd = TRUE;
    gl_rlen = length;
    gl_rbuf = (FPD *)pbuff;
    enable_interrupts();

    /* check every 0.1 seconds if recording is done */
    while(gl_recd)
        ev_timer(100L);

    /*
     * recording complete:
     * figure out actual length & reset globals for next time
     */
    disable_interrupts();
    length = (WORD)(gl_rbuf - (FPD *)pbuff);
    gl_rlen = 0;
    gl_rbuf = NULL;
    enable_interrupts();

    /* convert to standard format */
    for (i = 0; i < length; i++, pbuff++) {
        proutine = (FCODE)pbuff->ap_event;
        if (proutine == tchange)
            code = TCHNG;
        else if (proutine == bchange)
            code = BCHNG;
        else if (proutine == mchange)
            code = MCHNG;
        else if (proutine == kchange)
            code = KCHNG;
        else code = -1;
        pbuff->ap_event = code;
        /* ap_value is the (unchanged) f_data */
    }

    return length;
}

/*
 *  APplication EXIT
 */
void ap_exit(void)
{
    wm_update(BEG_UPDATE);
    mn_cleanup();
    wait_for_accs(AP_ACCLOSE);  /* block until all DAs have seen AC_CLOSE */
    if (rlr->p_qindex)
        ap_rdwr(MU_MESAG, rlr, rlr->p_qindex, (WORD *)D.g_valstr);
    wm_update(END_UPDATE);
    all_run();
    rlr->p_flags &= ~AP_OPEN;   /* say appl_exit() is done */
}
