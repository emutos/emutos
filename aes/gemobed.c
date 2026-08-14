/*      GEMOBED.C       05/29/84 - 06/20/85             Gregg Morris    */
/*      merge High C vers. w. 2.2               8/21/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2020 The EmuTOS development team
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

#include "emutos.h"
#include "struct.h"
#include "obdefs.h"
#include "aesext.h"
#include "intmath.h"
#include "gemlib.h"

#include "gemoblib.h"
#include "gemgraf.h"
#include "geminit.h"
#include "gemobjop.h"
#include "gemobed.h"

#include "string.h"
#include "scancode.h"


/*
 * validation strings used in check()
 *
 * note that most documentation on this is incorrect.  the following
 * strings agree with TOS 2/3/4 actual usage.
 */
#define VALIDATE_N "0..9A..Z \x80\x8e\x8f\x90\x92\x99\x9a\x9e\xa5\xb5\xb6\xb7\xb8\xc2..\xdc"
#define VALIDATE_A (VALIDATE_N+4)               /* 0..9 are omitted */
#define VALIDATE_n "0..9a..zA..Z \x80..\xff"
#define VALIDATE_a (VALIDATE_n+4)               /* 0..9 are omitted */
#define VALIDATE_F ":?*a..zA..Z0..9_\x80..\xff"
#define VALIDATE_f (VALIDATE_F+3)               /* :?* are omitted */
#define VALIDATE_P ".?*a..zA..Z0..9_\\:\x80..\xff"
#define VALIDATE_p (VALIDATE_P+3)               /* .?* are omitted */


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
    LONG spec;
    WORD dummy;
    GRECT t;
    WORD th;

    wd = tree->ob_width;
    hd = tree->ob_height;
    xd = (gl_width - wd) / 2;
    yd = gl_hbox + ((gl_height - gl_hbox - hd) / 2);
    tree->ob_x = xd;
    tree->ob_y = yd;

    /* account for outline */
    if (tree->ob_state & OUTLINED)
    {
        xd -= 3;
        if (xd < 0)     /* don't move object offscreen */
            xd = 0;
        yd -= 3;
        if (yd < 0)     /* don't move object offscreen */
            yd = 0;
        wd += 6;
        hd += 6;
    }

    /* account for shadow */
    if (tree->ob_state & SHADOWED)
    {
        ob_sst(tree, ROOT, &spec, &dummy, &dummy, &dummy, &t, &th);
        if (th < 0)
            th = -th;
        th += th;
        wd += th;
        hd += th;
    }
    r_set(pt, xd, yd, wd, hd);
}


/*
 *  Inserts character 'chr' into the string pointed to 'str', at
 *  position 'pos' (positions are relative to the start of the
 *  string; inserting at position 0 means inserting at the start
 *  of the string).  'tot_len' gives the maximum length the string
 *  can grow to; if necessary, the string will be truncated after
 *  inserting the character.
 */
void ins_char(char *str, WORD pos, char chr, WORD tot_len)
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


/*
 *  Routine to scan thru a string looking for the occurrence of the
 *  specified character.  IDX is updated as we go based on the '_'
 *  characters that are encountered.  The reason for this routine is
 *  so that if a user types a template character during field entry,
 *  the cursor will jump to the first raw string underscore after
 *  that character.
 */
static WORD scan_to_end(char *pstr, WORD idx, char chr)
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
static WORD find_pos(char *str, WORD pos)
{
    WORD i;

    /* stop at end of str[]: a malformed TEDINFO may have fewer '_' than characters */
    for (i = 0; (pos > 0) && str[i]; i++)
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
static WORD instr(char chr, char *str)
{
    char test1, test2;

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
static WORD check(char *in_char, char valchar)
{
    WORD upcase;
    char *rstr;

    upcase = TRUE;
    rstr = NULL;
    switch(valchar)
    {
    case '9':           /* 0..9 */
        rstr = "0..9";
        upcase = FALSE;
        break;
    case 'A':           /* A..Z, <SPACE>, uppercase non-Roman */
        rstr = VALIDATE_A;
        break;
    case 'N':           /* 0..9, A..Z, <SPACE>, uppercase non-Roman */
        rstr = VALIDATE_N;
        break;
    case 'a':           /* a..z, A..Z, <SPACE>, 0x80..0xff */
        rstr = VALIDATE_a;
        upcase = FALSE;
        break;
    case 'n':           /* 0..9, a..z, A..Z, <SPACE>, 0x80..0xff */
        rstr = VALIDATE_n;
        upcase = FALSE;
        break;
    case 'F':           /* ':', '?', '*' + DOS filename */
        rstr = VALIDATE_F;
        break;
    case 'f':           /* DOS filename */
        rstr = VALIDATE_f;
        break;
    case 'P':           /* '.', '?', '*' + DOS pathname */
        rstr = VALIDATE_P;
        break;
    case 'p':           /* DOS pathname */
        rstr = VALIDATE_p;
        break;
    case 'X':           /* anything */
        return TRUE;
    case 'x':           /* anything, but change lowercase to uppercase */
        *in_char = toupper(*in_char);
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
    WORD    pos, len, maxlen;
    WORD    ii, no_redraw, start, finish, nstart, nfinish;
    WORD    dist, tmp_back, cur_pos;
    char    bin_char;

    if ((kind == EDSTART) || (obj <= 0))
        return TRUE;

    /* copy TEDINFO struct to local struct */
    ob_getsp(tree, obj, &edblk);

    /* clamp client-supplied te_txtlen: also used as 'te_txtlen - 2' below.
     * the floor of 2 only affects our own bookkeeping (it keeps ins_char()
     * from underflowing D.g_rawstr[]); we still write back to the client's
     * te_ptext bounded by our own buffer size below, not by this clamped
     * value, so a client declaring a tiny/zero te_txtlen isn't truncated
     * any harder than it already was before this field was validated. */
    edblk.te_txtlen = min(max(edblk.te_txtlen, (WORD)2), (WORD)sizeof(D.g_rawstr));

    /* client-supplied strings are unbounded: truncate them to fit our buffers */
    strlcpy(D.g_tmpstr, edblk.te_ptmplt, sizeof(D.g_tmpstr));
    strlcpy(D.g_rawstr, edblk.te_ptext, sizeof(D.g_rawstr));
    strlcpy(D.g_valstr, edblk.te_pvalid, sizeof(D.g_valstr));
    len = ii = strlen(D.g_valstr);

    /* expand out valid str; te_tmplen is client-supplied too, so clamp it as well */
    maxlen = min(edblk.te_tmplen, (WORD)(sizeof(D.g_valstr)-1));
    while ((ii > 0) && (len < maxlen))
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

        /* clamp client-supplied idx, used below to index D.g_rawstr/D.g_valstr */
        if ((*idx < 0) || (*idx > (WORD)strlen(D.g_rawstr)))
            *idx = strlen(D.g_rawstr);

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

        /*
         * only write back if something actually changed: D.g_rawstr may be
         * truncated relative to the client's original string (te_txtlen can
         * exceed our internal buffer size), and unconditionally writing it
         * back would silently lose data on a no-op such as a cursor move.
         * D.g_rawstr can never exceed sizeof(D.g_rawstr), so bounding the
         * copy to that is enough to keep us from overrunning our own
         * buffer; we deliberately don't re-clamp to the client's declared
         * te_txtlen here, since that's the same amount of trust the
         * unbounded strcpy() this replaced already placed in the client.
         */
        if (!no_redraw)
        {
            strlcpy(edblk.te_ptext, D.g_rawstr, sizeof(D.g_rawstr));
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
