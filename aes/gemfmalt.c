/*      GEMFMALT.C              09/01/84 - 06/20/85     Lee Lorenzen    */
/*      merge High C vers. w. 2.2 & 3.0         8/20/87         mdf     */ 

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
#include "taddr.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemgsxif.h"
#include "gemctrl.h"
#include "gemoblib.h"
#include "gemobed.h"
#include "geminit.h"
#include "gemrslib.h"
#include "gemgraf.h"
#include "gemfmlib.h"
#include "gemwmlib.h"
#include "optimize.h"
#include "optimopt.h"
#include "rectfunc.h"
#include "gemfmalt.h"
#include "kprint.h"

#define DBG_ALERT   0

/*
 * parameters for form_alert():
 * MUST correspond to string lengths and counts in gem_rsc.c!
 */
#define MAX_LINENUM 5
#define MAX_LINELEN 40
#define MAX_BUTNUM  3
#define MAX_BUTLEN  20


#define MSG_OFF 2
#define BUT_OFF 7
#define NUM_ALOBJS 10
#define NUM_ALSTRS 8 
#define MAX_MSGLEN 40
#define INTER_WSPACE 0
#define INTER_HSPACE 0



/* Global variables: */
const BYTE gl_nils[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
LONG     ad_nils;



/*
*       Routine to break a string into smaller strings.  Breaks occur
*       whenever an | or a ] is encountered.
* 
*       Input:  start       starting object
*               maxnum      maximum number of substrings
*               maxlen      maximum length of substring
*               alert       starting point in alert string
*       Output: pnum        number of substrings found
*               plen        maximum length of substring found
*       Returns:            pointer to next character to process
*/
#define endstring(a)    ( ((a)==']') || ((a)=='\0') )
#define endsubstring(a) ( ((a)=='|') || ((a)==']') || ((a)=='\0') )

static char *fm_strbrk(OBJECT *start,WORD maxnum,WORD maxlen,char *alert,
                           WORD *pnum,WORD *plen)
{
    int i, j, len;
    OBJECT *obj;
    char *p;

    *plen = 0;

    if (*alert == '[')              /* ignore a leading [ */
        alert++;

    for (i = 0, obj = start; i < maxnum; i++, obj++, alert++) {
        p = (char *)obj->ob_spec;
        for (j = 0; j < maxlen; j++) {
            if (endsubstring(*alert))
                break;
            *p++ = *alert++;
        }
        *p = '\0';

        len = p - (char *)obj->ob_spec;
        if (len > *plen)            /* track max substring length */
            *plen = len;

        if (!endsubstring(*alert)) {/* substring was too long */
#if DBG_ALERT
            kprintf("form_alert(): substring > %d bytes long\n",maxlen);
#endif
            while(1) {              /* eat rest of substring */
                if (endsubstring(*alert))
                    break;
                alert++;
            }
        }
        if (endstring(*alert))      /* end of all substrings */
            break;
    }
#if DBG_ALERT
    if (i >= maxnum)                /* too many substrings */
        kprintf("form_alert(): more than %d substrings\n",maxnum);
#endif

    while(1) {                      /* eat any remaining characters */
        if (endstring(*alert))
            break;
        alert++;
    }

    *pnum = (i<maxnum)?(i+1):maxnum;/* count of substrings found */

    if (*alert)                     /* if not at null byte, */
        alert++;                    /* point to next one    */

    return alert;
}


/*
*       Routine to parse a string into an icon #, multiple message
*       strings, and multiple button strings.  For example,
*
*               [0][This is some text|for the screen.][Ok|Cancel]
*               0123456
*
*       becomes:
*               icon# = 0;
*               1st msg line = This is some text
*               2nd msg line = for the screen.
*               1st button = Ok
*               2nd button = Cancel
*
*       Input:  tree        address of tree
*               palstr      pointer to alert string
*       Output: pnummsg     number of message lines
*               plenmsg     length of biggest line
*               pnumbut     number of buttons
*               plenbut     length of biggest button
*/
static void fm_parse(LONG tree, LONG palstr, WORD *picnum, WORD *pnummsg,
                         WORD *plenmsg, WORD *pnumbut, WORD *plenbut)
{
    OBJECT *obj = (OBJECT *)tree;
    char *alert = (char *)palstr;

    *picnum = alert[1] - '0';

    alert = fm_strbrk(obj+MSG_OFF,MAX_LINENUM,MAX_LINELEN,alert+3,pnummsg,plenmsg);

    fm_strbrk(obj+BUT_OFF,MAX_BUTNUM,MAX_BUTLEN,alert,pnumbut,plenbut);

    *plenbut += 1;  /* allow 1/2 character space inside each end of button */
}


static void fm_build(LONG tree, WORD haveicon, WORD nummsg, WORD mlenmsg,
                     WORD numbut, WORD mlenbut)
{
        register WORD   i, k;
        GRECT           al, ic, bt, ms;

        r_set(&al, 0, 0, 1+INTER_WSPACE, 1+INTER_HSPACE);
        r_set(&ms, 1 + INTER_WSPACE, 1 + INTER_HSPACE, mlenmsg, 1);

        if (haveicon)
        {
          r_set(&ic, 1+INTER_WSPACE, 1+INTER_HSPACE, 4, 4);
          al.g_w += ic.g_w + INTER_WSPACE + 1;
          al.g_h += ic.g_h + INTER_HSPACE + 1;
          ms.g_x = ic.g_x + ic.g_w + INTER_WSPACE + 1;
        }

        r_set(&bt, 1+INTER_WSPACE, 2+INTER_HSPACE+max(nummsg, 1), mlenbut, 1);
        if (haveicon && nummsg < 2)
        {
            bt.g_y += 1;
        }

        if (mlenmsg + al.g_w > numbut * mlenbut + (numbut-1) + 1+INTER_WSPACE)
        {
          al.g_w += mlenmsg + INTER_WSPACE + 1;
          bt.g_x = (al.g_w - numbut * mlenbut - (numbut-1)) / 2;
        }
        else
        {
          al.g_w = numbut * mlenbut + (numbut-1) + 2 * (1+INTER_WSPACE);
        }

        al.g_h = max(al.g_h, 2 + (2 * INTER_HSPACE) + nummsg + 2);

                                                /* init. root object    */
        ob_setxywh(tree, ROOT, &al);
        ad_nils = (LONG) ADDR(&gl_nils[0]);
        for(i=0; i<NUM_ALOBJS; i++)
          LBCOPY(OB_NEXT(i), ad_nils, 6);
                                                /* add icon object      */
        if (haveicon)
        {
          ob_setxywh(tree, 1, &ic);
          ob_add(tree, ROOT, 1);
        }
                                                /* add msg objects      */
        for(i=0; i<nummsg; i++)
        {
          ob_setxywh(tree, MSG_OFF+i, &ms);
          ms.g_y++;
          ob_add(tree, ROOT, MSG_OFF+i);
        }
                                                /* add button objects   */
        for(i=0; i<numbut; i++)
        {
          k = BUT_OFF+i;
          LWSET(OB_FLAGS(k), SELECTABLE | EXIT);
          LWSET(OB_STATE(k), NORMAL);
          ob_setxywh(tree, k, &bt);

          bt.g_x += mlenbut + 1;

          ob_add(tree, ROOT, k);
        }
                                                /* set last object flag */
        LWSET(OB_FLAGS(BUT_OFF+numbut-1), SELECTABLE | EXIT | LASTOB);
}


WORD fm_alert(WORD defbut, LONG palstr)
{
        register WORD   i;
        WORD            inm, nummsg, mlenmsg, numbut, mlenbut;
        LONG            tree, plong;
        GRECT           d, t;

                                                /* init tree pointer    */
#ifdef USE_GEM_RSC
        rs_gaddr(ad_sysglo, R_TREE, DIALERT, &tree);
#else
        tree = (LONG) rs_tree[DIALERT];
#endif
        gsx_mfset(ad_armice);

        fm_parse(tree, palstr, &inm, &nummsg, &mlenmsg, &numbut, &mlenbut);
        fm_build(tree, (inm != 0), nummsg, mlenmsg, numbut, mlenbut);

        if (defbut)
        {
          plong = OB_FLAGS(BUT_OFF + defbut - 1);
          LWSET(plong, LWGET(plong) | DEFAULT);
        }

        if (inm != 0)
        {
#ifdef USE_GEM_RSC
          rs_gaddr(ad_sysglo, R_BITBLK, inm-1, &plong);
#else
          plong = (LONG) &rs_fimg[inm-1];
#endif
          LLSET(OB_SPEC(1), plong);
        }
                                                /* convert to pixels    */
        for(i=0; i<NUM_ALOBJS; i++)
          rs_obfix(tree, i);
                                                /* fix up icon, 32x32   */
        LWSET(OB_TYPE(1), G_IMAGE);
        LLSET(OB_WIDTH(1), 0x00200020L);        
                                                /* center tree on screen*/
        ob_center(tree, &d);

        /* Fix 2003-09-25: Limit drawing to the screen! */
        rc_intersect(&gl_rscreen, &d);
                                                /* save screen under-   */
                                                /*   neath the alert    */
        wm_update(TRUE);
        gsx_gclip(&t);
        bb_save(&d);
                                                /* draw the alert       */
        gsx_sclip(&d);
        ob_draw(tree, ROOT, MAX_DEPTH);
                                                /* turn on the mouse    */
        ct_mouse(TRUE);
                                                /* let user pick button */
        i = fm_do(tree, 0);
                                                /* turn off mouse if necessary */
        ct_mouse(FALSE);
                                                /* restore saved screen */
        gsx_sclip(&d);
        bb_restore(&d);
        gsx_sclip(&t);
        wm_update(FALSE);
                                                /* return selection     */
        return( i - BUT_OFF + 1 );
}


