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

#include "portab.h"
#include "machine.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "taddr.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemgsxif.h"
#include "gemoblib.h"
#include "gemobed.h"
#include "geminit.h"
#include "gemrslib.h"
#include "gemgraf.h"
#include "gemfmlib.h"
#include "gemwmlib.h"
#include "optimopt.h"
#include "rectfunc.h"


#define MSG_OFF 2
#define BUT_OFF 7
#define NUM_ALOBJS 10
#define NUM_ALSTRS 8 
#define MAX_MSGLEN 40
#define INTER_WSPACE 1
#define INTER_HSPACE 0



/* Global variables: */
BYTE     gl_nils[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
LONG     ad_nils;



/*
*       Routine to break a string into smaller strings.  Breaks occur
*       whenever an | or a ] is encountered.
*/
void fm_strbrk(LONG tree, LONG palstr, WORD stroff, WORD *pcurr_id,
               WORD *pnitem, WORD *pmaxlen)
{
        register WORD   nitem, curr_id;
        register WORD   len, maxlen;
        register BYTE   tmp;
        BYTE            *pstr;
        register BYTE   nxttmp;

        nitem = maxlen = 0; 
        curr_id = *pcurr_id;
        tmp = NULL;
        while( tmp != ']')
        {
          pstr = (BYTE *)LLGET(OB_SPEC(stroff + nitem));
          len = 0;
                                                /* get 1st char of new  */
                                                /*   string             */
          do
          {
            tmp = LBGET(palstr + curr_id);
            curr_id++;
            nxttmp = LBGET(palstr + curr_id);
            if ( (tmp == ']') ||
                 (tmp == '|') )
            {
              if (tmp == nxttmp)
                curr_id++;
              else
              {
                nxttmp = tmp;
                tmp = NULL;
              }
            }
            *(pstr + len++) = tmp;
          } while ( tmp != NULL );
          tmp = nxttmp;
          maxlen = max(len - 1, maxlen);
          nitem++;
        }
        *pcurr_id = curr_id;
        *pnitem = nitem;
        *pmaxlen = maxlen;
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
*/

void fm_parse(LONG tree, LONG palstr, WORD *picnum, WORD *pnummsg,
              WORD *plenmsg, WORD *pnumbut, WORD *plenbut)
{
        WORD            curr_id;

        *picnum = LBGET(palstr + 1) - '0';
        curr_id = 4;
        fm_strbrk(tree, palstr, MSG_OFF, &curr_id, pnummsg, plenmsg);
        curr_id++;
        fm_strbrk(tree, palstr, BUT_OFF, &curr_id, pnumbut, plenbut);
        *plenbut += 1;
}


void fm_build(LONG tree, WORD haveicon, WORD nummsg, WORD mlenmsg,
              WORD numbut, WORD mlenbut)
{
        register WORD   i, k;
        GRECT           al, ic, bt, ms;

        r_set(&al, 0, 0, 1+INTER_WSPACE, 1+INTER_HSPACE);
        r_set(&ms, 1 + INTER_WSPACE, 1 + INTER_HSPACE, mlenmsg, 1);

        if (haveicon)
        {
          r_set(&ic, 1+INTER_WSPACE, 1+INTER_HSPACE, 4, 4);
          al.g_w += ic.g_w + INTER_WSPACE;
          al.g_h += ic.g_h + INTER_HSPACE + 1;
          ms.g_x = ic.g_x + ic.g_w + INTER_WSPACE + 1;
        }

        al.g_w += ms.g_w + INTER_WSPACE + 1;
        r_set(&bt, al.g_w, 1 + INTER_HSPACE, mlenbut, 1);

        al.g_w += bt.g_w + INTER_WSPACE + 1;
        al.g_h = max(al.g_h, 2 + (2 * INTER_HSPACE) + nummsg );
        al.g_h = max(al.g_h, 2 + INTER_HSPACE + (numbut * 2) - 1);
        al.g_h |= 0xfd00;
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
          bt.g_y++;
          bt.g_y++;
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
                                                /* save screen under-   */
                                                /*   neath the alert    */
        wm_update(TRUE);
        gsx_gclip(&t);
        bb_save(&d);
                                                /* draw the alert       */
        gsx_sclip(&d);
        ob_draw(tree, ROOT, MAX_DEPTH);
                                                /* let user pick button */
        i = fm_do(tree, 0);
                                                /* restore saved screen */
        gsx_sclip(&d);
        bb_restore(&d);
        gsx_sclip(&t);
        wm_update(FALSE);
                                                /* return selection     */
        return( i - BUT_OFF + 1 );
}


