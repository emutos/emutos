/*  	GEMRSLIB.C	5/14/84 - 06/23/85	Lowell Webster		*/
/*	merge High C vers. w. 2.2 		8/24/87		mdf	*/ 

/*
*       Copyright 1999, Caldera Thin Clients, Inc.                      
*       This software is licenced under the GNU Public License.         
*       Please see LICENSE.TXT for further information.                 
*                                                                       
*                  Historical Copyright                                 
*	-------------------------------------------------------------
*	GEM Application Environment Services		  Version 2.3
*	Serial No.  XXXX-0000-654321		  All Rights Reserved
*	Copyright (C) 1987			Digital Research Inc.
*	-------------------------------------------------------------
*/

#include <portab.h>
#include <machine.h>
#include <struct.h>
#include <basepage.h>
#include <obdefs.h>
#include <taddr.h>
#include <gemlib.h>

#define NUM_OBS LWGET(rs_hdr + 2*R_NOBS)
#define NUM_TREE LWGET(rs_hdr + 2*R_NTREE)
#define NUM_TI LWGET(rs_hdr + 2*R_NTED)
#define NUM_IB LWGET(rs_hdr + 2*R_NICON)
#define NUM_BB LWGET(rs_hdr + 2*R_NBITBLK)
#define NUM_FRSTR LWGET(rs_hdr + 2*R_NSTRING)
#define NUM_FRIMG LWGET(rs_hdr + 2*R_IMAGES)

#define ROB_TYPE (psubstruct + 6)	/* Long pointer in OBJECT	*/
#define ROB_STATE (psubstruct + 10)	/* Long pointer in OBJECT	*/
#define ROB_SPEC (psubstruct + 12)	/* Long pointer in OBJECT	*/

#define RTE_PTEXT (psubstruct + 0)	/* Long pointers in TEDINFO	*/
#define RTE_PTMPLT (psubstruct + 4)
#define RTE_PVALID (psubstruct + 8)
#define RTE_TXTLEN (psubstruct + 24)
#define RTE_TMPLEN (psubstruct + 26)

#define RIB_PMASK (psubstruct + 0)	/* Long pointers in ICONBLK	*/
#define RIB_PDATA (psubstruct + 4)
#define RIB_PTEXT (psubstruct + 8)

#define RBI_PDATA (psubstruct + 0)	/* Long pointer in BITBLK	*/
#define RBI_WB (psubstruct + 4)
#define RBI_HL (psubstruct + 6)
					/* in global array		*/
#define APP_LOPNAME (rs_global + 10)
#define APP_LO1RESV (rs_global + 14)
#define APP_LO2RESV (rs_global + 18)
					/* in DOS.C			*/
WORD		rs_gaddr();

EXTERN	WORD	dos_open();
EXTERN	LONG	dos_lseek();
EXTERN	WORD	dos_read();
EXTERN	WORD	dos_close();
EXTERN	LONG	dos_alloc();
EXTERN	WORD	dos_free();
					/* in SHIB.C, added for hc      */
EXTERN  WORD	sh_find(); 

EXTERN LONG	ad_scmd;
EXTERN LONG	ad_g1loc;
EXTERN LONG	ad_sysglo;

EXTERN WORD	DOS_ERR;
EXTERN WORD	DOS_AX;
EXTERN WORD	gl_width;
EXTERN WORD	gl_height;
EXTERN WORD	gl_wchar;
EXTERN WORD	gl_hchar;

EXTERN THEGLO		D;


/*******  LOCALS  **********************/
LONG		rs_hdr;
LONG		rs_global;
UWORD		hdr_buff[HDR_LENGTH/2];



/*
*	Fix up a character position, from offset,row/col to a pixel value.
*	If column or width is 80 then convert to rightmost column or 
*	full screen width. 
*/
	VOID
fix_chpos(pfix, offset)
	LONG	pfix;
	WORD	offset;
{
	WORD	coffset;
	WORD	cpos;

	cpos = LWGET(pfix);
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
	LWSET(pfix, cpos);
}


/************************************************************************/
/* rs_obfix								*/
/************************************************************************/
	VOID
rs_obfix(tree, curob)
	LONG		tree;
	WORD		curob;
{
	REG WORD	offset;
	REG LONG	p;
						/* set X,Y,W,H */
	p = OB_X(curob);

	for (offset=0; offset<4; offset++)
	  fix_chpos(p+(LONG)(2*offset), offset);
}


	BYTE
*rs_str(stnum)
{
	LONG		ad_string;

	rs_gaddr(ad_sysglo, R_STRING, stnum, &ad_string);
	LSTCPY(ad_g1loc, ad_string);
	return( &D.g_loc1[0] );
}


	LONG
get_sub(rsindex, rtype, rsize)
	WORD		rsindex, rtype, rsize;
{
	UWORD		offset;

	offset = LWGET( rs_hdr + LW(rtype*2) );
						/* get base of objects	*/
						/*   and then index in	*/
	return( rs_hdr + LW(offset) + LW(rsize * rsindex) );
}


/*
 *	return address of given type and index, INTERNAL ROUTINE
*/
	LONG
get_addr(rstype, rsindex)
	REG UWORD	rstype;
	REG UWORD	rsindex;
{
	REG LONG	psubstruct;
	REG WORD	size;
	REG WORD	rt;
	WORD		valid;

	ULONG		junk;

	valid = TRUE;
	switch(rstype)
	{
	  case R_TREE:
		junk=LLGET(APP_LOPNAME) + LW(rsindex*4);   /*!!!*/
		return( LLGET( junk ) );
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
		psubstruct = get_addr(R_OBJECT, rsindex);
		return( ROB_SPEC );
	  case R_TEPTMPLT:
	  case R_TEPVALID:
		psubstruct = get_addr(R_TEDINFO, rsindex);
	  	if (rstype == R_TEPTMPLT)
	  	  return( RTE_PTMPLT );
	  	else
	  	  return( RTE_PVALID );
	  case R_IBPDATA:
	  case R_IBPTEXT:
	  	psubstruct = get_addr(R_ICONBLK, rsindex);
	 	if (rstype == R_IBPDATA)
		  return( RIB_PDATA );
		else
		  return( RIB_PTEXT );
	  case R_STRING:
		return( LLGET( get_sub(rsindex, RT_FREESTR, sizeof(LONG)) ) );
	  case R_IMAGEDATA:
		return( LLGET( get_sub(rsindex, RT_FREEIMG, sizeof(LONG)) ) );
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
	  return( get_sub(rsindex, rt, size) );
	else
	  return(-1L);
} /* get_addr() */


	LONG
fix_long(plong)
	REG LONG	plong;
{
	REG LONG	lngval;

	lngval = LLGET(plong);
	if (lngval != -1L)
	{
	  lngval += rs_hdr;
	  LLSET(plong, lngval);
	  return( lngval );
	}
	else
	  return( 0x0L );
}

        VOID
fix_trindex()
{
	REG WORD	ii;
	REG LONG	ptreebase;
	LONG		tree;

	ptreebase = get_sub(0, RT_TRINDEX, sizeof(LONG) );
	LLSET(APP_LOPNAME, ptreebase );

	for (ii = NUM_TREE-1; ii >= 0; ii--)
	{
	  tree = fix_long(ptreebase + LW(ii*4));
	  if ( (LWGET(OB_STATE(ROOT)) == OUTLINED) &&
	       (LWGET(OB_TYPE(ROOT)) == G_BOX) )
	   LWSET( OB_STATE(ROOT), SHADOWED );
	}
}

        VOID
fix_objects()
{
	REG WORD	ii;
	REG WORD	obtype;
	REG LONG	psubstruct;
	WORD		len;
	LONG		farstr;

	for (ii = NUM_OBS-1; ii >= 0; ii--)
	{
	  psubstruct = get_addr(R_OBJECT, ii);
	  rs_obfix(psubstruct, 0);
	  obtype = (LWGET( ROB_TYPE ) & 0x00ff);
	  if ( (obtype != G_BOX) &&
	       (obtype != G_IBOX) &&
	       (obtype != G_BOXCHAR) )
	    fix_long(ROB_SPEC);
						/* fix up menu divider	*/
	  if ( (obtype == G_STRING) &&
	       (LWGET( ROB_STATE ) & DISABLED) )
	  {
	    farstr = LLGET(ROB_SPEC);
	    len = LSTRLEN(farstr);
	    if ( (LBGET(farstr) == '-') &&
	    	 (LBGET(farstr + (ULONG)len - 1) == '-') )
	    {
	      while(len--)
	        LBSET(farstr++, 0x13);
	    }
	  }
		
	}
}


	VOID
fix_nptrs(cnt, type)
	WORD		cnt;
	WORD		type;
{
	REG WORD	i;

	for(i=cnt; i>=0; i--)
	  fix_long( get_addr(type, i) );
}


	WORD
fix_ptr(type, index)
	WORD		type;
	WORD		index;
{
	return( fix_long( get_addr(type, index) ) != 0x0L );
}


        VOID
fix_tedinfo()
{
	REG WORD	ii, i;
	REG LONG	psubstruct;
	LONG		tl[2], ls[2];


	for (ii = NUM_TI-1; ii >= 0; ii--)
	{
	  psubstruct = get_addr(R_TEDINFO, ii);
	  tl[0] = tl[1] = 0x0L;
	  if (fix_ptr(R_TEPTEXT, ii) )
	  {
	    tl[0] = RTE_TXTLEN;
	    ls[0] = RTE_PTEXT;
	  }
	  if (fix_ptr(R_TEPTMPLT, ii) )
	  {
	    tl[1] = RTE_TMPLEN;
	    ls[1] = RTE_PTMPLT;
	  }
	  for(i=0; i<2; i++)
	  {
	    if (tl[i])
	      LWSET( tl[i], LSTRLEN( LLGET(ls[i]) ) + 1 );
	  }
	  fix_ptr(R_TEPVALID, ii);
	}
}




/*
*	Set global addresses that are used by the resource library sub-
*	routines
*/
	VOID
rs_sglobe(pglobal)
	LONG		pglobal;
{
	rs_global = pglobal;
	rs_hdr = LLGET(APP_LO1RESV);
}


/*
*	Free the memory associated with a particular resource load.
*/
	WORD
rs_free(pglobal)
	LONG		pglobal;
{
	rs_global = pglobal;

	dos_free(LLGET(APP_LO1RESV));
	return(!DOS_ERR);
}/* rs_free() */


/*
*	Get a particular ADDRess out of a resource file that has been
*	loaded into memory.
*/
	WORD
rs_gaddr(pglobal, rtype, rindex, rsaddr)
	LONG		pglobal;
	UWORD		rtype;
	UWORD		rindex;
	REG LONG	*rsaddr;
{
	rs_sglobe(pglobal);

	*rsaddr = get_addr(rtype, rindex);
	return(*rsaddr != -1L);
} /* rs_gaddr() */


/*
*	Set a particular ADDRess in a resource file that has been
*	loaded into memory.
*/
	WORD
rs_saddr(pglobal, rtype, rindex, rsaddr)
	LONG		pglobal;
	UWORD		rtype;
	UWORD		rindex;
	LONG		rsaddr;
{
	REG LONG	psubstruct;

	rs_sglobe(pglobal);

	psubstruct = get_addr(rtype, rindex);
	if (psubstruct != -1L)
	{
	  LLSET( psubstruct, rsaddr);
	  return(TRUE);
	}
	else
	  return(FALSE);
} /* rs_saddr() */


/*
*	Read resource file into memory and fix everything up except the
*	x,y,w,h, parts which depend upon a GSX open workstation.  In the
*	case of the GEM resource file this workstation will not have
*	been loaded into memory yet.
*/
WORD rs_readit(LONG pglobal, LONG rsfname)
{
	WORD	ibcnt;
	UWORD	rslsize, fd, ret;
						/* make sure its there	*/
	LSTCPY(ad_scmd, rsfname);
	if ( !sh_find(ad_scmd) )
        {
//kprintf("GEM: rs_readit: sh_find failed!\n");
	  return(FALSE);
        }
						/* init global		*/
	rs_global = pglobal;
						/* open then file and	*/
						/*   read the header	*/
	fd = dos_open( ad_scmd, RMODE_RD);
//kprintf("rs_readit handle=0x%x\n",(int)fd);
	if ( !DOS_ERR )
	  dos_read(fd, HDR_LENGTH, ADDR(&hdr_buff[0]));
//else kprintf("rs_readit: dos_open failed!\n");
						/* read in resource and	*/
						/*   interpret it	*/
	if ( !DOS_ERR )
	{
						/* get size of resource	*/
	  rslsize = hdr_buff[RS_SIZE];
						/* allocate memory	*/
	  rs_hdr = dos_alloc( LW(rslsize) );
	  if ( !DOS_ERR )
	  {
						/* read it all in 	*/
	    dos_lseek(fd, SMODE, 0x0L);
	    dos_read(fd, rslsize, rs_hdr);
	    if ( !DOS_ERR)
	    {
	      LLSET(APP_LO1RESV, rs_hdr );
	      LWSET(APP_LO2RESV, rslsize);
					/* xfer RT_TRINDEX to global	*/
					/*   and turn all offsets from	*/
					/*   base of file into pointers	*/
	      fix_trindex();
	      fix_tedinfo();
	      ibcnt = NUM_IB-1;
	      fix_nptrs(ibcnt, R_IBPMASK);
	      fix_nptrs(ibcnt, R_IBPDATA);
	      fix_nptrs(ibcnt, R_IBPTEXT);
	      fix_nptrs(NUM_BB-1, R_BIPDATA);
	      fix_nptrs(NUM_FRSTR-1, R_FRSTR);
	      fix_nptrs(NUM_FRIMG-1, R_FRIMG);
	    }
//else kprintf("rs_readit: dos_read2 failed!\n");
	  }
//else kprintf("rs_readit: dos_alloc failed!\n");
	}
//else kprintf("rs_readit: dos_read1 failed!\n");
//	kprintf("rs_readit: DOS_AX=0x%x\n",(int)DOS_AX);
        					/* close file and return*/
	ret = !DOS_ERR;
	dos_close(fd);
	return(ret);
}


/*
*	Fix up objects separately so that we can read GEM resource before we
*	do an open workstation, then once we know the character sizes we
*	can fix up the objects accordingly.
*/
	VOID
rs_fixit(pglobal)
	LONG		pglobal;
{
	rs_sglobe(pglobal);
	fix_objects();
}


/*
*	RS_LOAD		mega resource load
*/
	WORD
rs_load(pglobal, rsfname)
	REG LONG	pglobal;
	LONG		rsfname;
{
	REG WORD	ret;

	ret = rs_readit(pglobal, rsfname);
	if (ret)
	  rs_fixit(pglobal);
	return(ret);
}

