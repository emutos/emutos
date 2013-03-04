/*
 * fs.h - file system defines
 *
 * Copyright (c) 2001 Lineo, Inc.
 *               2002 - 2013 The EmuTOS development team
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

#include "bpb.h"
#include "pd.h"

/*
 *  fix conditionals
 */

#define M0101052901     1

/*
 *  constants
 */

#define SLASH '\\'

#define SUPSIZ 1024     /* common supervisor stack size (in words) */
#define OPNFILES 40     /* max open files in system */
#define NCURDIR 40      /* max current directories in use in system */
#define NUMSTD 6        /* number of standard files */
#define KBBUFSZ 128     /* size of typeahead buffer -- must be power of 2!! */
#define KBBUFMASK       (KBBUFSZ-1)

/*
 *  code macros
 */

#define min(a,b) (((a) < (b)) ? (a) : (b))

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

#define BCB     struct  _bcb
#define FTAB    struct  _ftab
#define OFD     struct  _ofd
#define FCB     struct  _fcb
#define DND     struct  _dnd
#define DMD     struct  _dmd
#define FH      unsigned int    /*  file handle    */

# define CLNO    unsigned int   /*  cluster number */
# define RECNO   unsigned long  /*  record number  */


/*
 *  OFD - open file descriptor
 */

OFD
{
    OFD   *o_link;      /*  link to next OFD                    */
    int   o_flag;
    int   o_time;       /*  the next 4 items must be as in FCB  */
    int   o_date;       /*  time, date of creation              */
    CLNO  o_strtcl;     /*  starting cluster number             */
    long  o_fileln;     /*  length of file in bytes             */

    DMD   *o_dmd;       /*  link to media descr                 */
    DND   *o_dnode;     /*  link to dir for this file           */
    OFD   *o_dirfil;    /*  OFD for dir for this file           */
    long  o_dirbyt;     /*  pos in dir of this files fcb (dcnt) */

    long  o_bytnum;     /* byte pointer within file             */
    CLNO  o_curcl;      /* current cluster number for file      */
    RECNO o_currec;     /* current record number for file       */
    unsigned int o_curbyt; /* byte pointer within current cluster */
    int   o_usecnt;     /* use count for inherited files        */
    OFD   *o_thread;    /* mulitple open thread list            */
    int   o_mod;        /* mode file opened in (see below)      */
} ;

/*
 * bit usage in o_mod
 *
 * bits 8-15 are internal-use only
 * bits 0-7 correspond to bit usage in the Fopen() system call
 * note: bits 4-7 are only used if GEMDOS file-sharing/record-locking
 *       is implemented
 */
#define RO_WRITE_OK 0x8000  /* ok to write a R/O file (we're creating it) */
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

#if     ! M0101052901
/*
 * O_COMPLETE -
 *
 * 1: traversal of directory file (to bld dir tree) has completed
 */

#define O_COMPLETE      2
#endif



/*
 *  FCB - File Control Block
 */

FCB
{
    char f_name[11];
    char f_attrib;
    char f_fill[10];
    int  f_time;
    int  f_date;
    CLNO f_clust;
    long f_fileln;
} ;

#define FA_RO           0x01
#define FA_HIDDEN       0x02
#define FA_SYSTEM       0x04
#define FA_VOL          0x08
#define FA_SUBDIR       0x10
#define FA_ARCHIVE      0x20

#define FA_NORM         (FA_ARCHIVE|FA_SYSTEM|FA_HIDDEN|FA_RO)
#define FA_LFN          0x0f



/*
 *  DND - Directory Node Descriptor
 */

DND /* directory node descriptor */
{
    char d_name[11];    /*  directory name                      */
    char d_fill;        /*  attributes?                         */
    int  d_flag;
    CLNO d_strtcl;      /*  starting cluster number of dir      */

    int  d_time;        /*  last mod ?                          */
    int  d_date;        /*  ""   ""                             */
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

/* flags:       */
#define B_16    1                               /* device has 16-bit FATs       */
#define B_FIX   2                               /* device has fixed media       */



/*
 *  DMD - Drive Media Block
 */

/*  records == logical sectors  */

DMD /* drive media block */
{
    RECNO  m_recoff[3]; /*  record offsets for fat,dir,data     */
    int    m_drvnum;    /*  drive number for this media         */
    int    m_fsiz;      /*  fat size in records M01.01.03       */
    int    m_clsiz;     /*  cluster size in records M01.01.03   */
    unsigned int m_clsizb;  /*  cluster size in bytes           */
    int    m_recsiz;    /*  record size in bytes                */

    CLNO   m_numcl;     /*  total number of clusters in data    */
    int    m_clrlog;    /* log (base 2) of clsiz in records     */
    int    m_clrm;      /* clsiz in rec, mask                   */
    int    m_rblog;     /* log (base 2) of recsiz in bytes      */
    int    m_rbm;       /* recsiz in bytes, mask                */
    int    m_clblog;    /* log (base 2) of clsiz in bytes       */
    int    m_clbm;      /* clsiz in bytes, mask                 */
    OFD    *m_fatofd;   /* OFD for 'fat file'                   */

    OFD    *m_ofl;      /*  list of open files                  */
    DND    *m_dtl;      /* root of directory tree list          */
    int    m_16;        /* 16 bit fat ?                         */
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

BCB
{
    BCB     *b_link;    /*  next bcb (API)              */
    WORD    b_bufdrv;   /*  unit for buffer (API)       */
    RECNO   b_bufrec;   /*  record number               */
    UBYTE   b_buftyp;   /*  buffer type                 */
    UBYTE   b_dirty;    /*  true if buffer dirty        */
    DMD     *b_dm;      /*  ptr to drive media block    */
    char    *b_bufr;    /*  pointer to buffer (API)     */
} ;

/*
 * FTAB - Open File Table Entry
 */

/* point these at OFDs when needed */
FTAB
{
    OFD *f_ofd;
    PD  *f_own;         /* file owners */
    int f_use;          /* use count */
} ;



/*
 * DTAINFO - Information stored in the dta by srch-frst for use by srch-nxt.
 */

#define DTAINFO struct DtaInfo

DTAINFO
{
    char  dt_name[12];          /*  file name: filename.typ     00-11   */
    long  dt_pos;               /*  dir position                12-15   */
    DND   *dt_dnd;              /*  pointer to DND              16-19   */
    char  dt_attr;              /*  attributes of file          20      */
                                /*  --  below must not change -- [1]    */
    char  dt_fattr;             /*  attrib from fcb             21      */
    int   dt_time;              /*  time field from fcb         22-23   */
    int   dt_date;              /*  date field from fcb         24-25   */
    long  dt_fileln;            /*  file length field from fcb  26-29   */
    char  dt_fname[14];         /*  file name from fcb          30-43   */
} ;                             /*    includes null terminator          */




/* External Declarations */

extern  DND     *dirtbl[];
extern  DMD     *drvtbl[];
extern  char    diruse[];
extern  LONG    drvsel;
extern  PD      *run;
extern  int     logmsk[];
extern  FTAB    sft[];
extern  BCB     *bufl[2];              /*  in bios main.c              */



/*
 * Forward Declarations
 */


/*
 * in fsdrive.c
 */

/* check the drive, see if it needs to be logged in. */
long ckdrv(int d);

/* allocate storage for and initialize a DMD */
DMD  *getdmd(int drv);

/* log in media 'b' on drive 'drv'. */
long log_media(BPB *b, int drv);

/*
 * in fshand.c
 */

long xforce(int std, int h);
long ixforce(int std, int h, PD *p);

/* ??? */
int syshnd(int h);

/* ??? */
void ixdirdup(int h, int dn, PD *p);

/* duplicate a file handle. */
long dup(long h);

/*
 * in fsopnclo.c
 */

/* create file with specified name, attributes */
long xcreat(char *name, char attr);
long ixcreat(char *name, char attr);

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
char *getrec(RECNO recn, OFD *of, int wrtflg);
/* pack into user buffer */
char *packit(register char *s, register char *d);

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

/* seek to byte position n on file with handle h */
long xlseek(long n, int h, int flg);
long ixlseek(OFD *p, long n);

long xread(int h, long len, void *ubufr);
long ixread(OFD *p, long len, void *ubufr);

long xwrite(int h, long len, void *ubufr);
long ixwrite(OFD *p, long len, void *ubufr);

/*
 * in fsdir.c
 */

long xmkdir(char *s);
long xrmdir(char *p);
long xchmod(char *p, int wrt, char mod);
long ixsfirst(char *name, register WORD att, register DTAINFO *addr);
long xsfirst(char *name, int att);
long xsnext(void);
long xgsdtof(int *buf, int h, int wrt);
void builds(const char *s1 , char *s2 );
long xrename(int n, char *p1, char *p2);
long xchdir(char *p);
long xgetdir(char *buf, int drv);
FCB *dirinit(DND *dn);
DND *findit(char *name, const char **sp, int dflag);
FCB *scan(register DND *dnd, const char *n, WORD att, LONG *posp);

/*
 * in fsmain.c
 */

char uc(register char c);
char *xgetdta(void);
void xsetdta(char *addr);
long xsetdrv(int drv);
long xgetdrv(void);
OFD  *makofd(register DND *p);
OFD  *getofd(int h);



/* Misc. defines */

#define CL_DIR  0x0002
#define CL_FULL 0x0004

#endif /* FS_H */
