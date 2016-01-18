/*      GEMEVLIB.C      1/28/84 - 09/12/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2 & 3.0         8/20/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2015 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include "portab.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"

#include "gemasync.h"
#include "gemdisp.h"
#include "geminput.h"
#include "gemaplib.h"
#include "geminit.h"
#include "gemevlib.h"


/* Global variables: */
const WORD gl_dcrates[5] = {450, 330, 275, 220, 165};
WORD    gl_dcindex;
WORD    gl_dclick;  /* # of ticks to wait to see if a second click will occur */
WORD    gl_ticktime;


/*
 *  Stuff the return array with the mouse x, y, button, and keyboard state
 */
static void ev_rets(WORD rets[])
{
    if (mtrans)
    {
        rets[0] = pr_xrat;
        rets[1] = pr_yrat;
        rets[2] = pr_button;
    }
    else
    {
        rets[0] = xrat;
        rets[1] = yrat;
        rets[2] = button;
    }
    rets[3] = kstate;
    mtrans = 0;
}


/*
 *  Routine to block for a certain async event and return a single return code
 */
WORD ev_block(WORD code, LONG lvalue)
{
    mwait(iasync(code, lvalue));

    return apret(code);
}


/*
 *  Wait for the mouse buttons to reach the state where:
 *      ((bmask & (bstate ^ button)) == 0) != bflag
 *  Clicks is how many times to wait for it to reach the state, but the
 *  routine should return how many times it actually reached the state
 *  before some time interval.
 *
 *  High bit of bflgclks determines whether to return when state is
 *  entered or left. This is called bflag.
 *  The default case is bflag = 0 and we are waiting to ENTER the
 *  indicated state. If bflag = 1 then we are waiting to LEAVE the state.
 */
UWORD ev_button(WORD bflgclks, UWORD bmask, UWORD bstate, WORD rets[])
{
    WORD    ret;
    LONG    parm;

    parm = combine_cms(bflgclks,bmask,bstate);
    ret = ev_block(MU_BUTTON, parm);
    ev_rets(rets);

    return ret;
}


/*
 *  Wait for the mouse to leave or enter a specified rectangle
 */
UWORD ev_mouse(MOBLK *pmo, WORD rets[])
{
    WORD    ret;

    ret = ev_block(MU_M1, (LONG)pmo);
    ev_rets(rets);

    return ret;
}


/*
 *  Routine to wait a specified number of milli-seconds
 */
void ev_timer(LONG count)
{
    ev_block(MU_TIMER, count/gl_ticktime);
}


/*
 *  Do a multi-wait on the specified events
 */
WORD ev_multi(WORD flags, MOBLK *pmo1, MOBLK *pmo2, LONG tmcount,
              LONG buparm, LONG mebuff, WORD prets[])
{
    QPB     m;
    register EVSPEC which;
    register WORD   what;
    register CQUEUE *pc;

    /* say nothing has happened yet */
    what = 0;
    which = 0;

    /* do a pre-check for a keystroke & then clear out the forkq */
    chkkbd();
    forker();

    /* a keystroke */
    if (flags & MU_KEYBD)
    {
        /* if a character is ready then get it */
        pc = &rlr->p_cda->c_q;
        if (pc->c_cnt)
        {
            prets[4] = (UWORD) dq(pc);
            what |= MU_KEYBD;
        }
    }

    /* if we own the mouse then do quick chks */
    if (rlr == gl_mowner)
    {
        /* quick check button */
        if (flags & MU_BUTTON)
        {
            if ((mtrans > 1) && downorup(pr_button, buparm))
            {
                what |= MU_BUTTON;
                prets[5] = pr_mclick;
            }
            else
            {
                if (downorup(button, buparm))
                {
                    what |= MU_BUTTON;
                    prets[5] = mclick;
                }
            }
        }
        /* quick check mouse rectangles */
        if ((flags & MU_M1) && in_mrect(pmo1))
            what |= MU_M1;
        if ((flags & MU_M2) && in_mrect(pmo2))
            what |= MU_M2;
    }

    /* quick check timer */
    if (flags & MU_TIMER)
    {
        if (tmcount == 0L)
            what |= MU_TIMER;
    }
    /* quick check message */
    if (flags & MU_MESAG)
    {
        if (rlr->p_qindex > 0)
        {
            ap_rdwr(MU_MESAG, rlr, 16, mebuff);
            what |= MU_MESAG;
        }
    }

    /* if nothing has happened yet, wait for event */
    if (what == 0)
    {
        /* wait for a keystroke */
        if (flags & MU_KEYBD)
            iasync(MU_KEYBD, 0L);
        /* wait for a button */
        if (flags & MU_BUTTON)
            iasync(MU_BUTTON, buparm);
        /* wait for mouse rectangles */
        if (flags & MU_M1)
            iasync(MU_M1, (LONG)pmo1);
        if (flags & MU_M2)
            iasync(MU_M2, (LONG)pmo2);
        /* wait for message */
        if (flags & MU_MESAG)
        {
            m.qpb_ppd = rlr;
            m.qpb_cnt = 16;
            m.qpb_buf = mebuff;
            iasync(MU_MESAG, (LONG)&m);
        }
        /* wait for timer */
        if (flags & MU_TIMER)
            iasync(MU_TIMER, tmcount/gl_ticktime);
        /* wait for events */
        which = mwait(flags);

        /* cancel outstanding events */
        which |= acancel( flags );
    }

    /* get the returns */
    ev_rets(prets);

    /* do aprets() if something hasn't already happened */
    if (what == 0)
    {
        what = which;
        if (which & MU_KEYBD)
            prets[4] = apret(MU_KEYBD);
        if (which & MU_BUTTON)
            prets[5] = apret(MU_BUTTON);
        if (which & MU_M1)
            apret(MU_M1);
        if (which & MU_M2)
            apret(MU_M2);
        if (which & MU_MESAG)
            apret(MU_MESAG);
        if (which & MU_TIMER)
            apret(MU_TIMER);
    }

    /* return what happened */
    return what;
}


/*
 *  Set/return double-click rate index
 */
WORD ev_dclick(WORD rate, WORD setit)
{
    if (setit)
    {
        gl_dcindex = rate;
        gl_dclick = gl_dcrates[gl_dcindex] / gl_ticktime ;
    }

    return gl_dcindex;
}
