/*      STRUCT.H        1/28/84 - 01/18/85      Lee Jay Lorenzen        */

/*
*       Copyright 1999, Caldera Thin Clients, Inc.
*                 2002-2015 The EmuTOS development team
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
#include "config.h"                     /* for AES_STACK_SIZE */

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

CQUEUE
{
        WORD    c_buff[KBD_SIZE];
        WORD    c_front;
        WORD    c_rear;
        WORD    c_cnt;
};


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
        ULONG   u_super[AES_STACK_SIZE];/*  4A  */
        ULONG   u_supstk;
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
/* p_name */
#define AP_NAMELEN  8           /* architectural */
/* p_stat */
#define         WAITIN          0x0001
#define         SWITCHIN        0x8000
/* p_flags */
#define AP_OPEN     0x0001      /* application is between appl_init() & appl_exit() */

struct aespd                    /* process descriptor           */
{
        AESPD   *p_link;        /*  0 */
        AESPD   *p_thread;      /*  4 */
        UDA     *p_uda;         /*  8 */

        BYTE    p_name[AP_NAMELEN]; /*  C */

        CDA     *p_cda;         /* 14  cio data area        */
        LONG    p_ldaddr;       /* 18  long addr. of load   */
        WORD    p_pid;          /* 1C */
        WORD    p_stat;         /* 1E */

        EVSPEC  p_evbits;       /* 20  event bits in use    */
        EVSPEC  p_evwait;       /* 22  event wait mask      */
        EVSPEC  p_evflg;        /* 24  event flags          */

        UWORD   p_flags;        /* 26  process status flags, see above */
        EVB     *p_evlist;      /* 28 */
        EVB     *p_qdq;         /* 2C */
        EVB     *p_qnq;         /* 30 */
        BYTE    *p_qaddr;       /* 34 */
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

typedef void (*FCODE)(LONG fdata);      /* pointer to fonction used by forkq() */

struct fpd
{
        FCODE   f_code;
        LONG    f_data;
} ;


#endif /* GEMSTRUCT_H */
