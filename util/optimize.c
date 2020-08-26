/*      OPTIMIZE.C      1/25/84 - 06/05/85      Lee Jay Lorenzen        */
/*      merge High C vers. w. 2.2               8/25/87         mdf     */
/*      modify fs_sset                          10/30/87        mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team

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

#include "emutos.h"
#include "intmath.h"
#include "obdefs.h"
#include "optimize.h"

#include "string.h"
#include "xbiosbind.h"


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
void fmt_str(const char *instr, char *outstr)
{
    const char *p;
    char *q;

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
void unfmt_str(const char *instr, char *outstr)
{
    const char *pstr;
    char temp;

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
 *  Copies the specified string to the te_ptext field of the TEDINFO
 *  structure for (tree,object), truncating if necessary to fit
 */
void inf_sset(OBJECT *tree, WORD obj, const char *pstr)
{
    char    *text;
    TEDINFO *ted;
    OBJECT  *objptr = tree + obj;

    ted = (TEDINFO *)objptr->ob_spec;
    text = ted->te_ptext;
    strlcpy(text,pstr,ted->te_txtlen);
}


/*
 *  Copies the te_ptext field of the TEDINFO structure for (tree,object)
 *  to the specified string
 */
void inf_sget(OBJECT *tree, WORD obj, char *pstr)
{
    TEDINFO *ted;
    OBJECT  *objptr = tree + obj;

    ted = (TEDINFO *)objptr->ob_spec;
    strcpy(pstr, ted->te_ptext);
}


/*
 *  Examines 'numobj' objects in 'tree', starting at 'baseobj', looking
 *  for a SELECTED object.  Returns the relative number of the first
 *  SELECTED object, or -1 if none of the objects is selected.
 */
WORD inf_gindex(OBJECT *tree, WORD baseobj, WORD numobj)
{
    WORD    retobj;
    OBJECT  *objptr;

    for (retobj = 0, objptr = tree+baseobj; retobj < numobj; retobj++, objptr++)
    {
        if (objptr->ob_state & SELECTED)
            return retobj;
    }

    return -1;
}


/*
 *  Return 0 if cancel was selected, 1 if ok was selected, -1 if
 *  nothing was selected
 */
WORD inf_what(OBJECT *tree, WORD ok, WORD cncl)
{
    WORD    field;
    OBJECT  *objptr;

    field = inf_gindex(tree, ok, 2);

    if (field != -1)
    {
        objptr = tree + ok + field;
        objptr->ob_state = NORMAL;
        field = (field == 0);
    }

    return field;
}


/*
 *  Convert a single hex ASCII digit to the corresponding decimal number
 *
 *  Validation of input has been given up in order to minimize the size of the
 *  generated code enough to making inlining it result in smaller total code size.
 */
static UBYTE hex_dig(char achar)
{
    if (achar >= 'A')
        achar += 9;

    return achar & 0x0f;
}


/*
 *  Convert a 2-digit hex character string to a WORD value
 *
 *  Leading spaces are skipped and the next character is examined.  If
 *  it is a '\r', a value of 0 is returned.  Otherwise the next two
 *  characters (assumed to be hex digits) are returned as a WORD value.
 *  As a special case, a string of 0xff is converted to -1 (for
 *  reference, this is used in the assignment of a_aicon/a_dicon).
 *
 *  The returned pointer points to the '\r' or after the hex digits,
 *  as applicable.
 */
char *scan_2(char *pcurr, WORD *pwd)
{
    WORD temp = 0;

    while(*pcurr == ' ')
        pcurr++;

    if (*pcurr != '\r')
    {
        temp = hex_dig(*pcurr++) << 4;
        temp |= hex_dig(*pcurr++);
        if (temp == 0x00ff)
            temp = -1;
    }

    *pwd = temp;
    return pcurr;
}


/*
 * return pointer to start of last segment of path
 * (assumed to be the filename)
 */
char *filename_start(char *path)
{
    char *start = path;

    while (*path)
        if (*path++ == '\\')
            start = path;

    return start;
}


/*
 *  Routine to see if the test filename matches a standard TOS
 *  wildcard string.  For example:
 *      pattern = "*.BAT"
 *      filename = "MYFILE.BAT"
 */
WORD wildcmp(const char *pattern,const char *filename)
{
WORD i;

    /*
     * we process the name on pass1 and the extension on pass2
     */
    for (i = 0; i < 2; i++)
    {
        for ( ; *filename && (*filename != '.'); filename++)
        {
            if (*pattern == '*')
                continue;
            if ((*pattern == '?') || (*pattern == *filename))
            {
                pattern++;
                continue;
            }
            return FALSE;
        }

        /* soak up any remaining wildcard pattern characters */
        while((*pattern == '*') || (*pattern== '?'))
            pattern++;

        /* filename is nul or '.', pattern is a real character or '.' */
        if (*pattern == '.')
            pattern++;
        if (*filename == '.')
            filename++;
    }

    return (*pattern == *filename);
}
