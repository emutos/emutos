/*      GEMDISP.C       1/27/84 - 09/13/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */
/*      add beep in chkkbd                      11/12/87        mdf     */

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
#include "gemdisp.h"
#include "compat.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"

#include "geminput.h"
#include "gempd.h"
#include "gemgsxif.h"
#include "gemaplib.h"
#include "geminit.h"
#include "gemflag.h"
#include "gemasm.h"
#include "optimize.h"
#include "gemdosif.h"
#include "kprint.h"

#include "asm.h"

#define DBG_GEMDISP 0

#define KEYSTOP 0x00002b1cL                     /* control backslash    */


/* forkq puts a fork block with a routine in the fork ring      */

void forkq(FCODE fcode, LONG fdata)
{
        register FPD    *f;
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


static void disp_act(AESPD *p)
{
                                                /* process is ready,    */
                                                /*   so put him on RLR  */
        p->p_stat &= ~WAITIN;
        insert_process(p, &rlr);
}


static void mwait_act(AESPD *p)
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



void forker(void)
{
        register FPD    *f;
        register AESPD  *oldrl;
        FPD             g;

        oldrl = rlr;
        rlr = (AESPD *) -1;
        while(fpcnt)
        {
/* critical area        */
          cli();
          fpcnt--;
          f = &D.g_fpdx[fph++];
                                        /* copy FPD so an interrupt     */
                                        /*  doesn't overwrite it.       */
          memcpy(&g, f, sizeof(FPD));
          if (fph == NFORKS)
            fph = 0;
          sti();
/* */
                                                /* see if recording     */
          if (gl_recd)
          {
                                                  /* check for stop key */
            if ((g.f_code == kchange) && ((UWORD)g.f_data == KEYSTOP))
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
              if ((g.f_code == tchange) && ((gl_rbuf-1)->f_code == tchange))
              {
                (gl_rbuf-1)->f_data += g.f_data;
              }
              else
              {
                memcpy(gl_rbuf, f, sizeof(FPD));
                gl_rbuf++;
                gl_rlen--;
                gl_recd = gl_rlen;
              }
            }
          }
          (*g.f_code)(g.f_data);
        }
        rlr = oldrl;
}


void chkkbd(void)
{
        register WORD   achar, kstat;
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
            forkq(kchange, MAKE_ULONG(achar, kstat));
            sti();
          }
        }
}



static void schedule(void)
{
        register AESPD  *p;

                                                /* run through lists    */
                                                /*   until someone is   */
                                                /*   on the rlr or the  */
                                                /*   fork list          */
        do
        {
#if USE_STOP_INSN_TO_FREE_HOST_CPU
          stop_until_interrupt();
#endif
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
/*   This dispatcher is called frop dsptch().                           */
/*   Its job is to determine the next task to be run.                   */
/*   This function must end by calling switchto() and will never return.*/
/*   rlr -> p_stat determines the action to perform on the process that */
/*              was in context                                          */
/*   rlr -> p_uda -> dparam is used by the action routines              */
/*                                                                      */
/************************************************************************/

void disp(void)
{
        register AESPD  *p;

                                                /* take the process p   */
                                                /*   off the ready list */
                                                /*   root               */
        p = rlr;
        rlr = p->p_link;
#if DBG_GEMDISP
        kprintf("disp() to \"%8s\"\n", rlr->p_name);
#endif
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
          {
            forker();
          }
          schedule();
        } while (fpcnt);


/* switchto() is a machine dependent routine which:
 *      1) restores machine state
 *      2) clear "indisp" semaphore
 *      3) returns to appropriate address
 *              so we'll never return from this
 */
        switchto(rlr->p_uda);
}
