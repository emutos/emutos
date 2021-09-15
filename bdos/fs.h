/*
 * fs.h - file system defines
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2020 The EmuTOS development team
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



#ifndef FS_H
#define FS_H

#include "biosdefs.h"
#include "bdosdefs.h"


/*
 *  constants
 */

#define SLASH '\\'
#define FNAMELEN    (LEN_ZNODE+LEN_ZEXT)    /* as found in dirs etc */

/*
 * the following values are used by Atari TOS:
 *              Typeahead bufsize   Number of open files
 *  TOS 1.04            80                  75
 *  TOS 2.06            80                  75
 *  TOS 3.06            80                  75
 *  TOS 4.04            63                  19 [likely a per-process number]
 */
#define SUPSIZ      1024            /* common supervisor stack size (in words) */
#define OPNFILES    75              /* max open files in system */
#define NCURDIR     40              /* max current directories in use in system */
#define NUMHANDLES  (NUMSTD+OPNFILES)
#define KBBUFSZ     64              /* size of typeahead buffer -- must be power of 2!! */
#define KBBUFMASK   (KBBUFSZ-1)

/*
 *  Error handling
 */

#include "setjmp.h"

extern  long    rwerr;
extern  jmp_buf errbuf;
extern  int     errdrv;
extern  long    errcode;

#define longjmp_rwabs(flg,buf,cnt,rec,dev)            \
    if (rec <= 32767)                                 \
    {                                                 \
        /* kprintf("\nRwabs short, rec = %i\n", (int)(rec)); */ \
        if ((rwerr=Rwabs(flg,buf,cnt,rec,dev,0))!=0)  \
        {                                             \
             errdrv=dev; errcode=rwerr;               \
             longjmp(errbuf,1);                       \
        }                                             \
    }                                                 \
    else                                              \
    {                                                 \
        /* kprintf(__FILE__ "%i: Rwabs rec=%li\n", __LINE__, (long)(rec)); */ \
        if ((rwerr=Rwabs(flg,buf,cnt,-1,dev,rec))!=0) \
        {                                             \
             errdrv=dev; errcode=rwerr;               \
             longjmp(errbuf,1);                       \
        }                                             \
    }                                                 \

/*
 *  Type declarations
 */

typedef struct _ofd OFD;
typedef struct _dnd DND;
typedef struct _dmd DMD;

typedef UWORD FH;               /*  file handle    */
typedef UWORD CLNO;             /*  cluster number */
typedef ULONG RECNO;            /*  record number  */


/*
 *  DOSTIME - the standard layout of time & date
 */
typedef struct
{
    UWORD time;         /* Time like Tgettime() */
    UWORD date;         /* Date like Tgetdate() */
} DOSTIME;



/*
 *  OFD - open file descriptor
 *
 *  architectural restriction: for compatibility with FOLDRnnn.PRG,
 *  this structure must not exceed 64 bytes in length
 */
struct _ofd
{
    OFD   *o_link;      /*  link to next OFD                    */
    UWORD o_flag;
                    /* the following 3 items must be as in FCB: */
    DOSTIME o_td;       /*  creation time/date: little-endian!  */
    CLNO  o_strtcl;     /*  starting cluster number             */
    long  o_fileln;     /*  length of file in bytes             */

    DMD   *o_dmd;       /*  link to media descr                 */
    DND   *o_dnode;     /*  link to dir for this file           */
    OFD   *o_dirfil;    /*  OFD for dir for this file           */
    long  o_dirbyt;     /*  pos in dir of this files fcb (dcnt) */

    long  o_bytnum;     /* byte pointer within file             */
    CLNO  o_curcl;      /* current cluster number for file      */
    RECNO o_currec;     /* current record number for file       */
    UWORD o_curbyt;     /* byte pointer within current cluster  */
    WORD  o_usecnt;     /* use count for inherited files        */
    OFD   *o_thread;    /* mulitple open thread list            */
    UWORD o_mod;        /* mode file opened in (see below)      */
} ;

/*
 * bit usage in o_mod
 *
 * bits 8-15 are internal-use only
 * bits 0-7 correspond to bit usage in the Fopen() system call
 * note: bits 4-7 are only used if GEMDOS file-sharing/record-locking
 *       is implemented
 */
#define INH_MODE    0x80    /* bit 7 is inheritance flag (not yet implemented) */
#define MODE_FSM    0x70    /* bits 4-6 are file sharing mode (not yet implemented) */
#define MODE_FAC    0x07    /* bits 0-2 are file access code: */
#define RO_MODE        0
#define WO_MODE        1
#define RW_MODE        2
#define VALID_FOPEN_BITS    MODE_FAC    /* currently-valid bits for Fopen() */

/*
 * O_DIRTY - Dirty Flag
 *
 * T: OFD is dirty, because of chg to startcl, length, time, etc.
 */
#define O_DIRTY         1



/*
 *  FCB - File Control Block
 *
 *  architectural restriction: this is the structure of the
 *  directory entry on disk, compatible with MSDOS etc
 */
typedef struct
{
    char f_name[FNAMELEN];
    UBYTE f_attrib;
    UBYTE f_fill[10];
    DOSTIME f_td;           /* time, date */
    CLNO f_clust;
    long f_fileln;
} FCB;

#define ERASE_MARKER    '\xe5'  /* in f_name[0], indicates erased file */

#define FA_NORM         (FA_ARCHIVE|FA_SYSTEM|FA_HIDDEN|FA_RO)
#define FA_LFN          0x0f



/*
 *  DND - Directory Node Descriptor
 *
 *  architectural restriction: for compatibility with FOLDRnnn.PRG,
 *  this structure must not exceed 64 bytes in length
 */
struct _dnd         /* directory node descriptor */
{
    char d_name[FNAMELEN];  /*  directory name                  */
    UBYTE d_fill;       /*  attributes?                         */
    UWORD d_flag;
    CLNO d_strtcl;      /*  starting cluster number of dir      */

    DOSTIME d_td;       /*  time/date: little-endian!           */
    OFD  *d_ofd;        /*  open file descr for this dir        */
    DND  *d_parent;     /*  parent dir (..)                     */
    DND  *d_left;       /*  1st child                           */

    DND  *d_right;      /*  sibling in same dir                 */
    DMD  *d_drv;        /*  for drive                           */
    OFD  *d_dirfil;
    long d_dirpos;      /*  */

    long d_scan;        /*  current posn in dir for DND tree    */
    OFD  *d_files;      /* open files on this node              */
} ;

/*
 * bit usage in d_flag
 */
#define DND_LOCKED  0x8000  /* DND may not be scavenged (see     */
                            /* free_available_dnds() in fsdir.c) */



/*
 *  DMD - Drive Media Block
 *
 *  note: in the following comments, records == logical sectors
 */
struct _dmd         /* drive media block */
{
    RECNO  m_recoff[3]; /*  record offsets for fat,dir,data     */
    WORD   m_drvnum;    /*  drive number for this media         */
    WORD   m_fsiz;      /*  fat size in records M01.01.03       */
    WORD   m_clsiz;     /*  cluster size in records M01.01.03   */
    UWORD  m_clsizb;    /*  cluster size in bytes               */
    UWORD  m_recsiz;    /*  record size in bytes                */

    CLNO   m_numcl;     /*  total number of clusters in data    */
    WORD   m_clrlog;    /* log (base 2) of clsiz in records     */
    WORD   m_clrm;      /* clsiz in rec, mask                   */
    WORD   m_rblog;     /* log (base 2) of recsiz in bytes      */
    WORD   m_rbm;       /* recsiz in bytes, mask                */
    WORD   m_clblog;    /* log (base 2) of clsiz in bytes       */
    WORD   m_clbm;      /* clsiz in bytes, mask                 */
    OFD    *m_fatofd;   /* OFD for 'fat file'                   */

    OFD    *m_ofl;      /*  list of open files                  */
    DND    *m_dtl;      /* root of directory tree list          */
    UBYTE  m_16;        /* 16 bit fat ?                         */
    UBYTE  m_1fat;      /* 1 FAT only ?                         */
} ;



/*
 *  buffer type values
 */

#define BT_FAT          0               /*  fat buffer                  */
#define BT_ROOT         1               /*  root dir buffer             */
#define BT_DATA         2               /*  buffer for files, dirs      */

/*
 *  buffer list indexes
 */

#define BI_FAT          0               /*  fat buffer list             */
#define BI_DATA         1               /*  data buffer list            */

/*
 *  BCB - Buffer Control Block
 *
 *  note that this is part of the TOS API, via the pointers to BCB
 *  chains at bufl[2].  for compatibility with existing programs
 *  (such as CACHEnnn.PRG and HDDRIVER), the length must be 20 bytes,
 *  and the fields marked (API) below must remain the same (contents,
 *  size, and offset within structure).
 */
typedef struct _bcb BCB;
struct _bcb
{
    BCB     *b_link;    /*  next bcb (API)              */
    WORD    b_bufdrv;   /*  unit for buffer (API)       */
    RECNO   b_bufrec;   /*  record number               */
    UBYTE   b_buftyp;   /*  buffer type                 */
    UBYTE   b_dirty;    /*  true if buffer dirty        */
    DMD     *b_dm;      /*  ptr to drive media block    */
    UBYTE   *b_bufr;    /*  pointer to buffer (API)     */
} ;

/*
 * FTAB - Open File Table Entry
 */

/* point these at OFDs when needed */
typedef struct
{
    OFD *f_ofd;
    PD  *f_own;         /* file owners */
    WORD f_use;         /* use count */
} FTAB;



/*
 * DTAINFO - Information stored in the dta by srch-frst for use by srch-nxt.
 */
typedef struct
{
                            /* EmuTOS private area, subject to change   */
    char  dt_name[12];          /*  file spec from Fsfirst()            */
    LONG  dt_offset_drive;      /*  -1 => uninitialised DTA, else:      */
                                /*   bits 4-0: drive id                 */
                                /*   bits 31-5: if root, offset to next */
                                /*    FCB, otherwise 0                  */
    UWORD dt_cloffset;          /*  if subdir, offset within cluster to */
                                /*   next FCB, otherwise 0              */
    CLNO  dt_clnum;             /*  if subdir, current cluster number,  */
                                /*   otherwise 0                        */
    char  dt_attr;              /*  attribute from Fsfirst()            */
                            /* public area, must not change             */
    char  dt_fattr;             /*  attrib from fcb             21      */
    DOSTIME dt_td;              /*  time, date fields from fcb  22-25   */
    long  dt_fileln;            /*  file length field from fcb  26-29   */
    char  dt_fname[14];         /*  file name from fcb          30-43   */
} DTAINFO;                      /*    includes null terminator          */

#define DTA_DRIVEMASK   0x0000001fL

/* the following structure is used to track current directories */
typedef struct {
    DND   *dnd;                 /* DND for directory */
    WORD  use;                  /* use count */
} DIRTBL_ENTRY;


/* External Declarations */

extern  DIRTBL_ENTRY dirtbl[];
extern  DMD     *drvtbl[];
extern  LONG    drvsel;
extern  FTAB    sft[];



/*
 * Forward Declarations
 */


/*
 * in fsdrive.c
 */

/* check the drive, see if it needs to be logged in. */
long ckdrv(int d, BOOL checkrem);

/* log in media 'b' on drive 'drv'. */
long log_media(BPB *b, int drv);

/*
 * in fshand.c
 */

long xforce(int std, int h);
long ixforce(int std, int h, PD *p);

/* duplicate a file handle. */
long xdup(int h);

/*
 * in fsopnclo.c
 */

/* create file with specified name, attributes */
long xcreat(char *name, UBYTE attr);
long ixcreat(char *name, UBYTE attr);

/* open a file (path name) */
long xopen(char *name, int mod);

/* Close a file */
long xclose(int h);
long ixclose(OFD *fd, int part);

/* remove a file */
long xunlink(char *name);
/* internal delete file. */
long ixdel(DND *dn, FCB *f, long pos);

/* internal check for illegal name */
BOOL contains_illegal_characters(const char *test);

/*
 * in fsbuf.c
 */

void bufl_init(void);
/* ??? */
void flush(BCB *b);
/* return the ptr to the buffer containing the desired record */
UBYTE *getrec(RECNO recn, OFD *of, int wrtflg);
BCB *getbcb(DMD *dmd,WORD buftype,RECNO recnum);

/*
 * in fsfat.c
 */

RECNO cl2rec(CLNO cl, DMD *dm);
void clfix(CLNO cl, CLNO link, DMD *dm);
CLNO getrealcl(CLNO cl, DMD *dm);
CLNO getclnum(CLNO cl, OFD *of);
int nextcl(OFD *p, int wrtflg);
long xgetfree(long *buf, int drv);

/*
 * in fsio.c
 */

/* check for eof on file with handle h */
long eof(int h);

/* seek to byte position n on file with handle h */
long xlseek(long n, int h, int flg);
long ixlseek(OFD *p, long n);

FCB *ixgetfcb(OFD *p);

long xread(int h, long len, void *ubufr);
long ixread(OFD *p, long len, void *ubufr);

long xwrite(int h, long len, void *ubufr);
long ixwrite(OFD *p, long len, void *ubufr);

/*
 * in fsdir.c
 */

long xmkdir(char *s);
long xrmdir(char *p);
long xchmod(char *p, int wrt, UBYTE mod);
long ixsfirst(char *name, WORD att, DTAINFO *addr);
long xsfirst(char *name, int att);
long xsnext(void);
long xgsdtof(DOSTIME *buf, int h, int wrt);
void builds(const char *s1 , char *s2 );
long xrename(int n, char *p1, char *p2);
long xchdir(char *p);
long xgetdir(char *buf, int drv);
FCB *dirinit(DND *dn);
DND *findit(char *name, const char **sp, int dflag);
FCB *scan(DND *dnd, const char *n, WORD att, LONG *posp);
int incr_curdir_usage(DND *dnd);
void decr_curdir_usage(int index);
OFD *makofd(DND *p);
WORD free_available_dnds(void);


/*
 * in fsmain.c
 */
DTAINFO *xgetdta(void);
void xsetdta(DTAINFO *addr);
long xsetdrv(int drv);
long xgetdrv(void);
OFD  *getofd(int h);


/*
 * FAT chain defines
 */
#define FREECLUSTER     0x0000
#define ENDOFCHAIN      0xffff                  /* our end-of-chain marker */
#define endofchain(a)   (((a)&0xfff8)==0xfff8)  /* in case file was created by someone else */


/* Misc. defines */
                    /* the following are used for the second arg to ixclose() */
#define CL_DIR  0x0002      /* this is a directory file, flush, do not free */
#define CL_FULL 0x0004      /* even though it's a directory, full close */

#endif /* FS_H */
