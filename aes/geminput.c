/*      GEMINPUT.C      1/28/84 - 09/12/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */ 

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
#include "gemglobe.h"
#include "gemevlib.h"
#include "gemwmlib.h"
#include "gemasync.h"
#include "gemdisp.h"
#include "rectfunc.h"


#define MB_DOWN 0x01


GLOBAL WORD     dr_invdi, dr_xrat, dr_yrat, dr_doit;
GLOBAL WORD     button, xrat, yrat, kstate, mclick, mtrans;
GLOBAL WORD     pr_button, pr_xrat, pr_yrat, pr_mclick;

GLOBAL PD       *gl_mowner;                     /* current mouse/keybd  */
                                                /*   owner              */
GLOBAL PD       *gl_cowner;                     /* current control rect.*/
                                                /*   owner              */
GLOBAL PD       *ctl_pd;                        /* screen manager proces*/
                                                /*   that controls the  */
                                                /*   mouse when its     */
                                                /*   outside the control*/
                                                /*   rectangle.         */
GLOBAL GRECT    ctrl;                           /* current control rect.*/


GLOBAL WORD     gl_bclick;                      /* # of times into the  */
                                                /*   desired button     */
                                                /*   state              */
GLOBAL WORD     gl_bpend;                       /* number of pending    */
                                                /*   events desiring    */
                                                /*   more than a single */
                                                /*   clicks             */
GLOBAL WORD     gl_bdesired;                    /* the current desired  */
                                                /*   button state       */
GLOBAL WORD     gl_btrue;                       /* the current true     */
                                                /*   button state       */
GLOBAL WORD     gl_bdely;                       /* the current amount   */
                                                /*   of time before the */
                                                /*   button event is    */
                                                /*   considered finished*/


/* Prototypes: */
void post_mb(WORD ismouse, EVB *elist, WORD parm1, WORD parm2);





/*
*       Routine to return TRUE if mouse is in a position that 
*       satisfies a particular mouse rectangle block.
*/
UWORD in_mrect(MOBLK *pmo)
{ 
        return( pmo->m_out != inside(xrat, yrat, (GRECT *)&pmo->m_x) );
}


/*
*       Routine to check if the mouse is in part of the screen owned by
*       the control manager.  It returns -1 if it is, otherwise it
*       returns 0 if it is over the desktop, or +1 if it is over
*       the active window.
*/

WORD chk_ctrl(REG WORD mx, REG WORD my)
{
        WORD            wh;
                                                /* if inside ctrl rect  */
                                                /*   then owned by      */
                                                /*   active process     */
        if ( inside(mx, my, &ctrl) )
          return(1);
                                                /* if in menu bar then  */
                                                /*   owned by ctrl mgr  */
        if ( inside(mx, my, &gl_rmenu) )
          return(-1);
                                                /* if on any window     */
                                                /*   beside the desktop */
                                                /*   then ctrl mgr owns */
        wh = wm_find(mx, my);
        if (wh)
        {
          if ( (D.w_win[gl_wtop].w_flags & VF_SUBWIN) &&
               (D.w_win[wh].w_flags & VF_SUBWIN) &&
               (inside(mx, my, (GRECT *)&D.w_win[wh].w_xwork)) )
            return(1);
          else
            return(-1);
        }
        else
          return( 0 );
}


/*
*       Button click code call that is from the button interrupt
*       code with interrupts off.
*/

VOID b_click(REG WORD state)
{
                                                /* ignore it unless it  */
                                                /*   represents a change*/
        if ( state != gl_btrue)
        {
                                                /* see if we've already */
                                                /*   set up a wait      */
          if ( gl_bdely )
          {
                                                /* if the change is into*/
                                                /*   the desired state  */
                                                /*   then increment cnt */
            if (state == gl_bdesired)
            {
              gl_bclick++;
              gl_bdely += 3;
            }
          }
          else
          {
                                                /* if someone cares     */
                                                /*   about multiple     */
                                                /*   clicks and this is */
                                                /*   not a null mouse   */
                                                /*   then set up delay  */
                                                /*   else just fork it  */
            if ( (gl_bpend) &&
                 (state) )
            {
                                                /* start click cnt at 1 */
                                                /*   establish desired  */
                                                /*   state and set wait */
                                                /*   flag               */
              gl_bclick = 1;
              gl_bdesired = state;
                                                /* button delay set in  */
                                                /*   ev_dclick (5)      */
              gl_bdely = gl_dclick;
            }
            else
              forkq( (void(*)())bchange, state, 1);
          }
                                                /* update true state of */
                                                /*   the mouse          */
          gl_btrue = state;
        }
}


/*
*       Button delay code that is called from the tick interrupt code with 
*       interrupts off.
*/

VOID b_delay(WORD amnt)
{
                                                /* see if we have a     */
                                                /*   delay for mouse    */
                                                /*   click in progress  */
        if (gl_bdely)
        {
                                                /* see if decrementing  */
                                                /*  delay cnt causes    */
                                                /*  delay to be over    */
          gl_bdely -= amnt;
          if ( !(gl_bdely) )
          {
            forkq( (void(*)())bchange, gl_bdesired, gl_bclick);
            if (gl_bdesired != gl_btrue)
            {
              forkq( (void(*)())bchange, gl_btrue, 1);
            }
          }
        }
}


/*
*       Set the current control rectangle which is the part of the
*       screen owned by the active process.  Normally, the work area
*       of the top window.
*/

void set_ctrl(GRECT *pt)
{
        rc_copy(pt, &ctrl);
}



/*
*       Get the current control rectangle which is the part of the
*       screen owned by the active process.  Normally, the work area
*       of the top window, but sometimes the whole screen during 
*       form fill-in.
*/

void get_ctrl(GRECT *pt)
{
        rc_copy(&ctrl, pt);
}


/*
*       Used by form_do to remember the current keyboard and mouse
*       owners.
*/

void get_mown(PD **pmown)
{
        *pmown = gl_mowner;
}


/*
*       Used by control manager and form_do to give the mouse or keyboard
*       to another process.  The mouse should only be given with the 
*       buttons in an up state.
*/

void set_mown(PD *mp)
{
        if (!button)
        {
                                                /* change the owner     */
          gl_cowner = gl_mowner = mp;
                                                /* pretend mouse        */
                                                /*   moved to get the   */
                                                /*   right form showing */
                                                /*   and get mouse event*/
                                                /*   posted correctly   */
          post_mb(TRUE, gl_mowner->p_cda->c_msleep, xrat, yrat);
                                                /* post a button event  */
                                                /*   in case the new    */
                                                /*   owner was waiting  */
          post_mb(FALSE, gl_mowner->p_cda->c_bsleep, button, 1);
        }
}


/*
*       EnQueue a a character on a circular keyboard buffer.
*/
VOID nq(UWORD ch, REG CQUEUE *qptr)
{
        if (qptr->c_cnt < KBD_SIZE)
        {
          qptr->c_buff[qptr->c_rear++] = ch;
          if ((qptr->c_rear) == KBD_SIZE)
            qptr->c_rear = 0;
          qptr->c_cnt++ ;
        }
}


/*
*       DeQueue a a character from a circular keyboard buffer.
*/
UWORD dq(CQUEUE *qptr)
{
        REG WORD        q2;

        qptr->c_cnt--;
        q2 = qptr->c_front++;
        if ((qptr->c_front) == KBD_SIZE)
          qptr->c_front = 0;
        return( qptr->c_buff[q2] );
}


/*
*       Flush the characters from a circular keyboard buffer.
*/
void fq()
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


void kchange(UWORD ch, WORD kstat)
{
        
        kstate = kstat;
        if (ch)
          post_keybd(gl_mowner->p_cda, ch);
}


void post_keybd(REG CDA *c, REG UWORD ch)
{
        REG EVB         *e;

                                                /* if anyone waiting ?  */
        if ((e = c->c_iiowait) != 0)                    /*   wake him up        */
          evremove(e, ch);
        else
                                                /* no one is waiting,   */
                                                /*   just toss it in    */
                                                /*   the buffer         */
          nq(ch, &c->c_q);
}


/*
*       Routine to give mouse to right owner based on position.  Called
*       when button initially goes down, or when it is moved with the
*       mouse button down.
*/
VOID chkown()
{
        REG WORD        val;

        val = chk_ctrl(xrat, yrat);
        if (val == 1)
          gl_mowner = gl_cowner;
        else
          gl_mowner = (val == -1) ? ctl_pd : D.w_win[0].w_owner;
}


VOID bchange(WORD new, WORD clicks)
{
                                                /* see if this button   */
                                                /*   event causes an    */
                                                /*   ownership change   */
                                                /*   to or from ctrlmgr */
        if ( (!gl_ctmown) &&
             (new == MB_DOWN) && 
             (button == 0x0) )
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


WORD downorup(WORD new, REG LONG buparm)
{
        REG WORD        flag, mask, val;

        flag = (buparm >> 24) & 0x00ffL;
        mask = (buparm >> 8) & 0x00ffL;
        val = (buparm) & 0x00ffL;
        return( ((mask & (val ^ new)) == 0) != flag );
}


/*
VOID m_forkq(WORD (*fcode)(), WORD ratx, WORD raty)
{
  if ((dr_invdi) || (drawrat(ratx, raty)))
  {
    dr_xrat = ratx;
    dr_yrat = raty;
    dr_doit = TRUE;
  }
  forkq(fcode, ratx, raty);
}
*/


VOID m_drawit()
{
  dr_doit = FALSE;
  dr_invdi = TRUE;
  drawrat(dr_xrat, dr_yrat);
  dr_invdi = FALSE;
}


void mchange(WORD rx, WORD ry)
{
                                                /* zero out button wait */
                                                /*   if mouse moves more*/
                                                /*   then a little      */
        if ( (gl_bdely) && 
             ( (xrat-rx > 2) || 
               (xrat-rx < -2) || 
               (yrat-ry > 2) ||
               (yrat-ry < -2) ) )
          b_delay(gl_bdely );

                                                /* xrat, yrat hold true */
        xrat = rx;
        yrat = ry;

        if (gl_play)
        {
          dr_doit = TRUE;
          dr_xrat = rx;
          dr_yrat = ry;
        }

                                                /* post the event       */
        if (dr_doit)
          m_drawit();
                                                /* give mouse to screen */
                                                /*   handler when not   */
                                                /*   button down and    */
                                                /*   there is an active */
                                                /*   menu and it will   */
                                                /*   satisfy his event  */
        if ( (gl_mowner != ctl_pd) &&
             (button == 0x0) &&
             (gl_mntree) &&
             (in_mrect(&gl_ctwait)) )
          gl_mowner = ctl_pd;
        post_mb(TRUE, gl_mowner->p_cda->c_msleep, xrat, yrat);
}




WORD inorout(REG EVB *e, REG WORD rx, REG WORD ry)
{
        MOBLK           mo;
                                                /* in or out of         */
                                                /*   specified rectangle*/
        mo.m_out = ((e->e_flag & EVMOUT) != 0x0);
        mo.m_x = LHIWD(e->e_parm);
        mo.m_y = LLOWD(e->e_parm);
        mo.m_w = LHIWD(e->e_return);
        mo.m_h = LLOWD(e->e_return);
        /* FIXME: The following GRECT*-typecast is not very nice */
        return( mo.m_out != inside(rx, ry, (GRECT *)&mo.m_x) );
}

/*
*       Routine to walk the list of mouse or button events and remove
*       the ones that are satisfied.
*/
void post_mb(WORD ismouse, EVB *elist, WORD parm1, WORD parm2)
{
        REG EVB         *e1, *e;
        UWORD           clicks;

        for (e = elist; e; e = e1)
        {
          e1 = e->e_link;
          if (ismouse)
          {
            if ( inorout(e, parm1, parm2) )
              evremove(e, 0);
          }
          else
          {
            if ( downorup(parm1, e->e_parm) )
            {
                                                /* decrement counting   */
                                                /*   semaphore if one   */
                                                /*   of the multi-      */
                                                /*   click guys was     */
                                                /*   satisfied          */

              clicks = LHIWD(e->e_parm) & 0x00ff;
              if (clicks > 1)
                gl_bpend--;
              evremove(e, (parm2 > clicks) ? clicks : parm2);
            }
          }
        }
}



void akbin(EVB *e)
{
                                                /* see if already       */
                                                /*   satisfied          */
        if (rlr->p_cda->c_q.c_cnt)
          azombie(e, dq(&rlr->p_cda->c_q) );
        else                                    /* time to zzzzz...     */
          evinsert(e, &rlr->p_cda->c_iiowait);
}


void adelay(EVB *e, LONG c)
{
        REG EVB         *p, *q;
        
        if (c == 0x0L)
          c = 0x1L;
  
        cli();
        if (CMP_TICK)
        {
                                                /* if already counting  */
                                                /*   down then reset    */
                                                /*   CMP_TICK to the    */
                                                /*   lower number but   */
                                                /*   let NUM_TICK grow  */
                                                /*   from its accumulated*/
                                                /*   value              */
          if (c <= CMP_TICK)
            CMP_TICK = c; 
        }
        else
        {
                                                /* if we aren't currently*/
                                                /*   counting down for  */
                                                /*   someone else then  */
                                                /*   start ticking      */
          CMP_TICK = c;
                                                /* start NUM_TICK out   */
                                                /*   at zero            */
          NUM_TICK = 0x0L;
        }
        e->e_flag |= EVDELAY;
        q = (EVB *) ((BYTE *) &dlr - elinkoff);
        for (p = dlr; p; p = (q = p) -> e_link)
        {
          if (c <= (LONG) p->e_parm)
            break;
          c -= (LONG) p->e_parm;
        }
        e->e_pred = q;
        q->e_link = e;
        e->e_parm = (LONG) c;
        e->e_link = p;
        if ( p )
        {
          c = (LONG) p->e_parm - c;
          p->e_pred = e;
          p->e_parm = (LONG) c;
        }
        sti();
}


void abutton(EVB *e, LONG p)
{
        REG WORD        bclicks;

        if ( (rlr == gl_mowner) &&
             (downorup(button, p)) )
        {
          azombie(e, 1);                        /* 'nuff said           */
        }
        else
        {
                                                /* increment counting   */
                                                /*   semaphore to show  */
                                                /*   someone cares about*/
                                                /*   multiple clicks    */
          bclicks = LHIWD(p) & 0x00ff;
          if (bclicks > 1)
            gl_bpend++;
          e->e_parm = p;
          evinsert(e, &rlr->p_cda->c_bsleep);
        }
}


void amouse(EVB *e, LONG pmo)
{
        MOBLK           mob;
        
        LBCOPY(ADDR(&mob), pmo, sizeof(MOBLK));
                                               /* if already in (or out) */
                                               /* signal immediately     */
        if ( (rlr == gl_mowner) &&
             (in_mrect(&mob)) )
          azombie(e, 0);
        else
        {
          if (mob.m_out)
            e->e_flag |= EVMOUT;
          else
            e->e_flag &= ~EVMOUT;
          e->e_parm = HW(mob.m_x) + mob.m_y;
          e->e_return = HW(mob.m_w) + mob.m_h;
          evinsert(e, &rlr->p_cda->c_msleep );
        }
}

