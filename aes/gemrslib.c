/*      GEMRSLIB.C      5/14/84 - 06/23/85      Lowell Webster          */
/*      merge High C vers. w. 2.2               8/24/87         mdf     */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2013 The EmuTOS development team
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
#include "compat.h"
#include "struct.h"
#include "basepage.h"
#include "obdefs.h"
#include "gemlib.h"
#include "gem_rsc.h"

#include "gemdos.h"
#include "gemshlib.h"
#include "gemgraf.h"
#include "geminit.h"
#include "gemrslib.h"

#include "string.h"
#include "nls.h"


typedef union {
    LONG    base;
    LONG    *lptr;
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
static UWORD   hdr_buff[HDR_LENGTH/2];
static char    free_str[256];   /* must be long enough for longest freestring in gem.rsc */



/*
*       Fix up a character position, from offset,row/col to a pixel value.
*       If width is 80 then convert to full screen width.  If height is 25
*       then convert to full screen height.  Is this correct??
*/
static void fix_chpos(WORD *pfix, WORD offset)
{
        WORD    coffset;
        WORD    cpos;

        cpos = *pfix;
        coffset = (cpos >> 8) & 0x00ff;
        cpos &= 0x00ff;

        switch(offset)
        {
          case 0: cpos *= gl_wchar;
            break;
          case 1: cpos *= gl_hchar;
            break;
          case 2: if (cpos == 80)
               cpos = gl_width;
             else
               cpos *= gl_wchar;
            break;
          case 3: if (cpos == 25)
               cpos = gl_height;
             else
               cpos *= gl_hchar;
            break;
        }

        cpos += ( coffset > 128 ) ? (coffset - 256) : coffset;
        *pfix = cpos;
}


/************************************************************************/
/* rs_obfix                                                             */
/************************************************************************/
void rs_obfix(LONG tree, WORD curob)
{
        register WORD   offset;
        OBJECT *obj = (OBJECT *)tree + curob;
        WORD *p;
                                                /* set X,Y,W,H */
        p = &obj->ob_x;

        for (offset=0; offset<4; offset++)
          fix_chpos(p+offset, offset);
}



static RSCITEM get_sub(WORD rsindex, WORD rtype, WORD rsize)
{
        UWORD           offset;

        offset = rs_hdr.wordptr[rtype];
                                                /* get base of objects  */
                                                /*   and then index in  */
        return (RSCITEM)( rs_hdr.base + offset + rsize * rsindex );
}


/*
 *      return address of given type and index, INTERNAL ROUTINE
 */
static RSCITEM get_addr(UWORD rstype, UWORD rsindex)
{
        RSCITEM         item;
        register WORD   size;
        register WORD   rt;
        WORD            valid;

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


static LONG fix_long(RSCITEM item)
{
        register LONG   lngval;

        lngval = *item.lptr;
        if (lngval != -1L)
        {
          lngval += rs_hdr.base;
          *item.lptr = lngval;
          return lngval;
        }

        return 0x0L;
}


static void fix_trindex(void)
{
        register WORD   ii;
        RSCITEM item;
        OBJECT *root;

        item = get_sub(0, RT_TRINDEX, sizeof(LONG) );
        rs_global->ap_ptree = item.base;

        for (ii = rs_hdr.wordptr[R_NTREE]-1; ii >= 0; ii--)
        {
          root = (OBJECT *)fix_long((RSCITEM)(item.lptr+ii));
          if ( (root->ob_state == OUTLINED) &&
               (root->ob_type == G_BOX) )
            root->ob_state = SHADOWED;
        }
}


static void fix_objects(void)
{
        register WORD   ii;
        register WORD   obtype;
        RSCITEM         item;

        for (ii = rs_hdr.wordptr[R_NOBS]-1; ii >= 0; ii--)
        {
          item = get_addr(R_OBJECT, ii);
          rs_obfix(item.base, 0);
          obtype = item.obj->ob_type & 0x00ff;
          if ( (obtype != G_BOX) &&
               (obtype != G_IBOX) &&
               (obtype != G_BOXCHAR) )
            fix_long((RSCITEM)(&item.obj->ob_spec));
        }
}


static void fix_nptrs(WORD cnt, WORD type)
{
        register WORD   i;

        for(i=cnt; i>=0; i--)
          fix_long( get_addr(type, i) );
}


static WORD fix_ptr(WORD type, WORD index)
{
        return( fix_long( get_addr(type, index) ) != 0x0L );
}


static void fix_tedinfo(void)
{
        register WORD   ii;
        RSCITEM         item;

        for (ii = rs_hdr.wordptr[R_NTED]-1; ii >= 0; ii--)
        {
          item = get_addr(R_TEDINFO, ii);
          if (fix_ptr(R_TEPTEXT, ii) )
            item.ted->te_txtlen = strlen((char *)item.ted->te_ptext) + 1;
          if (fix_ptr(R_TEPTMPLT, ii) )
            item.ted->te_tmplen = strlen((char *)item.ted->te_ptmplt) + 1;
          fix_ptr(R_TEPVALID, ii);
        }
}


/*
*       Set global addresses that are used by the resource library sub-
*       routines
*/
static void rs_sglobe(AESGLOBAL *pglobal)
{
        rs_global = pglobal;
        rs_hdr.base = rs_global->ap_1resv;
}


/*
*       Free the memory associated with a particular resource load.
*/
WORD rs_free(AESGLOBAL *pglobal)
{
        rs_global = pglobal;

        dos_free(rs_global->ap_1resv);
        return(!DOS_ERR);
}/* rs_free() */


/*
*       Get a particular ADDRess out of a resource file that has been
*       loaded into memory.
*/
WORD rs_gaddr(AESGLOBAL *pglobal, UWORD rtype, UWORD rindex, LONG *rsaddr)
{
        RSCITEM item;

        rs_sglobe(pglobal);

        item = get_addr(rtype, rindex);
        *rsaddr = item.base;

        return (*rsaddr != -1L);
} /* rs_gaddr() */


/*
*       Set a particular ADDRess in a resource file that has been
*       loaded into memory.
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
} /* rs_saddr() */


/*
*       Read resource file into memory and fix everything up except the
*       x,y,w,h, parts which depend upon a GSX open workstation.  In the
*       case of the GEM resource file this workstation will not have
*       been loaded into memory yet.
*/
static WORD rs_readit(AESGLOBAL *pglobal, LONG rsfname)
{
        WORD    ibcnt;
        UWORD   rslsize, fd, ret;
                                                /* make sure its there  */
        strcpy(tmprsfname, (char *) rsfname);
        if ( !sh_find(tmprsfname) )
        {
          return(FALSE);
        }
                                                /* init global          */
        rs_global = pglobal;
                                                /* open then file and   */
                                                /*   read the header    */
        fd = dos_open((BYTE *)tmprsfname, RMODE_RD);

        if ( !DOS_ERR )
          dos_read(fd, HDR_LENGTH, ADDR(&hdr_buff[0]));
                                                /* read in resource and */
                                                /*   interpret it       */
        if ( !DOS_ERR )
        {
                                                /* get size of resource */
          rslsize = hdr_buff[RS_SIZE];
                                                /* allocate memory      */
          rs_hdr.base = dos_alloc( rslsize );
          if ( !DOS_ERR )
          {
                                                /* read it all in       */
            dos_lseek(fd, SMODE, 0x0L);
            dos_read(fd, rslsize, rs_hdr.base);
            if ( !DOS_ERR)
            {
              rs_global->ap_1resv = rs_hdr.base;
              rs_global->ap_2resv[0] = rslsize;
                                        /* xfer RT_TRINDEX to global    */
                                        /*   and turn all offsets from  */
                                        /*   base of file into pointers */
              fix_trindex();
              fix_tedinfo();
              ibcnt = rs_hdr.wordptr[R_NICON]-1;
              fix_nptrs(ibcnt, R_IBPMASK);
              fix_nptrs(ibcnt, R_IBPDATA);
              fix_nptrs(ibcnt, R_IBPTEXT);
              fix_nptrs(rs_hdr.wordptr[R_NBITBLK]-1, R_BIPDATA);
              fix_nptrs(rs_hdr.wordptr[R_NSTRING]-1, R_FRSTR);
              fix_nptrs(rs_hdr.wordptr[R_IMAGES]-1, R_FRIMG);
            }
          }
        }
                                                /* close file and return*/
        ret = !DOS_ERR;
        dos_close(fd);
        return(ret);
}


/*
*       Fix up objects separately so that we can read GEM resource before we
*       do an open workstation, then once we know the character sizes we
*       can fix up the objects accordingly.
*/
void rs_fixit(AESGLOBAL *pglobal)
{
        rs_sglobe(pglobal);
        fix_objects();
}


/*
*       RS_LOAD         mega resource load
*/
WORD rs_load(AESGLOBAL *pglobal, LONG rsfname)
{
        register WORD   ret;

        ret = rs_readit(pglobal, rsfname);
        if (ret)
          rs_fixit(pglobal);
        return ret;
}


/* Get a string from the GEM-RSC */
BYTE *rs_str(UWORD stnum)
{
        LONG            ad_string;

        ad_string = (LONG) gettext( rs_fstr[stnum] );
        strcpy(free_str, (char *) ad_string);
        return( &free_str[0] );
}
