/*      GEMRSLIB.C      5/14/84 - 06/23/85      Lowell Webster          */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */

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
#include "rsdefs.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemdos.h"
#include "gemshlib.h"
#include "gemgraf.h"
#include "geminit.h"
#include "gemrslib.h"

#include "string.h"
#include "nls.h"

/*
 * defines & typedefs
 */

/* type definitions for use by an application when calling      */
/*  rsrc_gaddr and rsrc_saddr                                   */

#define R_TREE      0
#define R_OBJECT    1
#define R_TEDINFO   2
#define R_ICONBLK   3
#define R_BITBLK    4
#define R_STRING    5               /* gets pointer to free strings */
#define R_IMAGEDATA 6               /* gets pointer to free images  */
#define R_OBSPEC    7
#define R_TEPTEXT   8               /* sub ptrs in TEDINFO  */
#define R_TEPTMPLT  9
#define R_TEPVALID  10
#define R_IBPMASK   11              /* sub ptrs in ICONBLK  */
#define R_IBPDATA   12
#define R_IBPTEXT   13
#define R_BIPDATA   14              /* sub ptrs in BITBLK   */
#define R_FRSTR     15              /* gets addr of ptr to free strings     */
#define R_FRIMG     16              /* gets addr of ptr to free images      */



/*******  LOCALS  **********************/

static RSHDR   *rs_hdr;
static AESGLOBAL *rs_global;
static char    tmprsfname[128];
static char    free_str[256];   /* must be long enough for longest freestring in gem.rsc */


/*
 *  Fix up a character position, from offset,row/col to a pixel value.
 *  If width is 80 then convert to full screen width.
 */
static void fix_chpos(WORD *pfix, WORD offset)
{
    WORD coffset;
    WORD cpos;

    cpos = *pfix;
    coffset = (cpos >> 8) & 0x00ff;
    cpos &= 0x00ff;

    switch(offset)
    {
    case 0:
        cpos *= gl_wchar;
        break;
    case 1:
        cpos *= gl_hchar;
        break;
    case 2:
        if (cpos == 80)
            cpos = gl_width;
        else
            cpos *= gl_wchar;
        break;
    case 3:
        cpos *= gl_hchar;
        break;
    }

    cpos += ( coffset > 128 ) ? (coffset - 256) : coffset;
    *pfix = cpos;
}


/************************************************************************/
/* rs_obfix                                                             */
/************************************************************************/
void rs_obfix(OBJECT *tree, WORD curob)
{
    WORD offset;
    WORD *p;

    /* set X,Y,W,H */
    p = &tree[curob].ob_x;

    for (offset=0; offset<4; offset++)
        fix_chpos(p+offset, offset);
}


static void *get_sub(UWORD rsindex, UWORD offset, UWORD rsize)
{
    /* get base of objects and then index in */
    return (char *)rs_hdr + offset + rsize * rsindex;
}


/*
 *  return address of given type and index, INTERNAL ROUTINE
 */
static void *get_addr(UWORD rstype, UWORD rsindex)
{
    WORD size;
    UWORD offset;
    OBJECT *obj;
    TEDINFO *tedinfo;
    ICONBLK *iconblk;

    switch(rstype)
    {
    case R_TREE:
        return rs_global->ap_ptree[rsindex];
    case R_OBJECT:
        offset = rs_hdr->rsh_object;
        size = sizeof(OBJECT);
        break;
    case R_TEDINFO:
    case R_TEPTEXT: /* same, because te_ptext is first field of TEDINFO */
        offset = rs_hdr->rsh_tedinfo;
        size = sizeof(TEDINFO);
        break;
    case R_ICONBLK:
    case R_IBPMASK: /* same, because ib_pmask is first field of ICONBLK */
        offset = rs_hdr->rsh_iconblk;
        size = sizeof(ICONBLK);
        break;
    case R_BITBLK:
    case R_BIPDATA: /* same, because bi_pdata is first field of BITBLK */
        offset = rs_hdr->rsh_bitblk;
        size = sizeof(BITBLK);
        break;
    case R_OBSPEC:
        obj = (OBJECT *)get_addr(R_OBJECT, rsindex);
        return &obj->ob_spec;
    case R_TEPTMPLT:
    case R_TEPVALID:
        tedinfo = (TEDINFO *)get_addr(R_TEDINFO, rsindex);
        if (rstype == R_TEPTMPLT)
            return &tedinfo->te_ptmplt;
        return &tedinfo->te_pvalid;
    case R_IBPDATA:
    case R_IBPTEXT:
        iconblk = (ICONBLK *)get_addr(R_ICONBLK, rsindex);
        if (rstype == R_IBPDATA)
            return &iconblk->ib_pdata;
        return &iconblk->ib_ptext;
    case R_STRING:
        return *((void **)get_sub(rsindex, rs_hdr->rsh_frstr, sizeof(LONG)));
    case R_IMAGEDATA:
        return *((void **)get_sub(rsindex, rs_hdr->rsh_frimg, sizeof(LONG)));
    case R_FRSTR:
        offset = rs_hdr->rsh_frstr;
        size = sizeof(LONG);
        break;
    case R_FRIMG:
        offset = rs_hdr->rsh_frimg;
        size = sizeof(LONG);
        break;
    default:
        return (void *)-1L;
    }

    return get_sub(rsindex, offset, size);
} /* get_addr() */


static BOOL fix_long(LONG *plong)
{
    LONG lngval;

    lngval = *plong;
    if (lngval != -1L)
    {
        *plong = (LONG)rs_hdr + lngval;
        return TRUE;
    }

    return FALSE;
}


static void fix_trindex(void)
{
    WORD ii;
    LONG *ptreebase;

    ptreebase = (LONG *)get_sub(R_TREE, rs_hdr->rsh_trindex, sizeof(LONG));
    rs_global->ap_ptree = (OBJECT **)ptreebase;

    for (ii = rs_hdr->rsh_ntree-1; ii >= 0; ii--)
    {
        fix_long(ptreebase+ii);
    }
}


static void fix_objects(void)
{
    WORD ii;
    WORD obtype;
    OBJECT *obj;

    for (ii = rs_hdr->rsh_nobs-1; ii >= 0; ii--)
    {
        obj = (OBJECT *)get_addr(R_OBJECT, ii);
        rs_obfix(obj, 0);
        obtype = obj->ob_type & 0x00ff;
        if ((obtype != G_BOX) && (obtype != G_IBOX) && (obtype != G_BOXCHAR))
            fix_long(&obj->ob_spec);
    }
}


static void fix_nptrs(WORD cnt, WORD type)
{
    WORD i;

    for (i = 0; i < cnt; i++)
        fix_long(get_addr(type, i));
}


static BOOL fix_ptr(WORD type, WORD index)
{
    return fix_long(get_addr(type, index));
}


static void fix_tedinfo(void)
{
    WORD ii;
    TEDINFO *ted;

    for (ii = rs_hdr->rsh_nted-1; ii >= 0; ii--)
    {
        ted = (TEDINFO *)get_addr(R_TEDINFO, ii);
        if (fix_ptr(R_TEPTEXT, ii))
            ted->te_txtlen = strlen(ted->te_ptext) + 1;
        if (fix_ptr(R_TEPTMPLT, ii))
            ted->te_tmplen = strlen(ted->te_ptmplt) + 1;
        fix_ptr(R_TEPVALID, ii);
    }
}


/*
 *  Set global addresses that are used by the resource library subroutines
 */
static void rs_sglobe(AESGLOBAL *pglobal)
{
    rs_global = pglobal;
    rs_hdr = rs_global->ap_rscmem;
}


/*
 *  Free the memory associated with a particular resource load
 */
WORD rs_free(AESGLOBAL *pglobal)
{
    rs_global = pglobal;

    return !dos_free((LONG)rs_global->ap_rscmem);
}


/*
 *  Get a particular ADDRess out of a resource file that has been
 *  loaded into memory
 */
WORD rs_gaddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG *rsaddr)
{
    rs_sglobe(pglobal);

    *rsaddr = (LONG)get_addr(rtype, rindex);
    return (*rsaddr != -1L);
}


/*
 *  Set a particular ADDRess in a resource file that has been
 *  loaded into memory
 */
WORD rs_saddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG rsaddr)
{
    LONG *psubstruct;

    rs_sglobe(pglobal);

    psubstruct = (LONG *)get_addr(rtype, rindex);
    if (psubstruct != (LONG *)-1L)
    {
        *psubstruct = rsaddr;
        return TRUE;
    }

    return FALSE;
}


/*
 *  Read resource file into memory and fix everything up except the
 *  x,y,w,h, parts which depend upon a GSX open workstation.  In the
 *  case of the GEM resource file this workstation will not have
 *  been loaded into memory yet.
 */
static WORD rs_readit(AESGLOBAL *pglobal,UWORD fd)
{
    WORD ibcnt;
    UWORD rslsize;
    RSHDR hdr_buff;

    /* read the header */
    if (dos_read(fd, sizeof(hdr_buff), &hdr_buff) != sizeof(hdr_buff))
        return FALSE;           /* error or short read */

    /* get size of resource & allocate memory */
    rslsize = hdr_buff.rsh_rssize;
    rs_hdr = (RSHDR *)dos_alloc_anyram(rslsize);
    if (!rs_hdr)
        return FALSE;

    /* read it all in */
    if (dos_lseek(fd, 0, 0x0L) < 0L)    /* mode 0: absolute offset */
        return FALSE;
    if (dos_read(fd, rslsize, rs_hdr) != rslsize)
        return FALSE;           /* error or short read */

    /* init global */
    rs_global = pglobal;
    rs_global->ap_rscmem = rs_hdr;
    rs_global->ap_rsclen = rslsize;

    /*
     * transfer RT_TRINDEX to global and turn all offsets from
     * base of file into pointers
     */
    fix_trindex();
    fix_tedinfo();
    ibcnt = rs_hdr->rsh_nib;
    fix_nptrs(ibcnt, R_IBPMASK);
    fix_nptrs(ibcnt, R_IBPDATA);
    fix_nptrs(ibcnt, R_IBPTEXT);
    fix_nptrs(rs_hdr->rsh_nbb, R_BIPDATA);
    fix_nptrs(rs_hdr->rsh_nstring, R_FRSTR);
    fix_nptrs(rs_hdr->rsh_nimages, R_FRIMG);

    return TRUE;
}


/*
 *  Fix up objects separately so that we can read GEM resource before we
 *  do an open workstation, then once we know the character sizes we
 *  can fix up the objects accordingly.
 */
void rs_fixit(AESGLOBAL *pglobal)
{
    rs_sglobe(pglobal);
    fix_objects();
}


/*
 *  rs_load: the rsrc_load() implementation
 */
WORD rs_load(AESGLOBAL *pglobal, LONG rsfname)
{
    LONG  dosrc;
    WORD  ret;
    UWORD fd;

    /*
     * use shel_find() to get resource location
     */
    strcpy(tmprsfname,(char *)rsfname);
    if (!sh_find(tmprsfname))
        return FALSE;

    dosrc = dos_open((BYTE *)tmprsfname,0); /* mode 0: read only */
    if (dosrc < 0L)
        return FALSE;
    fd = (UWORD)dosrc;

    ret = rs_readit(pglobal,fd);
    if (ret)
        rs_fixit(pglobal);
    dos_close(fd);

    return ret;
}


/* Get a string from the GEM-RSC */
BYTE *rs_str(UWORD stnum)
{
    LONG ad_string;

    ad_string = (LONG) gettext(rs_fstr[stnum]);
    strcpy(free_str, (char *)ad_string);
    return free_str;
}
