/*
 * fs.h - file system defines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *
 * Authors:
 *  JSL   Jason S. Loveman
 *  SCC   Steve C. Cavender
 *  EWF   Eric W. Fleischman
 *  KTB   Karl T. Braun (kral)
 *  MAD   Martin Doering
 *  LVL   Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */



/*
 *  fix conditionals
 */

#define M0101052901	1

/*
 *  constants
 */

#define SLASH '\\'

#define SUPSIZ 1024	/* common supervisor stack size (in words) */
#define OPNFILES 40	/* max open files in system */
#define NCURDIR 40	/* max current directories in use in system */
#define NUMSTD 6	/* number of standard files */
#define KBBUFSZ 128	/* size of typeahead buffer -- must be power of 2!! */
#define KBBUFMASK	(KBBUFSZ-1)

/*
 *  code macros
 */

#define swp68(x) s68(&x)
#define swp68l(x) s68l(&x)
#define NULPTR ((void *) 0)
#define MGET(x) ((x *) xmgetblk((sizeof(x) + 15)>>4))
#define bconstat(a) trap13(1,a)
#define bconin(a) trap13(2,a)
#define bconout(a,b) trap13(3,a,b)
/* SCC	11 Mar 85 */
#define bconostat(a) trap13(8,a)
#define getbpb(a) trap13(7,a)
#define getmpb(a) trap13(0,a)
#define rwabs(a,b,c,d,e) if((rwerr=trap13(4,a,b,c,d,e))!=0){errdrv=e;longjmp(errbuf,rwerr);}

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define xmovs(n,s,d)	bmove(s,d,n)

/*
 *  Type declarations
 */

#define BCB	struct	_bcb
#define FTAB	struct	_ftab
#define PD	struct	_pd
#define OFD	struct	_ofd
#define FCB	struct	_fcb
#define DND	struct	_dnd
#define DMD	struct	_dmd
#define CLNO	int		/*  cluster number M01.01.03	*/
#define RECNO	int		/*  record number M01.01.03	*/
				/*** this should be changed to a long!! ***/
#define FH	unsigned int		/*  file handle M01.01.04	*/

/*
**  PD - Process Descriptor
*/

#define PDCLSIZE	0x80		/*  size of command line in bytes  */
#define NUMCURDIR	16		/*  number of entries in curdir array */

PD	/* this is the basepage format */
{
/* 0x00 */
	long	p_lowtpa;
	long	p_hitpa;
	long	p_tbase;
	long	p_tlen;
/* 0x10 */
	long	p_dbase;
	long	p_dlen;
	long	p_bbase;
	long	p_blen;
/* 0x20 */
	char	*p_xdta;
	PD	*p_parent;	/* parent PD */
	long	p_0fill[1];
	char	*p_env; 	/* at offset 2ch (eat your heart out, Lee) */
/* 0x30 */
	char	p_uft[NUMSTD];	/* index into sys file table for std files */
	char	p_lddrv;
	char	p_curdrv;
	long	p_1fill[2];
/* 0x40 */
	char	p_curdir[NUMCURDIR];	/* index into sys dir table */
/* 0x50 */
	long	p_2fill[4];
/* 0x60 */
	long	p_3fill[2];
	long	p_dreg[1];	/* dreg[0] */
	long	p_areg[5];	/* areg[3..7] */
/* 0x80 */
	char	p_cmdlin[PDCLSIZE];
} ;


/*
**  OFD - open file descriptor
**	M01.01.03
*/


OFD 
{
	OFD	*o_link;	/*  link to next OFD			*/
	int	o_flag;
	int	o_time; 	/*  the next 4 items must be as in FCB	*/
	int	o_date; 	/*  time, date of creation		*/
	CLNO	o_strtcl;	/*  starting cluster number		*/
	long	o_fileln;	/*  length of file in bytes		*/

	DMD	*o_dmd; 	/*  link to media descr 		*/
	DND	*o_dnode;	/*  link to dir for this file		*/
	OFD	*o_dirfil;	/*  OFD for dir for this file		*/
	long	o_dirbyt;	/*  pos in dir of this files fcb (dcnt) */

	long	o_bytnum;	/* byte pointer within file		*/
	CLNO	o_curcl;	/* not used				*/
	RECNO	o_currec;	/* not used				*/
	int	o_curbyt;	/* not used				*/
	int	o_usecnt;	/* use count for inherited files	*/
	OFD	*o_thread;	/* mulitple open thread list		*/
	int	o_mod;		/* mode file opened in (r, w, r/w)	*/
} ;							/*  0x32  */


	/* 
	**  O_DIRTY - Dirty Flag
	**	T: OFD is dirty, because of chg to startcl, length, time, etc. 
	*/

#define O_DIRTY 	1

#if	! M0101052901
	/*
	**  O_COMPLETE - 
	**	1: traversal of directory file (to bld dir tree) has completed 
	*/

#define O_COMPLETE	2
#endif



/*
**  FCB - File Control Block
**	M01.01.03
*/


FCB
{
	char	f_name[11];
	char	f_attrib;
	char	f_fill[10];
	int	f_time;
	int	f_date;
	CLNO	f_clust;
	long	f_fileln;
} ;
#define FA_VOL		0x08
#define FA_SUBDIR	0x10
#define FA_NORM 	0x27
#define FA_RO		0x01

/*
**  DND - Directory Node Descriptor
**	M01.01.03
*/


DND /* directory node descriptor */
{
	char	d_name[11];	/*  directory name			*/
	char	d_fill; 	/*  attributes? 			*/
	int	d_flag;
	CLNO	d_strtcl;	/*  starting cluster number of dir	*/

	int	d_time; 	/*  last mod ?				*/
	int	d_date; 	/*  ""	 ""				*/
	OFD	*d_ofd; 	/*  open file descr for this dir	*/
	DND	*d_parent;	/*  parent dir (..)			*/
	DND	*d_left;	/*  1st child				*/

	DND	*d_right;	/*  sibling in same dir 		*/
	DMD	*d_drv; 	/*  for drive				*/
	OFD	*d_dirfil;
	long	d_dirpos;	/*  */

	long	d_scan; 	/*  current posn in dir for DND tree	*/
	OFD	*d_files;	/* open files on this node		*/
} ;

/* flags:	*/
#define B_16	1				/* device has 16-bit FATs	*/
#define B_FIX	2				/* device has fixed media	*/



/*
**  DMD - Drive Media Block
*/

/*  records == sectors	*/

DMD /* drive media block */
{
	int	m_recoff[3];	/*  record offsets for fat,dir,data	*/
	int	m_drvnum;	/*  drive number for this media 	*/
	RECNO	m_fsiz; 	/*  fat size in records M01.01.03	*/
	RECNO	m_clsiz;	/*  cluster size in records M01.01.03	*/
	int	m_clsizb;	/*  cluster size in bytes		*/
	int	m_recsiz;	/*  record size in bytes		*/

	CLNO	m_numcl;	/*  total number of clusters in data	*/
	int	m_clrlog;	/* clsiz in rec, log2 is shift		*/
	int	m_clrm; 	/* clsiz in rec, mask			*/
	int	m_rblog;	/* recsiz in bytes, shift		*/
	int	m_rbm;		/* recsiz in bytes, mask		*/
	int	m_clblog;	/* log of clus size in bytes		*/
	OFD	*m_fatofd;	/* OFD for 'fat file'			*/

	OFD	*m_ofl; 	/*  list of open files			*/
	DND	*m_dtl; 	/* root of directory tree list		*/
	int	m_16;		/* 16 bit fat ? 			*/
} ;



/*
**  BCB - Buffer Control Block			*  M01.01.05  *
*/

BCB
{
	BCB	*b_link;	/*  next bcb			*/
	int	b_bufdrv;	/*  unit for buffer		*/
	int	b_buftyp;	/*  buffer type 		*/
	int	b_bufrec;	/*  record number		*/
	BOOLEAN b_dirty;	/*  true if buffer dirty	*/
	long	b_dm;		/*  reserved for file system	*/
	BYTE	*b_bufr;	/*  pointer to buffer		*/
} ;

/*
 *  FTAB - Open File Table Entry
 */

/* point these at OFDs when needed */
FTAB
{
	OFD	*f_ofd;
	PD	*f_own; /* file owners */
	int	f_use;	/* use count */
} ;



/*
 *  DTAINFO - Information stored in the dta by srch-frst for use by srch-nxt.
 */

#define DTAINFO struct DtaInfo

DTAINFO
{
	char	dt_name[12] ;	/*  file name: filename.typ	00-11	*/
	long	dt_pos ;	/*  dir position		12-15	*/
	DND	*dt_dnd ;	/*  pointer to DND		16-19	*/
	char	dt_attr ;	/*  attributes of file		20	*/
				/*  --	below must not change -- [1]	*/
	char	dt_fattr ;	/*  attrib from fcb		21	*/
	int	dt_time ;	/*  time field from fcb 	22-23	*/
	int	dt_date ;	/*  date field from fcb 	24-25	*/
	long	dt_fileln ;	/*  file length field from fcb	26-29	*/
	char	dt_fname[12] ;	/*  file name from fcb		30-41	*/
} ;				/*    includes null terminator		*/



/******************************************
**
** BDOS level character device file handles
**
*******************************************
*/

#define H_Null		-1		/* not passed through to BIOS	*/
#define H_Print 	-2
#define H_Aux		-3
#define H_Console	-4
#define H_Clock 	-5
#define H_Mouse 	-6


/****************************************
**
** Character device handle conversion
** (BDOS-type handle to BIOS-type handle)
**
*****************************************
*/

#define HXFORM(h)	bios_dev[-h-2]

/**********************
**
** BIOS function macros
**
***********************
*/

#define CIStat(d)	trap13(0x01,d)		/* Character Input Status   */
#define GetBPB(d)	(BPB *)trap13(0x07,d)	/* Get BIOS Parameter Block */
#define COStat(d)	trap13(0x08,d)		/* Character Output Status  */
#define GetDM() 	trap13(0x0A)		/* Get Drive Map	    */
#define CIOCR(d,l,b)	trap13(0x0C,d,l,b)	/* Char IOCtl Read	    */
#define CIOCW(d,l,b)	trap13(0x0D,d,l,b)	/* Char IOCtl Write	    */
#define DIOCR(d,l,b)	trap13(0x0E,d,l,b)	/* Disk IOCtl Read	    */
#define DIOCW(d,l,b)	trap13(0x0F,d,l,b)	/* Disk IOCtl Write	    */
#define CVE(d,a)	trap13(0x10,d,a)	/* Char Vector Exchange     */


/**********************
**
** F_IOCtl subfunctions
**
***********************
*/

#define XCVECTOR	-1			/* Exchange vector	    */
#define GETINFO 	0			/* Get device info	    */
#define SETINFO 	1			/* NOT IMPLEMENTED	    */
#define CREADC		2			/* Character read control   */
#define CWRITEC 	3			/* Character write control  */
#define DREADC		4			/* Disk read control	    */
#define DWRITEC 	5			/* Disk write control	    */
#define INSTAT		6			/* Input status 	    */
#define OUTSTAT 	7			/* Output status	    */
#define REMEDIA 	8			/* Removeable indication    */

/*************************
**
** Device information bits
**
**************************
*/

#define Is_Console	0x0003			/* Both stdin & stdout	    */
#define Is_NUL		0x0004
#define Is_Clock	0x0008
#define Is_Character	0x00C0			/* Character is binary now  */
#define Does_IOCtl	0x4000


/*******************************
**
**  External Declarations
**
********************************
*/

extern	DND	*dirtbl[] ;
extern	DMD	*drvtbl[] ;
extern	char	diruse[] ;
extern	int	drvsel ;
extern	PD	*run ;
extern	int	logmsk[] ;
extern	FTAB	sft[] ;
extern	long	rwerr ;
extern	int	errdrv ;
extern	long	errbuf[] ;		/*  in sup.c			*/
extern	BCB	*bufl[2] ;		/*  in bios main.c		*/
extern	unsigned int	time, date ;
extern	int	bios_dev[] ;		/*  in fsfioctl.c		*/

extern   int   *xmgetblk(int);  /* LVL: prototype needed in macro MGET() */
extern	long	trap13();
extern	long	constat();



/*
 * Forward Declarations
 */

DND	*dcrack() ;
FCB	*scan(REG DND *dnd, BYTE *n, WORD att, LONG *posp);
DND	*findit() ;
DMD	*getdmd() ;
OFD	*getofd() ;
FCB	*dirinit() ;
OFD	*makofd() ;
char	*packit() ;
DND	*makdnd() ;
char	*getrec() ;
VOID	movs() ;
VOID	xfr2usr() ;
VOID	usr2xfr() ;
BYTE	uc(BYTE c);

long ixsfirst(char *name, REG WORD att, REG DTAINFO *addr);

long xread(), xwrite(), xlseek(), xrw();
long ixread(), ixwrite(), ixlseek(), ixforce();

long ixcreat(), ixopen(), ixdel(), ixclose();
long makopn(), log(), opnfil();

/********************************
**
**  Misc. defines
**
*********************************
*/

#define CL_DIR	0x0002
#define CL_FULL 0x0004


