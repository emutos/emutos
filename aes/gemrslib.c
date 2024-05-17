/*      GEMRSLIB.C      5/14/84 - 06/23/85      Lowell Webster          */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2022 The EmuTOS development team
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
#include "aesdefs.h"
#include "aesext.h"
#include "gem_rsc.h"

#include "gemdos.h"
#include "gemshlib.h"
#include "gemgraf.h"
#include "gemrslib.h"
#include "gemgsxif.h"

#include "intmath.h"
#include "string.h"
#include "nls.h"
#include "../vdi/vdi_defs.h"    /* for phys_work stuff */


#if CONF_WITH_VDI_16BIT
extern Vwk phys_work;           /* attribute area for physical workstation */
#endif

/*******  LOCALS  **********************/

static RSHDR   *rs_hdr;
static AESGLOBAL *rs_global;
static char tmprsfname[MAXPATHLEN];


/*
 *  Fix up a character position, from offset,row/col to a pixel value.
 *  If width is 80 then convert to full screen width.
 */
static void fix_chpos(WORD *pfix, WORD offset)
{
    WORD coffset;
    WORD cpos;

    cpos = *pfix;
    coffset = HIBYTE(cpos);
    cpos = LOBYTE(cpos);

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
    RSHDR *hdr = rs_hdr;

    switch(rstype)
    {
    case R_TREE:
        return rs_global->ap_ptree[rsindex];
    case R_OBJECT:
        offset = hdr->rsh_object;
        size = sizeof(OBJECT);
        break;
    case R_TEDINFO:
    case R_TEPTEXT: /* same, because te_ptext is first field of TEDINFO */
        offset = hdr->rsh_tedinfo;
        size = sizeof(TEDINFO);
        break;
    case R_ICONBLK:
    case R_IBPMASK: /* same, because ib_pmask is first field of ICONBLK */
        offset = hdr->rsh_iconblk;
        size = sizeof(ICONBLK);
        break;
    case R_BITBLK:
    case R_BIPDATA: /* same, because bi_pdata is first field of BITBLK */
        offset = hdr->rsh_bitblk;
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
        return *((void **)get_sub(rsindex, hdr->rsh_frstr, sizeof(LONG)));
    case R_IMAGEDATA:
        return *((void **)get_sub(rsindex, hdr->rsh_frimg, sizeof(LONG)));
    case R_FRSTR:
        offset = hdr->rsh_frstr;
        size = sizeof(LONG);
        break;
    case R_FRIMG:
        offset = hdr->rsh_frimg;
        size = sizeof(LONG);
        break;
    default:
        return (void *)-1L;
    }

    return get_sub(rsindex, offset, size);
} /* get_addr() */


#if CONF_WITH_COLOUR_ICONS
/*
 * fixup all of the CICONs for a CICONBLK
 *
 * returns a pointer to the next CICONBLK
 */
static CICONBLK *fixup_colour_icons(LONG num_cicons, LONG mono_words, CICON *start)
{
    CICON *p = start;
    WORD *q;
    LONG i;

    /*
     * loop through all the CICONs for one CICONBLK
     *
     * p points to the current CICON; q tracks sections of CICON data.
     * at the end, p will point to the start of the next CICONBLK
     */
    for (i = 0; i < num_cicons; i++)
    {
        q = (WORD *)(p+1);                  /* point to start of data area */
        p->col_data = q;
        q += mono_words * p->num_planes;
        p->col_mask = q;
        q += mono_words;                    /* mask is one plane */
        if (p->sel_data)
        {
            p->sel_data = q;
            q += mono_words * p->num_planes;
            p->sel_mask = q;
            q += mono_words;                /* mask is one plane */
        }
        if (p->next_res == (CICON *)1L)     /* more CICONs follow */
            p->next_res = (CICON *)q;
        else p->next_res = NULL;
        p = (CICON *)q;
    }

    return (CICONBLK *)p;
}

/*
 * this fixes up all of the CICONBLK-related pointers:
 *  . the CICONBLK pointer table
 *  . the pointers in the ICONBLK contained in the CICONBLK
 *  . the pointers in the CICON
 */
static void fixup_all_ciconblks(LONG num_blks, CICONBLK **ciconblkptr, CICON *cicondata)
{
    CICONBLK *p = (CICONBLK *)cicondata;
    WORD *q;
    LONG i, num_cicons, mono_words;

    for (i = 0; i < num_blks; i++)
    {
        ciconblkptr[i] = p;
        num_cicons = (LONG)(p->mainlist);   /* number of colour icons for this CICONBLK */
        mono_words = muls(p->monoblk.ib_wicon/16,p->monoblk.ib_hicon);
        q = (WORD *)(p+1);                  /* point to start of data area */

        p->monoblk.ib_pdata = q;            /* fixup mono icon */
        q += mono_words;
        p->monoblk.ib_pmask = q;
        q += mono_words;
        p->monoblk.ib_ptext = (char *)q;
        q += 12 / 2;                        /* length of icon text */
        if (num_cicons)
        {
            p->mainlist = (CICON *)q;
            p = fixup_colour_icons(num_cicons, mono_words, (CICON *)q);
        }
        else
        {
            p->mainlist = NULL;
            p = (CICONBLK *)q;
        }
    }
}

/*
 * returns pointer to a CICON that best matches the current resolution
 *
 * the order of preference is as follows:
 *  1. the CICON with the same number of planes as the current resolution
 *  2. of those CICONS with fewer planes than the current resolution, the
 *     one with the most number of planes
 * if neither applies, NULL is returned
 */
static CICON *best_match(CICONBLK *start)
{
    CICON *p, *found = NULL;

    for (p = start->mainlist; p; p = p->next_res)
    {
        if (p->num_planes > gl_nplanes)     /* too many planes */
            continue;
        if (p->num_planes == gl_nplanes)    /* exact match */
            return p;
        if (!found || (p->num_planes > found->num_planes))
            found = p;                      /* best so far */
    }

    return found;
}

/*
 * expand cicon data from S to D planes (S is strictly less than D)
 *
 * we use the same algorithm as Atari TOS:
 *  1. copy the source to the first S (0 to S-1) planes of the
 *     destination data
 *  2. create the Sth plane of the destination data by ANDing
 *     together all the source planes
 *  3. copy the Sth plane of the destination data to the remaining
 *     destination planes
 *  4. AND all the destination planes with the mask plane
 */
static void expand_cicondata(WORD *src, WORD *dst, WORD *mask, WORD w, WORD h, WORD src_planes, WORD dst_planes)
{
    WORD *p, *q;
    WORD plane_words, src_words;
    WORD i, j;

    plane_words = w / 16 * h;           /* in WORDS */
    src_words = plane_words * src_planes;

    /*
     * 1. the first src_planes of dst are the same as src
     */
    memcpy(dst, src, src_words*sizeof(WORD));

    /*
     * 2. copy the zeroth src plane to the next dst plane,
     *    then AND in the remaining planes
     */
    memcpy(dst+src_words, src, plane_words*sizeof(WORD));
    p = src + plane_words;      /* p -> start of remainder */
    for (i = 1; i < src_planes; i++)
    {
        q = dst + src_words;    /* q -> target */
        for (j = 0; j < plane_words; j++, p++, q++)
            *q = *p & *q;
    }

    /*
     * 3. copy the ANDed plane to the rest of the destination planes
     */
    p = dst + src_words;        /* p -> source (ANDed) plane */
    q = p + plane_words;        /* q -> start of remaining planes */
    for (i = 1; i < (dst_planes-src_planes); i++, q += plane_words)
    {
        memcpy(q, p, plane_words*sizeof(WORD));
    }

    /*
     * 4. AND the mask plane into all destination planes
     */
    q = dst;
    for (i = 0; i < dst_planes; i++)
    {
        p = mask;
        for (j = 0; j < plane_words; j++, p++, q++)
            *q &= *p;
    }
}

/*
 * transform a colour icon from device-independent to device-dependent form
 */
static void transform_cicon(WORD *src, WORD *dest, WORD w, WORD h, WORD planes)
{
    gsx_fix(&gl_src, src, w/8, h);
    gl_src.fd_stand = TRUE;
    gl_src.fd_nplanes = planes;

    gsx_fix(&gl_dst, dest, w/8, h);
    gl_dst.fd_nplanes = planes;

    vrn_trnfm(&gl_src, &gl_dst);

#if CONF_WITH_VDI_16BIT
    /*
     * for Truecolor, we now have the VDI colour code (0-255) in each
     * word/pixel.  however, the value is reversed: the least significant
     * bit is bit 15 and the most significant is bit 8.  we reverse it
     * manually, then use the value of 0-255 to look up the corresponding
     * pixel value in the palette associated with the physical workstation.
     *
     * NOTE: it might be faster to reverse the value by using a lookup
     * table, but we leave this as a possible future optimisation.
     */
    if (planes > 8)
    {
        WORD *p, i, n;
        for (i = w*h, p = dest; i > 0; i--, p++)
        {
            n = 0;              /* index into palette array */
            if (*p&0x8000)
                n |= 0x0001;
            if (*p&0x4000)
                n |= 0x0002;
            if (*p&0x2000)
                n |= 0x0004;
            if (*p&0x1000)
                n |= 0x0008;
            if (*p&0x0800)
                n |= 0x0010;
            if (*p&0x0400)
                n |= 0x0020;
            if (*p&0x0200)
                n |= 0x0040;
            if (*p&0x0100)
                n |= 0x0080;
            *p = phys_work.ext->palette[n];
        }
    }
#endif
}

/*
 * for each CICONBLK in the resource, select the CICON with the number of
 * planes that best matches the current resolution.  then expand the icon
 * if necessary, and transform it from standard to device-dependent format
 */
static void transform_all_cicons(LONG num_cicons, CICONBLK **ciconblkptr)
{
    CICONBLK *ciconblk;
    CICON *cicon;
    WORD *colbuf, *selbuf, *expandbuf, *src;
    LONG data_size, n;
    BOOL expand;
    WORD i, w, h;

    for (i = 0; i < num_cicons; i++)
    {
        ciconblk = ciconblkptr[i];
        cicon = best_match(ciconblk);   /* find a suitable CICON */
        ciconblk->mainlist = cicon;
        if (!cicon)                     /* nothing suitable ... */
            continue;
        w = ciconblk->monoblk.ib_wicon;
        h = ciconblk->monoblk.ib_hicon;
        data_size = muls(w/8*gl_nplanes,h);
        expand = (cicon->num_planes != gl_nplanes); /* boolean */

        /* if we need to expand the icon, we need a temp buffer */
        expandbuf = NULL;
        if (expand)
        {
            expandbuf = dos_alloc_anyram(data_size);
            if (!expandbuf)
            {
                ciconblk->mainlist = NULL;  /* no colour for this icon */
                continue;
            }
        }

        /* we always allocate a data buffer so we avoid transform-in-place */
        n = cicon->sel_data ? 2*data_size : data_size;
        colbuf = dos_alloc_anyram(n);
        if (!colbuf)
        {
            if (expandbuf)
                dos_free(expandbuf);
            ciconblk->mainlist = NULL;      /* no colour for this icon */
            continue;
        }

        /* handle standard icon */
        src = cicon->col_data;
        if (expand)
        {
            expand_cicondata(src, expandbuf, cicon->col_mask, w, h, cicon->num_planes, gl_nplanes);
            src = expandbuf;
        }
        transform_cicon(src, colbuf, w, h, gl_nplanes);
        cicon->col_data = colbuf;

        /* handle 'selected' icon (if present) */
        if (cicon->sel_data)
        {
            selbuf = colbuf + data_size/sizeof(WORD);
            src = cicon->sel_data;
            if (expand)
            {
                expand_cicondata(src, expandbuf, cicon->sel_mask, w, h, cicon->num_planes, gl_nplanes);
                src = expandbuf;
            }
            transform_cicon(src, selbuf, w, h, gl_nplanes);
            cicon->sel_data = selbuf;
        }

        cicon->num_planes = gl_nplanes;     /* neatness only */
        cicon->next_res = NULL;

        if (expandbuf)
            dos_free(expandbuf);
    }
}

/*
 * return pointer to start of CICONBLK pointer table
 *
 * returns NULL if none
 */
static CICONBLK **get_ciconblkptr(RSHDR *hdr)
{
    LONG *extarray;
    LONG cptr_offset;

    /* check if we could have CICONs */
    if ((hdr->rsh_vrsn & NEW_FORMAT_RSC) == 0)
        return NULL;

    /*
     * locate extension array, which has the following format (all longs):
     *  extarray[0]     true length of RSC file
     *  extarray[1]     offset of CICON table (-1L => none present)
     *  extarray[2]...  other extensions
     *  extarray[n]     0L indicates end of array
     */
    extarray = (LONG *)((char *)hdr + hdr->rsh_rssize);

    /* do we have CICONs? */
    cptr_offset = extarray[1];
    if ((cptr_offset == 0L) || (cptr_offset == -1L))
        return NULL;

    return (CICONBLK **)((char *)hdr + cptr_offset);
}

/*
 * free the CICON-related buffers allocated by transform_all_cicons()
 *
 * returns -1 iff dos_free() failed
 */
static WORD free_cicon_buffers(RSHDR *hdr)
{
    CICONBLK **ciconblkptr, **p;
    CICON *cicon;
    WORD rc = 0;

    /* find the CICONBLK ptr table & count the CICONBLKs */
    ciconblkptr = get_ciconblkptr(hdr);
    if (!ciconblkptr)   /* yes, we have no CICONBLKs */
        return 0;

    /* free any buffers allocated by transform_all_cicons() */
    for (p = ciconblkptr; *p != (CICONBLK *)-1L; p++)
    {
        cicon = (*p)->mainlist;
        if (cicon)
            if (dos_free(cicon->col_data))
                rc = -1;
    }

    return rc;
}

/*
 * initialise the colour icon stuff
 *
 * this includes:
 *  . filling in the CICONBLK pointer table
 *  . for each CICONBLK:
 *      . fixing up all of the internal data/mask/text pointers
 *      . determining the appropriate icon for the current resolution
 *      . expanding the icon if necessary
 *      . converting the icon to device-dependent form
 */
static void fix_cicons(void)
{
    RSHDR *hdr = rs_hdr;
    CICONBLK **ciconblkptr, **p;
    CICON *cicondata;
    LONG num_ciconblks;

    /* find the CICONBLK ptr table & count the CICONBLKs */
    ciconblkptr = get_ciconblkptr(hdr);
    if (!ciconblkptr)   /* yes, we have no CICONBLKs */
        return;

    for (num_ciconblks = 0, p = ciconblkptr; *p != (CICONBLK *)-1L; p++)
        num_ciconblks++;

    /* the CICON data area starts immediately after the pointer table */
    cicondata = (CICON *)(p+1);

    /* fixup the pointers in the resource */
    fixup_all_ciconblks(num_ciconblks, ciconblkptr, cicondata);

    /* transform all the icons to device-dependent format */
    transform_all_cicons(num_ciconblks, ciconblkptr);
}
#endif


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

    ptreebase = (LONG *)get_sub(0, rs_hdr->rsh_trindex, sizeof(LONG));
    rs_global->ap_ptree = (OBJECT **)ptreebase;

    for (ii = 0; ii < rs_hdr->rsh_ntree; ii++)
    {
        fix_long(ptreebase+ii);
    }
}


static void fix_objects(void)
{
    WORD ii;
    WORD obtype;
    OBJECT *obj;
#if CONF_WITH_COLOUR_ICONS
    CICONBLK **ciconblkptr = get_ciconblkptr(rs_hdr);
#endif

    for (ii = 0; ii < rs_hdr->rsh_nobs; ii++)
    {
        obj = (OBJECT *)get_addr(R_OBJECT, ii);
        rs_obfix(obj, 0);
        obtype = obj->ob_type & 0x00ff;
        switch(obtype)
        {
#if CONF_WITH_COLOUR_ICONS
        case G_CICON:
            if (ciconblkptr)
                obj->ob_spec = (LONG)ciconblkptr[obj->ob_spec];
            break;
#endif
        case G_BOX:
        case G_IBOX:
        case G_BOXCHAR:
            break;
        default:
            fix_long(&obj->ob_spec);
            break;
        }
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


static void fix_tedinfo_std(void)
{
    WORD ii;
    TEDINFO *ted;

    for (ii = 0; ii < rs_hdr->rsh_nted; ii++)
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
    WORD rc = 1;    /* default rc => OK */

    rs_sglobe(pglobal);

    if (rs_hdr)
    {

#if CONF_WITH_COLOUR_ICONS
        if (free_cicon_buffers(rs_hdr) < 0)
            rc = 0;
#endif

        if (dos_free(rs_global->ap_rscmem))
            rc = 0;
    }
    else
    {
        rc = 0;
    }

    return rc;
}


/*
 *  Get a particular ADDRess out of a resource file that has been
 *  loaded into memory
 */
WORD rs_gaddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, void **rsaddr)
{
    rs_sglobe(pglobal);

    *rsaddr = get_addr(rtype, rindex);
    return (*rsaddr != (void *)-1L);
}


/*
 *  Set a particular ADDRess in a resource file that has been
 *  loaded into memory
 */
WORD rs_saddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, void *rsaddr)
{
    void **psubstruct;

    rs_sglobe(pglobal);

    psubstruct = (void **)get_addr(rtype, rindex);
    if (psubstruct != (void **)-1L)
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
    LONG rslsize;
    RSHDR hdr_buff;

    /* read the header */
    if (dos_read(fd, sizeof(hdr_buff), &hdr_buff) != sizeof(hdr_buff))
        return FALSE;           /* error or short read */

    /* get size of resource & allocate memory */
    rslsize = hdr_buff.rsh_rssize;

#if CONF_WITH_COLOUR_ICONS
    /* for 'new format' resource files, get actual resource size */
    if (hdr_buff.rsh_vrsn & NEW_FORMAT_RSC)
    {
        if (dos_lseek(fd, 0, rslsize) < 0L)
            return FALSE;
        if (dos_read(fd, sizeof(rslsize), &rslsize) != sizeof(rslsize))
            return FALSE;
    }
#endif

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
#if CONF_WITH_COLOUR_ICONS
    fix_cicons();
#endif
    fix_tedinfo_std();
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
WORD rs_load(AESGLOBAL *pglobal, char *rsfname)
{
    LONG  dosrc;
    WORD  ret;
    UWORD fd;

    /*
     * use shel_find() to get resource location
     */
    strcpy(tmprsfname,rsfname);
    if (!sh_find(tmprsfname))
        return FALSE;

    dosrc = dos_open(tmprsfname,0); /* mode 0: read only */
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
char *rs_str(UWORD stnum)
{
    return (char *)gettext(rs_fstr[stnum]);
}

/*
 *  The xlate_obj_array() function below is used by
 *  the generated GEM rsc code in aes/gem_rsc.c, and by the desktop
 */

/* Translates the strings in an OBJECT array */
void xlate_obj_array(OBJECT *obj_array, int nobj)
{
    OBJECT *obj;
    char **str;

    for (obj = obj_array; --nobj >= 0; obj++) {
        switch(obj->ob_type)
        {
#if 0
        /*
         * at the moment, there are no G_TEXT or G_BOXTEXT items in the
         * EmuTOS resources.  note that, if they are added, erd will have
         * to be updated too.
         */
        case G_TEXT:
        case G_BOXTEXT:
            str = & ((TEDINFO *)obj->ob_spec)->te_ptext;
            *str = (char *)gettext(*str);
            break;
#endif
        case G_FTEXT:
        case G_FBOXTEXT:
            str = & ((TEDINFO *)obj->ob_spec)->te_ptmplt;
            *str = (char *)gettext(*str);
            break;
        case G_STRING:
        case G_BUTTON:
        case G_TITLE:
            obj->ob_spec = (LONG) gettext( (char *) obj->ob_spec);
            break;
        default:
            break;
        }
    }
}
