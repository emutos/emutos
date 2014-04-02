/*      STRUCT.H        1/28/84 - 01/18/85      Lee Jay Lorenzen        */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2014 The EmuTOS development team
*
*       This software is licenced under the GNU Public License.
*       Please see LICENSE.TXT for further information.
*
*                  Historical Copyright
*       -------------------------------------------------------------
*       GEM Application Environment Services              Version 2.3
*       Serial No.  XXXX-0000-654321              All Rights Reserved
*       Copyright (C) 1986                      Digital Research Inc.
*       -------------------------------------------------------------
*/

#ifndef GEMSTRUCT_H
#define GEMSTRUCT_H


typedef struct aespd   AESPD;           /* process descriptor           */
#define UDA     struct uda              /* user stack data area         */
#define CDA     struct cdastr           /* console data area structure  */
#define QPB     struct qpb              /* queue parameter block        */
#define EVB     struct evb              /* event block                  */
#define CQUEUE  struct cqueue           /* console kbd queue            */
#define SPB     struct spb              /* sync parameter block         */
typedef struct fpd     FPD;             /* fork process descriptor      */

typedef UWORD   EVSPEC;

#define NUM_PDS (NUM_ACCS + 2)          /* acc's + ctrlpd + dos appl.   */
#define EVBS_PER_PD     5               /* EVBs per AES process */
#define KBD_SIZE 8
#define QUEUE_SIZE 128
#define NFORKS 32

/* STACK_SIZE is the size of the private stack for each AES process.
 * It is used for the AES itself, including each call to the VDI, BIOS
 * and GEMDOS.
 *
 * Warning: When FreeMiNT and is used, EmuTOS' BDOS is replaced by FreeMiNT
 * implementation. Thus when GEM=ROM is used, the EmuTOS AES uses FreeMiNT's
 * GEMDOS. When called from supervisor mode, FreeMiNT's GEMDOS functions can
 * use significantly more stack than EmuTOS ones. For example, Fsfirst() uses
 * about 1.5 kB of stack space. It is used by scrp_write() and appl_init().
 *
 * Typically, the operation requiring the most stack usage is running FreeMiNT
 * with GEM=ROM, and double-click xaloader.prg to run XaAES. That calls
 * EmuTOS' appl_init() to determine if the physical VDI workstation is opened.
 *
 * The best value for STACK_SIZE can be estimated by enabling the define
 * CONF_DEBUG_AES_STACK in config.h.
 */
#define STACK_SIZE 590

CQUEUE
{
        WORD    c_buff[KBD_SIZE];
        WORD    c_front;
        WORD    c_rear;
        WORD    c_cnt;
};


/* mouse form */
#if 0  /* Defined in gsxdefs.h */
typedef struct mform
{
        WORD    mf_xhot;
        WORD    mf_yhot;
        WORD    mf_nplanes;
        WORD    mf_fg;
        WORD    mf_bg;
        WORD    mf_mask[16];
        WORD    mf_data[16];
} MFORM;
#endif


#define C_KOWNER 0x0001
#define C_MOWNER 0x0002

CDA
{
        UWORD   c_flags;
        EVB     *c_iiowait;     /* Waiting for Input            */
        EVB     *c_msleep;      /* wait for mouse rect          */
        EVB     *c_bsleep;      /* wait for button              */
        CQUEUE  c_q;            /* input queue                  */
};


UDA
{
        WORD    u_insuper;              /*   0  in supervisor flag       */
        ULONG   u_regs[15];             /*   2  d0-d7, a0-a6             */
        ULONG   *u_spsuper;             /*  3E  supervisor stack         */
        ULONG   *u_spuser;              /*  42  user stack               */
        ULONG   *u_oldspsuper;          /*  46  old ssp, used in trapaes [gemdosif.S] */
        ULONG   u_super[STACK_SIZE];    /*  4A  */
        ULONG   u_supstk;               /* 746  */
} ;


#define NOCANCEL 0x0001         /* event is occuring */
#define COMPLETE 0x0002         /* event completed */
#define EVDELAY  0x0004         /* event is delay event */
#define EVMOUT   0x0008         /* event flag for mouse wait outside of rect*/

EVB             /* event block structure */
{
        EVB     *e_nextp;       /* link to next event on PD event list */
        EVB     *e_link;        /* link to next block on event chain */
        EVB     *e_pred;        /* link to prev block on event chain */
        AESPD   *e_pd;          /* owner PD (data for fork) */
        LONG    e_parm;         /* parm for request -> event comm */
        WORD    e_flag;
        EVSPEC  e_mask;         /* mask for event notification */
        LONG    e_return;
} ;

/* pd defines */
/* p_stat */
#define         WAITIN          0x0001
#define         SWITCHIN        0x8000

struct aespd                    /* process descriptor           */
{
        AESPD   *p_link;        /*  0 */
        AESPD   *p_thread;      /*  4 */
        UDA     *p_uda;         /*  8 */

        BYTE    p_name[8];      /*  C */

        CDA     *p_cda;         /* 14  cio data area        */
        LONG    p_ldaddr;       /* 18  long addr. of load   */
        WORD    p_pid;          /* 1C */
        WORD    p_stat;         /* 1E */

        EVSPEC  p_evbits;       /* 20  event bits in use    */
        EVSPEC  p_evwait;       /* 22  event wait mask      */
        EVSPEC  p_evflg;        /* 24  event flags          */

        EVB     *p_evlist;      /* 28 */
        EVB     *p_qdq;         /* 2C */
        EVB     *p_qnq;         /* 30 */
        LONG    p_qaddr;        /* 34 */
        WORD    p_qindex;       /* 38 */
        BYTE    p_queue[QUEUE_SIZE];   /* 3A */
        BYTE    p_appdir[LEN_ZPATH+2];  /* directory containing the executable */
                                        /* (includes trailing path separator)  */
};



QPB
{
        AESPD   *qpb_ppd;
        WORD    qpb_cnt;
        LONG    qpb_buf;
} ;

SPB
{
        WORD    sy_tas;
        AESPD   *sy_owner;
        EVB     *sy_wait;
} ;

struct fpd
{
        void    (*f_code)();
        LONG    f_data;
} ;


#endif /* GEMSTRUCT_H */
