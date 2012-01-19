/*      OPTIMIZE.C      1/25/84 - 06/05/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/25/87         mdf     */ 
/*      modify fs_sset                          10/30/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002 The EmuTOS development team

*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 3.0
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1987                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#include "config.h"
#include "portab.h"
#include "compat.h"
#include "taddr.h"
#include "obdefs.h"
#include "struct.h"
#include "gemlib.h"
#include "geminit.h"
#include "gemrslib.h"
#include "rectfunc.h"
#include "optimopt.h"

#include "string.h"
#include "xbiosbind.h"



WORD sound(WORD isfreq, WORD freq, WORD dura)
{
    static UBYTE snddat[16];
    static WORD disabled;

    if (isfreq)     /* Play a sound? */
    {
      if (disabled)  return 1;

      snddat[0] = 0;  snddat[1] = (125000L / freq);       /* channel A pitch lo */
      snddat[2] = 1;  snddat[3] = (125000L / freq) >> 8;  /* channel A pitch hi */
      snddat[4] = 7;  snddat[5] = (isfreq ? 0xFE : 0xFF);
      snddat[6] = 8;  snddat[7] = 0x10;                   /* amplitude: envelop */
      snddat[8] = 11;  snddat[9] = 0;                     /* envelope lo */
      snddat[10] = 12;  snddat[11] = dura * 8;            /* envelope hi */
      snddat[12] = 13;  snddat[13] = 9;                   /* envelope type */
      snddat[14] = 0xFF;  snddat[15] = 0;

      Dosound((LONG)snddat);
    }
    else            /* else enable/disable sound */
    {
      if (freq != -1)
        disabled = freq;
    }

    return(disabled);
}


void rc_constrain(GRECT *pc, GRECT *pt)
{
          if (pt->g_x < pc->g_x)
            pt->g_x = pc->g_x;
          if (pt->g_y < pc->g_y)
            pt->g_y = pc->g_y;
          if ((pt->g_x + pt->g_w) > (pc->g_x + pc->g_w))
            pt->g_x = (pc->g_x + pc->g_w) - pt->g_w;
          if ((pt->g_y + pt->g_h) > (pc->g_y + pc->g_h))
            pt->g_y = (pc->g_y + pc->g_h) - pt->g_h;
}


void rc_union(GRECT *p1, GRECT *p2)
{
        WORD            tx, ty, tw, th;

        tw = max(p1->g_x + p1->g_w, p2->g_x + p2->g_w);
        th = max(p1->g_y + p1->g_h, p2->g_y + p2->g_h);
        tx = min(p1->g_x, p2->g_x);
        ty = min(p1->g_y, p2->g_y);
        p2->g_x = tx;
        p2->g_y = ty;
        p2->g_w = tw - tx;
        p2->g_h = th - ty;
}


WORD rc_intersect(GRECT *p1, GRECT *p2)
{
        WORD            tx, ty, tw, th;

        tw = min(p2->g_x + p2->g_w, p1->g_x + p1->g_w);
        th = min(p2->g_y + p2->g_h, p1->g_y + p1->g_h);
        tx = max(p2->g_x, p1->g_x);
        ty = max(p2->g_y, p1->g_y);
        p2->g_x = tx;
        p2->g_y = ty;
        p2->g_w = tw - tx;
        p2->g_h = th - ty;
        return( (tw > tx) && (th > ty) );
}


BYTE *strscn(BYTE *ps, BYTE *pd, BYTE stop)
{
        while ( (*ps) &&
                (*ps != stop) )
          *pd++ = *ps++;
        return(pd);
}



/*
*       Strip out period and turn into raw data.
*/
void fmt_str(BYTE *instr, BYTE *outstr)
{
        WORD            count;
        BYTE            *pstr;

        pstr = instr;
        while( (*pstr) && (*pstr != '.') )
          *outstr++ = *pstr++;
        if (*pstr)
        {
          count = 8 - (pstr - instr);
          while ( count-- )
            *outstr++ = ' ';
          pstr++;
          while (*pstr)
            *outstr++ = *pstr++;
        }
        *outstr = NULL;
}


/*
*       Insert in period and make into true data.
*/
void unfmt_str(BYTE *instr, BYTE *outstr)
{
        BYTE            *pstr, temp;

        pstr = instr;
        while( (*pstr) && ((pstr - instr) < 8) )
        {
          temp = *pstr++;
          if (temp != ' ')
            *outstr++ = temp;
        }
        if (*pstr)
        {
          *outstr++ = '.';
          while (*pstr)
            *outstr++ = *pstr++;
        }
        *outstr = NULL;
}


void fs_sset(LONG tree, WORD obj, LONG pstr, LONG *ptext, WORD *ptxtlen)
{
        LONG            spec;
        WORD            len;

    spec = LLGET(OB_SPEC(obj));   /*!!!*/
        *ptext =LLGET(spec);

        *ptxtlen = LWGET( spec + 24 );
        len = LSTRLEN(pstr);                    /* allow for null       */
        len = min(len, *ptxtlen - 1);
        LBCOPY(*ptext, pstr, len);
        LBSET(*ptext+len, '\0');                /* add null             */
}


void inf_sset(LONG tree, WORD obj, BYTE *pstr)
{
        LONG            text;
        WORD            txtlen;

        fs_sset(tree, obj, ADDR(pstr), &text, &txtlen);
}


void fs_sget(LONG tree, WORD obj, LONG pstr)
{
        LONG            ptext;

        ptext=LLGET(OB_SPEC(obj));  /*!!!*/
        ptext = LLGET( ptext );
        strcpy((char *)pstr, (char *)ptext);
}



void inf_sget(LONG tree, WORD obj, BYTE *pstr)
{
        fs_sget(tree, obj, ADDR(pstr));
}



void inf_fldset(LONG tree, WORD obj, UWORD testfld, UWORD testbit,
                UWORD truestate, UWORD falsestate)
{
        LWSET(OB_STATE(obj), (testfld & testbit) ? truestate : falsestate);
}



WORD inf_gindex(LONG tree, WORD baseobj, WORD numobj)
{
        WORD            retobj;

        for (retobj=0; retobj < numobj; retobj++)
        {
          if (LWGET(OB_STATE(baseobj+retobj)) & SELECTED)
            return(retobj);
        }
        return(-1);
}


/*
*       Return 0 if cancel was selected, 1 if okay was selected, -1 if
*       nothing was selected.
*/

WORD inf_what(LONG tree, WORD ok, WORD cncl)
{
        WORD            field;

        field = inf_gindex(tree, ok, 2);

        if (field != -1)
        {
          LWSET(OB_STATE(ok + field), NORMAL);
          field = (field == 0);
        }
        return(field);
}


/*
*       Routine to see if the test filename matches one of a set of 
*       comma delimited wildcard strings.
*               e.g.,   pwld = "*.COM,*.EXE,*.BAT"
*                       ptst = "MYFILE.BAT"
*/
WORD wildcmp(BYTE *pwld, BYTE *ptst)
{
        BYTE            *pwild;
        BYTE            *ptest;
                                                /* skip over *.*, and   */
                                                /*   *.ext faster       */
        while(*pwld)
        {
          ptest = ptst;
          pwild = pwld;
                                                /* move on to next      */
                                                /*   set of wildcards   */
          pwld = scasb(pwld, ',');
          if (*pwld)
            pwld++;
                                                /* start the checking   */
          if (pwild[0] == '*')
          {
            if (pwild[2] == '*')
              return(TRUE);
            else
            {
              pwild = &pwild[2];
              ptest = scasb(ptest, '.');
              if (*ptest)
                ptest++;
            }
          }
                                                /* finish off comparison*/
          while( (*ptest) && 
                 (*pwild) &&
                 (*pwild != ',') )
          {
            if (*pwild == '?')
            {
               pwild++;
               if (*ptest != '.')
                 ptest++;
            }
            else
            {
              if (*pwild == '*')
              {
                if (*ptest != '.')
                  ptest++;
                else            
                  pwild++;
              }
              else
              {
                if (*ptest == *pwild)
                {
                  pwild++;
                  ptest++;
                }
                else
                  break;
              }
            }
          }
                                                /* eat up remaining     */
                                                /*   wildcard chars     */
          while( (*pwild == '*') ||
                 (*pwild == '?') ||
                 (*pwild == '.') )
            pwild++;
                                                /* if any part of wild- */
                                                /*   card or test is    */
                                                /*   left then no match */
          if ( ((*pwild == NULL) || (*pwild == ',')) && 
               (!*ptest) )
            return( TRUE );
        }
        return(FALSE);
}



/*
*       Routine to insert a character in a string by
*/
void ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len)
{
        register WORD   ii, len;

        len = strlen(str);

        for (ii = len; ii > pos; ii--)
          str[ii] = str[ii-1];
        str[ii] = chr;
        if (len+1 < tot_len)
          str[len+1] = NULL;
        else
          str[tot_len-1] = NULL;
}

