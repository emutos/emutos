/*      GEMQUEUE.C      1/27/84 - 07/09/85      Lee Jay Lorenzen        */
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

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"

#include "optimize.h"
#include "gemasync.h"




void doq(WORD donq, PD *p, QPB *m)
{
        register WORD   n, index;
        register WORD   *om, *nm;

        n = m->qpb_cnt;
        if (donq)
        {
          LBCOPY(p->p_qaddr + p->p_qindex, m->qpb_buf, n);
                                                /* if its a redraw msg  */
                                                /*   try to find a      */
                                                /*   matching msg and   */
                                                /*   union the redraw   */
                                                /*   rects together     */
          nm = (WORD *) &p->p_queue[p->p_qindex];
          if ( (nm[0] == WM_REDRAW) ||
               (nm[0] == WM_ARROWED) ||
               (nm[0] == WM_HSLID) ||
               (nm[0] == WM_VSLID) )
          {
            index = 0;
            while ( (index < p->p_qindex) &&
                    (n) )
            {
              om = (WORD *) &p->p_queue[index];
                                                /* if redraw and same   */
                                                /*   handle then union  */
              if ( (om[0] == WM_REDRAW) &&
                   (nm[3] == om[3]) )
              {
                rc_union((GRECT *)&nm[4], (GRECT *)&om[4]);  /* FIXME: Ugly pointer typecasting */
                n = 0;
              }
              else
              {
                                                /* if another arrow     */
                                                /*   command then copy  */
                                                /*   over with new msg  */
                                                /*   else add in length */
                                                /*   and get next messg */
                if (om[0] == WM_ARROWED)
                {
                  movs(16, &nm[0], &om[0]);
                  n = 0;
                }
                else
                  index += (om[2] != 0xFFFF) ? (om[2] + 16) : 16;
              }
            }
          }
          p->p_qindex += n;
        }
        else
        {
          LBCOPY(m->qpb_buf, p->p_qaddr, n);
          p->p_qindex -= n;
          if ( p->p_qindex )
            LBCOPY(p->p_qaddr, p->p_qaddr + n, p->p_qindex);
        }
}



void aqueue(WORD isqwrite, EVB *e, LONG lm)
{
        register PD     *p;
        register QPB    *m;
        EVB             **ppe;
        WORD            qready;

        m = (QPB *) lm;

        p = m->qpb_ppd;

        if (isqwrite)
          qready = ( m->qpb_cnt <= (QUEUE_SIZE - p->p_qindex) );
        else
          qready = ( p->p_qindex > 0 );

        ppe = (isqwrite ^ qready) ? &p->p_qnq : &p->p_qdq;
                                                /* room for message     */
                                                /*  or messages in q    */
        if (qready)
        {
          doq(isqwrite, p, m);
          azombie(e, 0);
          if (( e = *ppe ) != 0)                /* assignment ok        */
          {
            e->e_flag |= NOCANCEL;
            *ppe = e->e_link;

            if ( e->e_link )
              e->e_link->e_pred = e->e_pred;

            doq(!isqwrite, p, (QPB *) e->e_parm);
            azombie(e, 0);
          }
        }
        else                                    /* "block" the event    */
        {
          e->e_parm = lm;
          evinsert(e, ppe);
        }
}


