/*      GEMDISP.C       1/27/84 - 09/13/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2 & 3.0         8/19/87         mdf     */
/*      add beep in chkkbd                      11/12/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2021 The EmuTOS development team
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

/* #define ENABLE_KDEBUG */

#include "emutos.h"
#include "gemdisp.h"
#include "string.h"
#include "struct.h"
#include "aesvars.h"
#include "obdefs.h"

#include "geminput.h"
#include "gempd.h"
#include "gemgsxif.h"
#include "gemaplib.h"
#include "geminit.h"
#include "gemflag.h"
#include "gemasm.h"
#include "optimize.h"
#include "gemdosif.h"

#include "asm.h"

#define KEYMASK 0xffff0000L             /* for comparing data to KEYSTOP */
#define KEYSTOP 0x2b1c0000L             /* control-backslash */


/*
 * forkq(): put an FPD (containing a function address and a parameter) into the fork ring
 *
 * this is expected to be called with interrupts disabled
 *
 * returns -ve value iff it fails (the fork ring is full)
 */
WORD forkq(FCODE fcode, LONG fdata)
{
    FPD *f;

    if (fpcnt < NFORKS)
    {
        f = &D.g_fpdx[fpt++];
        if (fpt == NFORKS)      /* wrap pointer around  */
            fpt = 0;

        f->f_code = fcode;
        f->f_data = fdata;

        fpcnt++;
        return 0;   /* forkq() succeeded */
    }

    KDEBUG(("forkq() failed: fcode=%p, fdata=0x%08lx\n",fcode,fdata));
    return -1;      /* forkq() failed */
}


static void disp_act(AESPD *p)
{
    /* process is ready, so put him on RLR */
    p->p_stat &= ~WAITIN;
    insert_process(p, &rlr);
}


static void mwait_act(AESPD *p)
{
    /* sleep on nrl if event flags are not set */
    if (p->p_evwait & p->p_evflg)
        disp_act(p);
    else
    {
        /* good night, Mrs. Calabash, wherever you are */
        p->p_link = nrl;
        nrl = p;
    }
}


/*
 * forker(): remove all FPDs from the fork ring, calling the specified function each time
 *
 * this also handles event recording for the AES function appl_trecd()
 */
void forker(void)
{
    FPD *f;
    AESPD *oldrl;
    FPD g;

    oldrl = rlr;
    rlr = (AESPD *) -1;
    while(fpcnt)
    {
        /* critical area        */
        disable_interrupts();
        fpcnt--;
        f = &D.g_fpdx[fph++];

        /* copy FPD so an interrupt doesn't overwrite it */
        memcpy(&g, f, sizeof(FPD));
        if (fph == NFORKS)
            fph = 0;
        enable_interrupts();

        /* see if recording */
        if (gl_recd)
        {
            /* check for stop key */
            if ((g.f_code == kchange) && ((g.f_data&KEYMASK) == KEYSTOP))
                gl_recd = FALSE;

            /* if still recording, then handle event */
            if (gl_recd)
            {
                /* if it's a time event & the previously recorded one
                 * was also a time event, then coalesce them.
                 * otherwise record the event
                 */
                if ((g.f_code == tchange) && ((gl_rbuf-1)->f_code == tchange))
                {
                    (gl_rbuf-1)->f_data += g.f_data;
                }
                else
                {
                    memcpy(gl_rbuf, f, sizeof(FPD));
                    gl_rbuf++;
                    gl_rlen--;
                    if (gl_rlen <= 0)
                        gl_recd = FALSE;
                }
            }
        }

        (*g.f_code)(g.f_data);
    }

    rlr = oldrl;
}


void chkkbd(void)
{
    WORD achar, kstat;

    if (gl_play)
        return;

    kstat = gsx_kstate();
    achar = 0;

    /* only get a key if there's room in the buffer */
    if (gl_mowner->p_cda->c_q.c_cnt < KBD_SIZE)
        achar = gsx_char();     /* returns 0 if no key available */

    if (achar || (kstat != kstate))
    {
        disable_interrupts();
        forkq(kchange, MAKE_ULONG(achar, kstat));
        enable_interrupts();
    }
}


static void schedule(void)
{
    AESPD *p;

    /* run through lists until someone is on the rlr
     * or the fork list
     */
    for (;;)
    {
        /* poll the keyboard    */
        chkkbd();
        /* now move drl processes to rlr */
        while (drl)
        {
            drl = (p = drl) -> p_link;
            disp_act(p);
        }
        /* check if there is something to run */
        if (rlr || fpcnt)
            break;
#if USE_STOP_INSN_TO_FREE_HOST_CPU
        stop_until_interrupt();
#endif
    }
}


/************************************************************************/
/*                                                                      */
/*   This dispatcher is called from dsptch().                           */
/*   Its job is to determine the next task to be run.                   */
/*   This function must end by calling switchto() and will never return.*/
/*   rlr -> p_stat determines the action to perform on the process that */
/*              was in context                                          */
/*   rlr -> p_uda -> dparam is used by the action routines              */
/*                                                                      */
/************************************************************************/

void disp(void); /* called only from aes/gemasm.S */

void disp(void)
{
    AESPD *p;

    /* take the process p off the ready list root */
    p = rlr;
    rlr = p->p_link;
    KDEBUG(("disp() to \"%8.8s\"\n", rlr->p_name));

    /* based on the state of the process p, do something */
    if (p->p_stat & WAITIN)
        mwait_act(p);
    else
        disp_act(p);

    /* run through and execute all the fork processes */
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
     * so we'll never return from this
     */
    switchto(rlr->p_uda);
}
