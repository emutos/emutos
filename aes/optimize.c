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

#include "portab.h"
#include "machine.h"
#include "taddr.h"
#include "obdefs.h"

#include "geminit.h"
#include "gemrslib.h"
#include "rectfunc.h"
#include "optimopt.h"


GLOBAL BYTE     gl_rsname[16];


WORD sound(WORD isfreq, WORD freq, WORD dura)
{
#if 0
        WORD            cnt;

        intin[0] = freq;
        intin[1] = dura;
        if (isfreq)
        {
                                                /* make a sound         */
          contrl[5] = 61;
          cnt = 2;
        }
        else
        {
                                                /* get / set mute status*/
          contrl[5] = 62;
          cnt = 1;
        }
        gsx_ncode(5, 0, cnt);
        return(intout[0]);
#endif
        return(0);
}


/*
WORD bit_num(UWORD flag)
{
        WORD            i;
        UWORD           test;

        if ( !flag )
          return(-1);
        for (i=0,test=1; !(flag & test); test <<= 1,i++);
        return(i);
}
*/


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


/*
        WORD
mid(lo, val, hi)
        WORD            lo, val, hi;
{
        if (val < lo)
          return(lo);
        if (val > hi)
          return(hi);
        return(val);
}
*/


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
        LSTCPY(pstr, ptext);
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


void merge_str(BYTE *pdst, BYTE *ptmp, UWORD parms[])
{
        WORD            num;
        WORD            do_value;
        BYTE            lholder[12];
        BYTE            *pnum, *psrc;
        LONG            lvalue, divten;
        WORD            digit;

        num = 0;
        lvalue = 0;

        while(*ptmp)
        {
          if (*ptmp != '%')
            *pdst++ = *ptmp++;
          else
          {
            ptmp++;
            do_value = FALSE;
            switch(*ptmp++)
            {
              case '%':
                *pdst++ = '%';
                break;
              case 'L':
                lvalue = *((LONG *) &parms[num]);
                num += 2;
                do_value = TRUE;
                break;
              case 'W':
                lvalue = parms[num];
#if MC68K
                num += 2;
#endif
#if I8086
                num++;
#endif  
                do_value = TRUE;
                break;
              case 'S':
#if MC68K
                psrc = (BYTE *) *((LONG *)&parms[num]);  /*???*/
                num += 2;
#endif
#if I8086
                psrc = (BYTE *) parms[num]; 
                num++;
#endif  
                while(*psrc)
                  *pdst++ = *psrc++;
                break;
            }
            if (do_value)
            {
              pnum = &lholder[0];
              while(lvalue)
              {
                divten = lvalue / 10;
                digit = (WORD) lvalue - (divten * 10);
                *pnum++ = '0' + digit;
                lvalue = divten;
              }
              if (pnum == &lholder[0])
                *pdst++ = '0';
              else
              {
                while(pnum != &lholder[0])
                  *pdst++ = *--pnum;
              }
            }
          }
        }
        *pdst = NULL;
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
        REG WORD        ii, len;

        len = strlen(str);

        for (ii = len; ii > pos; ii--)
          str[ii] = str[ii-1];
        str[ii] = chr;
        if (len+1 < tot_len)
          str[len+1] = NULL;
        else
          str[tot_len-1] = NULL;
}


/*
*       Used to get strings of 16 bytes or less from resource.
*/
#if MULTIAPP
BYTE *op_gname(WORD index)
{
        LONG    pname;
/* define R_STRING 5    */
        rs_gaddr(ad_sysglo, 5, index, &pname);
        LSTCPY(ADDR(&gl_rsname[0]), pname);
        return(&gl_rsname[0]);
}
#endif

