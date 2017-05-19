/*      GEMOBED.C       05/29/84 - 06/20/85             Gregg Morris    */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2017 The EmuTOS development team
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
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "intmath.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemoblib.h"
#include "gemgraf.h"
#include "geminit.h"
#include "gemrslib.h"
#include "optimize.h"
#include "optimopt.h"
#include "gemobed.h"

#include "string.h"
#include "scancode.h"


static TEDINFO  edblk;


static void ob_getsp(OBJECT *tree, WORD obj, TEDINFO *pted)
{
    LONG    spec;
    OBJECT  *objptr = tree + obj;

    spec = objptr->ob_spec;
    if (objptr->ob_flags & INDIRECT)
        spec = *((LONG *)spec);
    *pted = *(TEDINFO *)spec;   /* return TEDINFO */
}


void ob_center(OBJECT *tree, GRECT *pt)
{
    WORD    xd, yd, wd, hd;
    OBJECT  *root = tree;

    wd = root->ob_width;
    hd = root->ob_height;
    xd = (gl_width - wd) / 2;
    yd = gl_hbox + ((gl_height - gl_hbox - hd) / 2);
    root->ob_x = xd;
    root->ob_y = yd;

    /* account for outline or shadow */
    if (root->ob_state & (OUTLINED|SHADOWED))
    {
        xd -= 3;
        yd -= 3;
        wd += 6;
        hd += 6;
    }
    r_set(pt, xd, yd, wd, hd);
}


/*
 *  Routine to scan thru a string looking for the occurrence of the
 *  specified character.  IDX is updated as we go based on the '_'
 *  characters that are encountered.  The reason for this routine is
 *  so that if a user types a template character during field entry,
 *  the cursor will jump to the first raw string underscore after
 *  that character.
 */
static WORD scan_to_end(BYTE *pstr, WORD idx, BYTE chr)
{
    while(*pstr && (*pstr != chr))
    {
        if (*pstr++ == '_')
            idx++;
    }

    return idx;
}


/*
 *  Routine that returns a format/template string relative number
 *  for the position that was input (in raw string relative numbers).
 *  The returned position will always be right before an '_'.
 */
static WORD find_pos(BYTE *str, WORD pos)
{
    WORD i;

    for (i = 0; pos > 0; i++)
    {
        if (str[i] == '_')
            pos--;
    }

    /* skip to first one */
    while(str[i] && (str[i] != '_'))
        i++;

    return i;
}


static void pxl_rect(OBJECT *tree, WORD obj, WORD ch_pos, GRECT *pt)
{
    GRECT   o;

    ob_actxywh(tree, obj, &o);
    gr_just(edblk.te_just, edblk.te_font, edblk.te_ptmplt,
                o.g_w, o.g_h, &o);

    pt->g_x = o.g_x + (ch_pos * gl_wchar);
    pt->g_y = o.g_y;
    pt->g_w = gl_wchar;
    pt->g_h = gl_hchar;
}


/*
 *  Routine to redraw the cursor or the field being edited
 */
static void curfld(OBJECT *tree, WORD obj, WORD new_pos, WORD dist)
{
    GRECT   oc, t;

    pxl_rect(tree, obj, new_pos, &t);
    if (dist)
        t.g_w += (dist - 1) * gl_wchar;
    else
    {
        gsx_attr(FALSE, MD_XOR, BLACK);
        t.g_y -= 3;
        t.g_h += 6;
    }

    /* set the new clip rect*/
    gsx_gclip(&oc);
    gsx_sclip(&t);

    /* redraw the field */
    if (dist)
        ob_draw(tree, obj, 0);
    else
        gsx_cline(t.g_x, t.g_y, t.g_x, t.g_y+t.g_h-1);

    /* turn on cursor in new position */
    gsx_sclip(&oc);
}


/*
 *  Routine to check to see if given character is in the desired range.
 *  The character ranges are stored as enumerated characters (xyz) or
 *  ranges (x..z)
 */
static WORD instr(BYTE chr, BYTE *str)
{
    BYTE test1, test2;

    while(*str)
    {
        test1 = test2 = *str++;
        if ((*str == '.') && (*(str+1) == '.'))
        {
            str += 2;
            test2 = *str++;
        }
        if ((chr >= test1) && (chr <= test2))
            return TRUE;
    }

    return FALSE;
}


/*
 *  Routine to verify that the character matches the validation
 *  string.  If necessary, upshift it.
 */
static WORD check(BYTE *in_char, BYTE valchar)
{
    WORD upcase;
    BYTE *rstr;

    upcase = TRUE;
    rstr = NULL;
    switch(valchar)
    {
    case '9':           /* 0..9                 */
        rstr = "0..9";
        upcase = FALSE;
        break;
    case 'A':           /* A..Z, <space>        */
        rstr = "a..zA..Z ";
        break;
    case 'N':           /* 0..9, A..Z, <SPACE>  */
        rstr = "a..zA..Z0..9 ";
        break;
    case 'P':           /* DOS pathname + '\', '?', '*', ':','.',','*/
        rstr = "a..zA..Z0..9 $#&@!%()-{}'`_^~\\?*:.,";
        break;
    case 'p':           /* DOS pathname + '\` + ':'     */
        rstr = "a..zA..Z0..9 $#&@!%()-{}'`_^~\\:";
        break;
    case 'F':           /* DOS filename + ':', '?' + '*' */
        rstr = "a..zA..Z0..9 $#&@!%()-{}'`_^~:?*";
        break;
    case 'f':           /* DOS filename */
        rstr = "a..zA..Z0..9 $#&@!%()-{}'`_^~";
        break;
    case 'a':           /* a..z, A..Z, <SPACE>  */
        rstr = "a..zA..Z ";
        upcase = FALSE;
        break;
    case 'n':           /* 0..9, a..z, A..Z, <SPACE> */
        rstr = "a..zA..Z0..9 ";
        upcase = FALSE;
        break;
    case 'x':           /* anything, but upcase */
        *in_char = toupper(*in_char);
        return TRUE;
    case 'X':           /* anything             */
        return TRUE;
    }

    if (rstr)
    {
        if (instr(*in_char, rstr))
        {
             if (upcase)
                *in_char = toupper(*in_char);
             return TRUE;
        }
    }

    return FALSE;
}


/*
 *  Find STart and FiNish of a raw string relative to the template
 *  string.  The start is determined by the InDeX position given.
 */
static void ob_stfn(WORD idx, WORD *pstart, WORD *pfinish)
{
    *pstart = find_pos(D.g_tmpstr, idx);
    *pfinish = find_pos(D.g_tmpstr, strlen(D.g_rawstr));
}


static WORD ob_delit(WORD idx)
{
    if (D.g_rawstr[idx])
    {
        strcpy(&D.g_rawstr[idx], &D.g_rawstr[idx+1]);
        return FALSE;
    }

    return TRUE;
}


WORD ob_edit(OBJECT *tree, WORD obj, WORD in_char, WORD *idx, WORD kind)
{
    WORD    pos, len;
    WORD    ii, no_redraw, start, finish, nstart, nfinish;
    WORD    dist, tmp_back, cur_pos;
    BYTE    bin_char;

    if ((kind == EDSTART) || (obj <= 0))
        return TRUE;

    /* copy TEDINFO struct to local struct */
    ob_getsp(tree, obj, &edblk);

    /* copy passed in strings to local strings */
    strcpy(D.g_tmpstr, edblk.te_ptmplt);
    strcpy(D.g_rawstr, edblk.te_ptext);
    len = ii = strlencpy(D.g_valstr, edblk.te_pvalid);

    /* expand out valid str */
    while ((ii > 0) && (len < edblk.te_tmplen))
        D.g_valstr[len++] = D.g_valstr[ii-1];
    D.g_valstr[len] = '\0';

    /* init formatted string */
    ob_format(edblk.te_just, D.g_rawstr, D.g_tmpstr, D.g_fmtstr);

    switch(kind)
    {
    case EDINIT:
        *idx = strlen(D.g_rawstr);
        break;
    case EDCHAR:
        /*
         * at this point, D.g_fmtstr has already been formatted -- it has
         * both template & data.  now update D.g_fmtstr with in_char;
         * return it; strip out junk & update ptext string.
         */
        no_redraw = TRUE;

        /* find cursor & turn it off */
        ob_stfn(*idx, &start, &finish);
        cur_pos = start;
        curfld(tree, obj, cur_pos, 0);

        switch(in_char)
        {
        case BACKSPACE:
            if (*idx > 0)
            {
                *idx -= 1;
                no_redraw = ob_delit(*idx);
            }
            break;
        case ESCAPE:
            *idx = 0;
            D.g_rawstr[0] = '\0';
            no_redraw = FALSE;
            break;
        case DELETE:
            if (*idx <= (edblk.te_txtlen - 2))
                no_redraw = ob_delit(*idx);
            break;
        case ARROW_LEFT:
            if (*idx > 0)
                *idx -= 1;
            break;
        case ARROW_RIGHT:
            if (*idx < strlen(D.g_rawstr))
                *idx += 1;
            break;
        default:
            tmp_back = FALSE;
            if (*idx > (edblk.te_txtlen - 2))
            {
                cur_pos--;
                start = cur_pos;
                tmp_back = TRUE;
                *idx -= 1;
            }
            bin_char = in_char & 0x00ff;
            if (bin_char)
            {
                /* make sure char is in specified set */
                if (check(&bin_char, D.g_valstr[*idx]))
                {
                    ins_char(D.g_rawstr, *idx, bin_char, edblk.te_txtlen);
                    *idx += 1;
                    no_redraw = FALSE;
                }
                else
                {   /* see if we can skip ahead     */
                    if (tmp_back)
                    {
                        *idx += 1;
                        cur_pos++;
                    }
                    pos = scan_to_end(D.g_tmpstr+cur_pos, *idx, bin_char);
                    if (pos < (edblk.te_txtlen-2))
                    {
                        memset(&D.g_rawstr[*idx], ' ', pos - *idx);
                        D.g_rawstr[pos] = '\0';
                        *idx = pos;
                        no_redraw = FALSE;
                    }
                }
            }
            break;
        }   /* switch(in_char) */

        strcpy(edblk.te_ptext, D.g_rawstr);
        if (!no_redraw)
        {
            ob_format(edblk.te_just, D.g_rawstr, D.g_tmpstr, D.g_fmtstr);
            ob_stfn(*idx, &nstart, &nfinish);
            start = min(start, nstart);
            dist = max(finish, nfinish) - start;
            if (dist)
                curfld(tree, obj, start, dist);
        }
        break;
    case EDEND:
        break;
    }

    /* draw/erase the cursor */
    cur_pos = find_pos(D.g_tmpstr, *idx);
    curfld(tree, obj, cur_pos, 0);
    return TRUE;
}
