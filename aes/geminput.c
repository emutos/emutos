/*      GEMINPUT.C      1/28/84 - 09/12/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
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
#include "string.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"

#include "geminput.h"
#include "gemgraf.h"
#include "gemdosif.h"
#include "gemctrl.h"
#include "gemmnlib.h"
#include "gemaplib.h"
#include "geminit.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemasync.h"
#include "gemdisp.h"
#include "gemgsxif.h"
#include "rectfunc.h"
#include "kprint.h"


extern void wheel_change(WORD wheel_number, WORD wheel_amount); /* called only from aes/gemdosif.S */
extern void b_click(WORD state); /* called only from aes/gemdosif.S */
extern void b_delay(WORD amnt);  /* called only from aes/gemdosif.S */


#define MB_DOWN 0x01


GLOBAL WORD     button, xrat, yrat, kstate, mclick, mtrans;
GLOBAL WORD     pr_button, pr_xrat, pr_yrat, pr_mclick;

GLOBAL AESPD    *gl_mowner;     /* current mouse/keybd owner            */
GLOBAL AESPD    *ctl_pd;        /* screen manager process that controls the mouse */
                                /*  when it's outside the control rectangle.      */

GLOBAL WORD     gl_bclick;      /* # of times into the desired button state */
GLOBAL WORD     gl_bdesired;    /* the current desired button state  */
GLOBAL WORD     gl_btrue;       /* the current true button state     */
GLOBAL WORD     gl_bdely;       /* the current amount of time before the */
                                /*  button event is considered finished  */

static GRECT    ctrl;           /* current control rectangle            */
static AESPD    *gl_cowner;     /* current control rectangle owner      */
static WORD     gl_bpend;       /* number of pending events desiring */
                                /*  more than a single click         */

/* Prototypes: */
static void post_mb(WORD ismouse, EVB *elist, WORD parm1, WORD parm2);


/*
 *  Routine to return TRUE if mouse is in a position that
 *  satisfies a particular mouse rectangle block
 */
UWORD in_mrect(MOBLK *pmo)
{
    return (pmo->m_out != inside(xrat, yrat, &pmo->m_gr));
}


/*
 *  Routine to check if the mouse is in part of the screen owned by
 *  the control manager.  If so, return -1; otherwise return 0 if it
 *  is over the desktop, or +1 if it is over the active window.
 */
static WORD chk_ctrl(WORD mx, WORD my)
{
    WORD    wh;

    /* if inside ctrl rectangle, then owned by active process */
    if (inside(mx, my, &ctrl))
        return 1;

    /* if in menu bar, then owned by ctrl mgr */
    if (inside(mx, my, &gl_rmenu))
        return -1;

    /* if on any window beside the desktop, then ctrl mgr owns */
    wh = wm_find(mx, my);
    if (wh)
    {
        return -1;
    }
    else
        return 0;
}


/*
 *  Button click code call that is from the button interrupt code
 *  with interrupts off
 */
void b_click(WORD state)
{
    /* ignore it unless it represents a change */
    if (state != gl_btrue)
    {
        /* see if we've already set up a wait */
        if (gl_bdely)
        {
            /* if the change is into the desired state, increment cnt */
            if (state == gl_bdesired)
            {
                gl_bclick++;
                gl_bdely += 3;
            }
        }
        else
        {
            /*
             * if someone cares about multiple clicks and this is not
             * a null mouse then set up delay else just fork it
             */
            if (gl_bpend && state)
            {
                /* start click cnt at 1, establish desired state and set wait flag */
                gl_bclick = 1;
                gl_bdesired = state;
                gl_bdely = gl_dclick;   /* button delay set in ev_dclick() */
            }
            else
                forkq(bchange, MAKE_ULONG(state, 1));
        }
        /* update true state of the mouse */
        gl_btrue = state;
    }
}


/*
 *  Button delay code that is called from the tick interrupt code
 *  with interrupts off
 */
void b_delay(WORD amnt)
{
    /* see if we have a delay for mouse click in progress */
    if (gl_bdely)
    {
        /* see if decrementing delay cnt causes delay to be over */
        gl_bdely -= amnt;
        if (!gl_bdely)
        {
            forkq(bchange, MAKE_ULONG(gl_bdesired, gl_bclick));
            if (gl_bdesired != gl_btrue)
            {
                forkq(bchange, MAKE_ULONG(gl_btrue, 1));
            }
        }
    }
}


/*
 *  Set the current control rectangle which is the part of the screen
 *  owned by the active process.  Normally, the work area of the top window.
 */
void set_ctrl(GRECT *pt)
{
    rc_copy(pt, &ctrl);
}


/*
 *  Get the current control rectangle which is the part of the screen
 *  owned by the active process.  Normally, the work area of the top
 *  window, but sometimes the whole screen during form fill-in.
 */
void get_ctrl(GRECT *pt)
{
    rc_copy(&ctrl, pt);
}


/*
 *  Used by form_do to remember the current keyboard and mouse owners
 */
void get_mown(AESPD **pmown)
{
    *pmown = gl_mowner;
}


/*
 *  Used by control manager and form_do() to give the mouse or keyboard
 *  to another process.  The mouse should only be transferred with the
 *  buttons in an up state.
 */
void set_mown(AESPD *mp)
{
    if (!button)
    {
        /* change the owner */
        gl_cowner = gl_mowner = mp;

        /*
         * pretend mouse moved to get the right form showing and
         * get the mouse event posted correctly
         */
        post_mb(TRUE, gl_mowner->p_cda->c_msleep, xrat, yrat);
        /* post a button event in case the new owner was waiting */
        post_mb(FALSE, gl_mowner->p_cda->c_bsleep, button, 1);
    }
}


/*
 *  eNQueue a character on a circular keyboard buffer
 */
static void nq(UWORD ch, CQUEUE *qptr)
{
    if (qptr->c_cnt < KBD_SIZE)
    {
        qptr->c_buff[qptr->c_rear++] = ch;
        if ((qptr->c_rear) == KBD_SIZE)
            qptr->c_rear = 0;
        qptr->c_cnt++;
    }
}


/*
 *  DeQueue a character from a circular keyboard buffer
 */
UWORD dq(CQUEUE *qptr)
{
    WORD q2;

    qptr->c_cnt--;
    q2 = qptr->c_front++;
    if ((qptr->c_front) == KBD_SIZE)
        qptr->c_front = 0;

    return qptr->c_buff[q2];
}


/*
 *  Flush the characters from a circular keyboard buffer
 */
void fq(void)
{
    while (rlr->p_cda->c_q.c_cnt)
        dq(&rlr->p_cda->c_q);
}


void evremove(EVB *e, UWORD ret)
{
    e->e_pred->e_link = e->e_link;
    if (e->e_link)
        e->e_link->e_pred = e->e_pred;
    azombie(e, ret);
}


void kchange(LONG fdata)
{
    UWORD ch = HIWORD(fdata);
    WORD kstat = LOWORD(fdata);

    kstate = kstat;
    if (ch)
        post_keybd(gl_mowner->p_cda, ch);
}


void post_keybd(CDA *c, UWORD ch)
{
    EVB *e;

    /* if someone is waiting, wake him up */
    if ((e = c->c_iiowait) != 0)
        evremove(e, ch);
    else    /* no one is waiting, just toss it in the buffer */
        nq(ch, &c->c_q);
}


/*
 *  Routine to give mouse to right owner based on position.  Called
 *  when button initially goes down, or when the mouse is moved with
 *  the mouse button down.
 */
static void chkown(void)
{
    WORD val;

    val = chk_ctrl(xrat, yrat);
    if (val == 1)
        gl_mowner = gl_cowner;
    else
        gl_mowner = (val == -1) ? ctl_pd : D.w_win[0].w_owner;
}


void bchange(LONG fdata)
{
    WORD new = HIWORD(fdata);
    WORD clicks = LOWORD(fdata);

    /* see if this button event causes an ownership change to or from ctrlmgr */
    if (!gl_ctmown && (new == MB_DOWN) && (button == 0))
        chkown();

    mtrans++;
    pr_button = button;
    pr_mclick = mclick;
    pr_xrat = xrat;
    pr_yrat = yrat;
    button = new;
    mclick = clicks;
    post_mb(FALSE, gl_mowner->p_cda->c_bsleep, button, clicks);
}


WORD downorup(WORD new, LONG buparm)
{
    WORD flag, mask, val;

    flag = (buparm >> 24) & 0x00ffL;
    mask = (buparm >> 8) & 0x00ffL;
    val = (buparm) & 0x00ffL;

    return (((mask & (val^new)) == 0) != flag);
}


void mchange(LONG fdata)
{
    WORD rx = HIWORD(fdata);
    WORD ry = LOWORD(fdata);

    /* zero out button wait if mouse moves more than a little */
    if (gl_bdely &&
        ((xrat-rx > 2) || (xrat-rx < -2) || (yrat-ry > 2) || (yrat-ry < -2)) )
        b_delay(gl_bdely );

    /* xrat, yrat hold true */
    xrat = rx;
    yrat = ry;

    /*
     * if we are running because of appl_tplay(), then we need to draw
     * the cursor ourselves
     */
    if (gl_play)
    {
        drawrat(rx, ry);
        gsx_setmousexy(rx, ry);     /* synchronise VDI cursor */
    }

    /*
     * give mouse to screen handler when not button down and
     * there is an active menu and it will satisfy his event
     */
    if ( (gl_mowner != ctl_pd) && (button == 0) && gl_mntree && (in_mrect(&gl_ctwait)) )
        gl_mowner = ctl_pd;
    post_mb(TRUE, gl_mowner->p_cda->c_msleep, xrat, yrat);
}


#if CONF_WITH_VDI_EXTENSIONS
void wheel_change(WORD wheel_number, WORD wheel_amount)
{
    WORD wh;
    WORD type;

    /* Ignore the wheel messages if the menu is active */
    if (gl_mowner == ctl_pd)
        return;

    if (wheel_amount > 0)
        type = WA_DNLINE;
    else if (wheel_amount < 0)
        type = WA_UPLINE;
    else
        return;

    /* We have a problem here.
     * This function is called by forker(), where rlr is set to -1.
     * As a result, we can't call ap_sendmsg() which internally calls iasync().
     * Uncomment the following code when some solution has been found.
     */
/*
    assert(rlr != (AESPD *)-1);
    wh = wm_find(xrat, yrat);
    ap_sendmsg(appl_msg, WM_ARROWED, D.w_win[wh].w_owner, wh, type, 0, 0, 0);
*/
    (void)wh; /* silent warning */
    (void)type; /* silent warning */
}
#endif


static WORD inorout(EVB *e, WORD rx, WORD ry)
{
    MOBLK   mo;

    /* in or out of specified rectangle */
    mo.m_out = ((e->e_flag & EVMOUT) != 0);
    mo.m_gr.g_x = HIWORD(e->e_parm);
    mo.m_gr.g_y = LOWORD(e->e_parm);
    mo.m_gr.g_w = HIWORD(e->e_return);
    mo.m_gr.g_h = LOWORD(e->e_return);

    return (mo.m_out != inside(rx, ry, &mo.m_gr));
}

/*
 *  Routine to walk the list of mouse or button events and remove
 *  the ones that are satisfied
 */
static void post_mb(WORD ismouse, EVB *elist, WORD parm1, WORD parm2)
{
    EVB     *e1, *e;
    UWORD   clicks;

    for (e = elist; e; e = e1)
    {
        e1 = e->e_link;
        if (ismouse)
        {
            if (inorout(e, parm1, parm2))
                evremove(e, 0);
        }
        else
        {
            if (downorup(parm1, e->e_parm))
            {
                /*
                 * decrement counting semaphore if one of the multi-click
                 * guys was satisfied
                 */
                clicks = LOBYTE((HIWORD(e->e_parm)));
                if (clicks > 1)
                    gl_bpend--;
                evremove(e, (parm2 > clicks) ? clicks : parm2);
            }
        }
    }
}


void akbin(EVB *e)
{
    /* see if already satisfied */
    if (rlr->p_cda->c_q.c_cnt)
        azombie(e, dq(&rlr->p_cda->c_q));
    else                    /* time to zzzzz... */
        evinsert(e, &rlr->p_cda->c_iiowait);
}


void adelay(EVB *e, LONG c)
{
    EVB *p, *q;

    if (c == 0L)
        c = 1L;

    disable_interrupts();
    if (CMP_TICK)
    {
        /*
         * if already counting down, then reset CMP_TICK to the lower
         * number, but let NUM_TICK grow from its accumulated value
         */
        if (c <= CMP_TICK)
            CMP_TICK = c;
    }
    else
    {
        /*
         * if we aren't currently counting down for someone else,
         * then start ticking
         */
        CMP_TICK = c;
        NUM_TICK = 0L;      /* start NUM_TICK out at zero */
    }

    e->e_flag |= EVDELAY;
    q = (EVB *) ((BYTE *) &dlr - offsetof(EVB, e_link));
    for (p = dlr; p; p = (q = p) -> e_link)
    {
        if (c <= p->e_parm)
            break;
        c -= p->e_parm;
    }
    e->e_pred = q;
    q->e_link = e;
    e->e_parm = c;
    e->e_link = p;
    if (p)
    {
        c = p->e_parm - c;
        p->e_pred = e;
        p->e_parm = c;
    }
    enable_interrupts();
}


void abutton(EVB *e, LONG p)
{
    WORD bclicks;

    if ((rlr == gl_mowner) && downorup(button, p))
    {
        azombie(e, 1);      /* 'nuff said */
    }
    else
    {
        /*
         * increment counting semaphore to show someone cares
         * about multiple clicks
         */
        bclicks = LOBYTE(HIWORD(p));
        if (bclicks > 1)
            gl_bpend++;
        e->e_parm = p;
        evinsert(e, &rlr->p_cda->c_bsleep);
    }
}


void amouse(EVB *e, LONG pmo)
{
    MOBLK   mob;

    mob = *(MOBLK *)pmo;

    /* if already in (or out) of rectangle, signal immediately */
    if ((rlr == gl_mowner) && in_mrect(&mob))
        azombie(e, 0);
    else
    {
        if (mob.m_out)
            e->e_flag |= EVMOUT;
        else
            e->e_flag &= ~EVMOUT;
        e->e_parm = MAKE_ULONG(mob.m_gr.g_x, mob.m_gr.g_y);
        e->e_return = MAKE_ULONG(mob.m_gr.g_w, mob.m_gr.g_h);
        evinsert(e, &rlr->p_cda->c_msleep );
    }
}
