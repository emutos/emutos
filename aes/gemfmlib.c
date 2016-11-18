/*      GEMFMLIB.C      03/15/84 - 06/16/85     Gregg Morris            */
/*      merge High C vers. w. 2.2 & 3.0         8/20/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2016 The EmuTOS development team
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
#include "gem_rsc.h"

#include "gemwmlib.h"
#include "gemrslib.h"
#include "geminput.h"
#include "gemctrl.h"
#include "gemoblib.h"
#include "gemobjop.h"
#include "gemgrlib.h"
#include "gemevlib.h"
#include "gemgraf.h"
#include "gemobed.h"
#include "optimize.h"
#include "gemfmalt.h"
#include "geminit.h"
#include "gemmnlib.h"
#include "gemfmlib.h"
#include "gemgsxif.h"
#include "scancode.h"


#define FORWARD 0
#define BACKWARD 1
#define DEFLT 2


/* Global variables: */
WORD     ml_ocnt;    /* Needs to be 0 initially! */


/* Local variables: */
static LONG     ml_mnhold;
static GRECT    ml_ctrl;
static AESPD    *ml_pmown;
static BYTE     alert_str[256]; /* must be long enough for longest alert in gem.rsc */

/*
 * The following arrays are used by eralert() to generate values to
 * pass to fm_show().  eralert() is passed an alert number N which is
 * used to index into these arrays:
 *  ml_alrt[N]  contains the object number of the alert string within
 *              the resource
 *  ml_pwlv[N]  contains a 2-byte value PPLL where:
 *              LL is the alert level (0-3 currently supported)
 *              PP is a drive letter indicator; if non-zero, a pointer
 *              to the drive letter corresponding to drive number D is
 *              passed to fm_show(); otherwise a null pointer is passed
 *
 * These arrays must be synchronized with "err_tbl" in gemdosif.S.
 */
static const WORD ml_alrt[] =
    {AL00CRT,AL01CRT,AL02CRT,AL03CRT,AL04CRT,AL05CRT};
static const WORD ml_pwlv[] =
    {0x0102,0x0102,0x0102,0x0101,0x0002,0x0001};



void fm_own(WORD beg_ownit)
{
    if (beg_ownit)
    {
        wm_update(TRUE);
        if (ml_ocnt == 0)
        {
            ml_mnhold = gl_mntree;
            gl_mntree = 0x0L;
            get_ctrl(&ml_ctrl);
            get_mown(&ml_pmown);
            ct_chgown(rlr, &gl_rscreen);
        }
        ml_ocnt++;
    }
    else
    {
        ml_ocnt--;
        if (ml_ocnt == 0)
        {
            ct_chgown(ml_pmown, &ml_ctrl);
            gl_mntree = ml_mnhold;
        }
        wm_update(FALSE);
    }
}


/*
 *  Routine to find the next editable text field, or a field that
 *  is marked as a default return field.
 */
static WORD find_obj(LONG tree, WORD start_obj, WORD which)
{
    WORD obj, flag, state, inc;
    WORD            theflag;

    obj = 0;
    flag = EDITABLE;
    inc = 1;
    switch(which)
    {
    case BACKWARD:
        inc = -1;
        /* fall thru */
    case FORWARD:
        obj = start_obj + inc;
        break;
    case DEFLT:
        flag = DEFAULT;
        break;
    }

    while (obj >= 0)
    {
        state = ob_fs(tree, obj, &theflag);
        if (!(theflag & HIDETREE) && !(state & DISABLED))
        {
            if (theflag & flag)
                return(obj);
        }
        if (theflag & LASTOB)
            obj = -1;
        else
            obj += inc;
    }

    return start_obj;
}


static WORD fm_inifld(LONG tree, WORD start_fld)
{
    /* position cursor on the starting field */
    if (start_fld == 0)
        start_fld = find_obj(tree, 0, FORWARD);

    return start_fld;
}


WORD fm_keybd(LONG tree, WORD obj, WORD *pchar, WORD *pnew_obj)
{
    WORD    direction;

    /* handle character */
    direction = -1;
    switch(*pchar)
    {
    case RETURN:
    case ENTER:
        obj = 0;
        direction = DEFLT;
        break;
    case ARROW_UP:
        direction = BACKWARD;
        break;
    case TAB:       /* shift-tab, ctrl-tab, alt-tab have the same scancode */
    case ARROW_DOWN:
        direction = FORWARD;
        break;
    }

    if (direction != -1)
    {
        *pchar = 0x00;
        *pnew_obj = find_obj(tree, obj, direction);
        if ((direction == DEFLT) && (*pnew_obj != 0))
        {
            OBJECT *objptr = ((OBJECT *)tree) + *pnew_obj;
            ob_change(tree, *pnew_obj, objptr->ob_state | SELECTED, TRUE);
            return FALSE;
        }
    }

    return TRUE;
}


WORD fm_button(LONG tree, WORD new_obj, WORD clks, WORD *pnew_obj)
{
    WORD    tobj;
    WORD    orword;
    WORD    parent, state, flags;
    WORD    cont, junk, tstate, tflags;
    WORD    rets[6];
    OBJECT  *objptr;

    cont = TRUE;
    orword = 0;

    state = ob_fs(tree, new_obj, &flags);

    /* handle touchexit case: if double click, then set high bit */
    if (flags & TOUCHEXIT)
    {
        if (clks == 2)
            orword = 0x8000;
        cont = FALSE;
    }

    /* handle selectable case */
    if ((flags & SELECTABLE) && !(state & DISABLED))
    {
        /* if it's a radio button */
        if (flags & RBUTTON)
        {
            /* check siblings to find and turn off the old RBUTTON */
            parent = get_par(tree, new_obj, &junk);
            objptr = ((OBJECT *)tree) + parent;
            tobj = objptr->ob_head;
            while (tobj != parent)
            {
                tstate = ob_fs(tree, tobj, &tflags);
                if ((tflags & RBUTTON) &&
                    ((tstate & SELECTED) || (tobj == new_obj)))
                {
                    if (tobj == new_obj)
                        state = tstate |= SELECTED;
                    else
                        tstate &= ~SELECTED;
                    ob_change(tree, tobj, tstate, TRUE);
                }
                objptr = ((OBJECT *)tree) + tobj;
                tobj = objptr->ob_next;
            }
        }
        else
        {   /* turn on new object   */
            if (gr_watchbox(tree, new_obj, state^SELECTED, state))
                state ^= SELECTED;
        }
        /* if not touchexit then wait for button up */
        if (cont && (flags & (SELECTABLE | EDITABLE)))
            ev_button(1, 0x00001, 0x0000, rets);
    }

    /* see if this selection gets us out */
    if ((state & SELECTED) && (flags & EXIT))
        cont = FALSE;

    /* handle click on another editable field */
    if (cont &&
        ((flags & HIDETREE) || (state & DISABLED) || !(flags & EDITABLE)))
        new_obj = 0;

    *pnew_obj = new_obj | orword;

    return cont;
}


/*
 *  ForM DO routine to allow the user to interactively fill out a
 *  form.  The cursor is placed at the starting field.  This routine
 *  returns the object that caused the exit to occur
 */
WORD fm_do(LONG tree, WORD start_fld)
{
    WORD    edit_obj;
    WORD    next_obj;
    WORD    which, cont;
    WORD    idx;
    WORD    rets[6];

    /* grab ownership of screen and mouse */
    fm_own(TRUE);

    /* flush keyboard */
    fq();

    /* set clip so we can draw chars, and invert buttons */
    gsx_sclip(&gl_rfull);

    /* determine which is the starting field to edit */
    next_obj = fm_inifld(tree, start_fld);
    edit_obj = 0;

    /* interact with user */
    cont = TRUE;
    while(cont)
    {
        /* position cursor on the selected editing field */
        if ((next_obj != 0) && (edit_obj != next_obj))
        {
            edit_obj = next_obj;
            next_obj = 0;
            ob_edit(tree, edit_obj, 0, &idx, EDINIT);
        }
        /* wait for mouse or key */
        which = ev_multi(MU_KEYBD | MU_BUTTON, NULL, NULL,
                         0x0L, 0x0002ff01L, 0x0L, rets);

        /* handle keyboard event */
        if (which & MU_KEYBD)
        {
            cont = fm_keybd(tree, edit_obj, &rets[4], &next_obj);
            if (rets[4])
              ob_edit(tree, edit_obj, rets[4], &idx, EDCHAR);
        }

        /* handle button event */
        if (which & MU_BUTTON)
        {
            next_obj = ob_find(tree, ROOT, MAX_DEPTH, rets[0], rets[1]);
            if (next_obj == NIL)
            {
                sound(TRUE, 440, 2);
                next_obj = 0;
            }
            else
                cont = fm_button(tree, next_obj, rets[5], &next_obj);
        }

        /* handle end of field clean up */
        if (!cont ||
            ((next_obj != 0) && (next_obj != edit_obj)))
        {
            ob_edit(tree, edit_obj, 0, &idx, EDEND);
        }
    }

    /* give up mouse and screen ownership */
    fm_own(FALSE);

    /* return exit object */
    return next_obj;
}


/*
 *  Form DIALogue routine to handle visual effects of drawing and
 *  undrawing a dialogue
 */
WORD fm_dial(WORD fmd_type, GRECT *pt)
{
    /* adjust tree position */
    gsx_sclip(&gl_rscreen);
    switch(fmd_type)
    {
    case FMD_START:
        /* grab screen sync or some other mutual exclusion method */
        break;
    case FMD_FINISH:
        /* update certain portion of the screen */
        w_drawdesk(pt);
        w_update(DESKWH, pt, DESKWH, FALSE, TRUE);
        break;
    }

    return TRUE;
}


/*
 * issue form_alert(), with optional drive letter
 */
WORD fm_show(WORD string, WORD *pwd, WORD level)
{
    BYTE    *ad_alert;

    ad_alert = rs_str(string);
    if (pwd)
    {
        sprintf(alert_str, ad_alert, *pwd);
        ad_alert = alert_str;
    }

    return fm_alert(level, (LONG)ad_alert);
}


/*
 * eralert() is called by the graphics critical alert handler
 * in gemdosif.S when a DOS error occurs.  it issues an alert
 * selected by the lookup tables ml_alrt[] & ml_pwlv[].
 *
 * input:   n   alert number (0-5), not checked
 *          d   drive number
 * returns: FALSE   user selected button 1 of the alert
 *                  (assumed to be Cancel)
 *          TRUE    user selected button 2 or 3 of the alert
 *                  (assumed to be Retry)
 */
WORD eralert(WORD n, WORD d)
{
    WORD    level;
    WORD    drive_let;
    WORD    *pwd;

    drive_let = 'A' + d;

    level = ml_pwlv[n] & 0x00FF;
    pwd = (ml_pwlv[n] & 0xFF00) ? &drive_let : NULL;

    if (fm_show(ml_alrt[n], pwd, level) == 1)
        return FALSE;

    /* retry requested, we'll be busy again */
    set_mouse_to_hourglass();   /* set mouse to hourglass */
    return TRUE;
}


WORD fm_error(WORD n)             /* n = dos error number */
{
    WORD    ret, string;

    if (n > 63)
        return FALSE;

    switch (n)
    {
    case 2:
    case 18:
    case 3:
        string = AL18ERR;
        break;
    case 4:
        string = AL04ERR;
        break;
    case 5:
        string = AL05ERR;
        break;
    case 15:
        string = AL15ERR;
        break;
    case 8:
    case 10:
    case 11:
        string = AL08ERR;
        break;
    default:
        string = ALXXERR;
        break;
    }

    ret = fm_show(string, ((string == ALXXERR) ? &n : 0), 1);

    return (ret != 1);
}
