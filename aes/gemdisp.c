/*      GEMDISP.C       1/27/84 - 09/13/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */ 
/*      add beep in chkkbd                      11/12/87        mdf     */

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
#include "gempd.h"
#include "gemgsxif.h"
#include "gemaplib.h"
#include "gemglobe.h"
#include "gemflag.h"
#include "gemasm.h"
#include "optimize.h"
#include "gemdosif.h"


#define KEYSTOP 0x00002b1cL                     /* control backslash    */


#if MULTIAPP
GLOBAL LONG             gl_prret;
GLOBAL LONG             gl_prpid;
EXTERN WORD             gl_ldpid;
EXTERN SHELL            sh[];
EXTERN LONG             gl_mntree;
EXTERN PD               *gl_mnppd;
GLOBAL PD               *gl_displock = 0;
#endif


/* forkq puts a fork block with a routine in the fork ring      */

void forkq(void (*fcode)(), LONG fdata)
{
        REG FPD         *f;
                                                /* q a fork process,    */
                                                /*   enter with ints OFF*/
        if (fpcnt == 0)
          fpt = fph = 0;

        if (fpcnt < NFORKS)
        {
          f = &D.g_fpdx[fpt++];
                                                /* wrap pointer around  */
          if (fpt == NFORKS)
            fpt = 0;

          f->f_code = fcode;
          f->f_data = fdata;

          fpcnt++;
        }
}


void disp_act(PD *p)
{      
                                                /* process is ready,    */
                                                /*   so put him on RLR  */
        p->p_stat &= ~WAITIN;
        insert_process(p, &rlr);        
}


void mwait_act(PD *p)
{       
                                                /* sleep on nrl if      */
                                                /*   event flags are    */
                                                /*   not set            */
        if (p->p_evwait & p->p_evflg)
          disp_act(p);
        else
        { 
                                                /* good night, Mrs.     */
                                                /*   Calabash, wherever */
                                                /*   you are            */
          p->p_link = nrl;
          nrl = p;
        }
}



void forker()
{
        REG FPD         *f;
        REG PD          *oldrl;
        REG LONG        amt;
        FPD             g;
        
        oldrl = rlr;
        rlr = (PD *) -1;
        while(fpcnt)
        {
/* critical area        */
          cli();
          fpcnt--;
          f = &D.g_fpdx[fph++];
                                        /* copy FPD so an interrupt     */
                                        /*  doesn't overwrite it.       */
          LBCOPY(ADDR(&g), ADDR(f), sizeof(FPD) );
          if (fph == NFORKS) 
            fph = 0;
          sti();
/* */
                                                /* see if recording     */
          if (gl_recd)
          {
                                                  /* check for stop key */
            if ( ((void *)g.f_code == (void *)kchange) &&
                 ((g.f_data & 0x0000ffffL) == KEYSTOP) )
              gl_recd = FALSE;
                                                /* if still recording   */
                                                /*   then handle event  */
            if (gl_recd)
            {
                                                /* if its a time event &*/
                                                /*   previously recorded*/
                                                /*   was a time event   */
                                                /*   then coalesce them */ 
                                                /*   else record the    */
                                                /*   event              */
              if ( ((void *)g.f_code == (void *)tchange) &&
                   (LLGET(gl_rbuf - sizeof(FPD)) == (LONG)tchange) )
              {
                amt = g.f_data + LLGET(gl_rbuf-sizeof(LONG));
                LLSET(gl_rbuf - sizeof(LONG), amt);           
              }
              else
              {
                LBCOPY(gl_rbuf, ADDR(f), sizeof(FPD));
                gl_rbuf += sizeof(FPD);
                gl_rlen--;
                gl_recd = gl_rlen;
              }
            }
          }
          (*g.f_code)(g.f_data);
        }
        rlr = oldrl;
}


void chkkbd()
{
        REG WORD        achar, kstat;
                                                /* poll keybd           */
        if (!gl_play)
        {
          kstat = gsx_kstate();
          achar = gsx_char();
          if (achar && (gl_mowner->p_cda->c_q.c_cnt >= KBD_SIZE))
          {
            achar = 0x0;                        /* buffer overrun       */
            sound(TRUE, 880, 2);
          }
          if ( (achar) ||
             (kstat != kstate) )
          {
            cli();
            forkq( (void (*)())kchange, (((LONG)achar<<16)|kstat) );
            sti();
          }
        }
}



void schedule()
{
        REG PD          *p;

                                                /* run through lists    */
                                                /*   until someone is   */
                                                /*   on the rlr or the  */
                                                /*   fork list          */
        do
        {
                                                /* poll the keyboard    */
          chkkbd();
                                                /* now move drl         */
                                                /*   processes to rlr   */
          while ( drl )
          {
            drl = (p = drl) -> p_link;
            disp_act(p);
          }
                                                /* check if there is    */
                                                /*   something to run   */
        } while ( !rlr && !fpcnt );
}



/************************************************************************/
/*                                                                      */
/* dispatcher maintains all flags/regs so it looks like an rte to       */
/*   the caller.                                                        */
/*   dispatch() = rte                                                   */
/*   rlr -> p_stat determines the action to perform on the process that */
/*              was in context                                          */
/*   rlr -> p_uda -> dparam is used by the action routines              */
/*                                                                      */
/************************************************************************/

void disp()
{
        REG PD          *p;

#if MULTIAPP
skip:
#endif
                                                /* take the process p   */
                                                /*   off the ready list */
                                                /*   root               */
        p = rlr;
        rlr = p->p_link;
                                                /* based on the state   */
                                                /*   of the process p   */
                                                /*   do something       */
        if (p->p_stat & WAITIN) 
          mwait_act(p);
        else
          disp_act(p);
                                                /* run through and      */
                                                /*   execute all the    */
                                                /*   fork processes     */
        do
        {
          if (fpcnt)
            forker();
          schedule();
        } while (fpcnt);


/* switchto() is a machine dependent routine which:
 *      1) restores machine state
 *      2) clear "indisp" semaphore
 *      3) returns to appropriate address
 *              so we'll never return from this
 */
#if MULTIAPP
        if (gl_displock && (rlr != gl_displock))
          goto skip;

        if (rlr->p_pid == 1)
        {
          if (gl_mntree)
            pr_load(gl_mnppd->p_pid);   
        }
        else
          pr_load(rlr->p_pid);
#endif

  /* FIXME: rlr->p_uda was sometimes NULL, there might be a bug somewhere! */
  if(rlr->p_uda==0)
  {
    cprintf("disp : rlr->p_uda is NULL!\n");
  }

        switchto(rlr->p_uda);
}

