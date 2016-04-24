/*      OPTIMIZE.C      1/25/84 - 06/05/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/25/87         mdf     */
/*      modify fs_sset                          10/30/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2015 The EmuTOS development team

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
#include "obdefs.h"
#include "struct.h"
#include "gemlib.h"
#include "geminit.h"
#include "gemrslib.h"
#include "rectfunc.h"
#include "optimopt.h"
#include "optimize.h"

#include "string.h"
#include "xbiosbind.h"


/*
 *  sound() - an internal routine used by the AES and desktop
 *
 *  This routine has two functions:
 *  1. play a sound (iff isfreq==TRUE)
 *      'freq' is the frequency in Hz; must be > 0
 *      'dura' is the duration in ~250msec units: must be < 32
 *  2. enable/disable sound playing by this function (iff isfreq==FALSE)
 *      'freq' is the control:
 *          -1 => do nothing
 *           0 => enable
 *           otherwise disable
 *      'dura' is not used
 *
 *  in both cases, the function returns the current disabled state, as
 *  set previously (0 => enabled, otherwise disabled)
 */
WORD sound(WORD isfreq, WORD freq, WORD dura)
{
    static UBYTE snddat[16];
    static WORD disabled;

    if (isfreq)     /* Play a sound? */
    {
        if (disabled)
            return 1;

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

    return disabled;
}


/*
 *  Convert 'normal' filename to a value suitable for formatting
 *  with a TEDINFO FFFFFFFF.FFF text string.  For example:
 *      . 'SAMPLE.PRG' is converted to 'SAMPLE  PRG'
 *      . 'TESTPROG.C' is converted to 'TESTPROGC'
 *      . 'TEST' is converted to 'TEST'
 *
 *  This code also handles input filenames that are not in 8.3
 *  format that may be provided by e.g. Hatari GEMDOS drive emulation.
 *  For example:
 *      . 'TESTWINDOW.C' is converted to 'TESTWINDC'
 *      . 'TEST.A.B.C' is converted to 'TEST    A.B'
 *      . 'TESTTESTTEST' is converted to 'TESTTEST'
 */
void fmt_str(BYTE *instr,BYTE *outstr)
{
    BYTE *p, *q;

    /* copy up to 8 bytes before the (first) dot (we eat excess bytes) */
    for (p = instr, q = outstr; *p; p++)
    {
        if (*p == '.')
        {
            p++;
            break;
        }
        if (q-outstr < 8)
            *q++ = *p;
    }

    /* if any extension present, fill out with spaces, then copy extension */
    if (*p)
    {
        while(q-outstr < 8)
            *q++ = ' ';
        while((q-outstr < 11) && *p)
            *q++ = *p++;
    }

    *q = '\0';  /* always nul-terminate */
}


/*
 *  Does the reverse of fmt_str() above.  For example,
 *      'SAMPLE  PRG' is converted to 'SAMPLE.PRG'.
 */
void unfmt_str(BYTE *instr, BYTE *outstr)
{
    BYTE    *pstr, temp;

    pstr = instr;
    while(*pstr && ((pstr - instr) < 8))
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
    *outstr = '\0';
}


/*
 *  Insert an unsigned long value into the te_ptext field of the TEDINFO
 *  structure for the specified object, truncating if necessary
 */
void inf_numset(LONG tree, WORD obj, ULONG value)
{
    WORD len;
    TEDINFO *ted;
    OBJECT  *objptr = ((OBJECT *)tree) + obj;

    ted = (TEDINFO *)objptr->ob_spec;
    len = ted->te_txtlen - 1;

    sprintf(ted->te_ptext,"%*.*lu",len,len,value);
}


/*
 *  Copies the specified string to the te_ptext field of the TEDINFO
 *  structure for (tree,object), truncating if necessary to fit
 */
void inf_sset(LONG tree, WORD obj, BYTE *pstr)
{
    BYTE    *text;
    TEDINFO *ted;
    OBJECT  *objptr = ((OBJECT *)tree) + obj;

    ted = (TEDINFO *)objptr->ob_spec;
    text = (BYTE *)ted->te_ptext;
    strlcpy(text,pstr,ted->te_txtlen);
}


/*
 *  Copies the te_ptext field of the TEDINFO structure for (tree,object)
 *  to the specified string
 */
void inf_sget(LONG tree, WORD obj, BYTE *pstr)
{
    TEDINFO *ted;
    OBJECT  *objptr = ((OBJECT *)tree) + obj;

    ted = (TEDINFO *)objptr->ob_spec;
    strcpy(pstr, (BYTE *)ted->te_ptext);
}


/*
 *  Examines 'numobj' objects in 'tree', starting at 'baseobj', looking
 *  for a SELECTED onject.  Returns the relative number of the first
 *  SELECTED object, or -1 if none of the objects is selected.
 */
WORD inf_gindex(LONG tree, WORD baseobj, WORD numobj)
{
    WORD    retobj;
    OBJECT  *objptr;

    for (retobj = 0, objptr = ((OBJECT *)tree)+baseobj; retobj < numobj; retobj++, objptr++)
    {
        if (objptr->ob_state & SELECTED)
            return retobj;
    }

    return -1;
}


/*
 *  Return 0 if cancel was selected, 1 if okay was selected, -1 if
 *  nothing was selected
 */
WORD inf_what(LONG tree, WORD ok, WORD cncl)
{
    WORD    field;
    OBJECT  *objptr;

    field = inf_gindex(tree, ok, 2);

    if (field != -1)
    {
        objptr = ((OBJECT *)tree) + ok + field;
        objptr->ob_state = NORMAL;
        field = (field == 0);
    }

    return field;
}


/*
 *  Routine to see if the test filename matches one of a set of
 *  comma delimited wildcard strings.  For example:
 *      pwld = "*.COM,*.EXE,*.BAT"
 *      ptst = "MYFILE.BAT"
 */
WORD wildcmp(BYTE *pwld, BYTE *ptst)
{
    BYTE    *pwild;
    BYTE    *ptest;

    /* skip over *.*, and *.ext faster */
    while(*pwld)
    {
        ptest = ptst;
        pwild = pwld;

        /* move on to next set of wildcards */
        pwld = scasb(pwld, ',');
        if (*pwld)
            pwld++;

        /* start the checking */
        if (pwild[0] == '*')
        {
            if (pwild[2] == '*')        /* "*.*" matches everything */
                return TRUE;
            else
            {
                pwild = &pwild[2];
                ptest = scasb(ptest, '.');
                if (*ptest)
                    ptest++;
            }
        }

        /* finish off comparison */
        while(*ptest && *pwild && (*pwild != ','))
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

        /* eat up remaining wildcard chars */
        while((*pwild == '*') || (*pwild == '?') || (*pwild == '.'))
            pwild++;

        /* if no part of wildcard or test is left then it's a match */
        if (((*pwild == '\0') || (*pwild == ',')) && !*ptest)
            return TRUE;
    }

    return FALSE;
}


/*
 *  Inserts character 'chr' into the string pointed to 'str', at
 *  position 'pos' (positions are relative to the start of the
 *  string; inserting at position 0 means inserting at the start
 *  of the string).  'tot_len' gives the maximum length the string
 *  can grow to; if necessary, the string will be truncated after
 *  inserting the character.
 */
void ins_char(BYTE *str, WORD pos, BYTE chr, WORD tot_len)
{
    WORD ii, len;

    len = strlen(str);

    for (ii = len; ii > pos; ii--)
        str[ii] = str[ii-1];
    str[ii] = chr;
    if (len+1 < tot_len)
        str[len+1] = '\0';
    else
        str[tot_len-1] = '\0';
}
