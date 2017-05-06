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
                                    /* these must agree with RSHDR (see rsdefs.h) */
#define RT_VRSN     0
#define RT_OB       1
#define RT_TEDINFO  2
#define RT_ICONBLK  3
#define RT_BITBLK   4
#define RT_FREESTR  5
#define RT_STRING   6
#define RT_IMAGEDATA 7
#define RT_FREEIMG  8
#define RT_TRINDEX  9
#define R_NOBS      10
#define R_NTREE     11
#define R_NTED      12
#define R_NICON     13
#define R_NBITBLK   14
#define R_NSTRING   15
#define R_IMAGES    16


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


typedef union {
    LONG    base;
    LONG    *lptr;
    WORD    **wpp;
    BYTE    **bpp;
    OBJECT  *obj;
    TEDINFO *ted;
    ICONBLK *iblk;
} RSCITEM;


/*******  LOCALS  **********************/

static union {
    LONG    base;
    WORD    *wordptr;
} rs_hdr;
static AESGLOBAL *rs_global;
static char    tmprsfname[128];
static RSHDR   hdr_buff;
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


static RSCITEM get_sub(WORD rsindex, WORD rtype, WORD rsize)
{
    UWORD offset;

    offset = rs_hdr.wordptr[rtype];

    /* get base of objects and then index in */
    return (RSCITEM)(rs_hdr.base + offset + (UWORD)rsize * (UWORD)rsindex);
}


/*
 *  return address of given type and index, INTERNAL ROUTINE
 */
static RSCITEM get_addr(UWORD rstype, UWORD rsindex)
{
    RSCITEM item;
    WORD size;
    WORD rt;
    WORD valid;

    valid = TRUE;
    rt = size = 0;

    switch(rstype)
    {
    case R_TREE:
        item.base = rs_global->ap_ptree;
        return (RSCITEM)(item.lptr[rsindex]);
    case R_OBJECT:
        rt = RT_OB;
        size = sizeof(OBJECT);
        break;
    case R_TEDINFO:
    case R_TEPTEXT:
        rt = RT_TEDINFO;
        size = sizeof(TEDINFO);
        break;
    case R_ICONBLK:
    case R_IBPMASK:
        rt = RT_ICONBLK;
        size = sizeof(ICONBLK);
        break;
    case R_BITBLK:
    case R_BIPDATA:
        rt = RT_BITBLK;
        size = sizeof(BITBLK);
        break;
    case R_OBSPEC:
        item = get_addr(R_OBJECT, rsindex);
        return (RSCITEM)(&item.obj->ob_spec);
    case R_TEPTMPLT:
        item = get_addr(R_TEDINFO, rsindex);
        return (RSCITEM)(&item.ted->te_ptmplt);
    case R_TEPVALID:
        item = get_addr(R_TEDINFO, rsindex);
        return (RSCITEM)(&item.ted->te_pvalid);
    case R_IBPDATA:
        item = get_addr(R_ICONBLK, rsindex);
        return (RSCITEM)(&item.iblk->ib_pdata);
    case R_IBPTEXT:
        item = get_addr(R_ICONBLK, rsindex);
        return (RSCITEM)(&item.iblk->ib_ptext);
    case R_STRING:
        item = get_sub(rsindex, RT_FREESTR, sizeof(LONG));
        return (RSCITEM)(*item.lptr);
    case R_IMAGEDATA:
        item = get_sub(rsindex, RT_FREEIMG, sizeof(LONG));
        return (RSCITEM)(*item.lptr);
    case R_FRSTR:
        rt = RT_FREESTR;
        size = sizeof(LONG);
        break;
    case R_FRIMG:
        rt = RT_FREEIMG;
        size = sizeof(LONG);
        break;
    default:
        valid = FALSE;
        break;
    }

    if (valid)
        return get_sub(rsindex, rt, size);

    return (RSCITEM)-1L;
} /* get_addr() */


static BOOL fix_long(RSCITEM item)
{
    LONG lngval;

    lngval = *item.lptr;
    if (lngval != -1L)
    {
        lngval += rs_hdr.base;
        *item.lptr = lngval;
        return TRUE;
    }

    return FALSE;
}


static void fix_trindex(void)
{
    WORD ii;
    RSCITEM item;

    item = get_sub(0, RT_TRINDEX, sizeof(LONG) );
    rs_global->ap_ptree = item.base;

    for (ii = rs_hdr.wordptr[R_NTREE]-1; ii >= 0; ii--)
    {
        fix_long((RSCITEM)(item.lptr+ii));
    }
}


static void fix_objects(void)
{
    WORD ii;
    WORD obtype;
    RSCITEM item;

    for (ii = rs_hdr.wordptr[R_NOBS]-1; ii >= 0; ii--)
    {
        item = get_addr(R_OBJECT, ii);
        rs_obfix(item.obj, 0);
        obtype = item.obj->ob_type & 0x00ff;
        if ((obtype != G_BOX) && (obtype != G_IBOX) && (obtype != G_BOXCHAR))
            fix_long((RSCITEM)(&item.obj->ob_spec));
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
    RSCITEM item;

    for (ii = rs_hdr.wordptr[R_NTED]-1; ii >= 0; ii--)
    {
        item = get_addr(R_TEDINFO, ii);
        if (fix_ptr(R_TEPTEXT, ii))
            item.ted->te_txtlen = strlen(item.ted->te_ptext) + 1;
        if (fix_ptr(R_TEPTMPLT, ii))
            item.ted->te_tmplen = strlen(item.ted->te_ptmplt) + 1;
        fix_ptr(R_TEPVALID, ii);
    }
}


/*
 *  Set global addresses that are used by the resource library subroutines
 */
static void rs_sglobe(AESGLOBAL *pglobal)
{
    rs_global = pglobal;
    rs_hdr.base = rs_global->ap_rscmem;
}


/*
 *  Free the memory associated with a particular resource load
 */
WORD rs_free(AESGLOBAL *pglobal)
{
    rs_global = pglobal;

    return !dos_free(rs_global->ap_rscmem);
}


/*
 *  Get a particular ADDRess out of a resource file that has been
 *  loaded into memory
 */
WORD rs_gaddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG *rsaddr)
{
    RSCITEM item;

    rs_sglobe(pglobal);

    item = get_addr(rtype, rindex);
    *rsaddr = item.base;

    return (*rsaddr != -1L);
}


/*
 *  Set a particular ADDRess in a resource file that has been
 *  loaded into memory
 */
WORD rs_saddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG rsaddr)
{
    RSCITEM item;

    rs_sglobe(pglobal);

    item = get_addr(rtype, rindex);
    if (item.base != -1L)
    {
        *item.lptr = rsaddr;
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

    /* read the header */
    if (dos_read(fd, sizeof(hdr_buff), &hdr_buff) != sizeof(hdr_buff))
        return FALSE;           /* error or short read */

    /* get size of resource & allocate memory */
    rslsize = hdr_buff.rsh_rssize;
    rs_hdr.base = (LONG)dos_alloc_anyram(rslsize);
    if (!rs_hdr.base)
        return FALSE;

    /* read it all in */
    if (dos_lseek(fd, 0, 0x0L) < 0L)    /* mode 0: absolute offset */
        return FALSE;
    if (dos_read(fd, rslsize, (void *)rs_hdr.base) != rslsize)
        return FALSE;           /* error or short read */

    /* init global */
    rs_global = pglobal;
    rs_global->ap_rscmem = rs_hdr.base;
    rs_global->ap_rsclen = rslsize;

    /*
     * transfer RT_TRINDEX to global and turn all offsets from
     * base of file into pointers
     */
    fix_trindex();
    fix_tedinfo();
    ibcnt = rs_hdr.wordptr[R_NICON];
    fix_nptrs(ibcnt, R_IBPMASK);
    fix_nptrs(ibcnt, R_IBPDATA);
    fix_nptrs(ibcnt, R_IBPTEXT);
    fix_nptrs(rs_hdr.wordptr[R_NBITBLK], R_BIPDATA);
    fix_nptrs(rs_hdr.wordptr[R_NSTRING], R_FRSTR);
    fix_nptrs(rs_hdr.wordptr[R_IMAGES], R_FRIMG);

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
